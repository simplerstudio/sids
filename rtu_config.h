#ifndef _RTU_CONFIG_H_
#define _RTU_CINFIG_H_

#ifndef MAX_COUNT 
#define MAX_COUNT 200
#endif

#ifndef MOXA 
#define MOXA 0X400
#endif

#ifndef MOXA_SET_OP_MODE
#define MOXA_SET_OP_MODE (MOXA + 66)
#endif

#ifndef MOXA_GET_OP_MODE 
#define MOXA_GET_OP_MODE (MOXA + 67)
#endif

#define RS232_MODE 0
#define RS485_2WIRE_MODE 1
#define RS422_MODE 2
#define RS485_4WIRE_MODE 3
#define NOT_SET_MODE 4
#define TOKEN " "

int changePortType(char *port_name, char *port_type);
int changePortProperty(char *port_name, char *speed, int data_bit, char *parity, int stop_bit);
int sids_rtu_client_reg(void);
int moa_rtu_client_reg(void);
int calc_crc(char trame[], int n);
int check_recv_crc(char trame[], int n);
void *rtu_read_port(void * parm);
void writeRtuErrlog(int port_num, int addr, int fn_code, int slave_start, int gubun);
int checkRtuConfigFile(void);
int sids_port_setting(void);
int moa_port_setting(void);
int ceckProtSettingFile(void);
#endif
