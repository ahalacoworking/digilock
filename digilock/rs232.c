/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/

//
//  rs232.c
//  digilock
//
//  Modified by Olivier Huguenot on 26/12/2014.
//  Copyright (c) 2014 Etincelle Coworking. All rights reserved.
//

/* last revision: December 06, 2014 */

/* For more info and how to use this library, visit: http://www.teuniz.net/RS-232/ */


#include "rs232.h"
#include <pthread.h>

//static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;


#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)  /* Linux & FreeBSD */


#ifdef __APPLE__
#  define COMPORT_NUM 40
#else
#  define COMPORT_NUM 38
#endif


//int Cport[COMPORT_NUM],
  int  error;

struct termios new_port_settings,
       old_port_settings[COMPORT_NUM];

char comports[COMPORT_NUM][32]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
                       "/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
                       "/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
                       "/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
                       "/dev/ttyAMA0","/dev/ttyAMA1","/dev/ttyACM0","/dev/ttyACM1",
                       "/dev/rfcomm0","/dev/rfcomm1","/dev/ircomm0","/dev/ircomm1",
                       "/dev/cuau0","/dev/cuau1","/dev/cuau2","/dev/cuau3",
                       "/dev/cuaU0","/dev/cuaU1","/dev/cuaU2","/dev/cuaU3"
#ifdef __APPLE__
                        ,
                        "/dev/tty.usbserial",
                        "/dev/tty.usbserial5"
#endif
};

int RS232_OpenComport(char * comport_name, int baudrate, const char *mode) {
  int baudr,
      status;

    int comport = -1;
    
//  if((comport_number > COMPORT_NUM)|| (comport_number < 0))
//  {
//    printf("illegal comport number\n");
//    return(1);
//  }

  switch(baudrate)
  {
    case      50 : baudr = B50;
                   break;
    case      75 : baudr = B75;
                   break;
    case     110 : baudr = B110;
                   break;
    case     134 : baudr = B134;
                   break;
    case     150 : baudr = B150;
                   break;
    case     200 : baudr = B200;
                   break;
    case     300 : baudr = B300;
                   break;
    case     600 : baudr = B600;
                   break;
    case    1200 : baudr = B1200;
                   break;
    case    1800 : baudr = B1800;
                   break;
    case    2400 : baudr = B2400;
                   break;
    case    4800 : baudr = B4800;
                   break;
    case    9600 : baudr = B9600;
                   break;
    case   19200 : baudr = B19200;
                   break;
    case   38400 : baudr = B38400;
                   break;
    case   57600 : baudr = B57600;
                   break;
    case  115200 : baudr = B115200;
                   break;
    case  230400 : baudr = B230400;
                   break;
    #ifndef __APPLE__
    case  460800 : baudr = B460800;
                   break;
    case  500000 : baudr = B500000;
                   break;
    case  576000 : baudr = B576000;
                   break;
    case  921600 : baudr = B921600;
                   break;
    case 1000000 : baudr = B1000000;
                   break;
    case 1152000 : baudr = B1152000;
                   break;
    case 1500000 : baudr = B1500000;
                   break;
    case 2000000 : baudr = B2000000;
                   break;
    case 2500000 : baudr = B2500000;
                   break;
    case 3000000 : baudr = B3000000;
                   break;
    case 3500000 : baudr = B3500000;
                   break;
    case 4000000 : baudr = B4000000;
                   break;
    #endif
    default      : printf("invalid baudrate\n");
                   return(1);
                   break;
  }

  int cbits=CS8,
      cpar=IGNPAR,
      bstop=0;

  if(strlen(mode) != 3)
  {
    printf("invalid mode \"%s\"\n", mode);
    return(-1);
  }

  switch(mode[0])
  {
    case '8': cbits = CS8;
              break;
    case '7': cbits = CS7;
              break;
    case '6': cbits = CS6;
              break;
    case '5': cbits = CS5;
              break;
    default : printf("invalid number of data-bits '%c'\n", mode[0]);
              return(-1);
              break;
  }

  switch(mode[1])
  {
    case 'N':
    case 'n': cpar = IGNPAR;
              break;
    case 'E':
    case 'e': cpar = PARENB;
              break;
    case 'O':
    case 'o': cpar = (PARENB | PARODD);
              break;
    default : printf("invalid parity '%c'\n", mode[1]);
              return(-1);
              break;
  }

  switch(mode[2])
  {
    case '1': bstop = 0;
              break;
    case '2': bstop = CSTOPB;
              break;
    default : printf("invalid number of stop bits '%c'\n", mode[2]);
              return(-1);
              break;
  }

/*
http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html

http://man7.org/linux/man-pages/man3/termios.3.html
*/

  /*Cport[comport_number]*/comport = open(/*comports[comport_number]*/comport_name, O_RDWR | O_NOCTTY | O_NDELAY);
  if(/*Cport[comport_number]*/comport == -1)
  {
    perror("unable to open comport ");
    return(-1);
  }

  error = tcgetattr(/*Cport[comport_number]*/comport, old_port_settings + /*comport_number*/comport);
  if(error==-1)
  {
    close(/*Cport[comport_number]*/comport);
    perror("unable to read portsettings ");
    return(-1);
  }
  memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

  new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
  new_port_settings.c_iflag = IGNPAR;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
  new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

  cfsetispeed(&new_port_settings, baudr);
  cfsetospeed(&new_port_settings, baudr);

  error = tcsetattr(/*Cport[comport_number]*/comport, TCSANOW, &new_port_settings);
  if(error==-1)
  {
    close(/*Cport[comport_number]*/comport);
    perror("unable to adjust portsettings ");
    return(-1);
  }

  if(ioctl(/*Cport[comport_number]*/comport, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
    return(-1);
  }

  status |= TIOCM_DTR;    /* turn on DTR */
  status |= TIOCM_RTS;    /* turn on RTS */

  if(ioctl(/*Cport[comport_number]*/comport, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
    return(-1);
  }

  return comport;
}


int RS232_PollComport(int comport_number, unsigned char *buf, int size) {
    return (int)read(/*Cport[comport_number]*/comport_number, buf, size);
}


int RS232_SendByte(int comport_number, unsigned char byte)
{
  int n;

  n = (int)write(/*Cport[comport_number]*/comport_number, &byte, 1);
  if(n<0)  return(1);

  return(0);
}


int RS232_SendBuf(int comport_number, unsigned char *buf, int size)
{
  return((int)write(/*Cport[comport_number]*/comport_number, buf, size));
}


void RS232_CloseComport(int comport_number)
{
  int status;

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */
  status &= ~TIOCM_RTS;    /* turn off RTS */

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }

  tcsetattr(/*Cport[comport_number]*/comport_number, TCSANOW, old_port_settings + comport_number);
  close(/*Cport[comport_number]*/comport_number);
}

/*
Constant  Description
TIOCM_LE        DSR (data set ready/line enable)
TIOCM_DTR       DTR (data terminal ready)
TIOCM_RTS       RTS (request to send)
TIOCM_ST        Secondary TXD (transmit)
TIOCM_SR        Secondary RXD (receive)
TIOCM_CTS       CTS (clear to send)
TIOCM_CAR       DCD (data carrier detect)
TIOCM_CD        see TIOCM_CAR
TIOCM_RNG       RNG (ring)
TIOCM_RI        see TIOCM_RNG
TIOCM_DSR       DSR (data set ready)

http://man7.org/linux/man-pages/man4/tty_ioctl.4.html
*/

int RS232_IsDCDEnabled(int comport_number)
{
  int status;

  ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status);

  if(status&TIOCM_CAR) return(1);
  else return(0);
}

int RS232_IsCTSEnabled(int comport_number)
{
  int status;

  ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status);

  if(status&TIOCM_CTS) return(1);
  else return(0);
}

int RS232_IsDSREnabled(int comport_number)
{
  int status;

  ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status);

  if(status&TIOCM_DSR) return(1);
  else return(0);
}

void RS232_enableDTR(int comport_number)
{
  int status;

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status |= TIOCM_DTR;    /* turn on DTR */

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}

void RS232_disableDTR(int comport_number)
{
  int status;

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}

void RS232_enableRTS(int comport_number)
{
  int status;

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status |= TIOCM_RTS;    /* turn on RTS */

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}

void RS232_disableRTS(int comport_number)
{
  int status;

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_RTS;    /* turn off RTS */

  if(ioctl(/*Cport[comport_number]*/comport_number, TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}


#endif
