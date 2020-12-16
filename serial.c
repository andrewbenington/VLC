#include <errno.h>
#include <fcntl.h> 
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "serial.h"

bool connected=false;
int fd;
char *portname;

int set_interface_attribs (int fd, int speed, int parity)
{
        connected=false;
        struct termios tty;
        struct termios tty_old;
        memset(&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("PC: Error %d from tcgetattr  \n", errno);
                return -1;
        }
        tty_old = tty;
        
        cfsetospeed (&tty, (speed_t)speed);
        cfsetispeed (&tty, (speed_t)speed);
        
        //tty.c_cflag |= B19200;
        tty.c_cflag     &=  ~PARENB;            // Make 8n1
        tty.c_cflag     &=  ~CSTOPB;
        tty.c_cflag     &=  ~CSIZE;
        tty.c_cflag     |=  CS8;
        
        tty.c_cflag     &=  ~CRTSCTS;           // no flow control
        tty.c_cc[VMIN]   =  1;                  // read doesn't block
        tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
        tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
        cfmakeraw(&tty);
        tcflush( fd, TCIFLUSH );
        
        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf("PC: Error %d from tcsetattr  \n", errno);
                return -1;
        }
        return 0;
}



void set_blocking(int fd, int should_block)
{
        struct termios tty;
        memset(&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("PC: Error %d from tggetattr  \n", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 10;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf("PC: Error %d setting term attributes  \n", errno);
}


int OpenSerialPort(char * port)
{
    printf("PC: Opening port to table \n");
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    usleep(2000000);
    if (fd < 0)
    {
        printf("PC: Error %d opening %s: %s  \n", errno, portname, strerror(errno));
        return -1;
    }
    
    set_interface_attribs(fd, B19200, 0);  // set speed to 19,200 bps, 8n1 (no parity)
    set_blocking(fd, 0);                // set no blocking
    printf("PC: Connected\n");
    connected = true;
    return fd;
}

int serialport_init(const char* serialport, int baud)

{

   struct termios toptions;

   int fd;

   

   //fprintf(stderr,"init_serialport: opening port %s @ %d bps\n",

   //        serialport,baud);



   //fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);

   fd = open(serialport, O_RDWR | O_NOCTTY);

   if (fd == -1)  {

       perror("init_serialport: Unable to open port ");

       return -1;

   }

   

   if (tcgetattr(fd, &toptions) < 0) {

       perror("init_serialport: Couldn't get term attributes");

       return -1;

   }

   speed_t brate = baud; // let you override switch below if needed

   switch(baud) {

   case 4800:   brate=B4800;   break;

   case 9600:   brate=B9600;   break;

// if you want these speeds, uncomment these and set #defines if Linux

//#ifndef OSNAME_LINUX

//    case 14400:  brate=B14400;  break;

//#endif

   case 19200:  brate=B19200;  break;

//#ifndef OSNAME_LINUX

//    case 28800:  brate=B28800;  break;

//#endif

   //case 28800:  brate=B28800;  break;

   case 38400:  brate=B38400;  break;

   case 57600:  brate=B57600;  break;

   case 115200: brate=B115200; break;

   }

   cfsetispeed(&toptions, brate);

   cfsetospeed(&toptions, brate);



   // 8N1

   toptions.c_cflag &= ~PARENB;

   toptions.c_cflag &= ~CSTOPB;

   toptions.c_cflag &= ~CSIZE;

   toptions.c_cflag |= CS8;

   // no flow control

   toptions.c_cflag &= ~CRTSCTS;



   toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines

   toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl



   toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw

   toptions.c_oflag &= ~OPOST; // make raw



   // see: http://unixwiz.net/techtips/termios-vmin-vtime.html

   toptions.c_cc[VMIN]  = 0;

   toptions.c_cc[VTIME] = 20;

   

   if( tcsetattr(fd, TCSANOW, &toptions) < 0) {

       perror("init_serialport: Couldn't set term attributes");

       return -1;

   }



   return fd;

}