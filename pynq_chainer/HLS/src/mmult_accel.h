#ifndef MMULT_ACC_H_
#define MMULT_ACC_H_

#define __SYNTHESIS__

typedef unsigned short uint16_t;
//typedef int uint16_t;

#ifndef __SYNTHESIS__
extern "C" { // for CFFI compiler
#endif

typedef int outer_t;
//typedef ap_uint<8> outer_t;
#ifdef __SYNTHESIS__
#include <ap_int.h>

typedef ap_uint<32> inter_t;
#else
typedef unsigned int inter_t;
#endif
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
		outer_t* out_C, int b_ncols, int a_ncols);

#define MAX_X 1024
#define MAX_H 1024

void binary_connect(int op, int *x, int *w,	int *h, int layer);
#endif


#ifndef __SYNTHESIS__
} // extern "C"
#endif

#endif /* MMULT_ACC_H_ */
