#ifndef _TCP_CONFIG_H_
#define _TCP_CONFIG_H_

#define DEVICE_COUNT 200

int checkTcpConfigFile();// Check if there is a config file...
void getSidsTcpConfig();
void getMoaTcpConfig();
void writeErrLog(char* myIp, int Errnnum, int gubun);
int getIntervalTime();
struct tm* nowTime();


int tcp_lineCount;
int rtu_lineCount;
int tcp_total_addr;
int rtu_total_addr;



#endif
