/*
 * test.cpp
 *
 *  Created on: 2017/03/09
 *      Author: tkato
 */

#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"

int main(void) {

	int *x, *w, *y;
#define N_X 32
#define N_H 32

	int n_x[] = {256,32,32};
	int n_h[] = {32,32,10};
	for (int i = 0; i < 3; i++) {

		x = (int *) malloc(n_x[i] * sizeof(int));
		w = (int *) malloc(n_x[i] * n_h[i] * sizeof(int));
		y = (int *) malloc(n_h[i] * sizeof(int));

		printf("test start\n");
		fflush(stdout);

		binary_connect(0, x, w, y, i);

		printf("weight initialized.\n");
		fflush(stdout);

		binary_connect(1, x, w, y, i);

		printf("test end\n");

		free(x);
		free(y);
		free(w);
	}

	return 0;
}
