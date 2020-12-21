#ifndef _USER_DEFINE_H_
#define _USER_DEFINE_H_

#include <pthread.h>
#include "rtu_config.h"
#define TOKEN " "
#define DEVICE_COUNT 200

unsigned short moaDeviceCount;
pthread_t *moaThread;
pthread_t *sidsThread;
pthread_t rtu_thread[8];

int *moaSock;
int moaTcpLineCount;

typedef struct client_parm{
	char ip[20];
	char port[10];
	unsigned short fromNum;
	unsigned short toNum;
	unsigned short sock;
}client_parm;

typedef struct tcp_struct{
	char ip[20];
	char port[5];
	char idx;
	char fn_code;
	unsigned short err_addr;
	unsigned short gw_start;
	unsigned short gw_end;
	unsigned short slave_start;
	int scan_time;
	int time_out;
	char reset;
	char swap_code;
	char jump;
}tcp_struct;

typedef struct port_struct{
	char comm_type;
	int baud_rate;
	char data_bit;
	char parity;
	char stop_bit;
	char flow_control;	
	unsigned short interval;
	unsigned short time_out;
}port_struct;

typedef struct mbeclinet_struct
{
	char ip[20];
	char port[5];
	char idx;
	unsigned short err_addr;
	int scan_time;
	int time_out;
	char reset; 
}mbeclient_struct;

typedef struct rtuParm{
    int portNum;
    int (*portClient)[8];
    int timeout;
}rtuParm;


tcp_struct *moaTcpStruct;
tcp_struct *sidsTcpStruct;


unsigned short* holdingResisters;
#endif
