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

#include "rs232.h"

u_int8_t set_interface_attr (int speed, int parity)
{
  struct termios tty;
  memset(&tty, 0, sizeof(tty));
  if (tcgetattr (fd, &tty) != 0)
  {
    printf ("error %d from tcgetattr \n");
    return 1;
  }
  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);
  
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_cflag &= ~(PARENB | PARODD);
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_cflag |= (CLOCAL | CREAD);
  
  tty.c_iflag |= IGNBRK;
  tty.c_iflag &= ~ICRNL;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  tty.c_lflag = 0;
  tty.c_oflag = 0;
    
  tcflush(fd, TCIFLUSH);

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
  {
     printf ("error %d from tcsetattr \n");
     return 1;
  }
  //sleep (2);
  return 0;
}

//function not used in the code but maybe useful
u_int8_t set_blocking (u_int16_t number, int time)
{
  struct termios tty;
  memset(&tty, 0, sizeof(tty));
  if (tcgetattr (fd, &tty) != 0)
  {
    printf ("error %d from tggetattr \n");
    return 1;
  }
  //printf("bytes to read max: %i\n", number);

  tty.c_cc[VMIN]  = number;                   // # of bytes to read
  tty.c_cc[VTIME] = time;                     // read timeout

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
  {
    printf ("error %d setting term attributes \n");
    return 2;
  }
  return 0;
  
}

u_int8_t rs232_open(char* interface)
{
  fd = open (interface, O_RDWR | O_NOCTTY | O_SYNC | O_NDELAY);
  if (fd < 0)
  {
    printf ("error %d opening %s \n", interface);
    return 1;
  }
  else
  {
    //printf("open port %s \n", interface);
    set_interface_attr(B9600, 0);
    return 0;
  }

}

u_int8_t rs232_close(void)
{
  close(fd);
  return 0;
  
}

u_int8_t rs232_write(char *msg, int* size)
{
  //printf("size %i \n", *size);
  write (fd, msg, *size); 
  
  return 0;
  
}

u_int16_t rs232_read(char* buffer, u_int16_t size, u_int16_t time, u_int8_t min_bytes_to_read)
{
  u_int16_t n;
  fd_set set;
  struct timeval timeout;
  int rv;  

  FD_ZERO(&set);    
  FD_SET(fd, &set);
  timeout.tv_sec = time;
  timeout.tv_usec = 0;
  
  rv = select(fd + 1, &set, NULL, NULL, &timeout);
  if(rv == -1)
  {
    perror("read error\n"); /* an error accured */
  }
  else if(rv == 0)
  {
    printf("read timeout\n"); /* a timeout occured */
  }
  else
  {
    //printf("size buffer %i\n", size);
    n = read(fd, buffer, size);
    //printf("rs232 bytes read %i\n", n);
  }
  
  if (n <= 0)
  {
    printf("no bytes received\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    //printf("did read %i bytes\n", n);
    return n;
  }
  
}