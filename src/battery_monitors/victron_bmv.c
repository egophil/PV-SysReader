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

#include "victron_bmv.h"
#include "../rs232/rs232.h"

void bmv_init(char* interface)
{
  char* buffer=NULL;
  buffer=calloc(BUFFERSIZE_BMV, 1);

  bmv_read(buffer, interface);
  rs232_close();  
  if (buffer!=NULL) buffer=NULL;
  
}


void bmv_init_raw()
{
  char* buffer=NULL;
  buffer=calloc(BUFFERSIZE_BMV, 1);
  int idx=0;
  u_int16_t bytes_read = 0;
  
  set_interface_attr(B19200, 0);
  sleep(RS232_TIMEOUT_BMV);
  bytes_read=rs232_read(buffer, BUFFERSIZE_BMV, RS232_TIMEOUT_BMV, 0);
  rs232_close();
  printf("bytes read: %i\n", bytes_read);
  for (idx=0; idx<BUFFERSIZE_BMV;idx++)
  {
    printf("0x%02x ", buffer[idx]);
  }
  printf("\n");
  if (buffer!=NULL) buffer=NULL;
  
}

//checksum calculation is realy tricky
//1st check if enough bytes where read
//2st bring the buffer in the right block order
//a block is not specified in the victron document
//so it can by different from device to device
//3rd calculate the checksum and if okay return 0
//if a checksum error occurs do a retry, until retry limit is reached
u_int8_t bmv_read(char* buffer, char* interface)
{
  u_int8_t retry_cnt = 0;
  struct bmv_elements bmv_stat;
  memset(&bmv_stat,0, sizeof(bmv_stat));
  u_int16_t bytes_read = 0;
  
  while (retry_cnt < RET_LIMIT)
  {    
    set_interface_attr(B19200, 0);
    sleep(RS232_TIMEOUT_BMV); //wait to fill up the buffer from the usb rs232 interface
    bytes_read = rs232_read(buffer, BUFFERSIZE_BMV, RS232_TIMEOUT_BMV, 0);
    //printf("bytes read: %i\n", bytes_read);
    if (bytes_read>0)
    {
      if(bmv_checksum(buffer)==0)
      {
	printf("Checksum: OK\n");
	bmv_buffer_extract(buffer, &bmv_stat);
        bmv_print_stat(&bmv_stat);
	return 0;
	break;
      }
      else
      {
	retry_cnt++;
	//printf("Retry counter: %i\n",retry_cnt);
	rs232_close();
        sleep(0.3);
        rs232_open(interface);
      }
    }
    else
    {
      //printf("0 Bytes read\n");
      retry_cnt++;
      rs232_close();
      sleep(0.3);
      rs232_open(interface);
    }
  }
  
  if(retry_cnt>=RET_LIMIT)
  {
    printf("Checksum: Error\n");
    rs232_close();
    return 1;
  }
  else
  {
    rs232_close();
    return 0;
  }
  
}
u_int8_t bmv_checksum(char* buffer)
{
  int idx=0;
  u_int8_t bmv_model = 0;
  u_int16_t size = BUFFERSIZE_BMV;
  u_int8_t model_found=0;
  int cnt=0;
  u_int16_t calc=0;
  u_int8_t start_zero_out=0;
  
  //need to clean the buffer, so that we have only block 1 and block 2
  for(idx=0;idx<size;idx++)
  {
    //get hex values of Checksum and \t
    ////0x43 0x68 0x65 0x63 0x6b 0x73 0x75 0x6d 0x09
    if (start_zero_out==2) //zero the buffer after the 2nd Checksum \t
    {
      buffer[idx]=0x00;
      //printf("Zero out buffer at index %i\n", idx);
    }
    if ( buffer[idx-9]==0x43 && buffer[idx-8]==0x68 && buffer[idx-7]==0x65 
      && buffer[idx-6]==0x63 && buffer[idx-5]==0x6b && buffer[idx-4]==0x73 
      && buffer[idx-3]==0x75 && buffer[idx-2]==0x6d && buffer[idx-1]==0x09)
    {
      start_zero_out++;
    }
    
  }
  
  for(idx=0;idx<size;idx++)
  {
    if (buffer[idx-4]==0x42 && buffer[idx-3]==0x4d && buffer[idx-2]==0x56) //look for String "BMV"
    {
      model_found=1;
      bmv_model = (u_int8_t)buffer[idx];
      if (bmv_model==55) //if BMV 70x series -> calc checksum
      {
	buffer_shift_left(buffer, &size, &bmv_model);
	for(cnt=0; cnt<size; cnt++)
	{
	  calc=calc+(u_int8_t)(buffer[cnt]);
	}
	calc = calc % 256;
	if (calc==0)
	{
	  return 0;
	}
	else
	{
	  //printf("Checksum: Error - bmv function\n");
	  return 1;
	}
	
      }
      else if(bmv_model==54)
      {
	printf("Checksum: BMV 60x not supported\n");
	return 0;
      }
      else{
	printf("Checksum: Model not supported\n");
	return 0;
      }
    }//if end
  }//for end
  if(model_found==0)
  {
     //printf("Checksum: Error"); // value of BMV model not found
    return 1;
  }
  
}

u_int8_t buffer_shift_left(char* buffer, u_int16_t* size, u_int8_t* bmv_model)
{
  int n=0;
  int idx = *size-1;
  int cnt=0;
  int i= 0;
  char temp[1];
  
  //shifting buffer to start with: 0x0d 0x0a 0x50 0x49 0x44 (LF, PID)
  //for BMV 70x
  if (*bmv_model==7)
  {
    while(cnt<idx)
    {
      if(buffer[cnt]==0x50 && buffer[cnt+1]==0x49 && buffer[cnt+2]==0x44)
      {
        cnt = cnt-2; //need to catch 0x0d 0x0d (Line Feed)
        break;
      }
      cnt++;
    }
    
    for (i = 0; i < cnt; i++)
    {
      temp[0] = buffer[0];
      for (n=0; n<idx; n++)
      {
        buffer[n] = buffer[n+1];  
      }
      buffer[idx]=temp[0];
    }
  }
  return 0;
  
}

void bmv_print_stat(struct bmv_elements* elements)
{
  printf("S00: main battery voltage in mV: %i\n", elements->main_battery_voltage);
  printf("S01: auxiliary starter voltage in mV: %i\n",elements->auxiliary_starter_voltage);
  printf("S02: mid point voltage battery bank in mV: %i\n",elements->mid_point_voltage_battery_bank);
  printf("S03: mid point deviation battery bank: %i\n",elements->mid_point_deviation_battery_bank);
  printf("S04: charging current in mA: %i\n",elements->current);
  printf("S05: battery temperature in °C: %i\n",elements->battery_temperature);
  printf("S06: instantaneous power in W: %i\n",elements->instantaneous_power);
  printf("S07: consumed amp hours in mAh: %i\n",elements->consumed_amp_hours);
  printf("S08: state of charge(SoC) in %: %i\n",elements->state_of_charge);
  printf("S09: time to go in minutes: %i\n",elements->time_to_go);
  printf("S10: alarm condition active: %s\n",elements->alarm_condition_active);
  printf("S11: relay state: %s\n",elements->relay_state);
  printf("S12: alarm reason: %i\n", elements->alarm_reason);
  printf("S13: depth deepest discharge in mAh: %i\n",elements->depth_deepest_discharge);
  printf("S14: depth last discharge in mAh: %i\n",elements->depth_last_discharge);
  printf("S15: depth average discharge in mAh: %i\n",elements->depth_average_discharge);
  printf("S16: number charge cycles: %i\n",elements->number_charge_cycles);
  printf("S17: number full discharges: %i\n",elements->number_full_discharges);
  printf("S18: cumulative amp hours in mAh: %i\n",elements->cumulative_amp_hours);
  printf("S19: minimum main battery voltage in mV: %i\n",elements->minimum_main_battery_voltage);
  printf("S20: maximum main battery voltage in mV: %i\n",elements->maximum_main_battery_voltage);
  printf("S21: number seconds last full charge: %i\n",elements->number_seconds_last_full_charge);
  printf("S22: number automatic synchronizations: %i\n",elements->number_automatic_synchronizations);
  printf("S23: number low main voltage alarms: %i\n",elements->number_low_main_voltage_alarms);
  printf("S24: number high main voltage alarms: %i\n",elements->number_high_main_voltage_alarms);
  printf("S25: number low auxiliary voltage alarms: %i\n",elements->number_low_auxiliary_voltage_alarms);
  printf("S26: number high auxiliary voltage alarms: %i\n",elements->number_high_auxiliary_voltage_alarms);
  printf("S27: minimum auxiliary battery voltage in mV: %i\n",elements->minimum_auxiliary_battery_voltage);
  printf("S28: maximum auxiliary battery voltage in mV: %i\n",elements->maximum_auxiliary_battery_voltage);
  printf("S29: amount discharged energy in kWh: %i\n",elements->amount_discharged_energy);
  printf("S30: amount charged energy in kWh: %i\n",elements->amount_charged_energy);
  printf("S31: model description: %i\n",elements->model_description);
  printf("S32: firmware version: %i\n",elements->firmware_version);
  printf("S33: procduct id: %i \n",elements->product_id); 

}

u_int8_t bmv_buffer_extract(char* buffer, struct bmv_elements* elements)
{
  char* ptr=NULL;
  int ret=0;
  char limiter[] = "\n\t";
  
  ptr = strtok(buffer, limiter);
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
    else if(strcmp(ptr, "VS")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->auxiliary_starter_voltage=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of auxiliary starter voltage missing!\n");
      }
    }
    else if(strcmp(ptr, "VM")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->mid_point_voltage_battery_bank=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of mid point voltage battery bank missing!\n");
      }
    }
    else if(strcmp(ptr, "DM")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->mid_point_deviation_battery_bank=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of mid point deviation battery bank missing!\n");
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
    else if(strcmp(ptr, "T")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->battery_temperature=(int)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of battery temperature missing!\n");
      }
    }
    else if(strcmp(ptr, "P")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->instantaneous_power=(int)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of instantaneous power missing!\n");
      }
    }
    else if(strcmp(ptr, "CE")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->consumed_amp_hours=(int)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of consumed amp hours missing!\n");
      }
    }
    else if(strcmp(ptr, "SOC")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->state_of_charge=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of state of charge missing!\n");
      }
    }
    else if(strcmp(ptr, "TTG")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->time_to_go=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of time to go missing!\n");
      }
    }
    else if(strcmp(ptr, "Alarm")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        strncpy(elements->alarm_condition_active,ptr,3);
      }
      else
      {
        printf("value of alarm condition active missing!\n");
      }
    }
    else if(strcmp(ptr, "Relay")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        strncpy(elements->relay_state,ptr,3);
      }
      else
      {
        printf("value of relay state missing!\n");
      }
    }
    else if(strcmp(ptr, "AR")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->alarm_reason=(u_int8_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of relay state missing!\n");
      }
    }
    else if(strcmp(ptr, "H1")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->depth_deepest_discharge=(int)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of depth deepest discharge missing!\n");
      }
    }
    else if(strcmp(ptr, "H2")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->depth_last_discharge=(int)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of depth last discharge missing!\n");
      }
    }
    else if(strcmp(ptr, "H3")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->depth_average_discharge=(int)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of depth average discharge missing!\n");
      }
    }
    else if(strcmp(ptr, "H4")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_charge_cycles=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of number charge cycles missing!\n");
      }
    }
    else if(strcmp(ptr, "H5")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_full_discharges=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of number full discharges missing!\n");
      }
    }
    else if(strcmp(ptr, "H6")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->cumulative_amp_hours=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of cumulative amp hours missing!\n");
      }
    }
    else if(strcmp(ptr, "H7")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->minimum_main_battery_voltage=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of minimum main battery voltage missing!\n");
      }
    }
    else if(strcmp(ptr, "H8")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->maximum_main_battery_voltage=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of maximum main battery voltage missing!\n");
      }
    }
    else if(strcmp(ptr, "H9")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_seconds_last_full_charge=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of number seconds last full charge missing!\n");
      }
    }
    else if(strcmp(ptr, "H10")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_automatic_synchronizations=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of automatic synchronizations missing!\n");
      }
    }
    else if(strcmp(ptr, "H11")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_low_main_voltage_alarms=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of number low main voltage alarms missing!\n");
      }
    }
    else if(strcmp(ptr, "H12")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_high_main_voltage_alarms=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of number high main voltage alarms missing!\n");
      }
    }
    else if(strcmp(ptr, "H13")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_low_auxiliary_voltage_alarms=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of number low auxiliary voltage alarms missing!\n");
      }
    }
    else if(strcmp(ptr, "H14")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->number_high_auxiliary_voltage_alarms=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of number high auxiliary voltage alarms missing!\n");
      }
    }
    else if(strcmp(ptr, "H15")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->minimum_auxiliary_battery_voltage=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of minimum auxiliary battery voltage missing!\n");
      }
    }
    else if(strcmp(ptr, "H16")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->maximum_auxiliary_battery_voltage=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of maximum auxiliary battery voltage missing!\n");
      }
    }
    else if(strcmp(ptr, "H17")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->amount_discharged_energy=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of amount discharged energy missing!\n");
      }
    }
    else if(strcmp(ptr, "H18")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->amount_charged_energy=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of amount charged energy missing!\n");
      }
    }
    else if(strcmp(ptr, "BMV")==0)
    {
      ptr = strtok(NULL, limiter);
      if (ptr!=NULL)  
      {
        elements->model_description=(u_int16_t)strtol(ptr, NULL, 10);
      }
      else
      {
        printf("value of model description missing!\n");
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
  
    ptr = strtok(NULL, limiter);
  }
  if (ptr!=NULL) ptr=NULL;
  return ret;
  
}