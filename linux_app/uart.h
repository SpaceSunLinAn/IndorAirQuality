#ifndef UART_H
#define UART_H


class Uart {
    public:
        int open_port(const char *comport );
        void close_port();
        int set_opt(int nSpeed, int nBits, char nEvent, int nStop);
        int tty_read(unsigned char *buf, int size);
        int tty_write(unsigned char *buf, int size);
    private:
        int fd;
        int opened;
};


#endif
