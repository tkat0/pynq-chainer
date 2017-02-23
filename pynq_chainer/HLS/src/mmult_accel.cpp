#include <string.h>
#include <stdio.h>
//#include "mmult_accel.h"

#define debug(...) {printf(__VA_ARGS__);}
//#define debug(...) {}

#define CACHE_SIZE (1024*16)

float x_row_cache[CACHE_SIZE];
float w_row_cache[CACHE_SIZE];
float y_col_cache[CACHE_SIZE];

static int r_xcnt = 0;
static int n_xread = 0;
static int n_xread_pix = 0;
static int r_xflg = 0;
float* x_get_next_line_offset(float* input, int width, int height) {
	int n_cache_line = CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	if (width*height <= CACHE_SIZE) {
		if (r_xflg == 0) {
			debug("[%s] read from DRAM %d (1 time)\n", __func__, height*width);
			memcpy(x_row_cache, input, height * width * sizeof(float));
			r_xflg = 1;
		}
	} else {
		debug("[%s]r_xcnt:%d, cache_size: %d, width: %d, height: %d, n_cache_line:%d, n_xread:%d\n", __func__, r_xcnt, CACHE_SIZE, width, height, n_cache_line, n_xread);
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
	return &x_row_cache[idx * width];
}

static int r_wcnt = 0;
static int n_wread = 0;
static int n_wread_pix = 0;
static int r_wflg = 0;
float* w_get_next_line_offset(float* input, int width, int height) {
	int n_cache_line = CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	if (width*height <= CACHE_SIZE) {
		if (r_wflg == 0) {
			debug("[%s] read from DRAM %d (1 time)\n", __func__, height*width);
			memcpy(w_row_cache, input, height * width * sizeof(float));
			r_wflg = 1;
		}
	} else {
		debug("[%s]r_wcnt:%d, cache_size: %d, width: %d, height: %d, n_cache_line:%d, n_wread:%d\n", __func__, r_wcnt, CACHE_SIZE, width, height, n_cache_line, n_wread);
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
	return &w_row_cache[idx * width];
}

static int w_cnt = 0;
static int n_write = 0;
static int n_write_pix = 0;
void y_write_cahce(float* output, int width, int height, float data) {
	int n_cache_line = CACHE_SIZE / width;

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

#pragma SDS data sys_port(x:ACP, w:ACP, y:ACP)
#pragma SDS data access_pattern(x:SEQUENTIAL, w:SEQUENTIAL, y:SEQUENTIAL)
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS, w:PHYSICAL_CONTIGUOUS, y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(x[0:x_nrows*xw_ncols])
#pragma SDS data zero_copy(w[0:w_nrows*xw_ncols])
#pragma SDS data zero_copy(y[0:x_nrows*w_nrows])
int mmult_accel1(float *x, float *w, float *y, int x_nrows, int w_nrows, int xw_ncols) {
	float *x_row_cache_;
	float *w_row_cache_;
	
	debug("[%s] x: (%d, %d), w: (%d, %d) => y: (%d, %d)\n",__func__ , x_nrows, xw_ncols, w_nrows, xw_ncols, x_nrows, w_nrows);
	r_xcnt = 0;
	n_xread = 0;
	n_xread_pix = 0;
	r_wcnt = 0;
	n_wread = 0;
	n_wread_pix = 0;
	w_cnt = 0;
	n_write = 0;
	n_write_pix = 0;
	r_xflg = 0;
	r_wflg = 0;

	for (int w_row = 0; w_row < w_nrows; w_row++) {

		// read 1 row from DRAM
		debug("[%s] i:%d\n", __func__, w_row);
		w_row_cache_ = w_get_next_line_offset(w, xw_ncols, w_nrows);

		for (int x_row = 0; x_row < x_nrows; x_row++) {
#pragma HLS PIPELINE II=1
			// read 1 col from DRAM
			debug("[%s] i:%d, j:%d\n", __func__, w_row, x_row);
			x_row_cache_ = x_get_next_line_offset(x, xw_ncols, x_nrows);

			float result = 0.0;
			for (int k = 0; k < xw_ncols; k++) {
#pragma HLS unroll factor = 1024
				result += x_row_cache_[k] * w_row_cache_[k];
			}
			debug("[%s] result:%f\n", __func__, result);
			y_write_cahce(y, w_nrows, x_nrows, result);
		}
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "cf_stub.h"
int _p0_mmult_accel1_0(float * x, float * w, float * y, int x_nrows, int w_nrows, int xw_ncols);
int _p0_mmult_accel1_0(float * x, float * w, float * y, int x_nrows, int w_nrows, int xw_ncols)
{
  switch_to_next_partition(0);
  int start_seq[3];
  start_seq[0] = 0x00003f00;
  start_seq[1] = 0x00010100;
  start_seq[2] = 0x00020000;
  cf_request_handle_t _p0_swinst_mmult_accel1_0_cmd;
  cf_send_i(&(_p0_swinst_mmult_accel1_0.cmd_mmult_accel1), start_seq, 3*sizeof(int), &_p0_swinst_mmult_accel1_0_cmd);
  cf_wait(_p0_swinst_mmult_accel1_0_cmd);

  cf_send_ref_i(&(_p0_swinst_mmult_accel1_0.x), &x, (x_nrows*xw_ncols) * 4, &_p0_request_0);
  cf_send_ref_i(&(_p0_swinst_mmult_accel1_0.w), &w, (w_nrows*xw_ncols) * 4, &_p0_request_1);
  cf_send_ref_i(&(_p0_swinst_mmult_accel1_0.y), &y, (x_nrows*w_nrows) * 4, &_p0_request_2);
  cf_send_i(&(_p0_swinst_mmult_accel1_0.x_nrows), &x_nrows, 4, &_p0_request_3);
  cf_send_i(&(_p0_swinst_mmult_accel1_0.w_nrows), &w_nrows, 4, &_p0_request_4);
  cf_send_i(&(_p0_swinst_mmult_accel1_0.xw_ncols), &xw_ncols, 4, &_p0_request_5);

  int _p0_mmult_accel1_0_v_return;
  cf_receive_i(&(_p0_swinst_mmult_accel1_0.ap_return), &_p0_mmult_accel1_0_v_return, 4, &_p0_mmult_accel1_0_num_ap_return, &_p0_request_6);

  cf_wait(_p0_request_0);
  cf_wait(_p0_request_1);
  cf_wait(_p0_request_2);
  cf_wait(_p0_request_3);
  cf_wait(_p0_request_4);
  cf_wait(_p0_request_5);
  cf_wait(_p0_request_6);
  return _p0_mmult_accel1_0_v_return;
}


