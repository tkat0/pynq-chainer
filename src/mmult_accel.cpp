#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"

void mmult_kernel(inter_t in_A[A_NROWS*A_NCOLS],
		inter_t in_B[A_NCOLS*B_NCOLS], outer_t* out_C, int a_nrows,
		int b_ncols, int a_ncols) {
#pragma HLS INLINE self
#pragma HLS array_partition variable=in_A block factor=2
#pragma HLS array_partition variable=in_B block factor=2

	int index_a, index_b, index_d;
	index_a = 0;

//	for (index_a = 0; index_a < A_NROWS; index_a++) {
//		if (index_a > a_nrows-1)
//			break;
		for (index_b = 0; index_b < B_NCOLS; index_b++) {
//#pragma HLS PIPELINE II=1
//#pragma HLS unroll factor = 32
//			if (index_b < b_ncols) {
				//ap_uint<16> result = 0;
			int result = 0;
				//#pragma HLS RESOURCE variable=result core=FAddSub_fulldsp
				for (index_d = 0; index_d < A_NCOLS; index_d++) {
//#pragma HLS PIPELINE II=1
//#pragma HLS unroll
#if 1
					//inter_t product_term = ~(in_A[index_a][index_d] ^ in_B[index_d][index_b]); // XNOR
					int product_term = 1;
					if (index_d < a_ncols && index_b < b_ncols) {
						// multiply accumulate broken into individual operators
						// so that AutoESL can infer two FP operators

						//inter_t product_term = in_A[index_a][index_d] * in_B[index_d][index_b];

						//#pragma HLS RESOURCE variable=product_term core=FMul_fulldsp
						result += product_term;
					}
#else
					result += 1;
#endif

				}
				if (index_b < b_ncols) {
					out_C[index_a * B_NCOLS + index_b] = (outer_t) result;
				}
//			}

		}
//	}
}

#pragma SDS data access_pattern(in_A:SEQUENTIAL, in_B:SEQUENTIAL, out_C:SEQUENTIAL)
#pragma SDS data copy(in_A[0:a_nrows*a_ncols])
#pragma SDS data copy(in_B[0:a_ncols*b_ncols])
#pragma SDS data copy(out_C[0:a_nrows*b_ncols])
void mmult_accel(outer_t* in_A, outer_t* in_B, outer_t* out_C, int a_nrows,
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
	for (i = 0; i < a_nrows; i++) {
		for (j = 0; j < a_ncols; j++) {
#pragma HLS PIPELINE II=1
			a_buf[i*a_nrows + j] = (inter_t) in_A[i * a_ncols + j];
		}
	}

	// Transfer matrix B from multi-buffer into local RAM
	for (i = 0; i < a_ncols; i++) {
		for (j = 0; j < b_ncols; j++) {
#pragma HLS PIPELINE II=1
			b_buf[i*a_ncols + j] = (inter_t) in_B[i * b_ncols + j];
		}
	}

	// Matrix multiply call
	mmult_kernel(a_buf, b_buf, out_C, a_nrows, b_ncols, a_ncols);
}
