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

#ifndef _RS232_H
#define _RS232_H

  #include <termios.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <fcntl.h>
  #include <sys/select.h>

  int fd; //ToDo rewrite this to a function pointer

  u_int8_t set_interface_attr (int speed, int parity);
  u_int8_t set_blocking (u_int16_t number, int time);
  u_int8_t rs232_open(char* interface);
  u_int8_t rs232_close(void);
  u_int8_t rs232_write(char *msg, int* size);
  u_int16_t rs232_read(char* buffer, u_int16_t size, u_int16_t time, u_int8_t min_bytes_to_read);

#endif