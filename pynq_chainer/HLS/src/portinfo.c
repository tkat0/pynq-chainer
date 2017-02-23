/* File: C:/workspace2015.4/pynq-nn1/SDDebug/_sds/p0/.cf_work/portinfo.c */
#include "cf_lib.h"
#include "cf_request.h"
#include "devreg.h"

#include "portinfo.h"

#include "stdio.h"  // for printf

#include "xlnk_core_cf.h"
#include "accel_info.h"
#include "axi_lite_dm.h"
#include "zero_copy_dm.h"

extern accel_info_t _sds__p0_mmult_accel_0;

axi_lite_info_t _p0_swinst_mmult_accel_0_cmd_mmult_accel_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0x28"
};

zero_copy_info_t _p0_swinst_mmult_accel_0_in_x_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0x80",
  .needs_cache_flush_invalidate = 1,
  .dir_chan = XLNK_BI_DIRECTIONAL
};

zero_copy_info_t _p0_swinst_mmult_accel_0_in_w_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0x84",
  .needs_cache_flush_invalidate = 1,
  .dir_chan = XLNK_BI_DIRECTIONAL
};

zero_copy_info_t _p0_swinst_mmult_accel_0_out_y_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0x88",
  .needs_cache_flush_invalidate = 1,
  .dir_chan = XLNK_BI_DIRECTIONAL
};

axi_lite_info_t _p0_swinst_mmult_accel_0_x_nrows_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0x8C"
};

axi_lite_info_t _p0_swinst_mmult_accel_0_w_nrows_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0x90"
};

axi_lite_info_t _p0_swinst_mmult_accel_0_xw_ncols_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0x94"
};

axi_lite_info_t _p0_swinst_mmult_accel_0_ap_return_info = {
  .accel_info = &_sds__p0_mmult_accel_0,
  .reg_name = "0xC0"
};

struct _p0_swblk_mmult_accel _p0_swinst_mmult_accel_0 = {
  .cmd_mmult_accel = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_cmd_mmult_accel_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .in_x = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_in_x_info, 
		.open_i = &zero_copy_open, 
		.close_i = &zero_copy_close },
		.send_ref_i = &zero_copy_send_ref_i },
  .in_w = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_in_w_info, 
		.open_i = &zero_copy_open, 
		.close_i = &zero_copy_close },
		.send_ref_i = &zero_copy_send_ref_i },
  .out_y = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_out_y_info, 
		.open_i = &zero_copy_open, 
		.close_i = &zero_copy_close },
		.send_ref_i = &zero_copy_send_ref_i },
  .x_nrows = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_x_nrows_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .w_nrows = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_w_nrows_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .xw_ncols = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_xw_ncols_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .ap_return = { .base = { 
		.channel_info = &_p0_swinst_mmult_accel_0_ap_return_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.receive_i = &axi_lite_recv },
};

void _p0_cf_open_port (cf_port_base_t *port)
{
  port->open_i(port, NULL);
}

void _p0_cf_framework_open(int first)
{
  cf_context_init();
  xlnkCounterMap();
  _p0_cf_register(first);
  cf_get_current_context();
  accel_open(&_sds__p0_mmult_accel_0);
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.cmd_mmult_accel.base );
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.in_x.base );
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.in_w.base );
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.out_y.base );
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.x_nrows.base );
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.w_nrows.base );
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.xw_ncols.base );
  _p0_cf_open_port( &_p0_swinst_mmult_accel_0.ap_return.base );
}

void _p0_cf_framework_close(int last)
{
  cf_close_i( &_p0_swinst_mmult_accel_0.cmd_mmult_accel, NULL);
  cf_close_i( &_p0_swinst_mmult_accel_0.in_x, NULL);
  cf_close_i( &_p0_swinst_mmult_accel_0.in_w, NULL);
  cf_close_i( &_p0_swinst_mmult_accel_0.out_y, NULL);
  cf_close_i( &_p0_swinst_mmult_accel_0.x_nrows, NULL);
  cf_close_i( &_p0_swinst_mmult_accel_0.w_nrows, NULL);
  cf_close_i( &_p0_swinst_mmult_accel_0.xw_ncols, NULL);
  cf_close_i( &_p0_swinst_mmult_accel_0.ap_return, NULL);
  accel_close(&_sds__p0_mmult_accel_0);
  _p0_cf_unregister(last);
}

#define TOTAL_PARTITIONS 1
int current_partition_num = 0;
struct {
  void (*open)(int);
  void (*close)(int);
}

_ptable[TOTAL_PARTITIONS]  = {
    {.open = &_p0_cf_framework_open, .close= &_p0_cf_framework_close}, 
};

void switch_to_next_partition(int partition_num)
{
#ifdef __linux__
  if (current_partition_num != partition_num) {
    _ptable[current_partition_num].close(0);
    char buf[128];
    sprintf(buf, "cat /mnt/_sds/_p%d_.bin > /dev/xdevcfg", partition_num);
    system(buf);
    _ptable[partition_num].open(0);
    current_partition_num = partition_num;
  }
#endif
}

void init_first_partition() __attribute__ ((constructor));
void close_last_partition() __attribute__ ((destructor));
void init_first_partition()
{
    current_partition_num = 0;
    _ptable[current_partition_num].open(1);

    sds_trace_setup();
}


void close_last_partition()
{
#ifdef PERF_EST
    apf_perf_estimation_exit();
#endif
    sds_trace_cleanup();
    _ptable[current_partition_num].close(1);
    current_partition_num = 0;
}

