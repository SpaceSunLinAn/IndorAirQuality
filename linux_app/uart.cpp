#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>
#include <termios.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <asm/param.h>
#include <stdlib.h>
#include "uart.h"

//打开串口
int Uart::open_port(const char *comport )
{
//    if( opened = 1)
  //      return fd;
    opened = 0;
    fd = open(comport, O_RDWR|O_NOCTTY|O_NDELAY );
    if (-1 == fd) {
        printf("Can't Open %s \n",comport);
        return(-1);
    }
    //恢复串口的状态为阻塞状态，用于等待串口数据的读入
    if(fcntl(fd, F_SETFL, 0) < 0)
        printf("fcntl failed!\n");
    else
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
    //测试打开的文件描述符是否引用一个终端设备，以进一步确认串口是否正确打开
    if( isatty(STDIN_FILENO) == 0 ) {
        printf("standard input is not a terminal device\n");
        return -1;
    } 
    opened = 1;
    return fd;
}

//设置串口参数
int Uart::set_opt(int nSpeed, int nBits, char nEvent, int nStop)  
{  
    struct termios newtio,oldtio;  
    if  ( tcgetattr( fd,&oldtio)  !=  0) {  
        perror("SetupSerial 1");  
        return -1;  
    }  
    bzero( &newtio, sizeof( newtio ) ); 
    newtio.c_cflag  |=  CLOCAL | CREAD; //CLOCAL:忽略modem控制线  CREAD：打开接受者  
    newtio.c_cflag &= ~CSIZE; //字符长度掩码。取值为：CS5，CS6，CS7或CS8  

    switch( nBits )  
    {  
    case 7:  
        newtio.c_cflag |= CS7;  
        break;  
    case 8:  
        newtio.c_cflag |= CS8;  
        break;  
    }  

    switch( nEvent )  
    {  
    case 'O':  
        newtio.c_cflag |= PARENB; //允许输出产生奇偶信息以及输入到奇偶校验  
        newtio.c_cflag |= PARODD;  //输入和输出是奇及校验  
        newtio.c_iflag |= (INPCK | ISTRIP); // INPACK:启用输入奇偶检测；ISTRIP：去掉第八位  
        break;  
    case 'E':  
        newtio.c_iflag |= (INPCK | ISTRIP);  
        newtio.c_cflag |= PARENB;  
        newtio.c_cflag &= ~PARODD;  
        break;  
    case 'N':   
        newtio.c_cflag &= ~PARENB;  
        break;  
    }  

    switch( nSpeed )  
    {  
    case 2400:  
        cfsetispeed(&newtio, B2400);  
        cfsetospeed(&newtio, B2400);  
        break;  
    case 4800:  
        cfsetispeed(&newtio, B4800);  
        cfsetospeed(&newtio, B4800);  
        break;  
    case 9600:  
        cfsetispeed(&newtio, B9600);  
        cfsetospeed(&newtio, B9600);  
        break;  
    case 115200:  
        cfsetispeed(&newtio, B115200);  
        cfsetospeed(&newtio, B115200);  
        break;  
    case 1500000:  
        cfsetispeed(&newtio, B1500000);  
        cfsetospeed(&newtio, B1500000);  
        break;  
    case 460800:  
        cfsetispeed(&newtio, B460800);  
        cfsetospeed(&newtio, B460800);  
        break;  
    default:  
        cfsetispeed(&newtio, B9600);  
        cfsetospeed(&newtio, B9600);  
        break;  
    }  

    if( nStop == 1 )  
        newtio.c_cflag &=  ~CSTOPB; //CSTOPB:设置两个停止位，而不是一个  
    else if ( nStop == 2 )  
        newtio.c_cflag |=  CSTOPB;  

    newtio.c_cc[VTIME]  = 0; //VTIME:非cannoical模式读时的延时，以十分之一秒位单位  
    newtio.c_cc[VMIN] = 0; //VMIN:非canonical模式读到最小字符数  
    tcflush(fd,TCIFLUSH); // 改变在所有写入 fd 引用的对象的输出都被传输后生效，所有已接受但未读入的输入都在改变发生前丢弃。  
    if((tcsetattr(fd,TCSANOW,&newtio))!=0) //TCSANOW:改变立即发生  
    {  
        perror("com set error");  
        return -1;  
    }  
    return 0;  
}  

int Uart::tty_read(unsigned char *buf, int size){
    if(!opened )
        return -1;
    int num =0, sum = 0;
    while((size -sum) > 0) {
        num = read(fd, buf, size - sum);
        usleep(100);
        sum = sum +num;
    }
    return num;
}

int Uart::tty_write( unsigned char *buf, int size ) {
    if(!opened )
        return -1;
    int num = 0, sum = 0;//sum 是一共写入串口中的字节数
    while((size - sum) > 0) {
        num = write(fd, buf + sum, size-sum);
        sum = sum + num;
    }

    for(int m=0; m < 46;m++){                                                              
//        printf("-%u-",*(buf + m));                                                                                        
    } 
    return num;
}

void Uart::close_port() {
    close(fd);
}


