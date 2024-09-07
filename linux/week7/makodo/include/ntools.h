#include "myhdr.h"
#define MAXLINE 1024
#define MAX_MODBUS_DEVICES 32

typedef struct modbus {
    unsigned char deviceAddr;
    unsigned short regAddrStart;
    char ip[16];
    unsigned short port;
    unsigned char noPorts;
    unsigned int  enPorts;
} modbus_t;


typedef struct txFrame {
    char taskProcLabel[2];
    unsigned short protocol;  //0000 = modbus protocol
    unsigned short len;   //6bytes=len(deviceAddr to regNums) 
    unsigned char deviceAddr;
    unsigned char command;  //03 = read
    unsigned short regAddrStart; 
    unsigned short regNums; //32bits = 2 regs, 16port=16x2 regs
} txFrame_t;


void getParamers();
void lineNotify(char *message);
ssize_t readn(int fd, void *buff, size_t n);
ssize_t writen(int fd, const void *buff, size_t n);
ssize_t readline(int fd, void *buff, size_t maxlen);

