# 
# Synthesis run script generated by Vivado
# 

debug::add_scope template.lib 1
set_msg_config -id {HDL 9-1061} -limit 100000
set_msg_config -id {HDL 9-1654} -limit 100000
create_project -in_memory -part xc7a100tcsg324-1

set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
set_property webtalk.parent_dir c:/users/emily/documents/document/2015-2016/ece532/project/accel/managed_ip_project/managed_ip_project.tmp/accel_v1_0_v1_0_project/ACCEL_v1_0_v1_0_project.cache/wt [current_project]
set_property parent.project_path c:/users/emily/documents/document/2015-2016/ece532/project/accel/managed_ip_project/managed_ip_project.tmp/accel_v1_0_v1_0_project/ACCEL_v1_0_v1_0_project.xpr [current_project]
set_property default_lib xil_defaultlib [current_project]
set_property target_language Verilog [current_project]
set_property ip_repo_paths c:/Users/Emily/Documents/Document/2015-2016/ECE532/Project/ACCEL/ip_repo/ACCEL_1.0 [current_project]
read_verilog -library xil_defaultlib {
  c:/users/emily/documents/document/2015-2016/ece532/project/accel/ip_repo/accel_1.0/hdl/ACCEL_v1_0_ACCEL_AXI.v
  c:/users/emily/documents/document/2015-2016/ece532/project/accel/ip_repo/accel_1.0/hdl/ACCEL_v1_0.v
}
read_vhdl -library xil_defaultlib {
  c:/users/emily/documents/document/2015-2016/ece532/project/accel/ip_repo/accel_1.0/hdl/SPI_If.vhd
  c:/users/emily/documents/document/2015-2016/ece532/project/accel/ip_repo/accel_1.0/hdl/ADXL362Ctrl.vhd
  c:/users/emily/documents/document/2015-2016/ece532/project/accel/ip_repo/accel_1.0/hdl/AccelArithmetics.vhd
  c:/users/emily/documents/document/2015-2016/ece532/project/accel/ip_repo/accel_1.0/hdl/AccelerometerCtl.vhd
}
synth_design -top ACCEL_v1_0 -part xc7a100tcsg324-1
write_checkpoint -noxdef ACCEL_v1_0.dcp
catch { report_utilization -file ACCEL_v1_0_utilization_synth.rpt -pb ACCEL_v1_0_utilization_synth.pb }
