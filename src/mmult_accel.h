#ifndef MMULT_ACC_H_
#define MMULT_ACC_H_


#include <ap_int.h>


//typedef ap_uint<8> outer_t;
typedef unsigned int outer_t;
typedef ap_uint<1> inter_t;

//typedef ap_uint<8> index_t;
//typedef int index_t;

#if 0
#define A_NROWS 32
#define A_NCOLS 32
#define B_NCOLS 32
#define B_NROWS A_NCOLS
#pragma SDS data access_pattern(in_A:SEQUENTIAL, in_B:SEQUENTIAL, out_C:SEQUENTIAL)
void mmult_accel (float in_A[A_NROWS*A_NCOLS],
                  float in_B[A_NCOLS*B_NCOLS],
                  float out_C[A_NROWS*B_NCOLS]);
#else
#define A_NROWS 1
#define A_NCOLS 1024
#define B_NCOLS 1024
#define B_NROWS A_NCOLS

void mmult_accel (outer_t* in_A,
		outer_t* in_B,
		outer_t* out_C, int a_nrows, int b_ncols, int a_ncols);
#endif



#endif /* MMULT_ACC_H_ */
