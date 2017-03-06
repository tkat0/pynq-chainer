#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"

#ifndef __SYNTHESIS__
#define debug(...) {printf(__VA_ARGS__);}
#else
#define debug(...) {}
#endif

#ifndef __SYNTHESIS__
extern "C" { // for CFFI compiler
#endif

void init(inter_t bram_w[MAX_H*MAX_X], outer_t w[MAX_H*MAX_X], uint16_t n_x, uint16_t n_h) {
#pragma HLS INLINE self
#pragma HLS dataflow
#pragma HLS array_partition variable=bram_w cyclic factor=32
	// init weight for complete loop

	uint16_t i,j;

	// read to cache w
	for (i=0; i < MAX_X; i++) {
		for (j=0; j < MAX_H; j++) {
#pragma HLS PIPELINE II=1
			debug("[%s] bram_w init [%d]\n", __func__, i*n_h + j);
			if (i < n_x && j < n_h) {
				bram_w[i*n_h + j] = (inter_t) w[i*n_h + j];
			} else {
				bram_w[i*n_h + j] = (inter_t) 0;
			}
		}
	}

}

void xnor_mult(inter_t bram_w[MAX_H*MAX_X], outer_t x[MAX_X], outer_t w[MAX_H*MAX_X], outer_t h[MAX_H], uint16_t n_x, uint16_t n_h) {
#pragma HLS INLINE self

	static inter_t bram_x[MAX_X];
	static inter_t bram_h[MAX_H];
#pragma HLS array_partition variable=bram_x complete
#pragma HLS array_partition variable=bram_h complete


	debug("[%s] n_x: %d(max:%d), n_h: %d(max:%d)\n", __func__, n_x, MAX_X, n_h, MAX_H);

	uint16_t i,j;

	// read to cache x
	READ_X_LOOP:for (i=0; i < n_x; i++) {
#pragma HLS PIPELINE II=1
		bram_x[i] = (inter_t) x[i];
		// TODO relu
	}

	///
	// init outbuf
	for (j=0; j < MAX_H; j++) {
#pragma HLS unroll
		bram_h[j] = (inter_t) 0;
	}
	///

	////////////////////////////////////////
	
	// XNOR mmult
	XNOR_LOOP:for (i=0; i < MAX_X; i++) {
		if (i >= n_x)
			break;
		for (j=0; j < MAX_H; j++) {
#pragma HLS unroll factor=32
			inter_t product_term = ~(bram_x[i] ^ bram_w[i*n_h + j]) & 0x1; // XNOR
			debug("[%s] %d, xnor %d: %d\n", __func__, i, i*n_h+j, bram_w[i*n_h + j]);
			bram_h[j] += product_term;
		}
	}

	////////////////////////////////////////

	// write to dram
	// to [0, 1]
	WRITE_H_LOOP:for (i=0; i < n_h; i++) {
#pragma HLS PIPELINE II=1
		h[i] = (outer_t) ((bram_h[i] << 1) - n_x);
		debug("[%s] %d\n", __func__, h[i]);
	}

}

#pragma SDS data sys_port(x:AFI)
#pragma SDS data sys_port(w:AFI)
#pragma SDS data sys_port(h:AFI)
#pragma SDS data access_pattern(x:SEQUENTIAL, w:SEQUENTIAL, h:SEQUENTIAL)
#pragma SDS data copy(x[0:n_x])
#pragma SDS data copy(w[0:n_h*n_x])
#pragma SDS data copy(h[0:n_h])
void binary_connect(outer_t x[MAX_X], outer_t w[MAX_H*MAX_X], outer_t h[MAX_H], uint16_t n_x, uint16_t n_h) {
#pragma HLS INLINE self
	static inter_t bram_w[MAX_H*MAX_X];

	init(bram_w, w, n_x, n_h);
	xnor_mult(bram_w, x, w, h, n_x, n_h);
}



void mmult_kernel(inter_t in_A[A_NROWS*A_NCOLS],
		inter_t in_B[A_NCOLS*B_NCOLS], outer_t* out_C,
		int b_ncols, int a_ncols) {
#pragma HLS INLINE self
#pragma HLS array_partition variable=in_A block factor=32
#pragma HLS array_partition variable=in_B block factor=32

	int index_b, index_d;;
    outer_t result = 0;

//	for (index_a = 0; index_a < A_NROWS; index_a++) {
//		if (index_a > a_nrows-1)
//			break;
		for (index_b = 0; index_b < B_NCOLS; index_b++) {
//#pragma HLS unroll factor = 64  // 128: ERROR: [SDSoC 0-0] Hardware function 'mmult_accel' LUT resource requirement (58290) exceeds platform 'pynq' resource capacity (53200)
//#pragma HLS PIPELINE II=1 // XXX hlsç¸ºç¿«???¿½?¿½??¿½?¿½ç¹§å³¨â†‘ï¿½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½?

//			if (index_b < b_ncols) {
				//ap_uint<16> result = 0;
			    //outer_t result = 0;
				//#pragma HLS RESOURCE variable=result core=FAddSub_fulldsp
				for (index_d = 0; index_d < A_NCOLS; index_d++) {
#pragma HLS PIPELINE II=1 rewind
#pragma HLS unroll factor=32
//				    if (index_d == 0) {
//			            result = 0;
//					}
					//inter_t product_term = ~(in_A[index_a][index_d] ^ in_B[index_d][index_b]); // XNOR
					//int product_term = 0;
					if (index_d < a_ncols && index_b < b_ncols) {
						// multiply accumulate broken into individual operators
						// so that AutoESL can infer two FP operators

					    //inter_t product_term = in_A[index_a][index_d] * in_B[index_d][index_b];
						debug("mult %x(%d), %x(%d)\n", in_A[index_d], index_d, in_B[index_d*b_ncols+index_b], index_d*b_ncols+index_b);
						//debug("xor %x\n", in_A[index_d]^in_B[index_d*B_NCOLS+index_b]);
						//debug("xor %x\n", in_A[index_d]^in_B[index_d*B_NCOLS+index_b]);
						//debug("xnor %x\n", ~(in_A[index_d]^in_B[index_d*B_NCOLS+index_b]));
#ifndef __SYNTHESIS__
					    inter_t product_term = ~(in_A[index_d] ^ in_B[index_d*b_ncols + index_b]) & 0x1; // XNOR
#else
					    inter_t product_term = ~(in_A[index_d] ^ in_B[index_d*b_ncols + index_b]); // XNOR
#endif

						//#pragma HLS RESOURCE variable=product_term core=FMul_fulldsp
						debug("= %x\n", product_term);
						result += (outer_t)product_term;

#if 1
				        if (index_d == a_ncols-1) {
							// last time 
				        	debug("add = %d\n", result);
				        	//result = 2 * result - a_ncols; // [0,1]ç¸º???¿½?¿½??¿½?¿½è¬Œï½»???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½?
				        	result = (result << 1) - a_ncols; // [0,1]ç¸º???¿½?¿½??¿½?¿½è¬Œï½»???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½?
				        	debug("= %d (2*result-%d)\n", result, a_ncols);
				        	out_C[index_b] = result;
				        	result = 0;
				        }
#endif

					}

					// ç¸º??¿½?¿½????¿½?¿½??¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½?ç¸º???¿½?¿½??¿½?¿½ç¸ºè–™ï¿½?ç¸º???¿½?¿½??¿½?¿½éœ‘ï½½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½?

				}

#if 0
				if (index_b < b_ncols) {
					debug("add = %d\n", result);
					//result = 2 * result - a_ncols; // [0,1]ç¸º???¿½?¿½??¿½?¿½è¬Œï½»???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½?
					result = (result << 1) - a_ncols; // [0,1]ç¸º???¿½?¿½??¿½?¿½è¬Œï½»???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½???¿½?¿½??¿½?¿½?
					debug("= %d (2*result-%d)\n", result, a_ncols);
					out_C[index_a * b_ncols + index_b] = result;
				}
#endif
//			}

		}
//	}
}

#pragma SDS data access_pattern(in_A:SEQUENTIAL, in_B:SEQUENTIAL, out_C:SEQUENTIAL)
#pragma SDS data copy(in_A[0:a_ncols])
#pragma SDS data copy(in_B[0:a_ncols*b_ncols])
#pragma SDS data copy(out_C[0:b_ncols])
void mmult_accel(outer_t* in_A, outer_t* in_B, outer_t* out_C,
		int b_ncols, int a_ncols) {
	int i, j;
	inter_t a_buf[A_NROWS*A_NCOLS];
	inter_t b_buf[A_NCOLS*B_NCOLS];
//	for (i = 0; i < A_NROWS; i++) {
//		for (j = 0; j < A_NCOLS; j++) {
//#pragma HLS PIPELINE II=1
//			a_buf[i][j] = 1;
//		}
//	}

//#pragma HLS RESOURCE variable=a_buf core=RAM_2P_BRAM
//#pragma HLS RESOURCE variable=a_buf core=RAM_2P_BRAM

// Transfer matrix A from multi-buffer into local RAM
//	for (i = 0; i < a_nrows; i++) {
		for (j = 0; j < a_ncols; j++) {
#pragma HLS PIPELINE II=1
			//a_buf[i*a_nrows + j] = (inter_t) in_A[i * a_ncols + j];
			a_buf[j] = (inter_t) in_A[j];
		}
//	}

	// Transfer matrix B from multi-buffer into local RAM
	for (i = 0; i < a_ncols; i++) {
		for (j = 0; j < b_ncols; j++) {
#pragma HLS PIPELINE II=1
			//b_buf[i*a_ncols + j] = (inter_t) in_B[i * b_ncols + j];
			debug("%d\n", i*b_ncols+j);
			b_buf[i*b_ncols + j] = (inter_t) in_B[i * b_ncols + j];
		}
	}

	// Matrix multiply call
	mmult_kernel(a_buf, b_buf, out_C, b_ncols, a_ncols);
}

#ifndef __SYNTHESIS__
} // extern "C"
#endif
