/* File: C:/workspace2015.4/pynq-nn1/SDDebug/_sds/p0/.cf_work/portinfo.c */
#include "cf_lib.h"
#include "cf_request.h"
#include "devreg.h"

#include "portinfo.h"

#include "stdio.h"  // for printf

#include "xlnk_core_cf.h"
#include "accel_info.h"
#include "axi_lite_dm.h"

extern accel_info_t _sds__p0_BlackBoxJam_0;

axi_lite_info_t _p0_swinst_BlackBoxJam_0_cmd_BlackBoxJam_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x28"
};

axi_lite_info_t _p0_swinst_BlackBoxJam_0_in_r_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x80"
};

axi_lite_info_t _p0_swinst_BlackBoxJam_0_out_r_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x84"
};

axi_lite_info_t _p0_swinst_BlackBoxJam_0_doInit_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x88"
};

axi_lite_info_t _p0_swinst_BlackBoxJam_0_targetLayer_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x8C"
};

axi_lite_info_t _p0_swinst_BlackBoxJam_0_targetMem_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x90"
};

axi_lite_info_t _p0_swinst_BlackBoxJam_0_targetInd_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x94"
};

axi_lite_info_t _p0_swinst_BlackBoxJam_0_val_r_info = {
  .accel_info = &_sds__p0_BlackBoxJam_0,
  .reg_name = "0x98"
};

struct _p0_swblk_BlackBoxJam _p0_swinst_BlackBoxJam_0 = {
  .cmd_BlackBoxJam = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_cmd_BlackBoxJam_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .in_r = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_in_r_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .out_r = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_out_r_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .doInit = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_doInit_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .targetLayer = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_targetLayer_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .targetMem = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_targetMem_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .targetInd = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_targetInd_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
  .val_r = { .base = { 
		.channel_info = &_p0_swinst_BlackBoxJam_0_val_r_info, 
		.open_i = &axi_lite_open, 
		.close_i = &axi_lite_close },
		.send_i = &axi_lite_send },
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
  accel_open(&_sds__p0_BlackBoxJam_0);
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.cmd_BlackBoxJam.base );
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.in_r.base );
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.out_r.base );
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.doInit.base );
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.targetLayer.base );
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.targetMem.base );
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.targetInd.base );
  _p0_cf_open_port( &_p0_swinst_BlackBoxJam_0.val_r.base );
}

void _p0_cf_framework_close(int last)
{
  cf_close_i( &_p0_swinst_BlackBoxJam_0.cmd_BlackBoxJam, NULL);
  cf_close_i( &_p0_swinst_BlackBoxJam_0.in_r, NULL);
  cf_close_i( &_p0_swinst_BlackBoxJam_0.out_r, NULL);
  cf_close_i( &_p0_swinst_BlackBoxJam_0.doInit, NULL);
  cf_close_i( &_p0_swinst_BlackBoxJam_0.targetLayer, NULL);
  cf_close_i( &_p0_swinst_BlackBoxJam_0.targetMem, NULL);
  cf_close_i( &_p0_swinst_BlackBoxJam_0.targetInd, NULL);
  cf_close_i( &_p0_swinst_BlackBoxJam_0.val_r, NULL);
  accel_close(&_sds__p0_BlackBoxJam_0);
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

