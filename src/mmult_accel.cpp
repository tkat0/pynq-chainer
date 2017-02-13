#include <string.h>
#include "mmult_accel.h"

#define CACHE_SIZE (1024*16)

float x_row_cache[CACHE_SIZE];
float w_row_cache[CACHE_SIZE];

float* x_get_next_line_offset(float* input, int width) {
	static int cnt = 0;
	int n_cache_line = CACHE_SIZE / width;
	static int n_read = 0;

	if (cnt == 0) {
		memcpy(x_row_cache, &input[n_read * n_cache_line * width],
				n_cache_line * width * sizeof(float));
		n_read++;
	}

	int idx = cnt;
	if (cnt == n_cache_line) {
		cnt = 0;
	} else {
		cnt++;
	}
	return &x_row_cache[idx * width];
}

float* w_get_next_line_offset(float* input, int width) {
	static int cnt = 0;
	int n_cache_line = CACHE_SIZE / width;
	static int n_read = 0;

	if (cnt == 0) {
		memcpy(w_row_cache, &input[n_read * n_cache_line * width],
				n_cache_line * width * sizeof(float));
		n_read++;
	}

	int idx = cnt;
	if (cnt == n_cache_line) {
		cnt = 0;
	} else {
		cnt++;
	}
	return &w_row_cache[idx * width];
}

#pragma SDS data sys_port(x:ACP, w:ACP, y:ACP)
#pragma SDS data access_pattern(x:SEQUENTIAL)
#pragma SDS data access_pattern(w:SEQUENTIAL)
#pragma SDS data access_pattern(y:SEQUENTIAL)
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS, w:PHYSICAL_CONTIGUOUS, y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(x[0:CACHE_SIZE])
#pragma SDS data zero_copy(w[0:CACHE_SIZE])
#pragma SDS data zero_copy(y[0:CACHE_SIZE])
int mmult_accel1(float *x, float *w, float *y, int x_nrows, int w_nrows, int xw_ncols) {
	// 各軸のサイズがCACHE_SIZE以下でないといけないが、キャッシュを使い高速化したバージョン
	//float x_row_cache[CACHE_SIZE];
	float *x_row_cache_;
	float *w_row_cache_;
	float y_col_cache[CACHE_SIZE];

	for (int w_row = 0; w_row < w_nrows; w_row++) {

		// read 1 row from DRAM
		w_row_cache_ = w_get_next_line_offset(w, xw_ncols);
		//memcpy(w_row_cache, &w[w_row*xw_ncols], xw_ncols * sizeof(float));

		for (int x_row = 0; x_row < x_nrows; x_row++) {
#pragma HLS PIPELINE II=1
			// read 1 col from DRAM
			x_row_cache_ = x_get_next_line_offset(x, xw_ncols);
			//memcpy(x_row_cache, &x[x_row*xw_ncols], xw_ncols * sizeof(float));

			float result = 0.0;
			for (int k = 0; k < xw_ncols; k++) {
				result += x_row_cache_[k] * w_row_cache_[k];
				//result += in_A[row * a_ncols + k] * in_B[k * b_ncols + col];
			}
			y_col_cache[x_row] = result;
		}
		// write 1 col to DRAM
		//memcpy(&y[w_row*xw_ncols], y_col_cache, xw_ncols * sizeof(float));
		memcpy(&y[w_row*x_nrows], y_col_cache, x_nrows * sizeof(float));
	}
	return 0;
}
