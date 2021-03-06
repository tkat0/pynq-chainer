#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"
#include "binary_connect.h"
#define NUM_TESTS 1024

#include "sds_lib.h"

int main_BlackBoxJam(void) {

	int *in = (int *)sds_alloc(32 * sizeof(int));
	int *out = (int *)sds_alloc(32 * sizeof(int));

	BlackBoxJam(in, out, 1, 0, 0, 0, 0);
	return 0;
}

int main_bin(void) {
	int *tin1Buf, *tin2Buf, *toutBufHw;

	tin1Buf = (int *)sds_alloc(MAX_X * sizeof(int));
	tin2Buf = (int *)sds_alloc(MAX_H * MAX_X * sizeof(int));
	toutBufHw = (int *)sds_alloc(MAX_H * sizeof(int));

	binary_connect(0, tin1Buf, tin2Buf, toutBufHw, 0);
	binary_connect(1, tin1Buf, tin2Buf, toutBufHw, 0);

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
