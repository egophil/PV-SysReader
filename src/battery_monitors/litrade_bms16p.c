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

#include "litrade_bms16p.h"
#include "../rs232/rs232.h"

void bms16p_init(char* interface)
{
  char* buffer=NULL;
  buffer=calloc(BUFFERSIZE_BMS16P, 1);

  bms16p_read(buffer, interface);
  rs232_close();  
  if (buffer!=NULL) buffer=NULL;
  
}

void bms16p_raw_init()
{
  char* buffer=NULL;
  buffer=calloc(BUFFERSIZE_BMS16P, 1);
  int idx=0;
  u_int16_t bytes_read = 0;
  
  set_interface_attr(B9600, 0);
  sleep(RS232_TIMEOUT_BMS16P);
  bytes_read=rs232_read(buffer, BUFFERSIZE_BMS16P, RS232_TIMEOUT_BMS16P, 0);
  rs232_close();
  printf("bytes read: %i\n", bytes_read);
  for (idx=0; idx<BUFFERSIZE_BMS16P;idx++)
  {
    printf("0x%02x ", buffer[idx]);
  }
  printf("\n");
  if (buffer!=NULL) buffer=NULL;
  
}

u_int8_t bms16p_read(char* buffer, char* interface)
{
  u_int8_t retry_cnt = 0;
  struct bms16p_elements bms16p_stat;
  memset(&bms16p_stat,0, sizeof(bms16p_stat));
  u_int16_t bytes_read = 0;
  int idx=0;
  
  while (retry_cnt < BMS16P_RET_LIMIT)
  {    
    set_interface_attr(B9600, 0);
    sleep(RS232_TIMEOUT_BMS16P); //wait to fill up the buffer from the usb interface
    bytes_read = rs232_read(buffer, BUFFERSIZE_BMS16P, RS232_TIMEOUT_BMS16P, 0);
    //printf("bms16p bytes read: %i\n", bytes_read);
    if (bytes_read>10)
    {
      bms16p_buffer_extract(buffer, BUFFERSIZE_BMS16P ,&bms16p_stat);
      bms16p_print_stat(&bms16p_stat);
      return 0;
      break;
      
    }
    else
    {
      retry_cnt++;
      rs232_close();
      sleep(0.3);
      rs232_open(interface);
      
    }
    
  }
  
  if(retry_cnt>=BMS16P_RET_LIMIT)
  {
    printf("Error, received no data\n");
    return 1;
  }
  else
  {
    return 0;
  }
  
}


u_int8_t bms16p_buffer_extract(char* buffer, u_int16_t buffer_size, struct bms16p_elements* elements)
{
  u_int16_t idx, offset = 0;
  char* ptr=NULL;
  char limiter[] =";";
  
  for(idx=0;idx<buffer_size;idx++)
  {
    //get hex values of "end"
    //0x65 0x6e 0x64
    //extract block between two ends, all other values will be set to 0x00
    if (buffer[idx]==0x65 && buffer[idx+1]==0x6e && buffer[idx+2]==0x64) 
    {
	offset = idx+3;
	//well done, begin of a block found
	break;  
	
    }
    else
    {
      // no end found, keep searching
      
    };
    
  }
    
  idx = 0;
  ptr = strtok(buffer+offset, limiter);
  while(ptr!=NULL)
  {
    if (idx < BMS16P_NUM_CELLS)
    {
      elements->cell[idx]=(strtod(ptr, NULL)*100000);
      ptr = strtok(NULL, limiter);
      idx++;
      
    }
    else
    {
      break;
      
    }
    
  }
  
  return 0;
  
}


void bms16p_print_stat(struct bms16p_elements* elements)
{
  u_int8_t idx = 0;
  
  for (idx=0;idx < BMS16P_NUM_CELLS; idx++)
  {
    //printf("B%i: Voltage of Cell_%i in mV: %i\n", idx, idx , elements->cell[idx]);
    printf("B%i: %i\n", idx, elements->cell[idx]);
    
  }
}