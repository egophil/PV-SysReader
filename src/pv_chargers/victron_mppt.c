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

#include "victron_mppt.h"
#include "../rs232/rs232.h"

void mppt_init()
{
  char* buffer=NULL;
  buffer=calloc(MPPT_BUFFERSIZE, 1);
  
  mppt_read(buffer);
  rs232_close();
  
  if (buffer!=NULL) buffer=NULL;
  
}


void mppt_init_raw()
{
  char* buffer=NULL;
  buffer=calloc(MPPT_BUFFERSIZE, 1);
  int idx=0;
  
  set_interface_attr(B19200, 0);
  sleep(MPPT_RS232_TIMEOUT);
  rs232_read(buffer, MPPT_BUFFERSIZE, MPPT_RS232_TIMEOUT, 0);
  rs232_close();
  for (idx=0; idx<MPPT_BUFFERSIZE;idx++)
  {
    printf("0x%02x ", buffer[idx]);
  }
  printf("\n");
  if (buffer!=NULL) buffer=NULL;
  
}  


u_int8_t mppt_read(char* buffer)
{
  struct mppt_elements mppt_stat;
  memset(&mppt_stat,0, sizeof(mppt_stat));
  
  set_interface_attr(B19200, 0);
  sleep(MPPT_RS232_TIMEOUT);
  rs232_read(buffer, MPPT_BUFFERSIZE, MPPT_RS232_TIMEOUT, 0);
  mppt_buffer_extract(buffer, &mppt_stat);
  
  mppt_print_stat(&mppt_stat);
  
  return 0;
  
}


void mppt_print_stat(struct mppt_elements* elements)
{
  printf("S00: main battery voltage in mV: %i\n", elements->main_battery_voltage);
  printf("S01: panel voltage in mV: %i\n",elements->panel_voltage);
  printf("S02: panel power in W: %i\n",elements->panel_power);
  printf("S03: charging current in mA: %i\n",elements->current);
  printf("S04: yield total: %i\n",elements->yield_total);
  printf("S05: yield today: %i\n",elements->yield_today);
  printf("S06: maximum power today in W: %i\n",elements->maximum_power_today);
  printf("S07: yield yesterday in 0.01 kWh: %i\n",elements->yield_yeterday);
  printf("S08: maximum power yesterday in W: %i\n",elements->maximum_power_yesterday);
  printf("S09: error code: %i\n",elements->error_code);
  printf("S10: converter state: %i\n",elements->convert_state);
  printf("S11: firmware version: %i\n",elements->firmware_version);
  printf("S12: procduct id: %i \n",elements->product_id);
  printf("S13: serial Number: %i\n",elements->serial_number);
  
}


u_int8_t mppt_buffer_extract(char* mppt_buffer, struct mppt_elements* elements)
{
  char* ptr=NULL;
  int ret=0;
  char limiter[] = "\n\t";
  
  ptr = strtok(mppt_buffer, limiter);
  while(ptr!=NULL)
  {
    if(strcmp(ptr, "V")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->main_battery_voltage=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of main battery voltage missing!\n");
      }
    }
    else if(strcmp(ptr, "VPV")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->panel_voltage=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of panel voltage missing!\n");
      }
    }
    else if(strcmp(ptr, "PPV")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->panel_power=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of panel power missing!\n");
      }
    }
    else if(strcmp(ptr, "I")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->current=(int)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of charging current missing!\n");
      }
    }
    else if(strcmp(ptr, "H19")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->yield_total=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of yield total missing!\n");
      }
    }
    else if(strcmp(ptr, "H20")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->yield_yeterday=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of yield yesterday missing!\n");
      }
    }
    else if(strcmp(ptr, "H21")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->maximum_power_today=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of maximum power today missing!\n");
      }
    }
    else if(strcmp(ptr, "H22")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->yield_yeterday=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of yield yesterday missing!\n");
      }
    }
    else if(strcmp(ptr, "H23")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->maximum_power_yesterday=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of maximum power yesterday missing!\n");
      }
    }
    else if(strcmp(ptr, "ERR")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->error_code=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of error code missing!\n");
      }
    }
    else if(strcmp(ptr, "CS")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->convert_state=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of converter state missing!\n");
      }
    }
    else if(strcmp(ptr, "FW")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->firmware_version=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of firmware version missing!\n");
      }
    }
    else if(strcmp(ptr, "PID")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL) 
      {
        elements->product_id=(u_int16_t)strtol(ptr, NULL, 16);
      }
      else
      {
        printf("value of product id missing!\n");
      }
    }
    else if(strcmp(ptr, "SER#")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL) 
      {
        elements->serial_number=(u_int16_t)strtol(ptr, NULL, 16);
      }
      else
      {
        printf("value of serial number missing!\n");
      }
    }
    
    ptr = strtok(NULL, limiter);
  }
  if (ptr!=NULL) ptr=NULL;
  return ret;

}