#include <stdio.h>
#include <stdlib.h>

#include <hls_stream.h>

#include "binary_connect.h"

using namespace hls;
using namespace std;

#ifndef __SYNTHESIS__
#define debug(...) {printf(__VA_ARGS__);}
#else
#define debug(...) {}
#endif

#ifndef __SYNTHESIS__
extern "C" { // for CFFI compiler
#endif

template<unsigned int InWidth,		// width of input stream
		unsigned int OutWidth,		// width of output stream
		unsigned int NumInWords		// number of input words to process
>
void StreamingDataWidthConverter_Batch(stream<ap_uint<InWidth> > & in,
		stream<ap_uint<OutWidth> > & out, const unsigned int numReps) {
	if (InWidth > OutWidth) {
		// emit multiple output words per input word read
		CASSERT_DATAFLOW(InWidth % OutWidth == 0);
		const unsigned int outPerIn = InWidth / OutWidth;
		const unsigned int totalIters = NumInWords * outPerIn * numReps;
		unsigned int o = 0;
		ap_uint<InWidth> ei = 0;
		for (unsigned int t = 0; t < totalIters; t++) {
#pragma HLS PIPELINE II=1
			// read new input word if current out count is zero
			if (o == 0)
				ei = in.read();
			// pick output word from the rightmost position
			ap_uint<OutWidth> eo = ei(OutWidth - 1, 0);
			out.write(eo);
			// shift input to get new output word for next iteration
			ei = ei >> OutWidth;
			// increment written output count
			o++;
			// wraparound indices to recreate the nested loop structure
			if (o == outPerIn) {
				o = 0;
			}
		}
	} else if (InWidth == OutWidth) {
		// straight-through copy
		for (unsigned int i = 0; i < NumInWords * numReps; i++) {
#pragma HLS PIPELINE II=1
			ap_uint<InWidth> e = in.read();
			out.write(e);
		}

	} else { // InWidth < OutWidth
		// read multiple input words per output word emitted
		CASSERT_DATAFLOW(OutWidth % InWidth == 0);
		const unsigned int inPerOut = OutWidth / InWidth;
		const unsigned int totalIters = NumInWords * numReps;
		unsigned int i = 0;
		ap_uint<OutWidth> eo = 0;
		for (unsigned int t = 0; t < totalIters; t++) {
#pragma HLS PIPELINE II=1
			// read input and shift into output buffer
			ap_uint<InWidth> ei = in.read();
			eo = eo >> InWidth;
			eo(OutWidth - 1, OutWidth - InWidth) = ei;
			// increment read input count
			i++;
			// wraparound logic to recreate nested loop functionality
			if (i == inPerOut) {
				i = 0;
				out.write(eo);
			}
		}
	}
}

// essentially small DMA generators, moving data between mem-mapped arrays and
// streams
template<unsigned int DataWidth, unsigned int numBytes>
void Mem2Stream(ap_uint<DataWidth> * in, stream<ap_uint<DataWidth> > & out) {
	CASSERT_DATAFLOW(DataWidth % 8 == 0);
	const unsigned int numWords = numBytes / (DataWidth / 8);
	CASSERT_DATAFLOW(numWords != 0);
	for (unsigned int i = 0; i < numWords; i++) {
#pragma HLS PIPELINE II=1
		ap_uint<DataWidth> e = in[i];
		out.write(e);
	}
}

template<unsigned int DataWidth, unsigned int numBytes>
void Stream2Mem(stream<ap_uint<DataWidth> > & in, ap_uint<DataWidth> * out) {
	CASSERT_DATAFLOW(DataWidth % 8 == 0);

	const unsigned int numWords = numBytes / (DataWidth / 8);
	CASSERT_DATAFLOW(numWords != 0);
	for (unsigned int i = 0; i < numWords; i++) {
#pragma HLS PIPELINE II=1
		ap_uint<DataWidth> e = in.read();
		out[i] = e;
	}
}

// call different statically-sized variants of Mem2Stream and Stream2Mem to
// generate larger bursts when possible. otherwise, reading single images all
// the time limits the memory throughput.
// the 16 here can be any power of two (has to be power of two, otherwise
// checking the modulo takes a lot more resources)
template<unsigned int DataWidth, unsigned int numBytes>
void Mem2Stream_Batch(ap_uint<DataWidth> * in,
		stream<ap_uint<DataWidth> > & out, const unsigned int numReps) {
	const unsigned int indsPerRep = numBytes / (DataWidth / 8);
	unsigned int rep = 0;
	// make sure Mem2Stream does not get inlined here
	// we lose burst inference otherwise
	while (rep != numReps) {
		unsigned int repsLeft = numReps - rep;
		if ((repsLeft & 0xF) == 0) {
			// repsLeft divisable by 16, read 16 images
			Mem2Stream<DataWidth, numBytes * 16>(&in[rep * indsPerRep], out);
			rep += 16;
		} else {
			// fallback, read single image
			Mem2Stream<DataWidth, numBytes>(&in[rep * indsPerRep], out);
			rep += 1;
		}
	}
}
template<unsigned int DataWidth, unsigned int numBytes>
void Stream2Mem_Batch(stream<ap_uint<DataWidth> > & in,
		ap_uint<DataWidth> * out, const unsigned int numReps) {
	const unsigned int indsPerRep = numBytes / (DataWidth / 8);
	unsigned int rep = 0;
	// make sure Stream2Mem does not get inlined here
	// we lose burst inference otherwise
	while (rep != numReps) {
		unsigned int repsLeft = numReps - rep;
		if ((repsLeft & 0xF) == 0) {
			// repsLeft divisable by 16, write 16 images
			Stream2Mem<DataWidth, numBytes * 16>(in, &out[rep * indsPerRep]);
			rep += 16;
		} else {
			// fallback, write single image
			Stream2Mem<DataWidth, numBytes>(in, &out[rep * indsPerRep]);
			rep += 1;
		}
	}
}

// popcount implemented as unsigned 1-bit add
// HLS automatically balances this into an adder tree
template<unsigned int SIMDWidth, unsigned int PopCountWidth>
ap_uint<PopCountWidth> NaivePopCount(ap_uint<SIMDWidth> in) {
	ap_uint<PopCountWidth> pct = 0;
	for (unsigned int i = 0; i < SIMDWidth; i++) {
		pct += in(i, i);
	}
	return pct;
}

// TODO should be possible to integrate this into the baseline MVTU using a
// template parameter
// streaming matrix-vector multiply component with no activation:
// binarized inputs, binarized weights, PopCountWidth-bit outputs
template<unsigned int SIMDWidth, 		// number of SIMD lanes per PE
		unsigned int PECount,			// number of PEs
		unsigned int PopCountWidth, // number of bits in popcount accumulator (>=log2(fanin))
		unsigned int MatrixW,		// width of matrix, multiple of SIMDWidth
		unsigned int MatrixH,			// height of matrix, multiple of PECount
		unsigned int WMemCount			// entries in weight memory
>
void StreamingMatrixVector_NoActivation_Batch(stream<ap_uint<SIMDWidth> > & in,
		stream<ap_uint<PECount * PopCountWidth> > & out,
		const ap_uint<SIMDWidth> weightMem[PECount][WMemCount],
		const unsigned int numReps) {
	// how many different rows each neuron will compute
	// alternatively: number of vertical matrix chunks
	const unsigned int neuronFold = MatrixH / PECount;
	// how many synapse groups each row is split into
	// alternatively: number of horizontal matrix chunks
	const unsigned int synapseFold = MatrixW / SIMDWidth;
	// input vector buffer
	ap_uint<SIMDWidth> inputBuf[synapseFold];
	// PE accumulator registers, initialized to zero on first call to function
	// why not defined as static? then different calls to StreamingMatrixVector
	// with the same template parameters would share these accumulator registers
	ap_uint<PopCountWidth> accPopCount[PECount];
	for (unsigned int i = 0; i < PECount; i++) {
#pragma HLS UNROLL
		accPopCount[i] = 0;
	}

#pragma HLS ARRAY_PARTITION variable=accPopCount complete dim=1

	unsigned int nf = 0;
	unsigned int sf = 0;
	const unsigned int totalFold = neuronFold * synapseFold;
	// everything merged into a common iteration space (one "big" loop instead
	// of smaller nested loops) to get the pipelinening the way we want
	for (unsigned int i = 0; i < totalFold * numReps; i++) {
#pragma HLS PIPELINE II=1
		ap_uint<SIMDWidth> inElem;
		if (nf == 0) {
			// read input from stream
			inElem = in.read();
			// buffer for reuse
			inputBuf[sf] = inElem;
		} else {
			// reuse buffered input
			inElem = inputBuf[sf];
		}
		// compute matrix-vector product for each processing element
		for (unsigned int pe = 0; pe < PECount; pe++) {
#pragma HLS UNROLL
			ap_uint<SIMDWidth> weight = weightMem[pe][nf * synapseFold + sf];
			ap_uint<SIMDWidth> masked = ~(weight ^ inElem);
			accPopCount[pe] += NaivePopCount<SIMDWidth, PopCountWidth>(masked);
		}
		// keep track of which folded synapse/neuron we are processing
		sf++;
		if (sf == synapseFold) {
			// produce output and clear accumulators
			ap_uint<PECount * PopCountWidth> outElem = 0;
			for (unsigned int pe = 0; pe < PECount; pe++) {
#pragma HLS UNROLL
				outElem((pe + 1) * PopCountWidth - 1, pe * PopCountWidth) = accPopCount[pe];
				accPopCount[pe] = 0;	// clear the accumulator
			}
			out.write(outElem);
			// next folded neuron
			sf = 0;
			nf++;
		}
		if (nf == neuronFold) {
			// next image
			nf = 0;
		}
	}
}

// helper function for fully connected layers with no activation
// instantiates matrix vector unit plus data width converters
template<unsigned int InStreamW, unsigned int OutStreamW,
		unsigned int SIMDWidth, unsigned int PECount,
		unsigned int PopCountWidth, unsigned int MatrixW, unsigned int MatrixH,
		unsigned int WMemCount>
void StreamingFCLayer_NoActivation_Batch(stream<ap_uint<InStreamW> > & in,
		stream<ap_uint<OutStreamW> > & out,
		const ap_uint<SIMDWidth> weightMem[PECount][WMemCount],
		const unsigned int numReps) {
#pragma HLS INLINE
	stream<ap_uint<SIMDWidth> > in2mvu("StreamingFCLayer_NoAct_Batch.in2mvu");
	stream<ap_uint<PECount * PopCountWidth> > mvu2out(
			"StreamingFCLayer_NoAct_Batch.mvu2out");
	const unsigned int InpPerImage = MatrixW / InStreamW;
	StreamingDataWidthConverter_Batch<InStreamW, SIMDWidth, InpPerImage>(in,
			in2mvu, numReps);
	StreamingMatrixVector_NoActivation_Batch<SIMDWidth, PECount, PopCountWidth,
			MatrixW, MatrixH, WMemCount>(in2mvu, mvu2out, weightMem, numReps);
	const unsigned int OutPerImage = MatrixH / PECount;
	StreamingDataWidthConverter_Batch<PECount * PopCountWidth, OutStreamW,
			OutPerImage>(mvu2out, out, numReps);
}

// Network Weights
static ap_uint<L0_SIMD> weightMem0[L0_PE][L0_WMEM];
static ap_uint<L1_SIMD> weightMem1[L1_PE][L1_WMEM];
static ap_uint<L2_SIMD> weightMem2[L2_PE][L2_WMEM];


void DoMemInit(unsigned int targetLayer, unsigned int targetMem, unsigned int targetInd, ap_uint<32> val) {

  switch(targetLayer) {
		case 0:
			weightMem0[targetMem][targetInd] = val;
			break;
		case 1:
			weightMem1[targetMem][targetInd] = val;
			break;
		case 2:
			weightMem2[targetMem][targetInd] = val;
			break;
		}
}

void DoCompute(ap_uint<32> * in, ap_uint<32> * out, const unsigned int targetLayer) {
	stream<ap_uint<32> > memInStrm("DoCompute.memInStrm");
//	stream<ap_uint<32> >  inter0("DoCompute.inter0");
//	stream<ap_uint<32> > inter1("DoCompute.inter1");
//	stream<ap_uint<32> >  inter2("DoCompute.inter2");
	stream<ap_uint<32> >  memOutStrm("DoCompute.memOutStrm");

#pragma HLS DATAFLOW
#pragma HLS stream depth=1024 variable=memInStrm     	// mask memory latency
//#pragma HLS stream depth=256 variable=memInStrm     	// mask memory latency
//#pragma HLS stream depth=8 variable=inter0
//#pragma HLS stream depth=8 variable=inter1
//#pragma HLS stream depth=8 variable=inter2
#pragma HLS stream depth=1024 variable=memOutStrm		// mask memory latency
//#pragma HLS stream depth=256 variable=memOutStrm		// mask memory latency

	const unsigned int numReps = 1;

//  Mem2Stream_Batch<32, inBytesPadded>(in, memInStrm, numReps);

  switch(targetLayer) {
		case 0:
			Mem2Stream_Batch<32, 104>(in, memInStrm, numReps);
			StreamingFCLayer_NoActivation_Batch<32, 32, L0_SIMD, L0_PE, 16, L0_MW, L0_MH, L0_WMEM>(
					memInStrm, memOutStrm, weightMem0, numReps);
			Stream2Mem_Batch<32, 128>(memOutStrm, out, numReps);
			break;
		case 1:
			Mem2Stream_Batch<32, 128>(in, memInStrm, numReps);
			StreamingFCLayer_NoActivation_Batch<32, 32, L1_SIMD, L1_PE, 16, L1_MW, L1_MH, L1_WMEM>(
					memInStrm, memOutStrm, weightMem1, numReps);
			Stream2Mem_Batch<32, 128>(memOutStrm, out, numReps);
			break;
		case 2:
			Mem2Stream_Batch<32, 128>(in, memInStrm, numReps);
			StreamingFCLayer_NoActivation_Batch<32, 32, L2_SIMD, L2_PE, 16, L2_MW, L2_MH, L2_WMEM>(
					memInStrm, memOutStrm, weightMem2, numReps);
			Stream2Mem_Batch<32, 8>(memOutStrm, out, numReps);
			break;

  }

//  Stream2Mem_Batch<32, outBytesPadded>(memOutStrm, out, numReps);

}

//#pragma SDS data zero_copy(in[0:768])
//#pragma SDS data zero_copy(out[0:1024])

//#pragma SDS data sys_port(in:AFI)
//#pragma SDS data sys_port(out:AFI)
//#pragma SDS data access_pattern(in:SEQUENTIAL, out:SEQUENTIAL)
//#pragma SDS data zero_copy(in[0:768]) // max
//#pragma SDS data zero_copy(out[0:512])
void BlackBoxJam(ap_uint<32> *in, ap_uint<32> *out, bool doInit,
		unsigned int targetLayer, unsigned int targetMem,
		unsigned int targetInd, ap_uint<32> val) {
// pragmas for MLBP jam interface
// signals to be mapped to the AXI Lite slave port
//#pragma HLS INTERFACE s_axilite port=return bundle=control
//#pragma HLS INTERFACE s_axilite port=doInit bundle=control
//#pragma HLS INTERFACE s_axilite port=targetLayer bundle=control
//#pragma HLS INTERFACE s_axilite port=targetMem bundle=control
//#pragma HLS INTERFACE s_axilite port=targetInd bundle=control
//#pragma HLS INTERFACE s_axilite port=val bundle=control
//#pragma HLS INTERFACE s_axilite port=numReps bundle=control
// signals to be mapped to the AXI master port (hostmem)
//#pragma HLS INTERFACE m_axi offset=slave port=in bundle=hostmem depth=256
//#pragma HLS INTERFACE s_axilite port=in bundle=control
//#pragma HLS INTERFACE m_axi offset=slave port=out bundle=hostmem depth=256
//#pragma HLS INTERFACE s_axilite port=out bundle=control

// partition PE arrays
#pragma HLS ARRAY_PARTITION variable=weightMem0 complete dim=1
//#pragma HLS ARRAY_PARTITION variable=thresMem0 complete dim=1
#pragma HLS ARRAY_PARTITION variable=weightMem1 complete dim=1
//#pragma HLS ARRAY_PARTITION variable=thresMem1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=weightMem2 complete dim=1
//#pragma HLS ARRAY_PARTITION variable=thresMem2 complete dim=1
//#pragma HLS ARRAY_PARTITION variable=weightMem3 complete dim=1
//#pragma HLS ARRAY_PARTITION variable=thresMem3 complete dim=1

#pragma HLS RESOURCE variable=weightMem0 core=RAM_S2P_LUTRAM

	if (doInit) {
		DoMemInit(targetLayer, targetMem, targetInd, val);
	} else {
		//DoCompute(in, out, targetLayer);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include "cf_stub.h"
void _p0_BlackBoxJam_0(ap_uint<32> * in, ap_uint<32> * out, bool doInit, unsigned int targetLayer, unsigned int targetMem, unsigned int targetInd, ap_uint<32> val);
void _p0_BlackBoxJam_0(ap_uint<32> * in, ap_uint<32> * out, bool doInit, unsigned int targetLayer, unsigned int targetMem, unsigned int targetInd, ap_uint<32> val)
{
  switch_to_next_partition(0);
  int start_seq[3];
  start_seq[0] = 0x00007f00;
  start_seq[1] = 0x00010000;
  start_seq[2] = 0x00020000;
  cf_request_handle_t _p0_swinst_BlackBoxJam_0_cmd;
  cf_send_i(&(_p0_swinst_BlackBoxJam_0.cmd_BlackBoxJam), start_seq, 3*sizeof(int), &_p0_swinst_BlackBoxJam_0_cmd);
  cf_wait(_p0_swinst_BlackBoxJam_0_cmd);

  cf_send_i(&(_p0_swinst_BlackBoxJam_0.in_V), in, 4, &_p0_request_0);
  cf_send_i(&(_p0_swinst_BlackBoxJam_0.out_V), out, 4, &_p0_request_1);
  cf_send_i(&(_p0_swinst_BlackBoxJam_0.doInit), &doInit, 1, &_p0_request_2);
  cf_send_i(&(_p0_swinst_BlackBoxJam_0.targetLayer), &targetLayer, 4, &_p0_request_3);
  cf_send_i(&(_p0_swinst_BlackBoxJam_0.targetMem), &targetMem, 4, &_p0_request_4);
  cf_send_i(&(_p0_swinst_BlackBoxJam_0.targetInd), &targetInd, 4, &_p0_request_5);
  cf_send_i(&(_p0_swinst_BlackBoxJam_0.val_V), &val, 4, &_p0_request_6);


  cf_wait(_p0_request_0);
  cf_wait(_p0_request_1);
  cf_wait(_p0_request_2);
  cf_wait(_p0_request_3);
  cf_wait(_p0_request_4);
  cf_wait(_p0_request_5);
  cf_wait(_p0_request_6);
}




#ifndef __SYNTHESIS__
} // extern "C"
#endif
