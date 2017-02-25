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

//extern "C" {

#pragma SDS data access_pattern(in_x:SEQUENTIAL, in_w:SEQUENTIAL, out_y:SEQUENTIAL)
#pragma SDS data mem_attribute(in_x:PHYSICAL_CONTIGUOUS, in_w:PHYSICAL_CONTIGUOUS, out_y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(in_x[0:x_nrows*xw_ncols])
#pragma SDS data zero_copy(in_w[0:w_nrows*xw_ncols])
#pragma SDS data zero_copy(out_y[0:x_nrows*w_nrows])
int mmult_accel(float *in_x, float *in_w, float *out_y, int x_nrows, int w_nrows, int xw_ncols)
{
  float a_buf[1*784];
  float b_buf[32*784];
  float c_buf[1*32];

  debug("[%s] (%d, %d) T(%d, %d) (%d, %d)\n", __func__, x_nrows, xw_ncols, w_nrows, xw_ncols, x_nrows, w_nrows);
  memcpy(a_buf, in_x, x_nrows*xw_ncols*sizeof(float));
  memcpy(b_buf, in_w, w_nrows*xw_ncols*sizeof(float));

  for (int row = 0; row < 32; row++) {
  	if (row==w_nrows)
  	  break;

#pragma HLS PIPELINE II=1
      float result = 0.0;
      for (int k = 0; k < 784; k++) {
//#pragma HLS unroll factor=2
    	if (k==xw_ncols)
    	  break;
		float product_term = a_buf[k] * b_buf[row*xw_ncols+k];
		result += product_term;
      }
      c_buf[row] = result;

  }
  debug("[%s] (%d, %d) T(%d, %d) (%d, %d)\n", __func__, x_nrows, xw_ncols, w_nrows, xw_ncols, x_nrows, w_nrows);
  memcpy(out_y, c_buf, x_nrows*w_nrows*sizeof(float));
  return 0;
}

//}
