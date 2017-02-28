#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"

#define NUM_TESTS 1024

#include "sds_lib.h"

void _p0_mmult_accel_0(outer_t * in_A, outer_t * in_B, outer_t * out_C, int a_nrows, int b_ncols, int a_ncols);
int main(void) {
	outer_t *tin1Buf, *tin2Buf, *toutBufHw;

	  tin1Buf = (outer_t *)sds_alloc(A_NROWS * A_NCOLS * sizeof(float));
	  tin2Buf = (outer_t *)sds_alloc(A_NCOLS * B_NCOLS * sizeof(float));
	  toutBufHw = (outer_t *)sds_alloc(A_NROWS * B_NCOLS * sizeof(float));

	_p0_mmult_accel_0(tin1Buf, tin2Buf, toutBufHw, A_NROWS, B_NCOLS, A_NCOLS);

	return 0;
}
