
if {[llength [get_files *.bd]] == 1} {
  set diagram_handle [open_bd_design [get_files *.bd]]
} else {
  set diagram_handle [open_bd_design [get_files pynq.bd]]
}


#---------------------------
# Constant blocks
#---------------------------

#---------------------------
# Platform Parameters for pynq
#---------------------------
set ps7 [get_bd_cell /ps7]
    
set_property -dict [ list \
  CONFIG.PCW_USE_M_AXI_GP0 1 \
  CONFIG.PCW_USE_S_AXI_ACP 1 \
  CONFIG.PCW_USE_DEFAULT_ACP_USER_VAL 1 \
  ] $ps7
set xlconcat [get_bd_cell /xlconcat]
    
set_property -dict [ list \
  CONFIG.NUM_PORTS 1 \
  ] $xlconcat

#---------------------------
# Instantiating mmult_accel1_0
#---------------------------
set mmult_accel1_0 [create_bd_cell -type ip -vlnv xilinx.com:hls:mmult_accel1:1.0 mmult_accel1_0]
  

#---------------------------
# Instantiating mmult_accel1_0_if
#---------------------------
set mmult_accel1_0_if [create_bd_cell -type ip -vlnv xilinx.com:ip:axis_accelerator_adapter:2.1 mmult_accel1_0_if]
  
set_property -dict [ list \
  CONFIG.C_OUTPUT_SCALAR_0_WIDTH {32} \
  CONFIG.C_INPUT_SCALAR_0_WIDTH {32} \
  CONFIG.C_INPUT_SCALAR_1_WIDTH {32} \
  CONFIG.C_INPUT_SCALAR_2_WIDTH {32} \
  CONFIG.C_INPUT_SCALAR_3_WIDTH {32} \
  CONFIG.C_INPUT_SCALAR_4_WIDTH {32} \
  CONFIG.C_INPUT_SCALAR_5_WIDTH {32} \
  CONFIG.C_N_OUTPUT_ARGS {0} \
  CONFIG.C_N_INPUT_ARGS {0} \
  CONFIG.C_N_INPUT_SCALARS {6} \
  CONFIG.C_N_OUTPUT_SCALARS {1} \
  ] $mmult_accel1_0_if

#---------------------------
# Instantiating axi_ic_ps7_M_AXI_GP0
#---------------------------
set axi_ic_ps7_M_AXI_GP0 [create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_ic_ps7_M_AXI_GP0]
  
set_property -dict [ list \
  CONFIG.NUM_MI {1} \
  CONFIG.NUM_SI {1} \
  CONFIG.STRATEGY {2} \
  CONFIG.M00_HAS_REGSLICE {1} \
  CONFIG.S00_HAS_REGSLICE {1} \
  ] $axi_ic_ps7_M_AXI_GP0

#---------------------------
# Instantiating axi_ic_ps7_S_AXI_ACP
#---------------------------
set axi_ic_ps7_S_AXI_ACP [create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_ic_ps7_S_AXI_ACP]
  
set_property -dict [ list \
  CONFIG.NUM_MI {1} \
  CONFIG.NUM_SI {1} \
  CONFIG.STRATEGY {2} \
  CONFIG.M00_HAS_REGSLICE {1} \
  CONFIG.M00_HAS_DATA_FIFO {2} \
  CONFIG.S00_HAS_REGSLICE {1} \
  CONFIG.S00_HAS_DATA_FIFO {2} \
  ] $axi_ic_ps7_S_AXI_ACP

#---------------------------
# Instantiating ps7_irq_const
#---------------------------
set ps7_irq_const [create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 ps7_irq_const]
  
set_property -dict [ list \
  CONFIG.CONST_WIDTH {1} \
  CONFIG.CONST_VAL {0} \
  ] $ps7_irq_const

#---------------------------
# Instantiating acp_axcache_0xF
#---------------------------
set acp_axcache_0xF [create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 acp_axcache_0xF]
  
set_property -dict [ list \
  CONFIG.CONST_WIDTH {4} \
  CONFIG.CONST_VAL {15} \
  ] $acp_axcache_0xF

#---------------------------
# Connectivity
#---------------------------
connect_bd_net  \
  [get_bd_pins /mmult_accel1_0_if/aresetn] \
  [get_bd_pins /mmult_accel1_0/ap_rst_n] \

connect_bd_net  \
  [get_bd_pins /mmult_accel1_0/ap_return] \
  [get_bd_pins /mmult_accel1_0_if/ap_oscalar_0_din] \

connect_bd_net  \
  [get_bd_pins /mmult_accel1_0_if/ap_iscalar_0_dout] \
  [get_bd_pins /mmult_accel1_0/x] \

connect_bd_net  \
  [get_bd_pins /mmult_accel1_0_if/ap_iscalar_1_dout] \
  [get_bd_pins /mmult_accel1_0/w] \

connect_bd_net  \
  [get_bd_pins /mmult_accel1_0_if/ap_iscalar_2_dout] \
  [get_bd_pins /mmult_accel1_0/y] \

connect_bd_net  \
  [get_bd_pins /mmult_accel1_0_if/ap_iscalar_3_dout] \
  [get_bd_pins /mmult_accel1_0/x_nrows] \

connect_bd_net  \
  [get_bd_pins /mmult_accel1_0_if/ap_iscalar_4_dout] \
  [get_bd_pins /mmult_accel1_0/w_nrows] \

connect_bd_net  \
  [get_bd_pins /mmult_accel1_0_if/ap_iscalar_5_dout] \
  [get_bd_pins /mmult_accel1_0/xw_ncols] \

connect_bd_net  \
  [get_bd_pins /ps7/FCLK_CLK0] \
  [get_bd_pins /ps7/M_AXI_GP0_ACLK] \
  [get_bd_pins /ps7/S_AXI_ACP_ACLK] \
  [get_bd_pins /mmult_accel1_0/ap_clk] \
  [get_bd_pins /mmult_accel1_0_if/s_axi_aclk] \
  [get_bd_pins /mmult_accel1_0_if/aclk] \
  [get_bd_pins /axi_ic_ps7_M_AXI_GP0/ACLK] \
  [get_bd_pins /axi_ic_ps7_M_AXI_GP0/S00_ACLK] \
  [get_bd_pins /axi_ic_ps7_M_AXI_GP0/M00_ACLK] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/ACLK] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/M00_ACLK] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/S00_ACLK] \

connect_bd_net  \
  [get_bd_pins /proc_sys_reset_0_100M/interconnect_aresetn] \
  [get_bd_pins /axi_ic_ps7_M_AXI_GP0/ARESETN] \
  [get_bd_pins /axi_ic_ps7_M_AXI_GP0/S00_ARESETN] \
  [get_bd_pins /axi_ic_ps7_M_AXI_GP0/M00_ARESETN] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/ARESETN] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/M00_ARESETN] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/S00_ARESETN] \

connect_bd_net  \
  [get_bd_pins /proc_sys_reset_0_100M/peripheral_aresetn] \
  [get_bd_pins /mmult_accel1_0_if/s_axi_aresetn] \

connect_bd_net  \
  [get_bd_pins /ps7_irq_const/dout] \
  [get_bd_pins /xlconcat/In0] \

connect_bd_net  \
  [get_bd_pins /acp_axcache_0xF/dout] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/S00_AXI_arcache] \
  [get_bd_pins /axi_ic_ps7_S_AXI_ACP/S00_AXI_awcache] \

connect_bd_intf_net \
  [get_bd_intf_pins /mmult_accel1_0_if/ap_ctrl] \
  [get_bd_intf_pins /mmult_accel1_0/ap_ctrl] \

connect_bd_intf_net \
  [get_bd_intf_pins /ps7/M_AXI_GP0] \
  [get_bd_intf_pins /axi_ic_ps7_M_AXI_GP0/S00_AXI] \

connect_bd_intf_net \
  [get_bd_intf_pins /axi_ic_ps7_S_AXI_ACP/M00_AXI] \
  [get_bd_intf_pins /ps7/S_AXI_ACP] \

connect_bd_intf_net \
  [get_bd_intf_pins /mmult_accel1_0/m_axi_gmem] \
  [get_bd_intf_pins /axi_ic_ps7_S_AXI_ACP/S00_AXI] \

connect_bd_intf_net \
  [get_bd_intf_pins /axi_ic_ps7_M_AXI_GP0/M00_AXI] \
  [get_bd_intf_pins /mmult_accel1_0_if/S_AXI] \

#---------------------------
# Automation Commands
#---------------------------
assign_bd_address

include_bd_addr_seg [get_bd_addr_segs -excluded -of_objects [get_bd_addr_segs -of_objects [get_bd_intf_pin -of_objects [get_bd_cells] -filter "Mode==Slave"]]]


#---------------------------
# Create Address Map file
#---------------------------
set xml_file address_map.xml
set fp [open ${xml_file} w]
set addr_segs [get_bd_addr_segs -hier]
puts $fp "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
puts $fp "<xd:addressMap xmlns:xd=\"http://www.xilinx.com/xd\">"
foreach addr_seg $addr_segs {
  set path [get_property PATH $addr_seg]
  set offset [get_property OFFSET $addr_seg]
  if {$offset != ""} {
    set range [format 0x%X [get_property RANGE $addr_seg]]
    set high_addr [format 0x%X [expr $offset + $range - 1]]
    set slave [get_bd_addr_segs -of_object $addr_seg]
    regexp {([^/]+)/([^/]+)/([^/]+)$} $path match componentRef addressSpace segment
    regexp {([^/]+)/([^/]+)/([^/]+)$} $slave match slaveRef slaveMemoryMap slaveSegment  
    set slaveIntfPin [get_bd_intf_pins -of_objects $slave]
    regexp {([^/]+)$} $slaveIntfPin match slaveInterface
    puts $fp "  <xd:addressRange xd:componentRef=\"${componentRef}\" xd:addressSpace=\"${addressSpace}\" xd:segment=\"${segment}\" xd:slaveRef=\"${slaveRef}\" xd:slaveInterface=\"${slaveInterface}\" xd:slaveSegment=\"${slaveSegment}\" xd:baseAddr=\"${offset}\" xd:range=\"${range}\"/>"
  }
}
puts $fp "</xd:addressMap>"

close $fp
