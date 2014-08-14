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

#ifndef _MPPT_HANDLER_H
#define _MPPT_HANDLER_H

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

  #define MPPT_BUFFERSIZE 250
  #define MPPT_RS232_TIMEOUT 1  // in seconds
  
  struct mppt_elements 
  {
    u_int16_t main_battery_voltage;
    u_int16_t panel_voltage;
    u_int16_t panel_power;
    int       current;
    u_int16_t yield_total;
    u_int16_t yield_today;
    u_int16_t maximum_power_today;
    u_int16_t yield_yeterday;
    u_int16_t maximum_power_yesterday;
    u_int16_t error_code;
    u_int16_t convert_state;
    u_int16_t firmware_version;
    u_int16_t product_id;
    u_int16_t serial_number;
  };

  void mppt_init(void);
  u_int8_t mppt_read(char* buffer);
  void mppt_init_raw(void);
  void mppt_print_stat(struct mppt_elements* elements);
  u_int8_t mppt_buffer_extract(char* mppt_buffer, struct mppt_elements* elements);

#endif