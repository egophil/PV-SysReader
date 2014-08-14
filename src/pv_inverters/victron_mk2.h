// This file is part of PV-SysReader.
// 
// PV-SysReader is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// PV-SysReader is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with PV-SysReader.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _mk2_2_H
#define _mk2_2_H
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  #define BUFFERSIZE_MK2 255
  #define RS232_TIMEOUT_MK2 1
  #define MK2_FW_SUPPORTED 1130128
  #define LIMIT_GET_MK2_VERSION 5
  #define REQUEST01 {0x02, 0xff, 0x4c, 0xb3 } //request to get LED Status
  #define REQUEST02 {0x03, 0xff, 0x46, 0x00 , 0xb8 } //request to get InfoFrame0
  #define REQUEST03 {0x03, 0xff, 0x46, 0x01 , 0xb7 } //request to get InfoFrame1
  #define REQUEST04 {0x03, 0xff, 0x46, 0x02 , 0xb6 } //request to get InfoFrame2
  #define REQUEST05 {0x03, 0xff, 0x46, 0x03 , 0xb5 } //request to get InfoFrame3
  #define REQUEST06 {0x03, 0xff, 0x46, 0x04 , 0xb4 } //request to get InfoFrame4
  #define REQUEST07 {0x03, 0xff, 0x46, 0x05 , 0xb3 } //request to get InfoFrame5
  
  struct multiplus_elements 
  {
    u_int32_t mk2_version;
    char mk2_mode;
    u_int8_t LED_on_mains;
    u_int8_t LED_on_absorption;
    u_int8_t LED_on_bulk;
    u_int8_t LED_on_float;
    u_int8_t LED_on_inverter;
    u_int8_t LED_on_overload;
    u_int8_t LED_on_low_battery;
    u_int8_t LED_on_temperature;
    u_int8_t LED_blink_mains;
    u_int8_t LED_blink_absorption;
    u_int8_t LED_blink_bulk;
    u_int8_t LED_blink_float;
    u_int8_t LED_blink_inverter;
    u_int8_t LED_blink_overload;
    u_int8_t LED_blink_low_battery;
    u_int8_t LED_blink_temperature;
    
    //if0=InfoFrame0
    u_int8_t if0_phase_info;
    u_int16_t if0_DC_Voltage;
    u_int32_t if0_DC_Current_Inv;
    u_int32_t if0_DC_Current_Chg;
    u_int32_t if0_Inv_Periode;
    //if1=InfoFrame1
    u_int8_t if1_L1_bf_factor;
    u_int8_t if1_L1_inverter_factor;
    u_int8_t if1_L1_phase_info;
    u_int16_t if1_L1_mains_voltage;
    int16_t if1_L1_mains_current;
    u_int16_t if1_L1_inverter_voltage;
    int16_t if1_L1_inverter_current;
    u_int32_t if1_L1_mains_periode;
    //if2=InfoFrame2    
    u_int8_t if2_L2_bf_factor;
    u_int8_t if2_L2_inverter_factor;
    u_int8_t if2_L2_phase_info;
    u_int16_t if2_L2_mains_voltage;
    int16_t if2_L2_mains_current;
    u_int16_t if2_L2_inverter_voltage;
    int16_t if2_L2_inverter_current;
    u_int32_t if2_L2_mains_periode;
    //if3=InfoFrame3    
    u_int8_t if3_L3_bf_factor;
    u_int8_t if3_L3_inverter_factor;
    u_int8_t if3_L3_phase_info;
    u_int16_t if3_L3_mains_voltage;
    int16_t if3_L3_mains_current;
    u_int16_t if3_L3_inverter_voltage;
    int16_t if3_L3_inverter_current;
    u_int32_t if3_L3_mains_periode;
    //if4=InfoFrame4    
    u_int8_t if4_L4_bf_factor;
    u_int8_t if4_L4_inverter_factor;
    u_int8_t if4_L4_phase_info;
    u_int16_t if4_L4_mains_voltage;
    int16_t if4_L4_mains_current;
    u_int16_t if4_L4_inverter_voltage;
    int16_t if4_L4_inverter_current;
    u_int32_t if4_L4_mains_periode;
    //mplf=MultiPlusLED frame
    u_int8_t mplf_ac_input_conf;
    u_int16_t mplf_min_inp_cur_lmt;
    u_int16_t mplf_max_inp_cur_lmt;
    u_int16_t mplf_actual_inp_cur_lmt;
  };
  
  void mk2_init(void);
  u_int8_t mk2_read(char* buffer);
  void mk2_raw_read(void);
  void mk2_raw_request(char* request, int *size);
  u_int8_t mk2_calc_checksum(char* buffer);
  u_int32_t get_mk2_version(char* buffer, struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_led_status(struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_infoframe_0(struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_infoframe_1(struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_infoframe_2(struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_infoframe_3(struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_infoframe_4(struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_infoframe_5(struct multiplus_elements* multiplus_stat);
  u_int8_t pars_multiplus_master_multi_led_frame(struct multiplus_elements* multiplus_stat);
  u_int8_t print_multiplus_stat(struct multiplus_elements* multiplus_stat);
  u_int8_t print_multiplus_L1(struct multiplus_elements* multiplus_stat);
  u_int8_t print_multiplus_L2(struct multiplus_elements* multiplus_stat);
  u_int8_t print_multiplus_L3(struct multiplus_elements* multiplus_stat);
  u_int8_t print_multiplus_L4(struct multiplus_elements* multiplus_stat);
    
  u_int8_t remove_mk2_version_frame(char* buffer, u_int16_t size);
  u_int8_t mk2_write(char* msg, int* size);
  u_int8_t get_multiplus_stats (struct multiplus_elements* multiplus_stat);
  
#endif