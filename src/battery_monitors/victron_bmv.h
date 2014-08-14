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

#ifndef _BMV_HANDLER_H
#define _BMV_HANDLER_H

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

  #define BUFFERSIZE_BMV 300
  #define RS232_TIMEOUT_BMV 2 //in seconds
  #define RET_LIMIT 3 
  
  struct bmv_elements 
  {
    u_int16_t main_battery_voltage;
    u_int16_t auxiliary_starter_voltage;
    u_int16_t mid_point_voltage_battery_bank;
    u_int16_t mid_point_deviation_battery_bank;
    int       current;
    int       battery_temperature;
    int       instantaneous_power;
    int       consumed_amp_hours;
    u_int16_t state_of_charge;
    u_int16_t time_to_go;
    char      alarm_condition_active[4];
    char      relay_state[4];
    u_int8_t  alarm_reason;
    int       depth_deepest_discharge;
    int       depth_last_discharge;
    int       depth_average_discharge;
    u_int16_t number_charge_cycles;
    u_int16_t number_full_discharges;
    int       cumulative_amp_hours;
    u_int16_t minimum_main_battery_voltage;
    u_int16_t maximum_main_battery_voltage;
    u_int16_t number_seconds_last_full_charge;
    u_int16_t number_automatic_synchronizations;
    u_int16_t number_low_main_voltage_alarms;
    u_int16_t number_high_main_voltage_alarms;
    u_int16_t number_low_auxiliary_voltage_alarms;
    u_int16_t number_high_auxiliary_voltage_alarms;
    u_int16_t minimum_auxiliary_battery_voltage;
    u_int16_t maximum_auxiliary_battery_voltage;
    u_int16_t amount_discharged_energy;
    u_int16_t amount_charged_energy;
    u_int16_t model_description;
    u_int16_t firmware_version;
    u_int16_t product_id;
  
  };

  void bmv_init(char* interface);
  void bmv_init_raw(void);
  u_int8_t bmv_read(char* buffer, char* interface);
  void bmv_print_stat(struct bmv_elements* elements);
  u_int8_t bmv_checksum(char* buffer);
  u_int8_t buffer_shift_left(char* buffer, u_int16_t* size, u_int8_t* bmv_model);
  u_int8_t bmv_buffer_extract(char* buffer, struct bmv_elements* elements);

#endif