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

#include "victron_bs.h"
#include "../rs232/rs232.h"

void bs_inverter_init(u_int8_t bs_mode)
{
  char* buffer=NULL;
  buffer=calloc(BUFFERSIZE_BS_INVERTER, 1);
  struct bs_inverter_elements bs_inverter_stat;
  memset(&bs_inverter_stat,0, sizeof(bs_inverter_stat));
  
  switch (bs_mode)
  {
    case 0:
      bs_inverter_get_stats(buffer, &bs_inverter_stat);
      bs_inverter_get_param(buffer, &bs_inverter_stat);
      rs232_close();
      bs_inverter_print_stats(&bs_inverter_stat);
      bs_inverter_print_param(&bs_inverter_stat);
    break;
    case 1:
      bs_inverter_get_stats(buffer, &bs_inverter_stat);
      rs232_close();
      bs_inverter_print_stats(&bs_inverter_stat);      
    break;
    case 2:
      bs_inverter_get_param(buffer, &bs_inverter_stat);
      rs232_close();
      bs_inverter_print_param(&bs_inverter_stat);
    break;
    case 3:
      bs_raw_mode();
      rs232_close();
    break;
  }

  if (buffer!=NULL) buffer=NULL;
  
}

u_int8_t bs_inverter_write(char* msg, int* size)
{
  set_interface_attr(B9600, 0);
  rs232_write(msg, size);
  
  return 0;
}

u_int8_t bs_inverter_read(char* buffer)
{
  int bytes_read=0;
  set_interface_attr(B9600, 0);
  sleep(RS232_TIMEOUT_BS_INVERTER);
  bytes_read=rs232_read(buffer, BUFFERSIZE_BS_INVERTER, RS232_TIMEOUT_BS_INVERTER, 0);
   
  return bytes_read;
}

void bs_inverter_print_stats(struct bs_inverter_elements* bs_inverter_stat)
{
  
  printf("S00: Today generated energy in KW: %0.1f\n", (double)bs_inverter_stat->kw_day/10);
  printf("S01: Total generated energy in KW: %0.1f\n", (double)bs_inverter_stat->kw_total/10);

  printf("S02: Total runtime in Hours: %i\n", bs_inverter_stat->total_runtime_h);
  printf("S03: Today runtime in Minutes: %i\n", bs_inverter_stat->runtime_min);
  
  printf("S04: Photovoltaic DC voltage in V: %0.1f\n", (double)bs_inverter_stat->pv_dc_voltage/10);
  printf("S05: Photovoltaic power in W: %i\n", bs_inverter_stat->pv_watt);
  printf("S06: AC Voltage in V: %0.1f\n", (double)bs_inverter_stat->ac_voltage/10);
  printf("S07: AC current in A: %0.2f\n", (double)bs_inverter_stat->ac_current/100);
  printf("S08: AC frequency in Hz: %0.2f\n", (double)bs_inverter_stat->frequency_ac/100);

  printf("S09: Inverter temperature in °C: %0.1f\n", (double)bs_inverter_stat->temperature/10);
  printf("S10: A cpu bus voltage in V: %0.1f\n", (double)bs_inverter_stat->a_cpu_bus_u/10);

  printf("S11: ISO samples: %i\n", bs_inverter_stat->iso_samples);

}

/*
 * msg1 request for: KW total, runtime in min., total runtime in hour and inverter temperature
 * msg2 request for: PV DC Voltage, AC Voltage, AC Current, Freq AC, PV Watt and KW day
 */
u_int8_t bs_inverter_get_stats(char* buffer, struct bs_inverter_elements* bs_inverter_stat)
{
  
  char msg1[] = MSG1;
  char msg2[] = MSG2;
  u_int16_t msg_frame_id=0;
  u_int8_t msg_frame_size=0;
  u_int8_t bytes_read=0;
  int size = 0;
  
  memset(buffer,0, BUFFERSIZE_BS_INVERTER); 
  size = sizeof(msg1);
  bs_inverter_write(msg1, &size);
  bytes_read = bs_inverter_read(buffer);

  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG1_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->kw_total = ((u_int8_t)buffer[3] << 24) | ((u_int8_t)buffer[4] << 16) | ((u_int8_t)buffer[5] << 8) | (u_int8_t)buffer[6];
    bs_inverter_stat->runtime_min = ((u_int8_t)buffer[7] << 8) | (u_int8_t)buffer[8];
    bs_inverter_stat->total_runtime_h = ((u_int8_t)buffer[9] << 24) | ((u_int8_t)buffer[10] << 16)| ((u_int8_t)buffer[11] << 8) | (u_int8_t)buffer[12];
    bs_inverter_stat->temperature = ((u_int8_t)buffer[25] << 8) | (u_int8_t)buffer[26];
    bs_inverter_stat->a_cpu_bus_u = ((u_int8_t)buffer[27] << 8) | (u_int8_t)buffer[28];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg1 frame. %i %i\n", bytes_read, msg_frame_size);
  }
  
  
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg2);
  bs_inverter_write(msg2, &size);
  bytes_read = bs_inverter_read(buffer);
  
  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG2_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->pv_dc_voltage = ((u_int8_t)buffer[3] << 8) | (u_int8_t)buffer[4];
    bs_inverter_stat->iso_samples = ((u_int8_t)buffer[5] << 8) + (u_int8_t)buffer[6];
    bs_inverter_stat->ac_voltage = ((u_int8_t)buffer[15] << 8) + (u_int8_t)buffer[16];
    bs_inverter_stat->ac_current = ((u_int8_t)buffer[21] << 8) + (u_int8_t)buffer[22];
    bs_inverter_stat->frequency_ac = ((u_int8_t)buffer[27] << 8) + (u_int8_t)buffer[28];
    bs_inverter_stat->pv_watt = ((u_int8_t)buffer[37] << 8) + (u_int8_t)buffer[38];
    bs_inverter_stat->kw_day = ((u_int8_t)buffer[41] << 8) + (u_int8_t)buffer[42];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg2 frame. %i %i\n", bytes_read, msg_frame_size);
  }

  return 0;
  
}  

u_int8_t bs_inverter_get_param(char* buffer, struct bs_inverter_elements* bs_inverter_stat)
{
  char msg3[] = MSG3;
  char msg4[] = MSG4;
  char msg5[] = MSG5;
  char msg6[] = MSG6;
  char msg9[] = MSG9;
  char msg10[] = MSG10;
  char msg11[] = MSG11;
  char msg12[] = MSG12;
  char msg14[] = MSG14;
     
  u_int16_t msg_frame_id=0;
  u_int8_t msg_frame_size=0;
  u_int8_t bytes_read=0;
  int size = 0;  
    
  size = sizeof(msg3);
  bs_inverter_write(msg3, &size);
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  bytes_read = bs_inverter_read(buffer);
    
  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG3_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->b_cpu_ac_u = ((u_int8_t)buffer[15] << 8) + (u_int8_t)buffer[16];
    bs_inverter_stat->dci1_in_ma = (u_int8_t)buffer[40];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg3 frame.\n");
  }
   
  
  size = sizeof(msg4);
  bs_inverter_write(msg4, &size);
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  bytes_read = bs_inverter_read(buffer);   
  
  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG4_FRAME_ID && bytes_read >= msg_frame_size)
  {
    int idx=0,i=0;
    for (i=0; i < 6; i++)
    {
      snprintf(bs_inverter_stat->serial_number + idx, 3, "%02x", (u_int8_t)buffer[15+i]);
      idx+=2;
    };
    
    snprintf(bs_inverter_stat->firmware, 3, "%02x", (u_int8_t)buffer[7]);
    snprintf(bs_inverter_stat->firmware+2, 3, "%02x", (u_int8_t)buffer[8]);    
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg4 frame.\n");
  }
  
  
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg5);
  bs_inverter_write(msg5, &size);
  bytes_read = bs_inverter_read(buffer);

  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG5_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->voltage_pv_start = ((u_int8_t)buffer[3] << 8) + (u_int8_t)buffer[4];
    bs_inverter_stat->t_starts = ((u_int8_t)buffer[11] << 8) + (u_int8_t)buffer[12];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg5 frame.\n");
  }
   
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg6);
  bs_inverter_write(msg6, &size);
  bytes_read = bs_inverter_read(buffer);

  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG6_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->code = ((u_int8_t)buffer[3] << 8) + (u_int8_t)buffer[4];
    bs_inverter_stat->v_ac_low_voltage = ((u_int8_t)buffer[5] << 8) + (u_int8_t)buffer[6];
    bs_inverter_stat->v_ac_ultra_low_voltage = ((u_int8_t)buffer[7] << 8) + (u_int8_t)buffer[8];
    bs_inverter_stat->v_ac_low_voltage_time = ((u_int8_t)buffer[9] << 8) + (u_int8_t)buffer[10];
    bs_inverter_stat->v_ac_ultra_low_voltage_time = ((u_int8_t)buffer[11] << 8) + (u_int8_t)buffer[12];
    bs_inverter_stat->v_ac_high_voltage = ((u_int8_t)buffer[13] << 8) + (u_int8_t)buffer[14];
    bs_inverter_stat->v_ac_ultra_high_voltage = ((u_int8_t)buffer[15] << 8) + (u_int8_t)buffer[16];
    bs_inverter_stat->v_ac_high_voltage_time = ((u_int8_t)buffer[17] << 8) + (u_int8_t)buffer[18];
    bs_inverter_stat->v_ac_ultra_high_voltage_time = ((u_int8_t)buffer[19] << 8) + (u_int8_t)buffer[20];
    bs_inverter_stat->low_frequency = ((u_int8_t)buffer[21] << 8) + (u_int8_t)buffer[22];
    bs_inverter_stat->ultra_low_frequency = ((u_int8_t)buffer[23] << 8) + (u_int8_t)buffer[24];
    bs_inverter_stat->low_frequency_time = ((u_int8_t)buffer[25] << 8) + (u_int8_t)buffer[26];
    bs_inverter_stat->ultra_low_frequency_time = ((u_int8_t)buffer[27] << 8) + (u_int8_t)buffer[28];
    bs_inverter_stat->high_frequency = ((u_int8_t)buffer[29] << 8) + (u_int8_t)buffer[30];
    bs_inverter_stat->ultra_high_frequency = ((u_int8_t)buffer[31] << 8) + (u_int8_t)buffer[32];
    bs_inverter_stat->high_frequency_time = ((u_int8_t)buffer[33] << 8) + (u_int8_t)buffer[34];
    bs_inverter_stat->ultra_high_frequency_time = ((u_int8_t)buffer[35] << 8) + (u_int8_t)buffer[36];
    bs_inverter_stat->dci_limit = ((u_int8_t)buffer[37] << 8) + (u_int8_t)buffer[38];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg6 frame.\n");
  }
  
  
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg9);
  bs_inverter_write(msg9, &size);
  bytes_read = bs_inverter_read(buffer);

  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG9_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->full_power_limit = ((u_int8_t)buffer[7] << 8) + (u_int8_t)buffer[8];
    bs_inverter_stat->active_power_reduction_rate = ((u_int8_t)buffer[11] << 8) + (u_int8_t)buffer[12];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg9 frame.\n");
  }
  
  
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg10);
  bs_inverter_write(msg10, &size);
  bytes_read = bs_inverter_read(buffer);

  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG10_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->kpv_gain = ((u_int8_t)buffer[15] << 8) + (u_int8_t)buffer[16];
    bs_inverter_stat->kac_gain = ((u_int8_t)buffer[31] << 8) + (u_int8_t)buffer[32];
    bs_inverter_stat->ka_bus_gain = ((u_int8_t)buffer[39] << 8) + (u_int8_t)buffer[40];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg10 frame.\n");
  }
    
  
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg11);
  bs_inverter_write(msg11, &size);
  bytes_read = bs_inverter_read(buffer);

  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG11_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->kb_cpu_ac_gain = ((u_int8_t)buffer[15] << 8) + (u_int8_t)buffer[16];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg11 frame.\n");
  }
  

  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg12);
  bs_inverter_write(msg12, &size);
  bytes_read = bs_inverter_read(buffer);
  
  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG12_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->dci_shift = ((u_int8_t)buffer[11] << 8) + (u_int8_t)buffer[12];
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg12 frame.\n");
  }  
  
  memset(buffer,0, BUFFERSIZE_BS_INVERTER);
  size = sizeof(msg14);
  bs_inverter_write(msg14, &size);
  bytes_read = bs_inverter_read(buffer);  

  msg_frame_id = ((u_int8_t)buffer[0] << 8) + (u_int8_t)buffer[1];
  msg_frame_size = (u_int8_t)buffer[2];
  
  if (msg_frame_id == MSG14_FRAME_ID && bytes_read >= msg_frame_size)
  {
    bs_inverter_stat->fac_high_spi = ((u_int8_t)buffer[3] << 8) + (u_int8_t)buffer[4];
    bs_inverter_stat->fac_low_spi = ((u_int8_t)buffer[5] << 8) + (u_int8_t)buffer[6];
    bs_inverter_stat->fac_high_spi_time = ((u_int8_t)buffer[7] << 8) + (u_int8_t)buffer[8];
    bs_inverter_stat->fac_low_spi_time = ((u_int8_t)buffer[9] << 8) + (u_int8_t)buffer[10];
    bs_inverter_stat->connecting_fac_high = ((u_int8_t)buffer[11] << 8) + (u_int8_t)buffer[12];
    bs_inverter_stat->connecting_fac_low = ((u_int8_t)buffer[13] << 8) + (u_int8_t)buffer[14];  
  }
  else
  {
    printf("Number of bytes read, smaller then expected for msg14 frame.\n");
  } 
  
  return 0;
  
}


void bs_inverter_print_param(struct bs_inverter_elements* bs_inverter_stat)
{
  printf("P00: B_CPU_AC: %0.1f\n", (double)bs_inverter_stat->b_cpu_ac_u/10); 
  printf("P01: Serial number: %s\n", bs_inverter_stat->serial_number);
  printf("P02: Firmware Version: %s\n", bs_inverter_stat->firmware);
  printf("P03: Photovoltaic voltage start in V: %0.1f\n", (double)bs_inverter_stat->voltage_pv_start/10);
  printf("P04: T-Start(grid connect) in Seconds: %i\n", bs_inverter_stat->t_starts);
  printf("P05: Standard code (10=Island-Mode): %i\n", bs_inverter_stat->code);
  
  printf("P06: Vac low voltage in V: %0.1f\n", (double)bs_inverter_stat->v_ac_low_voltage/10);
  printf("P07: Vac low voltage time in ms: %i\n", bs_inverter_stat->v_ac_low_voltage_time*10);
  printf("P08: Vac ultra low voltage in V: %0.1f\n", (double)bs_inverter_stat->v_ac_ultra_low_voltage/10);
  printf("P09: Vac ultra low voltage time in ms: %i\n", bs_inverter_stat->v_ac_ultra_low_voltage_time*10);
  printf("P10: Vac high voltage in V: %0.1f\n", (double)bs_inverter_stat->v_ac_high_voltage/10);
  printf("P11: Vac high voltage time in ms: %i\n", bs_inverter_stat->v_ac_high_voltage_time*10);
  printf("P12: Vac ultra high voltage in V: %0.1f\n", (double)bs_inverter_stat->v_ac_ultra_high_voltage/10);
  printf("P13: Vac ultra high voltage time in ms: %i\n", bs_inverter_stat->v_ac_ultra_high_voltage_time*10);
  
  printf("P14: Low frequency in Hz: %0.2f\n", (double)bs_inverter_stat->low_frequency/100);
  printf("P15: Low frequency time in ms: %i\n", bs_inverter_stat->low_frequency_time*10);
  printf("P16: Ultra low frequency in Hz: %0.2f\n", (double)bs_inverter_stat->ultra_low_frequency/100);
  printf("P17: Ultra low frequency time in ms: %i\n", bs_inverter_stat->ultra_low_frequency_time*10);
  printf("P18: High frequency in Hz: %0.2f\n", (double)bs_inverter_stat->high_frequency/100);
  printf("P19: High frequency time in ms: %i\n", bs_inverter_stat->high_frequency_time*10);
  printf("P20: Ultra high frequency in Hz: %0.2f\n", (double)bs_inverter_stat->ultra_high_frequency/100);
  printf("P21: Ultra high frequency time in ms: %i\n", bs_inverter_stat->ultra_high_frequency_time*10);
  
  printf("P22: Full power limit: %0.1f\n", (double)bs_inverter_stat->full_power_limit/10);
  printf("P23: Active power reduction rate in Pf/Hz: %0.2f\n", (double)bs_inverter_stat->active_power_reduction_rate/100);
    
  printf("P24: Frequency AC high spi in Hz: %0.2f\n", (double)bs_inverter_stat->fac_high_spi/100);
  printf("P25: Frequency AC high spi time in ms: %i\n", bs_inverter_stat->fac_high_spi_time*10);
  printf("P26: Frequency AC low spi in Hz: %0.2f\n", (double)bs_inverter_stat->fac_low_spi/100);
  printf("P27: Frequency AC low spi time in ms: %0.1f\n", bs_inverter_stat->fac_low_spi_time*10);
  printf("P28: Connecting frequency AC high in Hz: %0.2f\n", (double)bs_inverter_stat->connecting_fac_high/100);
  printf("P29: Connecting frequency AC low in Hz: %0.2f\n", (double)bs_inverter_stat->connecting_fac_low/100);
    
  printf("P30: DCI in mA: %0.1f\n", (double)bs_inverter_stat->dci1_in_ma/10);
  printf("P31: DCI limit: %i\n", bs_inverter_stat->dci_limit);
  printf("P32: DCI shift: %i\n", bs_inverter_stat->dci_shift);
  
  printf("P33: Kpv gain: %0.1f\n", (double)bs_inverter_stat->kpv_gain/10);
  printf("P34: Kac gain: %0.1f\n", (double)bs_inverter_stat->kac_gain/10);
  printf("P35: KA bus gain: %0.1f\n", (double)bs_inverter_stat->ka_bus_gain/10);
  printf("P36: KB cpu ac gain: %0.1f\n", (double)bs_inverter_stat->kb_cpu_ac_gain/10);  
  
}
 
//
//  For debug  
void bs_raw_read(char* request, int* size)
{
  int size_request = *size;
  u_int8_t bytes_read = 0;
  u_int8_t i = 0;
  char* buffer=NULL;
  buffer=calloc(BUFFERSIZE_BS_INVERTER, 1);
    
  bs_inverter_write(request, &size_request);
  bytes_read = bs_inverter_read(buffer);

  printf("received bytes: %i\n", bytes_read);
  while(i<size_request)
  {
    printf("0x%02x ", buffer[i]);
    i++;
  }
  printf("\n");
  
}


void bs_raw_mode()
{
  char msg1[] = MSG1;
  char msg2[] = MSG2;
  char msg3[] = MSG3;
  char msg4[] = MSG4;
  char msg5[] = MSG5;
  char msg6[] = MSG6;
  char msg7[] = MSG7;
  char msg8[] = MSG8;
  char msg9[] = MSG9;
  char msg10[] = MSG10;
  char msg11[] = MSG11;
  char msg12[] = MSG12;
  char msg13[] = MSG13;
  char msg14[] = MSG14;
  int size = 0;
  
  size = sizeof(msg1);
  printf("content msg1:\n");
  bs_raw_read(msg1, &size);
  
  size = sizeof(msg2);
  printf("content msg2:\n");
  bs_raw_read(msg2, &size);
  
  size = sizeof(msg3);
  printf("content msg3:\n");
  bs_raw_read(msg3, &size);
  
  size = sizeof(msg4);
  printf("content msg4:\n");
  bs_raw_read(msg4, &size);
  
  size = sizeof(msg5);
  printf("content msg5:\n");
  bs_raw_read(msg5, &size);
  
  size = sizeof(msg6);
  printf("content msg6:\n");
  bs_raw_read(msg6, &size);
  
  size = sizeof(msg7);
  printf("content msg7:\n");
  bs_raw_read(msg7, &size);
  
  size = sizeof(msg8);
  printf("content msg8:\n");
  bs_raw_read(msg8, &size);
  
  size = sizeof(msg9);
  printf("content msg9:\n");
  bs_raw_read(msg9, &size);
  
  size = sizeof(msg10);
  printf("content msg10:\n");
  bs_raw_read(msg10, &size);
  
  size = sizeof(msg11);
  printf("content msg11:\n");
  bs_raw_read(msg11, &size);
  
  size = sizeof(msg12);
  printf("content msg12:\n");
  bs_raw_read(msg12, &size);
  
  size = sizeof(msg13);
  printf("content msg13:\n");
  bs_raw_read(msg13, &size);
  
  size = sizeof(msg14);
  printf("content msg14:\n");
  bs_raw_read(msg14, &size);
    
}