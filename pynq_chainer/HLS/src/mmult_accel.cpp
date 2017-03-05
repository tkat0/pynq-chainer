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

void mmult_kernel(inter_t in_A[A_NROWS*A_NCOLS],
		inter_t in_B[A_NCOLS*B_NCOLS], outer_t* out_C,
		int b_ncols, int a_ncols) {
#pragma HLS INLINE self
#pragma HLS array_partition variable=in_A block factor=32
#pragma HLS array_partition variable=in_B block factor=32

	int index_a, index_b, index_d;
	index_a = 0;
    outer_t result = 0;

//	for (index_a = 0; index_a < A_NROWS; index_a++) {
//		if (index_a > a_nrows-1)
//			break;
		for (index_b = 0; index_b < B_NCOLS; index_b++) {
//#pragma HLS unroll factor = 64  // 128: ERROR: [SDSoC 0-0] Hardware function 'mmult_accel' LUT resource requirement (58290) exceeds platform 'pynq' resource capacity (53200)
//#pragma HLS PIPELINE II=1 // XXX hlsおわらな�?

//			if (index_b < b_ncols) {
				//ap_uint<16> result = 0;
			    //outer_t result = 0;
				//#pragma HLS RESOURCE variable=result core=FAddSub_fulldsp
				for (index_d = 0; index_d < A_NCOLS; index_d++) {
#pragma HLS PIPELINE II=1
//#pragma HLS unroll
				    if (index_d == 0) {
			            result = 0;
					}
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
				        	//result = 2 * result - a_ncols; // [0,1]に戻�?
				        	result = (result << 1) - a_ncols; // [0,1]に戻�?
				        	debug("= %d (2*result-%d)\n", result, a_ncols);
				        	out_C[index_a * b_ncols + index_b] = result;
				        }
#endif

					}

					// ある�?はここに追�?

				}

#if 0
				if (index_b < b_ncols) {
					debug("add = %d\n", result);
					//result = 2 * result - a_ncols; // [0,1]に戻�?
					result = (result << 1) - a_ncols; // [0,1]に戻�?
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

#include <stdio.h>
#include <stdlib.h>
#include "cf_stub.h"
void _p0_mmult_accel_0(outer_t * in_A, outer_t * in_B, outer_t * out_C, int b_ncols, int a_ncols);
void _p0_mmult_accel_0(outer_t * in_A, outer_t * in_B, outer_t * out_C, int b_ncols, int a_ncols)
{
  switch_to_next_partition(0);
  int start_seq[3];
  start_seq[0] = 0x00000300;
  start_seq[1] = 0x00010000;
  start_seq[2] = 0x00020000;
  cf_request_handle_t _p0_swinst_mmult_accel_0_cmd;
  cf_send_i(&(_p0_swinst_mmult_accel_0.cmd_mmult_accel), start_seq, 3*sizeof(int), &_p0_swinst_mmult_accel_0_cmd);
  cf_wait(_p0_swinst_mmult_accel_0_cmd);

  cf_send_i(&(_p0_swinst_mmult_accel_0.in_A), in_A, (a_ncols) * 4, &_p0_request_0);
  cf_send_i(&(_p0_swinst_mmult_accel_0.in_B), in_B, (a_ncols*b_ncols) * 4, &_p0_request_1);
  cf_send_i(&(_p0_swinst_mmult_accel_0.b_ncols), &b_ncols, 4, &_p0_request_3);
  cf_send_i(&(_p0_swinst_mmult_accel_0.a_ncols), &a_ncols, 4, &_p0_request_4);

  cf_receive_i(&(_p0_swinst_mmult_accel_0.out_C), out_C, (b_ncols) * 4, &_p0_mmult_accel_0_num_out_C, &_p0_request_2);

  cf_wait(_p0_request_0);
  cf_wait(_p0_request_1);
  cf_wait(_p0_request_2);
  cf_wait(_p0_request_3);
  cf_wait(_p0_request_4);
}



#ifndef __SYNTHESIS__
} // extern "C"
#endif
