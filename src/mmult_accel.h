#ifndef MMULT_ACC_H_
#define MMULT_ACC_H_

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
#define A_NCOLS 256 //784
#define B_NCOLS 32
#define B_NROWS A_NCOLS

void mmult_accel (float* in_A,
                  float* in_B,
                  float* out_C, int a_nrows, int b_ncols, int a_ncols);
#endif



#endif /* MMULT_ACC_H_ */
