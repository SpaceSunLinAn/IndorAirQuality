#include "uart.h"
#include "stdio.h"
#include "unistd.h"
struct  _air_data {
    int _iaq;
    float _tvoc;
    float _eco2;
    float _rmox;
};

struct _air_data air_data;

int main(int argc, char **argv)
{
    unsigned char data;
     Uart  win_uart;
    int ret;
    if( (ret = win_uart.open_port("/dev/ttyS0")) < 0)  {
        printf("open_port error \n");
        return -1;
    }
    if(( win_uart.set_opt(115200,8,'N',1)) < 0)
    {
        printf("set_opt error0\n");
        return -1;
    }
    while(1) {
//        usleep(10);
        ret = win_uart.tty_read( &data, 1);
        printf("%c",data );
    }
    win_uart.close_port();
    return 0;  
}

