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

#ifndef _BS_INVERTER_H
#define _BS_INVERTER_H

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

  #define BUFFERSIZE_BS_INVERTER 100
  #define RS232_TIMEOUT_BS_INVERTER 1
  
  #define MSG1_FRAME_ID 260
  #define MSG2_FRAME_ID 260
  #define MSG3_FRAME_ID 260
  #define MSG4_FRAME_ID 259
  #define MSG5_FRAME_ID 259
  #define MSG6_FRAME_ID 259
  #define MSG7_FRAME_ID 259
  #define MSG8_FRAME_ID 259
  #define MSG9_FRAME_ID 259
  #define MSG10_FRAME_ID 259
  #define MSG11_FRAME_ID 259
  #define MSG12_FRAME_ID 259
  #define MSG13_FRAME_ID 259
  #define MSG14_FRAME_ID 259
  
  #define MSG1  { 0x01, 0x04, 0x00, 0x14, 0x00, 0x11, 0x70, 0x02 }
  #define MSG2  { 0x01, 0x04, 0x00, 0x00, 0x00, 0x14, 0xF0, 0x05 }
  #define MSG3  { 0x01, 0x04, 0x03, 0xE8, 0x00, 0x14, 0x70, 0x75 }
  #define MSG4  { 0x01, 0x03, 0x00, 0x00, 0x00, 0x0C, 0x45, 0xCF }
  #define MSG5  { 0x01, 0x03, 0x00, 0x64, 0x00, 0x09, 0xC4, 0x13 }
  #define MSG6  { 0x01, 0x03, 0x00, 0xC8, 0x00, 0x12, 0x44, 0x39 }
  #define MSG7  { 0x01, 0x03, 0x00, 0xDC, 0x00, 0x02, 0x05, 0xF1 } //3K Model does not contain any useable data
  #define MSG8  { 0x01, 0x03, 0x00, 0xDC, 0x00, 0x02, 0x05, 0xF1 } //3K Model does not contain any useable data
  #define MSG9  { 0x01, 0x03, 0x01, 0x90, 0x00, 0x14, 0x44, 0x14 }
  #define MSG10 { 0x01, 0x03, 0x03, 0xE8, 0x00, 0x14, 0xC5, 0xB5 }
  #define MSG11 { 0x01, 0x03, 0x03, 0xE8, 0x00, 0x14, 0xC5, 0xB5 }
  #define MSG12 { 0x01, 0x03, 0x04, 0x24, 0x00, 0x05, 0xC4, 0xF2 }
  #define MSG13 { 0x01, 0x03, 0x04, 0x4C, 0x00, 0x14, 0x85, 0x22 } //reply contains data, TODO decoding; rigth now no glue
  #define MSG14 { 0x01, 0x03, 0x01, 0xF5, 0x00, 0x06, 0xD4, 0x06 }

  struct bs_inverter_elements
  {
    u_int32_t kw_total;
    u_int16_t runtime_min;
    u_int32_t total_runtime_h;
    u_int16_t temperature;
    u_int16_t a_cpu_bus_u;
    u_int16_t iso_samples;
    u_int16_t pv_dc_voltage;
    u_int16_t ac_voltage;
    u_int16_t ac_current;
    u_int16_t frequency_ac;
    u_int16_t pv_watt;
    u_int16_t kw_day;
    
    u_int16_t b_cpu_ac_u;
    u_int16_t voltage_pv_start;
    u_int16_t t_starts;
    u_int16_t code;
   
    u_int16_t v_ac_low_voltage;
    u_int16_t v_ac_low_voltage_time;
    u_int16_t v_ac_ultra_low_voltage;
    u_int16_t v_ac_ultra_low_voltage_time;
    
    u_int16_t v_ac_high_voltage;
    u_int16_t v_ac_high_voltage_time;
    u_int16_t v_ac_ultra_high_voltage;
    u_int16_t v_ac_ultra_high_voltage_time;
    
    u_int16_t low_frequency;
    u_int16_t low_frequency_time;
    u_int16_t ultra_low_frequency;
    u_int16_t ultra_low_frequency_time;
    
    u_int16_t high_frequency;
    u_int16_t high_frequency_time;
    u_int16_t ultra_high_frequency;
    u_int16_t ultra_high_frequency_time;
    
    u_int16_t dci1_in_ma;
    u_int16_t dci_limit;
    u_int16_t dci_shift;
    u_int16_t full_power_limit;
    u_int16_t active_power_reduction_rate;
    u_int16_t kpv_gain;
    u_int16_t kac_gain;
    u_int16_t ka_bus_gain;
    u_int16_t kb_cpu_ac_gain;
    
    u_int16_t fac_high_spi;
    u_int16_t fac_high_spi_time;
    u_int16_t fac_low_spi;
    u_int16_t fac_low_spi_time;
    
    u_int16_t connecting_fac_high;
    u_int16_t connecting_fac_low;
    
    char serial_number[13];
    char firmware[5];
    u_int8_t  modbus_addr;

  };
  
  void bs_inverter_init(u_int8_t bs_mode);
  u_int8_t bs_inverter_write(char* msg, int* size);
  u_int8_t bs_inverter_read(char* buffer);
  u_int8_t bs_inverter_get_stats(char* buffer, struct bs_inverter_elements* bs_inverter_stat);
  u_int8_t bs_inverter_get_param(char* buffer, struct bs_inverter_elements* bs_inverter_stat);
  void bs_inverter_print_stats(struct bs_inverter_elements* bs_inverter_stat);
  void bs_inverter_print_param(struct bs_inverter_elements* bs_inverter_stat);
  void bs_raw_mode(void);
  void bs_raw_read(char* request, int* size);

#endif