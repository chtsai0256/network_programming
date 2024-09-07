#include <myhdr.h>
#include <ntools.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ini.h>


modbus_t modBus[MAX_MODBUS_DEVICES];
unsigned int NoDevices;
char token[100]={0};

void getParamers() {
    
    ini_t *config = ini_load("/home/robert/2023/socket_programming/linux/week7/makodo/config.ini");
    char variable[20];
    char enPortList[100];
    char regAddr[5]={0};
    int base = 1;

    ini_sget(config, "main", "NoDevices", "%d", &NoDevices);
    ini_sget(config, "LINE", "TOKEN", "%s", token);
    for (int i=0;i<NoDevices;i++) {
        sprintf(variable,"modbus_%d",i+1);
        ini_sget(config,variable,"deviceAddr", "%d", &(modBus[i].deviceAddr));
        ini_sget(config,variable,"regAddrStart", "%s", regAddr);
        for(int j=strlen(regAddr)-1;j>=0;j--) {
            modBus[i].regAddrStart += (regAddr[j]-'0')*base;
            base = base * 16;
        }
        ini_sget(config,variable,"ip", "%s", modBus[i].ip);
        ini_sget(config,variable,"port", "%d", &(modBus[i].port));
        ini_sget(config,variable,"NoPorts", "%d", &(modBus[i].noPorts));
        ini_sget(config,variable,"enPorts", "%s", enPortList);
        char *value = enPortList;
        char *portNo;
        while((portNo = strsep(&value,","))) {
            int bit = atoi(portNo)-1;
            modBus[i].enPorts |= 1 << bit;
        }       
    }
}

