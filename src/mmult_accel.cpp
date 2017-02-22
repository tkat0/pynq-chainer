#include <string.h>
#include <stdio.h>
//#include "mmult_accel.h"

#ifndef __SYNTHESIS__
#define debug(...) {printf(__VA_ARGS__);}
#else
#define debug(...) {}
#endif

#define CACHE_SIZE (1024*16)

#define MAX_IN (1024)
#define MAX_OUT (1024)

#if 0
class InCacheManager{
private:
	float* cache_addr_;
	float* dram_addr_;

	int width_;
	int height_;
	int n_cache_line_;
	int cache_read_count_;
	int dram_read_count_;
	int is_read_once_;

public:

	void setup(float* dram_addr, float* cache_addr,  int cache_size, int width, int height) {

		dram_addr_ = dram_addr;
		cache_addr_ = cache_addr;
		width_ = width;
		height_ = height;

		n_cache_line_ = cache_size / width_;
		if (n_cache_line_ > height_) {
			n_cache_line_ = height_;
		}

		if (width_*height_ <= cache_size) {
			is_read_once_ = 1;
			debug("[%s] read once height:%d, width: %d %d <= %d\n", __func__, height_, width_, width_*height_, cache_size);
		}
		
		debug("[%s] read from DRAM (init) height:%d, width: %d n_cache_line: %d\n", __func__, height_, width_, n_cache_line_);
		//memcpy(cache_addr_, dram_addr_, width_*n_cache_line_ * sizeof(float));

	}

	float* get_line_offset() {

		debug("[%s] call height:%d, width: %d, cache_read_count_: %d\n", __func__, height_, width_, cache_read_count_);

		if (is_read_once_ != 1 && cache_read_count_ == 0) {
		    debug("[%s] read from DRAM height:%d, width: %d n_cache_line: %d\n", __func__, height_, width_, n_cache_line_);
			//memcpy(cache_addr_, &dram_addr_[dram_read_count_*n_cache_line_*width_], n_cache_line_*width_ * sizeof(float));

			dram_read_count_++;
			if (dram_read_count_*width_*n_cache_line_ == width_*height_) {
			    debug("[%s] read all pixels\n", __func__);
				dram_read_count_ = 0;
			}
		}

		int ret_idx = cache_read_count_*width_;
		if (cache_read_count_ == n_cache_line_ - 1) {
			cache_read_count_ = 0;
		} else {
			cache_read_count_++;
		}
		return &cache_addr_[ret_idx];

	}

};


namespace OutCacheManager{

//	float* cache_addr_;
//	float* dram_addr_;

    //float cache[CACHE_SIZE];

	static int width_;
	static int height_;
	static int n_cache_line_;
	static int cache_write_count_;
	static int dram_write_count_;

	void setup(float* dram_addr, float* cache_addr,  int cache_size, int width, int height) {

//		dram_addr_ = dram_addr;
//		cache_addr_ = cache_addr;
		width_ = width;
		height_ = height;

		n_cache_line_ = cache_size / width_;
		if (n_cache_line_ > height_) {
			n_cache_line_ = height_;
		}

	}

	void write(float* dram_addr_, float* cache_addr_, float data, int cache_size, int width_, int height_) {

		cache_addr_[cache_write_count_] = data;

		// if cache is full then write to DRAM.
		if (cache_write_count_ == width_*n_cache_line_ - 1) {
			// write n lines
			debug("[%s] write to DRAM offset:%d. size:%d\n",__func__ , dram_write_count_*width_, width_);

			memcpy(&dram_addr_[dram_write_count_*width_*n_cache_line_], cache_addr_, width_ * n_cache_line_ * sizeof(float));

			dram_write_count_++;
			if (dram_write_count_*n_cache_line_*width_ == width_*height_) {
				dram_write_count_ = 0;
			}
			cache_write_count_ = 0;
		} else {
			cache_write_count_++;
		}

	}

}
#endif


#ifndef __SYNTHESIS__
extern "C" {
#endif

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
		if (r_xcnt == 0 && r_xflg==1) {
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
		if (r_wcnt == 0 && r_wflg==1) {
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
#pragma SDS data access_pattern(x:SEQUENTIAL)
#pragma SDS data access_pattern(w:SEQUENTIAL)
#pragma SDS data access_pattern(y:SEQUENTIAL)
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS, w:PHYSICAL_CONTIGUOUS, y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(x[0:CACHE_SIZE])
#pragma SDS data zero_copy(w[0:CACHE_SIZE])
#pragma SDS data zero_copy(y[0:CACHE_SIZE])
int mmult_accel1(float *x, float *w, float *y, int x_nrows, int w_nrows, int xw_ncols) {
	float *x_row_cache_;
	float *w_row_cache_;

	//static InCacheManager x_cache = InCacheManager();
	//static InCacheManager w_cache = InCacheManager();
	//static OutCacheManager y_cache = OutCacheManager();
	//static OutCacheManager y_cache = OutCacheManager(y, y_col_cache, CACHE_SIZE, w_nrows, x_nrows);

	//x_cache.setup(x, x_row_cache, CACHE_SIZE, xw_ncols, x_nrows);
	//w_cache.setup(w, w_row_cache, CACHE_SIZE, xw_ncols, w_nrows);
	//y_cache.setup(y, y_col_cache, CACHE_SIZE, w_nrows, x_nrows);

	//setup(y, y_col_cache, CACHE_SIZE, w_nrows, x_nrows);
	
	debug("[%s] x: (%d, %d), w: (%d, %d) => y: (%d, %d)\n",__func__ , x_nrows, xw_ncols, w_nrows, xw_ncols, x_nrows, w_nrows);

	for (int w_row = 0; w_row < w_nrows; w_row++) {

		// read 1 row from Cache/DRAM
		debug("[%s] i:%d\n", __func__, w_row);
		//w_row_cache_ = w_cache.get_line_offset();
		w_row_cache_ = w_get_next_line_offset(w, xw_ncols, w_nrows);

		for (int x_row = 0; x_row < x_nrows; x_row++) {
//#pragma HLS PIPELINE II=1
			// read 1 col from Cache/DRAM
			debug("[%s] i:%d, j:%d\n", __func__, w_row, x_row);
			//x_row_cache_ = x_cache.get_line_offset();
			x_row_cache_ = x_get_next_line_offset(x, xw_ncols, x_nrows);

			float result = 0.0;
			for (int k = 0; k < xw_ncols; k++) {
#pragma HLS unroll// factor = 4
				result += x_row_cache_[k] * w_row_cache_[k];
			}
			debug("[%s] result:%f\n", __func__, result);
			//y_cache.write(result);
			//write(y, y_col_cache, result);
			y_write_cahce(y, w_nrows, x_nrows, result);
		}
	}
	return 0;
}

#ifndef __SYNTHESIS__
}
#endif
