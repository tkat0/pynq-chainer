#ifndef MMULT_ACC_H_
#define MMULT_ACC_H_

#define A_NROWS 32
#define A_NCOLS 64
#define B_NCOLS 32
#define B_NROWS A_NCOLS


//#pragma SDS data sys_port(in_A:MIG, in_B:MIG, out_C:MIG)
//#pragma SDS data zero_copy(in_A[0:A_NROWS*A_NCOLS])
//#pragma SDS data zero_copy(in_B[0:A_NROWS*A_NCOLS])
//#pragma SDS data zero_copy(out_C[0:A_NROWS*A_NCOLS])
int mmult_accel1(float *in_A, float *in_B, float *out_C, int a_nrows, int a_ncols, int b_ncols);
int _mmult_accel(float *in_A, float *in_B, float *out_C, int a_nrows, int a_ncols, int b_ncols);
int mmult_accel (float *in_A, float *in_B, float *out_C);
//int mmult_accel (float *in_A, float *in_B, float *out_C);

#endif /* MMULT_ACC_H_ */

