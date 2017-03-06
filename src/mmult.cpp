#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"

#define NUM_TESTS 1024

#include "sds_lib.h"

int main_bin(void) {
	outer_t *tin1Buf, *tin2Buf, *toutBufHw;

	tin1Buf = (outer_t *)sds_alloc(MAX_X * sizeof(outer_t));
	tin2Buf = (outer_t *)sds_alloc(MAX_H * MAX_X * sizeof(outer_t));
	toutBufHw = (outer_t *)sds_alloc(MAX_H * sizeof(outer_t));

	binary_connect(tin1Buf, tin2Buf, toutBufHw, MAX_X, MAX_H);

	return 0;
}

int main(void) {
	outer_t *tin1Buf, *tin2Buf, *toutBufHw;

	  tin1Buf = (outer_t *)sds_alloc(A_NROWS * A_NCOLS * sizeof(float));
	  tin2Buf = (outer_t *)sds_alloc(A_NCOLS * B_NCOLS * sizeof(float));
	  toutBufHw = (outer_t *)sds_alloc(A_NROWS * B_NCOLS * sizeof(float));

	mmult_accel(tin1Buf, tin2Buf, toutBufHw, B_NCOLS, A_NCOLS);

	return 0;
}
