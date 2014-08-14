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

#include "rs232/rs232.h"
#include "battery_monitors/victron_bmv.h"
#include "pv_chargers/victron_mppt.h"
#include "pv_inverters/victron_bs.h"
#include "pv_inverters/victron_mk2.h"

void print_usage() 
{
  printf("usage:\n");
  printf("-i: interface i.e.:  /dev/ttyUSB0 \n");
  printf("-o: stat | param | all\n");
  printf("-d: read from: mppt | bmv | bs_inverter | mk2\n");
  printf("-r: raw read from: mppt | bmv | bs_inverter | mk2\n");
  
  printf("Option \"-o\" only for BlueSolar Inverter:\n -o stat\n -o param\n -o all\n\n");
  
  printf("Examples:\n");
  printf("read from bmv: \"PV-SysReader -i /dev/ttyUSB0 -d bmv\"\n");
  printf("read from mppt charger: \"PV-SysReader -i /dev/ttyUSB0 -d mppt\"\n");
  printf("read from BlueSolar Inverter: \"PV-SysReader -i /dev/ttyUSB0 -o stat -d bs_inverter\"\n"); 
  printf("read from mk2: \"PV-SysReader -i /dev/ttyUSB0 -d mk2\"\n\n");  
  
  printf("Examples for raw rs2323 read, output will be in Hex:\n");
  printf("bmv: \"PV-SysReader -i /dev/ttyUSB0 -r bmv\" \n");
  printf("mppt: \"PV-SysReader -i /dev/ttyUSB0 -r mppt\" \n");
  printf("BlueSolar Grid Inverter: \"PV-SysReader -i /dev/ttyUSB0 -r bs_inverter\" \n");  
  printf("mk2: \"PV-SysReader -i /dev/ttyUSB0 -r mk2\" \n");
  
}


int main(int argc, char **argv)
{
  char* bmv_cmd=NULL;
  char* interface=NULL;
  u_int8_t pflag = 0;
  
  //selects the read mode for blues solar inverter
  //this has an impact of reading speed until the parameters are received
  //0: get all values for BlueSolar Inverter, 
  //1: get statistic values, 
  //2: get parameters
  u_int8_t bs_mode = 0; 
  
  int c;
  if (argc < 2) 
  {
    print_usage(); 
    exit(EXIT_FAILURE);
  }
  while ((c = getopt (argc, argv, "i:o:d:r:t")) != -1)
  switch (c)
  {
    case 'i':
      interface = strdup(optarg);
      if (rs232_open(interface) > 0)
      {
	exit(EXIT_FAILURE);
      }
      else
      {
	pflag=1;
      }
    break;
    case 'o':
      if (strcmp(optarg,"stat")==0)
      {
	bs_mode=1;
      }
      else if(strcmp(optarg,"param")==0)
      {
	bs_mode=2;
      }
      else
      {
	bs_mode=0;
      }
    break;
    case 'd':
      if (pflag)
      {
	if (strcmp(optarg,"bmv")==0) 
	{
          bmv_init(interface);
	}
	else if(strcmp(optarg,"mppt")==0) 
	{
	  mppt_init();
	}
	else if(strcmp(optarg,"bs_inverter")==0) 
	{
	  bs_inverter_init(bs_mode);
	}
	else if(strcmp(optarg,"mk2")==0) 
	{
	  mk2_init();
	}
	else
	{
	  printf("unknown option %s\n", optarg);
	  print_usage(); 
          exit(EXIT_FAILURE);
	}
      }
      else
      {
	printf("error, interface missing\n");
	print_usage(); 
        exit(EXIT_FAILURE);
      }
    break;
    case 'r':
      if (pflag)
      {
	if (strcmp(optarg,"bmv")==0) 
	{
          bmv_init_raw();
	}
	else if(strcmp(optarg,"mppt")==0)
	{
 	  mppt_init_raw();
	}
	else if(strcmp(optarg,"bs_inverter")==0)
	{
	  bs_mode = 3;
	  bs_inverter_init(bs_mode);
	}
	else if(strcmp(optarg,"mk2")==0)
	{
	  mk2_raw_read();
	}
        else
	{
	  printf("unknown option %s\n", optarg);
	  print_usage(); 
          exit(EXIT_FAILURE);
	}
      }
    case 't':
        //not used
        break;
    default:
      print_usage(); 
      if (fd != 0)
      {
        rs232_close();
      }
      exit(EXIT_FAILURE);
    break;
  }

  if (fd != 0)
  {
    rs232_close();
    //printf("closing of rs232 file discriptor done\n");
  }
  return 0;
  
}