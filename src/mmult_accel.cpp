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

typedef struct {
	int is_copy;
	int dram_idx;
	int cache_idx;
	int size;
} config_t;

class OutCacheManager{
private:
	float* cache_addr_;
	float* dram_addr_;

	int width_;
	int height_;
	int n_cache_line_;
	int cache_write_count_;
	int dram_write_count_;

public:

#if 0
	OutCacheManager(float* dram_addr, float* cache_addr,  int cache_size, int width, int height):dram_addr_(dram_addr), cache_addr_(cache_addr) {
		width_ = width;
		height_ = height;

		n_cache_line_ = cache_size / width_;
		if (n_cache_line_ > height_) {
			n_cache_line_ = height_;
		}
	}
#else
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
#endif

	void write(float data, config_t* config) {

		//cache_addr_[cache_write_count_] = data;

		// if cache is full then write to DRAM.
		if (cache_write_count_ == width_*n_cache_line_ - 1) {
			// write n lines
			debug("[%s] write to DRAM offset:%d. size:%d\n",__func__ , dram_write_count_*width_, width_);

			//memcpy(&dram_addr_[dram_write_count_*width_*n_cache_line_], cache_addr_, width_ * n_cache_line_ * sizeof(float));
			config->is_copy = 1;
			config->dram_idx = dram_write_count_*width_*n_cache_line_;
			config->cache_idx = 0;
			config->size = width_ * n_cache_line_ * sizeof(float);

			dram_write_count_++;
			if (dram_write_count_*n_cache_line_*width_ == width_*height_) {
				dram_write_count_ = 0;
			}
			cache_write_count_ = 0;
		} else {
			config->is_copy = 0;

			cache_write_count_++;
		}

	}

};

#ifndef __SYNTHESIS__
extern "C" {
#endif

float x_row_cache[CACHE_SIZE];
float w_row_cache[CACHE_SIZE];
float y_col_cache[CACHE_SIZE];

#pragma SDS data sys_port(x:ACP, w:ACP, y:ACP)
#pragma SDS data access_pattern(x:SEQUENTIAL)
#pragma SDS data access_pattern(w:SEQUENTIAL)
#pragma SDS data access_pattern(y:SEQUENTIAL)
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS, w:PHYSICAL_CONTIGUOUS, y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(x[0:CACHE_SIZE])
#pragma SDS data zero_copy(w[0:CACHE_SIZE])
#pragma SDS data zero_copy(y[0:CACHE_SIZE])
//#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave bundle=memorybus register
//#pragma HLS INTERFACE m_axi depth=1024 port=w offset=slave bundle=memorybus register
//#pragma HLS INTERFACE m_axi depth=1024 port=y offset=slave bundle=memorybus register
int mmult_accel1(float *x, float *w, float *y, int x_nrows, int w_nrows, int xw_ncols) {
	float *x_row_cache_;
	float *w_row_cache_;

	static InCacheManager x_cache = InCacheManager();
	static InCacheManager w_cache = InCacheManager();
	static OutCacheManager y_cache = OutCacheManager();
	//static OutCacheManager y_cache = OutCacheManager(y, y_col_cache, CACHE_SIZE, w_nrows, x_nrows);

	x_cache.setup(x, x_row_cache, CACHE_SIZE, xw_ncols, x_nrows);
	w_cache.setup(w, w_row_cache, CACHE_SIZE, xw_ncols, w_nrows);
	y_cache.setup(y, y_col_cache, CACHE_SIZE, w_nrows, x_nrows);
	
	config_t y_config;

	debug("[%s] x: (%d, %d), w: (%d, %d) => y: (%d, %d)\n",__func__ , x_nrows, xw_ncols, w_nrows, xw_ncols, x_nrows, w_nrows);

	for (int w_row = 0; w_row < w_nrows; w_row++) {

		// read 1 row from Cache/DRAM
		debug("[%s] i:%d\n", __func__, w_row);
		w_row_cache_ = w_cache.get_line_offset();

		for (int x_row = 0; x_row < x_nrows; x_row++) {
//#pragma HLS PIPELINE II=1
			// read 1 col from Cache/DRAM
			debug("[%s] i:%d, j:%d\n", __func__, w_row, x_row);
			x_row_cache_ = x_cache.get_line_offset();

			float result = 0.0;
			for (int k = 0; k < xw_ncols; k++) {
#pragma HLS unroll// factor = 4
				result += x_row_cache_[k] * w_row_cache_[k];
			}
			debug("[%s] result:%f\n", __func__, result);
			y_cache.write(result, &y_config);
			if (y_config.is_copy == 1) {
				memcpy(&y[y_config.dram_idx], y_col_cache, y_config.size);
			}
		}
	}
	return 0;
}

#ifndef __SYNTHESIS__
}
#endif
