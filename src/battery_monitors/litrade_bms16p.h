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
  
  #define BUFFERSIZE_BMS16P 4096
  #define RS232_TIMEOUT_BMS16P 10 //in seconds
  #define BMS16P_RET_LIMIT 3
  #define BMS16P_NUM_CELLS 16
  
  struct bms16p_elements 
  {
    u_int32_t cell[BMS16P_NUM_CELLS];
  };
  
  void bms16p_init(char* interface);
  u_int8_t bms16p_read(char* buffer, char* interface);
  u_int8_t bms16p_buffer_extract(char* buffer, struct bms16p_elements* elements);
  void bms16p_print_stat(struct bms16p_elements* elements);
  void bms16p_raw_init(void);

  
#endif