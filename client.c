#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include "hex_info.h"
#include "user_define.h"
#include "tcp_config.h"
#include "rtu_config.h"

extern int device[]; //config_setting.c
extern int total_pthread; //config_stting.c

int sidsSock[DEVICE_COUNT];

tcp_struct *moaTcpStruct;
tcp_struct *sidsTcpStruct;
client_parm *sids_client_parm;
client_parm *moa_client_parm;

extern int port1_client[MAX_COUNT][8]; //rtu_config.c
extern int port2_client[MAX_COUNT][8];
extern int port3_client[MAX_COUNT][8];
extern int port4_client[MAX_COUNT][8];
extern int port5_client[MAX_COUNT][8];
extern int port6_client[MAX_COUNT][8];
extern int port7_client[MAX_COUNT][8];
extern int port8_client[MAX_COUNT][8];

extern int port1_cnt;
extern int port2_cnt;
extern int port3_cnt;
extern int port4_cnt;
extern int port5_cnt;
extern int port6_cnt;
extern int port7_cnt;
extern int port8_cnt;
extern port_struct ps[8];

void *moaStartTcpClient(void* parm);
void *sidsStartTcpClient(void* parm);
rtuParm parm[8];

int interval;

int main(int argc,  char *argv[])
{

/************************************** SHARED MEMORY *************************************************/ 
	int shmid;
	void* shmaddr;
//	struct shmid_ds shm_stat;

	if((shmid=shmget((key_t)3131, sizeof(unsigned short) * 0xFFFF, IPC_CREAT|0666)) == -1)
	{
		perror("shmid failed");
		exit(1);
	}
	
    if((shmaddr=shmat(shmid, (void*)0,0))==(void*)-1)
	{
		perror("shmat failed");
		exit(1);
	}
	holdingResisters = (unsigned short*)shmaddr;
/***************************************SHARED MEMORY********************************************************/



/*************************************** TCP MODBUS  ***************************************************/    

   if((argc==2) && (strcmp(argv[1], "r")==0))
    {
        memset(holdingResisters, 0, sizeof(unsigned short)* 0xFFFF);
    }
    
    int result;		 
    memset(sidsSock, 0, sizeof(sidsSock));
    result = checkTcpConfigFile(); // -1:Error, 1:sids, 2:moa, 3:Error(There are two config files(sids, moa))
    if(result == 1)
	{
		getSidsTcpConfig(); //config_setting.c

		if(device[1]==0)
		{	
			printf("%s\n", "There are no information in tcp_client.txt");  
			exit(1);
		}
		
		sidsThread = malloc(sizeof(pthread_t) * total_pthread); //create thread for modbus device
		sids_client_parm  = malloc(sizeof(client_parm)*total_pthread);
		int i=0;

		for(i=0;i<total_pthread;i++)
		{
			strcpy(sids_client_parm[i].ip, sidsTcpStruct[device[i]].ip);
			strcpy(sids_client_parm[i].port, sidsTcpStruct[device[i]].port);		
			if(device[i+1] == 9999)
			{
				sids_client_parm[i].fromNum = device[i];
				sids_client_parm[i].toNum = tcp_lineCount;
			}
			else
			{
				sids_client_parm[i].fromNum = device[i];
				sids_client_parm[i].toNum = device[i+1];
			}
			
			sids_client_parm[i].sock = i;
			pthread_create(&sidsThread[i], NULL, sidsStartTcpClient, (void*)(&sids_client_parm[i]));
			pthread_detach(sidsThread[i]);

			usleep(100*1000);
		}
	
	}
	else if(result == 2)
	{
	 getMoaTcpConfig();		
	 if(moaDeviceCount > 0)
		{
			int num = 0;
			int i=0, startAdr=0;
			char ip[20];
			strcpy(ip, moaTcpStruct[0].ip);
			moa_client_parm = malloc(sizeof(client_parm) * moaDeviceCount);
			
			for(i=0;i<moaTcpLineCount;i++)
			{
			    moa_client_parm[num].fromNum = startAdr;				
				moa_client_parm[num].sock = num;
				if(i==moaTcpLineCount-1)
				{
					moa_client_parm[num].toNum = i+1;
				}
				else
				{
					moa_client_parm[num].toNum = i;
				}
			

				if(strcmp(moaTcpStruct[i].ip, ip) != 0)
				{	
					strcpy(moa_client_parm[num].ip,  moaTcpStruct[i-1].ip);
					strcpy(moa_client_parm[num].port, moaTcpStruct[i-1].port);			
					pthread_create(&moaThread[num], NULL, moaStartTcpClient, (void*)(&moa_client_parm[num]));				
					pthread_detach(moaThread[num]);	
					strcpy(ip, moaTcpStruct[i].ip);					
					startAdr = i;
					num++;
				}				
				
				if(i==moaTcpLineCount-1)
				{					
					strcpy(moa_client_parm[num].ip,  moaTcpStruct[i].ip);
					strcpy(moa_client_parm[num].port, moaTcpStruct[i].port);					
					pthread_create(&moaThread[num], NULL, moaStartTcpClient, (void*)(&moa_client_parm[num]));				
					pthread_detach(moaThread[num]);
				}

			usleep(100*1000);	
			}		
		}

	}
	else
	{
		puts("There is an Error, Check Tcp Config files");
	//	return -1;
	}	

    
    pause();
	free(holdingResisters);
    return 0;
}

void *sidsStartTcpClient(void *parm)
{
    int i=0; int j=0;
    client_parm * p1 = (client_parm*)parm;
    struct sockaddr_in serv_addr;    
    unsigned short TransID = 0;
    char send_buffer[12];
    char recv_buffer[BUFSIZ];
    int errCheck = 0;
    char connectionState = 0; // 0:disconnected, 1:connected
    int startCount=0;
    int optval;
    socklen_t optlen = sizeof(optval);

	interval = getIntervalTime();
    
SIDS_ERROR:
    sidsSock[p1->sock]=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(p1->ip);
    serv_addr.sin_port=htons(atoi(p1->port));		
    memset(send_buffer, 0, sizeof(send_buffer));
    memset(recv_buffer, 0, sizeof(recv_buffer));


    if (connect(sidsSock[p1->sock], (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    {
        close(sidsSock[p1->sock]); 
        sleep(1);
        writeErrLog(sidsTcpStruct[p1->fromNum].ip, errno, 1);
        connectionState = 0;

		if(startCount > 0)
		{
		  goto SIDS_ERROR;
		}
    }
    else
    {  
       connectionState = 1;
    }

        startCount = 1;
	errCheck   = 0;
	int wRtn   = 0;
	int rRtn   = 0;

	while(1)
	{
        if(errCheck)
        {
         goto SIDS_ERROR;
        }

		for(i=p1->fromNum; i<p1->toNum ; i++)
		{	
		 
		 if(*(holdingResisters+9980)==2) 
		   {
			  int i;
			  for(i=0;i<tcp_lineCount;i++)
			  {
				if(sidsTcpStruct[i].err_addr == *(holdingResisters+9981))
				{
				  sidsTcpStruct[i].jump = *(holdingResisters+9982);
				}
			  }

			  *(holdingResisters+9980)=0;
			  *(holdingResisters+9981)=0;
			  *(holdingResisters+9982)=0;
		   }
		 
			if(sidsTcpStruct[i].jump == 1) continue;

		    if(TransID > 65536) TransID = 0;
			int getCount = sidsTcpStruct[i].gw_end - sidsTcpStruct[i].gw_start + 1;
	
			send_buffer[0]  = (char)(TransID >>8);
			send_buffer[1]  = (char)TransID;
			send_buffer[2]  = 0x00;
			send_buffer[3]  = 0x00;
			send_buffer[4]  = 0x00;
			send_buffer[5]  = 0x06;
			send_buffer[6]  = 0x01;
			send_buffer[7]  = sidsTcpStruct[i].fn_code;
			send_buffer[8]  = (char)((sidsTcpStruct[i].slave_start-1) >> 8);
			send_buffer[9]  = (char)(sidsTcpStruct[i].slave_start -1);
			send_buffer[10] = (char)(getCount >> 8);
			send_buffer[11] = (char)getCount;
               
		    wRtn = send(sidsSock[p1->sock], send_buffer, sizeof(send_buffer)/sizeof(char),0);
	              
            if((wRtn == -1) || (wRtn==0)) 
		  	{	
			    printf("%s, %s \n", sidsTcpStruct[i].ip, "send error");
				close(sidsSock[p1->sock]);
				sleep(1);
			    writeErrLog(sidsTcpStruct[i].ip, errno, 2);	
			    errCheck++;
			}
		
			rRtn = recv(sidsSock[p1->sock],  recv_buffer, sizeof(recv_buffer)/sizeof(char),0);
		        
            if((rRtn == -1) || (rRtn==0))
			{                         
			   printf("%s, %s \n", sidsTcpStruct[i].ip,"recv error");
			   close(sidsSock[p1->sock]);
			   sleep(1);
			   writeErrLog(sidsTcpStruct[i].ip, errno, 2);	
               errCheck++;
			}
			else if((rRtn>0) && (rRtn<=9))
			{                         
			   writeErrLog(sidsTcpStruct[i].ip, errno, 4);	
			   continue;	
            }
 
 
			if(errCheck)
			{
			   int k;
			   for(k=p1->fromNum;k<p1->toNum;k++)
			   {
				// printf("%d \n", sidsTcpStruct[k].err_addr);
				*(holdingResisters + sidsTcpStruct[k].err_addr-1)	= 1;
			   }                           
				
			   goto SIDS_ERROR;
			}
           
			if((errCheck == 0) && (connectionState == 1))
            {
               if((sidsTcpStruct[i].fn_code==1)||(sidsTcpStruct[i].fn_code==2))
               { 
                    int k=0;
                    int h=0;
                    int share = getCount/8;
                    int remain = getCount%8;
                        
                    for(h=0;h<recv_buffer[8];h++)
                    {
                        if(h<share)
                        {
                            int addr = sidsTcpStruct[i].gw_start-1 + (h*8);
                            for(k=7;k>=0;k--)
                            {
                               *(holdingResisters + addr + k) = recv_buffer[9+h]>>k&1;	
                            }

                        }
                        else
                        {
                            int addr = sidsTcpStruct[i].gw_start + (8*(share)) + remain -2;
                            for(k=remain-1;k>=0; k--)                          
                            {
                               *(holdingResisters  + addr) = recv_buffer[9+h]>>k&1;	
                                addr--;
                             }
                        }
                    }

               }
               else if((sidsTcpStruct[i].fn_code==3)||(sidsTcpStruct[i].fn_code==4))
               {
                       /* for(j = 0; j < getCount*2 ; j = j+2)
                        {
						   	printf("%d ",hexToInt(recv_buffer[9+j], recv_buffer[10 +j]));
                               //*(holdingResisters + sidsTcpStruct[i].gw_start -1 + (j/2)) = hexToInt(recv_buffer[9+j], recv_buffer[10 +j]);	
						}
						printf("\n");*/
		   
                   if(sidsTcpStruct[i].swap_code == 1)
                   {
                        for(j = 0; j < getCount*2 ; j = j+2)
                        {
                               *(holdingResisters + sidsTcpStruct[i].gw_start -1 + (j/2)) = hexToInt(recv_buffer[9+j], recv_buffer[10 +j]);	
						}
                   }
                   else if(sidsTcpStruct[i].swap_code == 2) //2byte swap
                   {
                        for(j = 0; j < getCount*2 ; j = j+4)
                        {
                               *(holdingResisters + sidsTcpStruct[i].gw_start + (j/2)) = hexToInt(recv_buffer[9+j], recv_buffer[10 +j]);	
                               *(holdingResisters + sidsTcpStruct[i].gw_start-1 + (j/2)) = hexToInt(recv_buffer[11+j], recv_buffer[12 +j]);	
                        }
                        
                   }                
                   else if(sidsTcpStruct[i].swap_code == 3) // 4byte swap 
                   {
                        for(j = 0; j < getCount*2 ; j = j+8)
                        {
							   *(holdingResisters + sidsTcpStruct[i].gw_start-1 + (j/2)) = hexToInt(recv_buffer[15+j], recv_buffer[16 +j]);	
                               *(holdingResisters + sidsTcpStruct[i].gw_start + (j/2)) = hexToInt(recv_buffer[13+j], recv_buffer[14 +j]);	
                               *(holdingResisters + sidsTcpStruct[i].gw_start +1 + (j/2)) = hexToInt(recv_buffer[11+j], recv_buffer[12 +j]);	
                               *(holdingResisters + sidsTcpStruct[i].gw_start +2 + (j/2)) = hexToInt(recv_buffer[9+j], recv_buffer[10 +j]);	
                        }


                   }
               }
               
               *(holdingResisters + sidsTcpStruct[i].err_addr-1)	= 0;
            }

            TransID++;

            usleep(interval*1000);
		}
		sleep(sidsTcpStruct[0].scan_time);
        
          if(connectionState == 0)  goto SIDS_ERROR;
	}
	puts("close");
	close(sidsSock[p1->sock]);
}

void *moaStartTcpClient(void *parm)
{	
	int i=0; int j=0;
	client_parm * p1 = (client_parm*)parm;
    struct sockaddr_in serv_addr;
	char send_buffer[12];
    char recv_buffer[BUFSIZ];
	unsigned short TransID = 250;
	char connectionState = 0;  //0:disconnected, 1:connected
	int errCheck = 0;
	time_t t;
	struct tm tm;
	int startCount = 0;

	MOA_ERROR:	
	moaSock[p1->sock]=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(p1->ip);
	serv_addr.sin_port=htons(atoi(p1->port));
	
	memset(send_buffer, 0, sizeof(send_buffer));
	memset(recv_buffer, 0, sizeof(recv_buffer));
	
	if (connect(moaSock[p1->sock], (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
	{		
	//	perror("Connect error");		
		close(moaSock[p1->sock]); 
		sleep(3);
		writeErrLog(moaTcpStruct[p1->fromNum].ip, errno, 1);
		connectionState = 0;
		if(startCount>0)
		{
		  goto MOA_ERROR;
		}
	}
	else
	{
	   connectionState = 1;
	}	
	
	startCount = 0;
	errCheck   = 0;
	int wRtn   = 0;
	int rRtn   = 0;

	while(1)
	{
		if(errCheck)
		{
		   goto MOA_ERROR;  
		}

		for(i=p1->fromNum; i<p1->toNum ; i++)
		{	
			if(TransID > 65536) TransID = 0;
			int getCount = moaTcpStruct[i].gw_end - moaTcpStruct[i].gw_start + 1;

			 wRtn = 0;
	    rRtn = 0;
			//printf("%s, %d %d %d \n", moaTcpStruct[i].ip, moaTcpStruct[i].gw_start, moaTcpStruct[i].gw_end, moaTcpStruct[i].slave_start);

			memset(send_buffer, 0, sizeof(send_buffer));
			memset(recv_buffer, 0, sizeof(recv_buffer));	
	
			send_buffer[0]  = (char)(TransID >> 8);
			send_buffer[1]  = (char)TransID;
			send_buffer[2]  = 0x00;
			send_buffer[3]  = 0x00;
			send_buffer[4]  = 0x00;
			send_buffer[5]  = 0x06;
			send_buffer[6]  = 0x01;
			send_buffer[7]  = moaTcpStruct[i].fn_code;			
			send_buffer[8]  = (char)((moaTcpStruct[i].slave_start -1) >> 8);
			send_buffer[9]  = (char)(moaTcpStruct[i].slave_start -1);
			send_buffer[10] = (char)(getCount >> 8);
			send_buffer[11] = (char)getCount;

			t=(time(NULL) + (moaTcpStruct[i].time_out / 1000));
			tm = *localtime(&t);
		
		    wRtn = send(moaSock[p1->sock], send_buffer, sizeof(send_buffer)/sizeof(char),0);	
			//printf("send %s , %d, %d  \n", moaTcpStruct[i].ip, p1->sock, wRtn);
			if(wRtn < 1) 
			{	
			    //perror("send error;");
				//close(moaSock[p1->sock]); 
				sleep(3);
				writeErrLog(moaTcpStruct[i].ip, errno, 2);	
			    errCheck++;			
			}
			
			rRtn = recv(moaSock[p1->sock],  recv_buffer, sizeof(recv_buffer)/sizeof(char),0);
			//printf("recv %s , %d, %d  \n", moaTcpStruct[i].ip, p1->sock, rRtn);
			if(t<time(NULL))
			{
				writeErrLog(moaTcpStruct[i].ip, errno, 3);	
				errCheck++;					
			}

			if(rRtn < 1) 
			{
				//perror("receive error");
				//close(moaSock[p1->sock]); 
				sleep(3);
				writeErrLog(moaTcpStruct[i].ip, errno, 2);	
				errCheck++;					
			}
			
			if(errCheck)
			{
			   goto MOA_ERROR;
			}

			
			if((errCheck == 0) && (connectionState == 1))  
			{
			    for(j = 0; j < getCount*2 ; j = j+2)
			     {
				 	 *(holdingResisters + moaTcpStruct[i].gw_start -1 + (j/2)) = hexToInt(recv_buffer[9+j], recv_buffer[10 +j]);

				//	 if(strcmp(moaTcpStruct[i].ip, "192.168.0.135") == 0)
				//	 {
				//	   printf("%s: %d %d  ", moaTcpStruct[i].ip, hexToInt(recv_buffer[9+j], recv_buffer[10 +j]));
			//		 }
			     }
			//	printf("\n");
				*(holdingResisters + moaTcpStruct[i].err_addr-1) = 0;
			}
			else
			{
			     if(moaTcpStruct[i].reset == 1)
			     {
				  for(j = 0; j < getCount*2 ; j = j+2)
				     {
					      *(holdingResisters + moaTcpStruct[i].gw_start -1 + (j/2)) = 0;	
	      		      }
			      }
				*(holdingResisters + moaTcpStruct[i].err_addr-1) = 1;
			}

			TransID++;
			usleep(moaTcpStruct[i].scan_time * 1000);						
		}
		if(connectionState==0) goto MOA_ERROR;
		sleep(1);
	}
	close(moaSock[p1->sock]);
	free(moaThread);
}







