#include <string.h>
#include <stdio.h>
//#include "mmult_accel.h"

#define debug(...) {printf(__VA_ARGS__);}
//#define debug(...) {}

#if 0
#define X_CACHE_SIZE (1024*4)
#define W_CACHE_SIZE (1024*64)
#define Y_CACHE_SIZE (1024*2)

static float x_row_cache[X_CACHE_SIZE];
static float w_row_cache[W_CACHE_SIZE];
static float y_col_cache[Y_CACHE_SIZE];

static int r_xcnt = 0;
static int n_xread = 0;
static int n_xread_pix = 0;
static int r_xflg = 0;
int x_get_next_line_offset(float* input, int width, int height) {
	int n_cache_line = X_CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	if (width*height <= X_CACHE_SIZE) {
		if (r_xflg == 0) {
			debug("[%s] read from DRAM %d (1 time)\n", __func__, height*width);
			memcpy(x_row_cache, input, height * width * sizeof(float));
			r_xflg = 1;
		}
	} else {
		debug("[%s]r_xcnt:%d, cache_size: %d, width: %d, height: %d, n_cache_line:%d, n_xread:%d\n", __func__, r_xcnt, X_CACHE_SIZE, width, height, n_cache_line, n_xread);
		if (r_xcnt == 0 && r_xflg!=1) {
			debug("[%s]read from DRAM %d\n", __func__, n_xread*n_cache_line*width);
			memcpy(x_row_cache, &input[n_xread * n_cache_line * width], n_cache_line * width * sizeof(float));
			n_xread++;
			n_xread_pix += n_cache_line * width;
			if (n_xread_pix == width*height) {
				debug("[%s] read all pixels\n", __func__);
				n_xread = 0;
				n_xread_pix = 0;
			}
		}
	}

	int idx = r_xcnt;
	if (r_xcnt == n_cache_line - 1) {
		r_xcnt = 0;
	} else {
		r_xcnt++;
	}
	debug("[%s] offset:%d\n", __func__, idx*width);
	//return &x_row_cache[idx * width];
	return idx * width;
}

static int r_wcnt = 0;
static int n_wread = 0;
static int n_wread_pix = 0;
static int r_wflg = 0;
int w_get_next_line_offset(float* input, int width, int height) {
	int n_cache_line = W_CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	if (width*height <= W_CACHE_SIZE) {
		if (r_wflg == 0) {
			debug("[%s] read from DRAM %d (1 time)\n", __func__, height*width);
			memcpy(w_row_cache, input, height * width * sizeof(float));
			r_wflg = 1;
		}
	} else {
		debug("[%s]r_wcnt:%d, cache_size: %d, width: %d, height: %d, n_cache_line:%d, n_wread:%d\n", __func__, r_wcnt, W_CACHE_SIZE, width, height, n_cache_line, n_wread);
		if (r_wcnt == 0 && r_wflg != 1) {
			memcpy(w_row_cache, &input[n_wread * n_cache_line * width], n_cache_line * width * sizeof(float));
			n_wread++;
			n_wread_pix += n_cache_line * width;
			if (n_wread_pix == width*height) {
				n_wread = 0;
				n_wread_pix = 0;
			}
		}
	}

	int idx = r_wcnt;
	if (r_wcnt == n_cache_line - 1) {
		r_wcnt = 0;
	} else {
		r_wcnt++;
	}
	debug("[%s] offset:%d\n", __func__, idx*width);
	//return &w_row_cache[idx * width];
	return idx * width;
}

static int w_cnt = 0;
static int n_write = 0;
static int n_write_pix = 0;
void y_write_cahce(float* output, int width, int height, float data) {
	int n_cache_line = Y_CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	debug("[%s] w_cnt:%d, width:%d, height:%d, n_cache_line:%d, n_write:%d, data:%f\n", __func__, w_cnt, width, height, n_cache_line, n_write, data);
	y_col_cache[w_cnt] = data;

	// if cache is full then write to DRAM.
	if (w_cnt == width*n_cache_line - 1) {
		// write n lines
		debug("[%s] write to DRAM offset:%d. size:%d\n",__func__ , n_write*width, width);
		memcpy(&output[n_write*width*n_cache_line], y_col_cache, width * n_cache_line * sizeof(float));
		n_write++;
		n_write_pix += n_cache_line * width;
		if (n_write_pix == width*height) {
			n_write = 0;
			n_write_pix = 0;
		}
		w_cnt = 0;
	} else {
		w_cnt++;
	}
}

//#pragma SDS data sys_port(x:ACP, w:ACP, y:ACP)
#pragma SDS data access_pattern(x:SEQUENTIAL, w:SEQUENTIAL, y:SEQUENTIAL)
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS, w:PHYSICAL_CONTIGUOUS, y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(x[0:x_nrows*xw_ncols])
#pragma SDS data zero_copy(w[0:w_nrows*xw_ncols])
#pragma SDS data zero_copy(y[0:x_nrows*w_nrows])
int mmult_accel1(float *x, float *w, float *y, int x_nrows, int w_nrows, int xw_ncols) {
	//float *x_row_cache_;
	//float *w_row_cache_;
	int x_row_cache_;
	int w_row_cache_;

	debug("[%s] x: (%d, %d), w: (%d, %d) => y: (%d, %d)\n",__func__ , x_nrows, xw_ncols, w_nrows, xw_ncols, x_nrows, w_nrows);
//	r_xcnt = 0;
//	n_xread = 0;
//	n_xread_pix = 0;
//	r_wcnt = 0;
//	n_wread = 0;
//	n_wread_pix = 0;
//	w_cnt = 0;
//	n_write = 0;
//	n_write_pix = 0;
//	r_xflg = 0;
//	r_wflg = 0;

	memcpy(x_row_cache, x, x_nrows * xw_ncols * sizeof(float));
	memcpy(w_row_cache, w, w_nrows * xw_ncols * sizeof(float));

	for (int w_row = 0; w_row < w_nrows; w_row++) {

		// read 1 row from DRAM
		debug("[%s] i:%d\n", __func__, w_row);
		//w_row_cache_ = w_get_next_line_offset(w, xw_ncols, w_nrows);
		//w_row_cache_ = w_get_next_line_offset(w, xw_ncols, w_nrows);

		for (int x_row = 0; x_row < x_nrows; x_row++) {
#pragma HLS PIPELINE II=1
			// read 1 col from DRAM
			debug("[%s] i:%d, j:%d\n", __func__, w_row, x_row);
			//x_row_cache_ = x_get_next_line_offset(x, xw_ncols, x_nrows);
			//x_row_cache_ = x_get_next_line_offset(x, xw_ncols, x_nrows);

			float result = 0.0;
			for (int k = 0; k < xw_ncols; k++) {
//#pragma HLS unroll
				float product_term = x_row_cache[x_row*xw_ncols+k] * w_row_cache[w_row*xw_ncols+k];
				result += product_term;
			}
			debug("[%s] result:%f\n", __func__, result);
			//y_write_cahce(y, w_nrows, x_nrows, result);
			y_col_cache[w_row * x_nrows + x_row] = result;
		}
	}
	return 0;
}
#endif

//extern "C" {

//C:\Xilinx\SDSoC\2015.4\samples\zc706_mem_apps\mmult_sp0_all

#pragma SDS data access_pattern(in_x:SEQUENTIAL, in_w:SEQUENTIAL, out_y:SEQUENTIAL)
#pragma SDS data mem_attribute(in_x:PHYSICAL_CONTIGUOUS, in_w:PHYSICAL_CONTIGUOUS, out_y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(in_x[0:x_nrows*xw_ncols])
#pragma SDS data zero_copy(in_w[0:w_nrows*xw_ncols])
#pragma SDS data zero_copy(out_y[0:x_nrows*w_nrows])
int _mmult_accel(float *in_x, float *in_w, float *out_y, int x_nrows,
		int w_nrows, int xw_ncols) {
	float a_buf[1 * 768]; // 1x4096
	float b_buf[32 * 768]; // 4096x4096
	float c_buf[1 * 32]; // 1x4096

	memcpy(a_buf, in_x, x_nrows * xw_ncols * sizeof(float));
	memcpy(b_buf, in_w, w_nrows * xw_ncols * sizeof(float));

	for (int row = 0; row < w_nrows; row++) {
		for (int col = 0; col < x_nrows; col++) {
#pragma HLS PIPELINE II=1
			float result = 0.0;
			for (int k = 0; k < xw_ncols; k++) {
				//result += a_buf[col*xw_ncols+k] * b_buf[row*xw_ncols+k];
				//result += 1 * b_buf[row*xw_ncols+k];
				//result += a_buf[col*xw_ncols+k] * 1;
				result += a_buf[0] * b_buf[0];
			}
			c_buf[col * x_nrows + row] = result;
		}
	}
	memcpy(out_y, c_buf, x_nrows * w_nrows * sizeof(float));
	return 0;
}

#define A_NROWS 1
#define A_NCOLS 768
#define B_NCOLS 32
#define B_NROWS A_NCOLS

#pragma SDS data access_pattern(in_x:SEQUENTIAL, in_w:SEQUENTIAL, out_y:SEQUENTIAL)
#pragma SDS data mem_attribute(in_x:PHYSICAL_CONTIGUOUS, in_w:PHYSICAL_CONTIGUOUS, out_y:PHYSICAL_CONTIGUOUS)

#pragma SDS data zero_copy(in_A[0:A_NROWS*A_NCOLS])
#pragma SDS data zero_copy(in_B[0:A_NROWS*A_NCOLS])
#pragma SDS data zero_copy(out_C[0:A_NROWS*A_NCOLS])
int mmult_accel(float *in_A, float *in_B, float *out_C, int a_nrows, int b_ncols, int a_ncols) {
	float a_buf[A_NROWS * A_NCOLS];
	float b_buf[A_NCOLS * B_NCOLS];
	float c_buf[A_NROWS * B_NCOLS];

	// Copy to BRAM
	memcpy(a_buf, in_A, a_nrows * a_ncols * sizeof(float));
	memcpy(b_buf, in_B, a_ncols * b_ncols * sizeof(float));

	debug("[%s] (%d, %d) T(%d, %d) (%d, %d)\n", __func__, a_nrows, a_ncols,
			a_ncols, b_ncols, a_nrows, b_ncols);

	for (int col = 0; col < B_NCOLS; col++) {
#pragma HLS PIPELINE II=1

		if (col == b_ncols)
			break;

//#pragma HLS loop_tripcount min=1 max=32
		float result = 0.0;
		for (int k = 0; k < A_NCOLS; k++) {
//#pragma HLS unroll factor=4
//#pragma HLS loop_tripcount min=32 max=768

			if (k == a_ncols)
				break;

			debug("[%s] col: %d, k: %d, a_buf: %f, b_buf: %f\n", __func__, col,
					k, a_buf[k], b_buf[k*B_NCOLS+col]);
			result += a_buf[k] * b_buf[k * B_NCOLS + col];
		}
		debug("[%s] write cache: %d\n", __func__, col);
		c_buf[col] = result;
	}

	debug("[%s] write dram:\n", __func__);
	memcpy(out_C, c_buf, a_ncols * sizeof(float));
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "cf_stub.h"
int _p0_mmult_accel_0(float * in_A, float * in_B, float * out_C, int a_nrows, int b_ncols, int a_ncols);
int _p0_mmult_accel_0(float * in_A, float * in_B, float * out_C, int a_nrows, int b_ncols, int a_ncols)
{
  switch_to_next_partition(0);
  int start_seq[3];
  start_seq[0] = 0x00003f00;
  start_seq[1] = 0x00010100;
  start_seq[2] = 0x00020000;
  cf_request_handle_t _p0_swinst_mmult_accel_0_cmd;
  cf_send_i(&(_p0_swinst_mmult_accel_0.cmd_mmult_accel), start_seq, 3*sizeof(int), &_p0_swinst_mmult_accel_0_cmd);
  cf_wait(_p0_swinst_mmult_accel_0_cmd);

  cf_send_ref_i(&(_p0_swinst_mmult_accel_0.in_A), &in_A, (1*768) * 4, &_p0_request_0);
  cf_send_ref_i(&(_p0_swinst_mmult_accel_0.in_B), &in_B, (1*768) * 4, &_p0_request_1);
  cf_send_ref_i(&(_p0_swinst_mmult_accel_0.out_C), &out_C, (1*768) * 4, &_p0_request_2);
  cf_send_i(&(_p0_swinst_mmult_accel_0.a_nrows), &a_nrows, 4, &_p0_request_3);
  cf_send_i(&(_p0_swinst_mmult_accel_0.b_ncols), &b_ncols, 4, &_p0_request_4);
  cf_send_i(&(_p0_swinst_mmult_accel_0.a_ncols), &a_ncols, 4, &_p0_request_5);

  int _p0_mmult_accel_0_v_return;
  cf_receive_i(&(_p0_swinst_mmult_accel_0.ap_return), &_p0_mmult_accel_0_v_return, 4, &_p0_mmult_accel_0_num_ap_return, &_p0_request_6);

  cf_wait(_p0_request_0);
  cf_wait(_p0_request_1);
  cf_wait(_p0_request_2);
  cf_wait(_p0_request_3);
  cf_wait(_p0_request_4);
  cf_wait(_p0_request_5);
  cf_wait(_p0_request_6);
  return _p0_mmult_accel_0_v_return;
}



//#pragma SDS data access_pattern(in_x:SEQUENTIAL, in_w:SEQUENTIAL, out_y:SEQUENTIAL)
//#pragma SDS data mem_attribute(in_x:PHYSICAL_CONTIGUOUS, in_w:PHYSICAL_CONTIGUOUS, out_y:PHYSICAL_CONTIGUOUS)
//#pragma SDS data zero_copy(in_A[0:a_nrows*a_ncols])
//#pragma SDS data zero_copy(in_B[0:a_ncols*b_ncols])
//#pragma SDS data zero_copy(out_C[0:a_nrows*b_ncols])

#pragma SDS data mem_attribute(in_A:PHYSICAL_CONTIGUOUS, in_B:PHYSICAL_CONTIGUOUS, out_C:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(in_A[0:a_nrows*a_ncols])
#pragma SDS data zero_copy(in_B[0:a_ncols*b_ncols])
#pragma SDS data zero_copy(out_C[0:a_nrows*b_ncols])
#pragma SDS data access_pattern(in_A:SEQUENTIAL, in_B:SEQUENTIAL, out_C:SEQUENTIAL)
void mmult_kernel(float in_A[A_NROWS][A_NCOLS], float in_B[A_NCOLS][B_NCOLS],
		float out_C[A_NROWS * B_NCOLS], int a_nrows, int b_ncols, int a_ncols) {
//#pragma HLS INLINE self
//#pragma HLS array_partition variable=in_A block factor=16 dim=2
//#pragma HLS array_partition variable=in_B block factor=16 dim=1

	float a_buf[A_NROWS][A_NCOLS];
	float b_buf[A_NCOLS][B_NCOLS];        //
	float c_buf[A_NROWS][A_NCOLS];

	int index_a, index_b, index_d;

	for (index_a = 0; index_a < a_nrows; index_a++) {
		for (index_b = 0; index_b < b_ncols; index_b++) {
			float result = 0;
			for (index_d = 0; index_d < a_ncols; index_d++) {
#pragma HLS PIPELINE II=1
//#pragma HLS loop_tripcount min=32 max=1024
				// multiply accumulate broken into individual operators
				// so that AutoESL can infer two FP operators
				float product_term = in_A[index_a][index_d]
						* in_B[index_d][index_b];
				result += product_term;
			}
			out_C[index_a * a_ncols + index_b] = result;
		}
	}
}

#pragma SDS data mem_attribute(in_A:PHYSICAL_CONTIGUOUS, in_B:PHYSICAL_CONTIGUOUS, out_C:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(in_A[0:a_nrows*a_ncols])
#pragma SDS data zero_copy(in_B[0:a_ncols*b_ncols])
#pragma SDS data zero_copy(out_C[0:a_nrows*b_ncols])
#pragma SDS data access_pattern(in_A:SEQUENTIAL, in_B:SEQUENTIAL, out_C:SEQUENTIAL)
void mmult_accela(float in_A[A_NROWS * A_NCOLS], float in_B[A_NCOLS * B_NCOLS],
		float out_C[A_NROWS * B_NCOLS], int a_nrows, int b_ncols, int a_ncols) {
	int i, j;
	float a_buf[A_NROWS][A_NCOLS];
	float b_buf[A_NCOLS][B_NCOLS];        //

	// Transfer matrix A from multi-buffer into local RAM
	for (i = 0; i < a_ncols; i++) {
		for (j = 0; j < a_nrows; j++) {
#pragma HLS PIPELINE II=1
			a_buf[i][j] = in_A[i * a_nrows + j];
		}
	}

	// Transfer matrix B from multi-buffer into local RAM
	for (i = 0; i < b_ncols; i++) {
		for (j = 0; j < a_ncols; j++) {
#pragma HLS PIPELINE II=1
			b_buf[i][j] = in_B[i * a_ncols + j];
		}
	}

	// Matrix multiply call
	mmult_kernel(a_buf, b_buf, out_C, a_nrows, b_ncols, a_ncols);
}

//}
