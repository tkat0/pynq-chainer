#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"

#define NUM_TESTS 1024

#include "sds_lib.h"

void _p0_binary_connect_0(uint16_t op, outer_t x[32], outer_t w[1024], outer_t h[32], uint16_t n_x, uint16_t n_h);
int main_bin(void) {
	outer_t *tin1Buf, *tin2Buf, *toutBufHw;

	tin1Buf = (outer_t *)sds_alloc(MAX_X * sizeof(outer_t));
	tin2Buf = (outer_t *)sds_alloc(MAX_H * MAX_X * sizeof(outer_t));
	toutBufHw = (outer_t *)sds_alloc(MAX_H * sizeof(outer_t));

	_p0_binary_connect_0(0, tin1Buf, tin2Buf, toutBufHw, MAX_X, MAX_H);
	_p0_binary_connect_0(1, tin1Buf, tin2Buf, toutBufHw, MAX_X, MAX_H);

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
