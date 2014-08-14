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

#include "victron_mk2.h"
#include "../rs232/rs232.h"


void mk2_init()
{
  struct multiplus_elements multiplus_stat;
  memset(&multiplus_stat,0, sizeof(multiplus_stat));
  
  get_multiplus_stats(&multiplus_stat);
  print_multiplus_stat(&multiplus_stat);

  rs232_close();
  
}

u_int8_t print_multiplus_stat(struct multiplus_elements* multiplus_stat)
{
  printf("M00: MK2 firmware: %i\n",    multiplus_stat->mk2_version);
  printf("M01: MK2 modus: 0x%02x\n",   multiplus_stat->mk2_mode);
  printf("M02: LED mains: %i\n",       multiplus_stat->LED_on_mains);
  printf("M03: LED absoprtion: %i\n",  multiplus_stat->LED_on_absorption);
  printf("M04: LED bulk: %i\n",        multiplus_stat->LED_on_bulk);
  printf("M05: LED float: %i\n",       multiplus_stat->LED_on_float);
  printf("M06: LED inverter: %i\n",    multiplus_stat->LED_on_inverter);
  printf("M07: LED overlaod: %i\n",    multiplus_stat->LED_on_overload);
  printf("M08  LED low battery: %i\n", multiplus_stat->LED_on_low_battery);
  printf("M09: LED temperature: %i\n", multiplus_stat->LED_on_temperature);
  
  printf("M10: LED mains blinking: %i\n",       multiplus_stat->LED_blink_mains);
  printf("M11: LED absorption blinking: %i\n",  multiplus_stat->LED_blink_absorption);
  printf("M12: LED bulk blinking: %i\n",        multiplus_stat->LED_blink_bulk);
  printf("M13: LED float blinking: %i\n",       multiplus_stat->LED_blink_float);
  printf("M15: LED inverter blinking: %i\n",    multiplus_stat->LED_blink_inverter);
  printf("M16: LED overload blinking: %i\n",    multiplus_stat->LED_blink_overload);
  printf("M17: LED low battery blinking: %i\n", multiplus_stat->LED_blink_low_battery);
  printf("M18: LED temperature: %i\n",          multiplus_stat->LED_blink_temperature);
    
  //printf("M19: DC Phase info: %i\n",                  multiplus_stat->if0_phase_info); // it will contain "0x0c = 12" and describe the dc info frame
  printf("M20: DC Voltage : %i\n",                    multiplus_stat->if0_DC_Voltage);
  printf("M21: DC current by inverter devices: %i\n", multiplus_stat->if0_DC_Current_Inv);
  printf("M22: DC current by charger devices: %i\n",  multiplus_stat->if0_DC_Current_Chg);
  printf("M23: DC inverter periode: %.2f\n",          (10000000/(float)(multiplus_stat->if0_Inv_Periode)));

  switch((u_int8_t)multiplus_stat->if1_L1_phase_info)
  { 
    case 8:  printf("M24: 1 phase system \n");
             print_multiplus_L1(multiplus_stat);
	     break;
    case 9:  printf("M24: 2 phase system \n");
             print_multiplus_L1(multiplus_stat);
             print_multiplus_L2(multiplus_stat);
             break;
    case 10: printf("M24: 3 phase system \n");
             print_multiplus_L1(multiplus_stat);
             print_multiplus_L2(multiplus_stat);
             print_multiplus_L3(multiplus_stat);
	     break;
    case 11: printf("M24: 4 phase system \n");
             print_multiplus_L1(multiplus_stat);
             print_multiplus_L2(multiplus_stat);
             print_multiplus_L3(multiplus_stat);
             print_multiplus_L4(multiplus_stat);
	     break;
    default: //should not happen
             break;
  }
  
  u_int8_t ac0=(multiplus_stat->mplf_ac_input_conf & 0x01) ? 1 : 0;
  u_int8_t ac1=(multiplus_stat->mplf_ac_input_conf & 0x02) ? 1 : 0;
  printf("M57: Master last active input AC0: %i\n",  ac0);
  printf("M58: Master last active input AC1: %i\n",  ac1);
  
  printf("M59: Master minimum input current limit: %i\n",  multiplus_stat->mplf_min_inp_cur_lmt);
  printf("M60: Master maximum input current limit: %i\n",  multiplus_stat->mplf_max_inp_cur_lmt);
  printf("M61: Master actual input current limit: %i\n",  multiplus_stat->mplf_actual_inp_cur_lmt);
  
  return 0;
  
}


u_int8_t print_multiplus_L1(struct multiplus_elements* multiplus_stat)
{
  printf("M25: Phase L1 bf factor: %i\n",        multiplus_stat->if1_L1_bf_factor);
  printf("M26: Phase L1 inverter factor: %i\n",  multiplus_stat->if1_L1_inverter_factor);
  printf("M27: Phase L1 phase info: %i\n",       multiplus_stat->if1_L1_phase_info);
  printf("M28: Phase L1 mains voltage: %i\n",    multiplus_stat->if1_L1_mains_voltage);
  printf("M29: Phase L1 mains current: %i\n",    multiplus_stat->if1_L1_mains_current);
  printf("M30: Phase L1 inverter voltage: %i\n", multiplus_stat->if1_L1_inverter_voltage);
  printf("M31: Phase L1 inverter current: %i\n", multiplus_stat->if1_L1_inverter_current);
  printf("M32: Phase L1 mains periode: %0.2f\n", (10000000/((float)multiplus_stat->if1_L1_mains_periode)));
  
  return 0;
  
}


u_int8_t print_multiplus_L2(struct multiplus_elements* multiplus_stat)
{
  printf("M33: Phase L2 bf factor: %i\n",        multiplus_stat->if2_L2_bf_factor);
  printf("M34: Phase L2 inverter factor: %i\n",  multiplus_stat->if2_L2_inverter_factor);
  printf("M35: Phase L2 phase info: %i\n",       multiplus_stat->if2_L2_phase_info);
  printf("M36: Phase L2 mains voltage: %i\n",    multiplus_stat->if2_L2_mains_voltage);
  printf("M37: Phase L2 mains current: %i\n",    multiplus_stat->if2_L2_mains_current);
  printf("M38: Phase L2 inverter voltage: %i\n", multiplus_stat->if2_L2_inverter_voltage);
  printf("M39: Phase L2 inverter current: %i\n", multiplus_stat->if2_L2_inverter_current);
  printf("M40: Phase L2 mains periode: %0.2f\n", (10000000/((float)multiplus_stat->if2_L2_mains_periode)));

  return 0;
    
}


u_int8_t print_multiplus_L3(struct multiplus_elements* multiplus_stat)
{

  printf("M41: Phase L3 bf factor: %i\n",        multiplus_stat->if3_L3_bf_factor);
  printf("M42: Phase L3 inverter factor: %i\n",  multiplus_stat->if3_L3_inverter_factor);
  printf("M43: Phase L3 phase info: %i\n",       multiplus_stat->if3_L3_phase_info);
  printf("M44: Phase L3 mains voltage: %i\n",    multiplus_stat->if3_L3_mains_voltage);
  printf("M45: Phase L3 mains current: %i\n",    multiplus_stat->if3_L3_mains_current);
  printf("M46: Phase L3 inverter voltage: %i\n", multiplus_stat->if3_L3_inverter_voltage);
  printf("M47: Phase L3 inverter current: %i\n", multiplus_stat->if3_L3_inverter_current);
  printf("M48: Phase L3 mains periode: %0.2f\n", (10000000/((float)multiplus_stat->if3_L3_mains_periode)));

  return 0;
    
}


u_int8_t print_multiplus_L4(struct multiplus_elements* multiplus_stat)
{
  printf("M49: Phase L4 bf factor: %i\n",        multiplus_stat->if4_L4_bf_factor);
  printf("M50: Phase L4 inverter factor: %i\n",  multiplus_stat->if4_L4_inverter_factor);
  printf("M51: Phase L4 phase info: %i\n",       multiplus_stat->if4_L4_phase_info);
  printf("M52: Phase L4 mains voltage: %i\n",    multiplus_stat->if4_L4_mains_voltage);
  printf("M53: Phase L4 mains current: %i\n",    multiplus_stat->if4_L4_mains_current);
  printf("M54: Phase L4 inverter voltage: %i\n", multiplus_stat->if4_L4_inverter_voltage);
  printf("M55: Phase L4 inverter current: %i\n", multiplus_stat->if4_L4_inverter_current);
  printf("M46: Phase L4 mains periode: %0.2f\n", (10000000/((float)multiplus_stat->if4_L4_mains_periode)));
  
  return 0;
    
}


u_int8_t mk2_read(char* buffer)
{
  u_int8_t bytes_read=0;
  set_interface_attr(B2400, 0);
  sleep(RS232_TIMEOUT_MK2);
  bytes_read=rs232_read(buffer, BUFFERSIZE_MK2, RS232_TIMEOUT_MK2, 0);
 
  return bytes_read;
  
}


u_int8_t mk2_write(char* msg, int* size)
{
  set_interface_attr(B2400, 0);
  rs232_write(msg, size);
  
  return 0;

}


u_int8_t remove_mk2_version_frame(char* buffer, u_int16_t size)
{
  int n=0;
  int idx = size-1;
  int cnt=0;
  int i= 0;
  char temp[1];
  
  //shifting buffer to remove MK2 version frame
  cnt =  (u_int8_t)buffer[0];

  for(i=0; i<=cnt+1;i++)
  {
    buffer[i]=0x00; //remove MK2 version frame
  }
  
  //ToDo this is realy ugly, I know and sorry, I will go over it soon
  for (i = 0; i <= cnt+1; i++)
  {
    temp[0] = buffer[0];
    for (n=0; n<idx; n++)
    {
      buffer[n] = buffer[n+1];  
    }
    buffer[idx]=temp[0];
  }
  return 0;
  
}


u_int8_t get_multiplus_stats(struct multiplus_elements* multiplus_stat)
{
  char* buffer=NULL;
  int cnt=0;
  buffer=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_read(buffer); 
  
  while(get_mk2_version(buffer, multiplus_stat)!=0)
  {
    sleep(0.2);
    mk2_read(buffer);
    
    if (cnt==LIMIT_GET_MK2_VERSION)
    {
      printf("Could not get MK2 Version\n");
      printf("Please check hardware (i.e.: connection, CPU-load, interference)\n");
      exit(EXIT_FAILURE);
    }
    cnt++;
  }
  
  if (multiplus_stat->mk2_version>=MK2_FW_SUPPORTED)
  {
    //printf("mk2 interface version: %i\n", multiplus_stat->mk2_version);
    pars_multiplus_led_status(multiplus_stat);
    pars_multiplus_infoframe_0(multiplus_stat);
    pars_multiplus_infoframe_1(multiplus_stat);  
    
    switch((u_int8_t)multiplus_stat->if1_L1_phase_info)
    { 
      case 9: //2 phase system
	      pars_multiplus_infoframe_2(multiplus_stat);
              break;
      case 10://3 phase system
	      pars_multiplus_infoframe_2(multiplus_stat);
              pars_multiplus_infoframe_3(multiplus_stat);
              break;
      case 11://4 phase system
	      pars_multiplus_infoframe_2(multiplus_stat);
              pars_multiplus_infoframe_3(multiplus_stat);
              pars_multiplus_infoframe_4(multiplus_stat);
              break;
      default: break;
    }
    
    pars_multiplus_master_multi_led_frame(multiplus_stat);
  }
  else
  {
    printf("Error, MK2 Firmware not supported\n");
    return 1;
  }
  
  return 0;
  
}


u_int8_t pars_multiplus_led_status(struct multiplus_elements* multiplus_stat)
{
  char request[] = REQUEST01;
  int size = sizeof(request);
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_write(request, &size);
  mk2_read(buf);
  
  if((u_int8_t)buf[1]==0xff && (u_int8_t)buf[2]==0x56)
  {
    remove_mk2_version_frame(buf, BUFFERSIZE_MK2);
  }
  
  if(mk2_calc_checksum(buf)==0)
  {
    u_int8_t led_on= (u_int8_t)buf[3];
    u_int8_t led_blink= (u_int8_t)buf[4];
    
    multiplus_stat->LED_on_mains =       (led_on & 0x01) ? 1 : 0;
    multiplus_stat->LED_on_absorption =  (led_on & 0x02) ? 1 : 0;
    multiplus_stat->LED_on_bulk =        (led_on & 0x04) ? 1 : 0;
    multiplus_stat->LED_on_float =       (led_on & 0x08) ? 1 : 0;
    multiplus_stat->LED_on_inverter =    (led_on & 0x10) ? 1 : 0;
    multiplus_stat->LED_on_overload =    (led_on & 0x20) ? 1 : 0;
    multiplus_stat->LED_on_low_battery = (led_on & 0x40) ? 1 : 0;
    multiplus_stat->LED_on_temperature=  (led_on & 0x80) ? 1 : 0;
    
    multiplus_stat->LED_blink_mains =       (led_blink & 0x01) ? 1 : 0;
    multiplus_stat->LED_blink_absorption =  (led_blink & 0x02) ? 1 : 0;
    multiplus_stat->LED_blink_bulk =        (led_blink & 0x04) ? 1 : 0;
    multiplus_stat->LED_blink_float =       (led_blink & 0x08) ? 1 : 0;
    multiplus_stat->LED_blink_inverter =    (led_blink & 0x10) ? 1 : 0;
    multiplus_stat->LED_blink_overload =    (led_blink & 0x20) ? 1 : 0;
    multiplus_stat->LED_blink_low_battery = (led_blink & 0x40) ? 1 : 0;
    multiplus_stat->LED_blink_temperature=  (led_blink & 0x80) ? 1 : 0;
  }
  else
  {
    printf("Checksum InfoFrame0: Error");
    return 1;
  }
  
  return 0;
  
}


//Parse DC InfoFrame 0
u_int8_t pars_multiplus_infoframe_0(struct multiplus_elements* multiplus_stat)
{
  char request[] = REQUEST02;
  int size = sizeof(request);
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_write(request, &size);
  mk2_read(buf);
  
  if((u_int8_t)buf[1]==0xff && (u_int8_t)buf[2]==0x56)
  {
    remove_mk2_version_frame(buf, BUFFERSIZE_MK2);
  }
  
  if(mk2_calc_checksum(buf)==0)
  {
    multiplus_stat->if0_phase_info = (u_int8_t)buf[6];
    multiplus_stat->if0_DC_Voltage = (u_int8_t)buf[7] | (u_int8_t)buf[8] << 8;
    multiplus_stat->if0_DC_Current_Inv = (u_int8_t)buf[9] | (u_int8_t)buf[10] << 8 | (u_int8_t)buf[11] << 16;
    multiplus_stat->if0_DC_Current_Chg = (u_int8_t)buf[12] | (u_int8_t)buf[13] << 8 | (u_int8_t)buf[14] << 16;
    multiplus_stat->if0_Inv_Periode = (u_int8_t)buf[15];
    multiplus_stat->if0_Inv_Periode |= 0x100;
    multiplus_stat->if0_Inv_Periode <<= 9;
  }
  else
  {
    printf("Checksum InfoFrame0: Error");
    return 1;
  }
  
  return 0;
  
}


//Parse DC InfoFrame 1
u_int8_t pars_multiplus_infoframe_1(struct multiplus_elements* multiplus_stat)
{
  char request[] = REQUEST03;
  int size = sizeof(request);
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_write(request, &size);
  mk2_read(buf);
  
  if((u_int8_t)buf[1]==0xff && (u_int8_t)buf[2]==0x56)
  {
    remove_mk2_version_frame(buf, BUFFERSIZE_MK2);
  }
 
  if(mk2_calc_checksum(buf)==0)
  {
    multiplus_stat->if1_L1_bf_factor = (u_int8_t)buf[2];
    multiplus_stat->if1_L1_inverter_factor = (u_int8_t)buf[3];
    multiplus_stat->if1_L1_phase_info = (u_int8_t)buf[6];
    multiplus_stat->if1_L1_mains_voltage = (u_int8_t)buf[7] | (u_int8_t)buf[8] << 8;
    multiplus_stat->if1_L1_mains_current =  (u_int8_t)buf[9] | (u_int8_t)buf[10] << 8;
    multiplus_stat->if1_L1_mains_current = multiplus_stat->if1_L1_mains_current * multiplus_stat->if1_L1_bf_factor;
    multiplus_stat->if1_L1_inverter_voltage = (u_int8_t)buf[11] | (u_int8_t)buf[12] << 8;
    multiplus_stat->if1_L1_inverter_current = (u_int8_t)buf[13] | (u_int8_t)buf[14] << 8;
    multiplus_stat->if1_L1_inverter_current = multiplus_stat->if1_L1_inverter_current * multiplus_stat->if1_L1_inverter_factor;
    multiplus_stat->if1_L1_mains_periode = (u_int8_t)buf[15];
    multiplus_stat->if1_L1_mains_periode <<= 10;
  }
  else
  {
    printf("Checksum InfoFrame1: Error");
    return 1;
  }
  
  return 0;
  
}


//Parse DC InfoFrame 2
u_int8_t pars_multiplus_infoframe_2(struct multiplus_elements* multiplus_stat)
{
  char request[] = REQUEST04;
  int size = sizeof(request);
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_write(request, &size);
  mk2_read(buf);
  
  if((u_int8_t)buf[1]==0xff && (u_int8_t)buf[2]==0x56)
  {
    remove_mk2_version_frame(buf, BUFFERSIZE_MK2);
  } 
    
  if(mk2_calc_checksum(buf)==0)
  {
    multiplus_stat->if2_L2_bf_factor = (u_int8_t)buf[2];
    multiplus_stat->if2_L2_inverter_factor = (u_int8_t)buf[3];
    multiplus_stat->if2_L2_phase_info = (u_int8_t)buf[6];
    multiplus_stat->if2_L2_mains_voltage = (u_int8_t)buf[7] | (u_int8_t)buf[8] << 8;
    multiplus_stat->if2_L2_mains_current =  (u_int8_t)buf[9] | (u_int8_t)buf[10] << 8;
    multiplus_stat->if2_L2_mains_current = multiplus_stat->if1_L1_mains_current * multiplus_stat->if1_L1_bf_factor;
    multiplus_stat->if2_L2_inverter_voltage = (u_int8_t)buf[11] | (u_int8_t)buf[12] << 8;
    multiplus_stat->if2_L2_inverter_current = (u_int8_t)buf[13] | (u_int8_t)buf[14] << 8;
    multiplus_stat->if2_L2_inverter_current = multiplus_stat->if1_L1_inverter_current * multiplus_stat->if1_L1_inverter_factor;
    multiplus_stat->if2_L2_mains_periode = (u_int8_t)buf[15];
    multiplus_stat->if2_L2_mains_periode <<= 10;
  }
  else
  {
    printf("Checksum InfoFrame2: Error");
    return 1;
  }
  
  return 0;
  
}


//Parse DC InfoFrame 3
u_int8_t pars_multiplus_infoframe_3(struct multiplus_elements* multiplus_stat)
{
  char request[] = REQUEST05;
  int size = sizeof(request);
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_write(request, &size);
  mk2_read(buf);
  
  if((u_int8_t)buf[1]==0xff && (u_int8_t)buf[2]==0x56)
  {
    remove_mk2_version_frame(buf, BUFFERSIZE_MK2);
  } 
    
  if(mk2_calc_checksum(buf)==0)
  {
    multiplus_stat->if3_L3_bf_factor = (u_int8_t)buf[2];
    multiplus_stat->if3_L3_inverter_factor = (u_int8_t)buf[3];
    multiplus_stat->if3_L3_phase_info = (u_int8_t)buf[6];
    multiplus_stat->if3_L3_mains_voltage = (u_int8_t)buf[7] | (u_int8_t)buf[8] << 8;
    multiplus_stat->if3_L3_mains_current =  (u_int8_t)buf[9] | (u_int8_t)buf[10] << 8;
    multiplus_stat->if3_L3_mains_current = multiplus_stat->if1_L1_mains_current * multiplus_stat->if1_L1_bf_factor;
    multiplus_stat->if3_L3_inverter_voltage = (u_int8_t)buf[11] | (u_int8_t)buf[12] << 8;
    multiplus_stat->if3_L3_inverter_current = (u_int8_t)buf[13] | (u_int8_t)buf[14] << 8;
    multiplus_stat->if3_L3_inverter_current = multiplus_stat->if1_L1_inverter_current * multiplus_stat->if1_L1_inverter_factor;
    multiplus_stat->if3_L3_mains_periode = (u_int8_t)buf[15];
    multiplus_stat->if3_L3_mains_periode <<= 10;
  }
  else
  {
    printf("Checksum InfoFrame3: Error");
    return 1;
  }
  
  return 0;
  
}


//Parse DC InfoFrame 4
u_int8_t pars_multiplus_infoframe_4(struct multiplus_elements* multiplus_stat)
{
  char request[] = REQUEST06;
  int size = sizeof(request);
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_write(request, &size);
  mk2_read(buf);
  
  if((u_int8_t)buf[1]==0xff && (u_int8_t)buf[2]==0x56)
  {
    remove_mk2_version_frame(buf, BUFFERSIZE_MK2);
  } 
    
  if(mk2_calc_checksum(buf)==0)
  {
    multiplus_stat->if4_L4_bf_factor = (u_int8_t)buf[2];
    multiplus_stat->if4_L4_inverter_factor = (u_int8_t)buf[3];
    multiplus_stat->if4_L4_phase_info = (u_int8_t)buf[6];
    multiplus_stat->if4_L4_mains_voltage = (u_int8_t)buf[7] | (u_int8_t)buf[8] << 8;
    multiplus_stat->if4_L4_mains_current =  (u_int8_t)buf[9] | (u_int8_t)buf[10] << 8;
    multiplus_stat->if4_L4_mains_current = multiplus_stat->if1_L1_mains_current * multiplus_stat->if1_L1_bf_factor;
    multiplus_stat->if4_L4_inverter_voltage = (u_int8_t)buf[11] | (u_int8_t)buf[12] << 8;
    multiplus_stat->if4_L4_inverter_current = (u_int8_t)buf[13] | (u_int8_t)buf[14] << 8;
    multiplus_stat->if4_L4_inverter_current = multiplus_stat->if1_L1_inverter_current * multiplus_stat->if1_L1_inverter_factor;
    multiplus_stat->if4_L4_mains_periode = (u_int8_t)buf[15];
    multiplus_stat->if4_L4_mains_periode <<= 10;
  }
  else
  {
    printf("Checksum InfoFrame4: Error");
    return 1;
  }
  
  return 0;
  
}


u_int8_t pars_multiplus_master_multi_led_frame(struct multiplus_elements* multiplus_stat)
{
  char request[] = REQUEST07;
  int size = sizeof(request);
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
    
  mk2_write(request, &size);
  mk2_read(buf);
  
  if((u_int8_t)buf[1]==0xff && (u_int8_t)buf[2]==0x56)
  {
    remove_mk2_version_frame(buf, BUFFERSIZE_MK2);
  } 
    
  if(mk2_calc_checksum(buf)==0)
  {
    multiplus_stat->mplf_ac_input_conf=(u_int8_t)buf[6];
    multiplus_stat->mplf_min_inp_cur_lmt=(u_int8_t)buf[7] | (u_int8_t)buf[8] << 8;;
    multiplus_stat->mplf_max_inp_cur_lmt=(u_int8_t)buf[9] | (u_int8_t)buf[10] << 8;;
    multiplus_stat->mplf_actual_inp_cur_lmt=(u_int8_t)buf[11] | (u_int8_t)buf[12] << 8;;
  }
  else
  {
    printf("Checksum MultiPlusLED frame: Error");
    return 1;
  }
  
  return 0;
  
}


u_int32_t get_mk2_version(char* buffer, struct multiplus_elements* multiplus_stat)
{
  int idx = 0;
  
  if ((u_int8_t)buffer[1]==0xff && (u_int8_t)buffer[2]==0x56)
  {
    if (mk2_calc_checksum(buffer)==0)
    {
      multiplus_stat->mk2_version = ((u_int8_t)buffer[3] ) | (u_int8_t)buffer[4] << 8 | (u_int8_t)buffer[5] << 16 | (u_int8_t)buffer[6]<< 24;
      multiplus_stat->mk2_mode = buffer[7];
      return 0;
    }
    else
    {
      //printf("Checksum error in mk2 version frame\n");
      return 2;
    }
  }
  else
  {
    //printf("Received data does not contain mk2 version frame\n");
    return 1;
  }
  
}


u_int8_t mk2_calc_checksum(char* buffer)
{
  int idx=0;
  int size = 0;
  u_int16_t calc=0;
    
  size = buffer[0]+1;
  
  for(idx=0;idx<=size;idx++)
  {
    calc=calc+(u_int8_t)(buffer[idx]);
  }
  calc = calc % 256;
  if (calc==0)
  {
    return 0;
  }
  else
  {
    printf("Checksum: Error - mk2_calc_checksum function\n");
    return 1;
  }
}

void mk2_raw_read()
{
  u_int16_t idx=0;
  char* buf=NULL;
  buf=calloc(BUFFERSIZE_MK2, 1);
  int size=0;
  u_int8_t bytes_read=0;
  int cnt=0;
  
  bytes_read=mk2_read(buf);
  
  while(buf[1]!=0xff)
  {
    sleep(0.2);
    bytes_read=mk2_read(buf);
    
    if (cnt==LIMIT_GET_MK2_VERSION)
    {
      printf("Could not get MK2 Version Frame\n");
      break;
    }
    cnt++;
  }
  
  printf("bytes read: %i\n", bytes_read);
  for (idx=0; idx<BUFFERSIZE_MK2;idx++)
  {
    printf("0x%02x ", (u_int8_t)buf[idx]);
  }
  printf("\n");
  
  char request01[]=REQUEST01;
  size=sizeof(request01);
  mk2_raw_request(request01, &size);
   
  char request02[]=REQUEST02;
  size=sizeof(request02);
  mk2_raw_request(request02, &size);
  
  char request03[]=REQUEST03;
  size=sizeof(request03);
  mk2_raw_request(request03, &size);
  
  char request04[]=REQUEST04;
  size=sizeof(request04);
  mk2_raw_request(request04, &size);
  
  char request05[]=REQUEST05;
  size=sizeof(request05);
  mk2_raw_request(request05, &size);
  
  char request06[]=REQUEST06;
  size=sizeof(request06);
  mk2_raw_request(request06, &size);
  
  char request07[]=REQUEST07;
  size=sizeof(request07);
  mk2_raw_request(request07, &size);
    
}

void mk2_raw_request(char* request, int* size)
{
  int idx =0;
  int size_request = *size;
  char* buf=NULL;
  int bytes_read=0;
  buf=calloc(BUFFERSIZE_MK2, 1);
  
  mk2_write(request, &size_request);
  bytes_read=mk2_read(buf);
  
  printf("bytes read: %i\n", bytes_read);
  for (idx=0; idx<BUFFERSIZE_MK2;idx++)
  {
    printf("0x%02x ", (u_int8_t)buf[idx]);
  }
  printf("\n");  
     
}