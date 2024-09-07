#include "myhdr.h"
#include "ntools.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ini.h>
#include <string.h>

extern modbus_t modBus[MAX_MODBUS_DEVICES];
extern unsigned int NoDevices;
static int txFlag=0;

void rx(int sockfd) {
    char buff[9+64];
    unsigned char deviceAddr;
    unsigned char retByteCount;
    unsigned char retDataNo;
    while(1) {
        bzero(buff, 73);
        if (readn(sockfd, buff, 9+16*4)==0){ //16port
			perror("server terminated");
			exit(0);
		}        
        deviceAddr = buff[5];
        retByteCount = buff[8];
        retDataNo = retByteCount/4;
        unsigned char offset = 9;

        for(int i=0;i<retDataNo;i++) 
            printf("Port %d => %fA", i, *((float *)&buff[offset+i]));
    }
}

void tx(int sockfd) {
    txFrame_t tx_packet;
    while(1) {
        if (txFlag==1) {
            //get float
            for(int i=0;i<NoDevices;i++) {
                tx_packet.taskProcLabel[0]=0x0;
                tx_packet.taskProcLabel[1]=0x1;
                tx_packet.protocol = 0x0;
                tx_packet.len = htons(0x6);
                tx_packet.deviceAddr = modBus[i].deviceAddr;
                tx_packet.command = 0x3;
                tx_packet.regAddrStart = htons(modBus[i].regAddrStart);
                tx_packet.regNums = htons(modBus[i].noPorts*2);
                writen(sockfd, &tx_packet, sizeof(tx_packet));
            }
            txFlag = 0;
        }
    }
}

int main(int argc, char **argv) {
    int i, sockfd[MAX_MODBUS_DEVICES];
   
    getParamers();
    lineNotify("你好，我來自Linux modbus client"); 
    exit(0);
    for(int device = 0; device<NoDevices; i++) {
        if ( (sockfd[device] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket error");
            exit(0);
        }
        struct sockaddr_in *servaddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        bzero(servaddr, sizeof(struct sockaddr_in));
        servaddr->sin_family = AF_INET;
        servaddr->sin_addr.s_addr = inet_addr(modBus[device].ip);
        servaddr->sin_port = htons(modBus[device].port);

        if (connect(sockfd[device], (struct sockaddr *) servaddr, sizeof(struct sockaddr)) < 0) {
            fprintf(stderr, "modbus no %d, ip=%s, port=%d connect error\n", device, modBus[device].ip, modBus[device].port);
        }
        free(servaddr);
        pid_t child[2];
        child[0] = fork();
        if (child[0]==0) {
            tx(sockfd[device]);
        } else {
            child[1]=fork();
            if (child[1]==0) {
                rx(sockfd[device]);
            }
            close(sockfd[device]);
        }
    }
    exit(0);
}
        