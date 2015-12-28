#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>

void phoneof(int onORoff)
{
  int fd;
  fd = open("/dev/mux0",O_WRONLY);
  switch (onORoff)
  {
	  case 0: //set off
	  	write(fd,"AT+CFUN=0\r",10);
	  	break;
	  case 1: //set on
	  	write(fd,"AT+CFUN=1\r",10);
	  	break;
  }
  close(fd);
  return;
}

