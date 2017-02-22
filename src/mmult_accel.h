#ifndef MMULT_ACC_H_
#define MMULT_ACC_H_

#define A_NROWS 32
#define A_NCOLS 64
#define B_NCOLS 32
#define B_NROWS A_NCOLS


int mmult_accel1(float *in_A, float *in_B, float *out_C, int a_nrows, int a_ncols, int b_ncols);

#endif /* MMULT_ACC_H_ */

