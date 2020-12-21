#include <stdio.h>
#include <termios.h>
#include <sys/fcntl.h>
#include  <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>
#include "rtu_config.h"
#include "hex_info.h"
#include "user_define.h"
#include "tcp_config.h"


int port1_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port
int port2_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port
int port3_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port
int port4_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port
int port5_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port
int port6_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port
int port7_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port
int port8_client[MAX_COUNT][8]={{0},{0}}; //MAX_COUNT line per each port

int port1_cnt=0, port2_cnt=0, port3_cnt=0, port4_cnt=0; 
int port5_cnt=0, port6_cnt=0, port7_cnt=0, port8_cnt=0;

port_struct ps[8]; // sisd port setting

int checkPortSettingFile(void)
{
	FILE *fp_sids = fopen("port_setting.txt",   "r");  // read sids rtu config file	
	FILE *fp_moa  = fopen("portset.config", "r");  // read moa rtu config file
    int result;    

    if(fp_sids != NULL && fp_moa != NULL)
    {
        result = 3;
    }
    else if(fp_sids == NULL && fp_moa == NULL)
    {
        result = -1;
    }
    else if(fp_sids !=  NULL) 
    {
        result = 1;    
    }
    else if(fp_moa !=  NULL)  
    {        
        result =  2;
    }
    else
    {
        result = -1;
    }

    //fclose(fp_sids);
    //fclose(fp_moa);
    return result;	
	

}


int checkRtuConfigFile(void)
{
	FILE *fp_sids = fopen("rtu_config.txt",   "r");  // read sids rtu config file	
	FILE *fp_moa  = fopen("mbserial.config", "r");  // read moa rtu config file
    int result;    

    if(fp_sids != NULL && fp_moa != NULL)
    {
        result = 3;
    }
    else if(fp_sids == NULL && fp_moa == NULL)
    {
        result = -1;
    }
    else if(fp_sids !=  NULL) 
    {
        result = 1;    
    }
    else if(fp_moa !=  NULL)  
    {        
        result =  2;
    }
    else
    {
        result = -1;
    }

    //fclose(fp_sids);
    //fclose(fp_moa);
    return result;	
	
}

int sids_port_setting(void) {
   char * pi[6];
   char str_port[30];
   char * info_port;
   int i=0, j=0;

   FILE * fp_port = fopen("/root/port_setting.txt", "rt");
   
   if(fp_port==NULL)
   {
        puts("port_setting.txt opening is failed.....");
        return -1;
   }
   while(fgets(str_port, sizeof(str_port), fp_port) != NULL)   
   {   
//        printf("%d %d \n", !feof(fp_port), atoi(str_port));
       
       //if(!feof(fp_port)==0 && atoi(str_port)==0) break;       
       info_port = strtok(str_port, TOKEN);
       
       for(i=0;i<6;i++)       
       {       
           pi[i] = info_port;           
           info_port = strtok(NULL, TOKEN);           
       }   
       ps[j].comm_type = *pi[0];
       ps[j].baud_rate = atoi(pi[1]);
       ps[j].data_bit = *pi[2];
       ps[j].parity = *pi[3];
       ps[j].stop_bit = *pi[4];
       ps[j].flow_control = 'N';
	   ps[j].interval = 0;// not use in sids config
	   ps[j].time_out = 0;// not use in sids config


       j++;

       if(j==8) break;
       
   }
   
   return 0;
}

int moa_port_setting(void) {
   char * pi[7];
   char str_port[30];
   char * info_port;
   int i=0, j=0;

   FILE * fp_port = fopen("/root/portset.config", "rt");
   if(fp_port==NULL)
   {
        puts("portset.config  opening is failed.....");
        return -1;
   }
   while(fgets(str_port, sizeof(str_port), fp_port) != NULL)   
   {   
//        printf("%d %d \n", !feof(fp_port), atoi(str_port));
       
       //if(!feof(fp_port)==0 && atoi(str_port)==0) break;       
       info_port = strtok(str_port, ",");
       
       for(i=0;i<7;i++)       
       {       
           pi[i] = info_port;           
           info_port = strtok(NULL, ",");           
       }   
       ps[j].comm_type = *pi[0];
       ps[j].baud_rate = atoi(pi[1]);
       ps[j].data_bit  = *pi[2];
       ps[j].parity    = *pi[3];
       ps[j].stop_bit  = *pi[4];
       ps[j].flow_control = 'N';
	   ps[j].interval  = atoi(pi[5]);
	   ps[j].time_out  = atoi(pi[6]);
       j++;
       if(j==8) break;
       
   }
   
   return 0;
}

int sids_rtu_client_reg(void) {     
    char * pi[8];
    char str_port[30];
    char * info_port = NULL;
    int i=0;
    
    FILE * fp_port = fopen("/root/rtu_config.txt", "rt");
    
    if(fp_port==NULL)
    {
        puts("rtu_config.txt  opening is failed.....");
        return -1;
    }
     
    while(fgets(str_port, sizeof(str_port), fp_port) != NULL)
    {
        
        // if(atoi(str_port)==0) break;
        if(!feof(fp_port)==0 && atoi(str_port)==0) break;
        
        info_port = strtok(str_port, TOKEN);
        
        for(i=0;i<8;i++)
        {
            pi[i] = info_port;
            // printf("%s \n",  info_port);
            info_port = strtok(NULL, TOKEN);
        }
        
        
        if(atoi(pi[0])==1)
        {
            port1_client[port1_cnt][1] = atoi(pi[1]); port1_client[port1_cnt][2] = atoi(pi[2]); port1_client[port1_cnt][3] = atoi(pi[3]); port1_client[port1_cnt][4] = atoi(pi[4]);
            port1_client[port1_cnt][5] = atoi(pi[5]); port1_client[port1_cnt][6] = atoi(pi[6]); port1_client[port1_cnt][7] = atoi(pi[7]); //port1_client[port1_cnt][8] = 0;
            // printf("%d, %d\n", port1_client[port1_cnt][3], port1_client[port1_cnt][4]);
            port1_cnt++;
        }
        else if(atoi(pi[0])==2)
        {
            port2_client[port2_cnt][1] = atoi(pi[1]); port2_client[port2_cnt][2] = atoi(pi[2]); port2_client[port2_cnt][3] = atoi(pi[3]); port2_client[port2_cnt][4] = atoi(pi[4]);
            port2_client[port2_cnt][5] = atoi(pi[5]); port2_client[port2_cnt][6] = atoi(pi[6]); port2_client[port2_cnt][7] = atoi(pi[7]);// port2_client[port1_cnt][8] = 0;
            // printf("%d, %d\n", port2_client[port2_cnt][3], port2_client[port2_cnt][4]);
            port2_cnt++;
        }
        else if(atoi(pi[0])==3)
        {
            port3_client[port3_cnt][1] = atoi(pi[1]); port3_client[port3_cnt][2] = atoi(pi[2]); port3_client[port3_cnt][3] = atoi(pi[3]); port3_client[port3_cnt][4] = atoi(pi[4]);
            port3_client[port3_cnt][5] = atoi(pi[5]); port3_client[port3_cnt][6] = atoi(pi[6]); port3_client[port3_cnt][7] = atoi(pi[7]);// port3_client[port3_cnt][8] = 0;
            // printf("%d, %d\n", port3_client[port3_cnt][3], port3_client[port3_cnt][4]);
            port3_cnt++;
        }
        else if(atoi(pi[0])==4)
        {
            port4_client[port4_cnt][1] = atoi(pi[1]); port4_client[port4_cnt][2] = atoi(pi[2]); port4_client[port4_cnt][3] = atoi(pi[3]); port4_client[port4_cnt][4] = atoi(pi[4]);
            port4_client[port4_cnt][5] = atoi(pi[5]); port4_client[port4_cnt][6] = atoi(pi[6]); port4_client[port4_cnt][7] = atoi(pi[7]);// port4_client[port4_cnt][8] = 0;
            // printf("%d, %d\n", port4_client[port4_cnt][3], port4_client[port4_cnt][4]);
            port4_cnt++;
        }
        else if(atoi(pi[0])==5)
        {
            port5_client[port5_cnt][1] = atoi(pi[1]); port5_client[port5_cnt][2] = atoi(pi[2]); port5_client[port5_cnt][3] = atoi(pi[3]); port5_client[port5_cnt][4] = atoi(pi[4]);
            port5_client[port5_cnt][5] = atoi(pi[5]); port5_client[port5_cnt][6] = atoi(pi[6]); port5_client[port5_cnt][7] = atoi(pi[7]);// port5_client[port5_cnt][8] = 0;
            //printf("%d, %d\n", port5_client[port5_cnt][3], port5_client[port5_cnt][4]);
            port5_cnt++;
        }
        else if(atoi(pi[0])==6)
        {
            port6_client[port6_cnt][1] = atoi(pi[1]); port6_client[port6_cnt][2] = atoi(pi[2]); port6_client[port6_cnt][3] = atoi(pi[3]); port6_client[port6_cnt][4] = atoi(pi[4]);
            port6_client[port6_cnt][5] = atoi(pi[5]); port6_client[port6_cnt][6] = atoi(pi[6]); port6_client[port6_cnt][7] = atoi(pi[7]);// port6_client[port6_cnt][8] = 0;
            //printf("%d, %d\n", port6_client[port6_cnt][3], port6_client[port6_cnt][4]);
            port6_cnt++;
        }
        else if(atoi(pi[0])==7)
        {
            port7_client[port7_cnt][1] = atoi(pi[1]); port7_client[port7_cnt][2] = atoi(pi[2]); port7_client[port7_cnt][3] = atoi(pi[3]); port7_client[port7_cnt][4] = atoi(pi[4]);
            port7_client[port7_cnt][5] = atoi(pi[5]); port7_client[port7_cnt][6] = atoi(pi[6]); port7_client[port7_cnt][7] = atoi(pi[7]);// port7_client[port7_cnt][8] = 0;
            //printf("%d, %d\n", port7_client[port7_cnt][3], port7_client[port7_cnt][4]);
            port7_cnt++;
        }
        else if(atoi(pi[0])==8)
        {
            port8_client[port8_cnt][1] = atoi(pi[1]); port8_client[port8_cnt][2] = atoi(pi[2]); port8_client[port8_cnt][3] = atoi(pi[3]); port8_client[port8_cnt][4] = atoi(pi[4]);
            port8_client[port8_cnt][5] = atoi(pi[5]); port8_client[port8_cnt][6] = atoi(pi[6]); port8_client[port8_cnt][7] = atoi(pi[7]);// port8_client[port8_cnt][8] = 0;
            //printf("%d, %d\n", port8_client[port8_cnt][3], port8_client[port8_cnt][4]);
            port8_cnt++;
        }
        
    }
    
    return 0;
    
}

int moa_rtu_client_reg(void) {     
    char * pi[6];
    char str_port[30];
    char * info_port = NULL;
    int i=0;
    
	FILE * fp_port = fopen("/root/mbserial.config", "rt");
    
    if(fp_port==NULL)
    {
        puts("mbserial.config  opening is failed.....");
        return -1;
    }
     
    while(fgets(str_port, sizeof(str_port), fp_port) != NULL)
    {
        
        if(str_port==NULL) break;
        if(!feof(fp_port)==0 && atoi(str_port)==0) break;
        
        info_port = strtok(str_port, ",");
        
        for(i=0;i<5;i++)
        {
            pi[i] = info_port;
            
			 if(i==4)
			 {
			  info_port = strtok(pi[i], "TOKEN");
			  for(i=4;i<6;i++)
			  {
			   pi[i] = info_port;
			   info_port = strtok(NULL, "TOKEN");
			  }
			 }
			 info_port = strtok(NULL, ",");
        }
	  
        
		if(atoi(pi[2])==1)
        {
            port1_client[port1_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port1_client[port1_cnt][2] = 1; 
			   port1_client[port1_cnt][4] = atoi(pi[4])-10000;//gw start
               port1_client[port1_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port1_client[port1_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port1_client[port1_cnt][2] = 2; 
			   port1_client[port1_cnt][4] = atoi(pi[4])-20000;//gw start
               port1_client[port1_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port1_client[port1_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port1_client[port1_cnt][2] = 4; 
			   port1_client[port1_cnt][4] = atoi(pi[4])-30000;//gw start
               port1_client[port1_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port1_client[port1_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port1_client[port1_cnt][2] = 3; 
			   port1_client[port1_cnt][4] = atoi(pi[4])-40000;//gw start
               port1_client[port1_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port1_client[port1_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port1_client[port1_cnt][3] = 11000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port1_client[port1_cnt][7] = 1; //swap code
            port1_cnt++;
        }
		else if(atoi(pi[2])==2)
        {
            port2_client[port2_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port2_client[port2_cnt][2] = 1; 
			   port2_client[port2_cnt][4] = atoi(pi[4])-10000;//gw start
               port2_client[port2_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port2_client[port2_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port2_client[port2_cnt][2] = 2; 
			   port2_client[port2_cnt][4] = atoi(pi[4])-20000;//gw start
               port2_client[port2_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port2_client[port2_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port2_client[port2_cnt][2] = 4; 
			   port2_client[port2_cnt][4] = atoi(pi[4])-30000;//gw start
               port2_client[port2_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port2_client[port2_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port2_client[port2_cnt][2] = 3; 
			   port2_client[port2_cnt][4] = atoi(pi[4])-40000;//gw start
               port2_client[port2_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port2_client[port2_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port2_client[port2_cnt][3] = 12000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port2_client[port2_cnt][7] = 1; //swap code
            port2_cnt++;
        }
		else if(atoi(pi[2])==3)
        {
            port3_client[port3_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port3_client[port3_cnt][2] = 1; 
			   port3_client[port3_cnt][4] = atoi(pi[4])-10000;//gw start
               port3_client[port3_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port3_client[port3_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port3_client[port3_cnt][2] = 2; 
			   port3_client[port3_cnt][4] = atoi(pi[4])-20000;//gw start
               port3_client[port3_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port3_client[port3_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port3_client[port3_cnt][2] = 4; 
			   port3_client[port3_cnt][4] = atoi(pi[4])-30000;//gw start
               port3_client[port3_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port3_client[port3_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port3_client[port3_cnt][2] = 3; 
			   port3_client[port3_cnt][4] = atoi(pi[4])-40000;//gw start
               port3_client[port3_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port3_client[port3_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port3_client[port3_cnt][3] = 13000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port3_client[port3_cnt][7] = 1; //swap code
            port3_cnt++;
        }
		else if(atoi(pi[2])==4)
        {
            port4_client[port4_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port4_client[port4_cnt][2] = 1; 
			   port4_client[port4_cnt][4] = atoi(pi[4])-10000;//gw start
               port4_client[port4_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port4_client[port4_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port4_client[port4_cnt][2] = 2; 
			   port4_client[port4_cnt][4] = atoi(pi[4])-20000;//gw start
               port4_client[port4_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port4_client[port4_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port4_client[port4_cnt][2] = 4; 
			   port4_client[port4_cnt][4] = atoi(pi[4])-30000;//gw start
               port4_client[port4_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port4_client[port4_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port4_client[port4_cnt][2] = 3; 
			   port4_client[port4_cnt][4] = atoi(pi[4])-40000;//gw start
               port4_client[port4_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port4_client[port4_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port4_client[port4_cnt][3] = 14000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port4_client[port4_cnt][7] = 1; //swap code
            port4_cnt++;
        }
		else if(atoi(pi[2])==5)
        {
            port5_client[port5_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port5_client[port5_cnt][2] = 1; 
			   port5_client[port5_cnt][4] = atoi(pi[4])-10000;//gw start
               port5_client[port5_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port5_client[port5_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port5_client[port5_cnt][2] = 2; 
			   port5_client[port5_cnt][4] = atoi(pi[4])-20000;//gw start
               port5_client[port5_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port5_client[port5_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port5_client[port5_cnt][2] = 4; 
			   port5_client[port5_cnt][4] = atoi(pi[4])-30000;//gw start
               port5_client[port5_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port5_client[port5_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port5_client[port5_cnt][2] = 3; 
			   port5_client[port5_cnt][4] = atoi(pi[4])-40000;//gw start
               port5_client[port5_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port5_client[port5_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port5_client[port5_cnt][3] = 15000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port5_client[port5_cnt][7] = 1; //swap code
            port5_cnt++;
        }
		else if(atoi(pi[2])==6)
        {
            port6_client[port6_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port6_client[port6_cnt][2] = 1; 
			   port6_client[port6_cnt][4] = atoi(pi[4])-10000;//gw start
               port6_client[port6_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port6_client[port6_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port6_client[port6_cnt][2] = 2; 
			   port6_client[port6_cnt][4] = atoi(pi[4])-20000;//gw start
               port6_client[port6_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port6_client[port6_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port6_client[port6_cnt][2] = 4; 
			   port6_client[port6_cnt][4] = atoi(pi[4])-30000;//gw start
               port6_client[port6_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port6_client[port6_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port6_client[port6_cnt][2] = 3; 
			   port6_client[port6_cnt][4] = atoi(pi[4])-40000;//gw start
               port6_client[port6_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port6_client[port6_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port6_client[port4_cnt][3] = 16000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port6_client[port4_cnt][7] = 1; //swap code
            port6_cnt++;
        }		 
		else if(atoi(pi[2])==7)
        {
            port7_client[port7_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port7_client[port7_cnt][2] = 1; 
			   port7_client[port7_cnt][4] = atoi(pi[4])-10000;//gw start
               port7_client[port7_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port7_client[port7_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port7_client[port7_cnt][2] = 2; 
			   port7_client[port7_cnt][4] = atoi(pi[4])-20000;//gw start
               port7_client[port7_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port7_client[port7_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port7_client[port7_cnt][2] = 4; 
			   port7_client[port7_cnt][4] = atoi(pi[4])-30000;//gw start
               port7_client[port7_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port7_client[port7_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port7_client[port7_cnt][2] = 3; 
			   port7_client[port7_cnt][4] = atoi(pi[4])-40000;//gw start
               port7_client[port7_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port7_client[port7_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port7_client[port7_cnt][3] = 17000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port7_client[port7_cnt][7] = 1; //swap code
            port7_cnt++;
        }
		else if(atoi(pi[2])==8)
        {
            port8_client[port8_cnt][1] = atoi(pi[3]); //index number
			
			if(atoi(pi[0])>10000 && atoi(pi[0])<20000) //fn code
			{
			   port8_client[port8_cnt][2] = 1; 
			   port8_client[port8_cnt][4] = atoi(pi[4])-10000;//gw start
               port8_client[port8_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -10000; // gw_end 
			   port8_client[port8_cnt][6] = atoi(pi[0])-10000; // slave start
			}
			else if(atoi(pi[0])>20000 && atoi(pi[0])<30000)
			{
			   port8_client[port8_cnt][2] = 2; 
			   port8_client[port8_cnt][4] = atoi(pi[4])-20000;//gw start
               port8_client[port8_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -20000; // gw_end 
			   port8_client[port8_cnt][6] = atoi(pi[0])-20000; // slave start
			}
			else if(atoi(pi[0])>30000 && atoi(pi[0])<40000)
			{
			   port8_client[port8_cnt][2] = 4; 
			   port8_client[port8_cnt][4] = atoi(pi[4])-30000;//gw start
               port8_client[port8_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -30000; // gw_end 
			   port8_client[port8_cnt][6] = atoi(pi[0])-30000; // slave start
			}
			else if(atoi(pi[0])>40000 && atoi(pi[0])<50000)
			{
			   port8_client[port8_cnt][2] = 3; 
			   port8_client[port8_cnt][4] = atoi(pi[4])-40000;//gw start
               port8_client[port8_cnt][5] = atoi(pi[4]) +  atoi(pi[1]) - atoi(pi[0]) -40000; // gw_end 
			   port8_client[port8_cnt][6] = atoi(pi[0])-40000; // slave start
			}
			port8_client[port8_cnt][3] = 18000 + atoi(pi[3]); // Error address (default 10000) + (port number * 1000) + index number 					
			port8_client[port8_cnt][7] = 1; //swap code
            port8_cnt++;
        }
		 
        
    }
    
    return 0;
    
}

void *rtu_read_port(void * parm)
{
    rtuParm *p1 = (rtuParm*)parm;

    int timeout = p1->timeout;
    char trame[256], buf[1024];
    int fd, res, count;
    struct termios oldtio,newtio;
    int modemctlline;
    int txemptystate;
    int i,j;
    struct timeval tv; 
    fd_set readfds;
    time_t t;
    struct tm tm;
    int crcLen, loopCount;  


    if(p1->portNum == 1)
        fd = open( "/dev/ttyMI0", O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if(p1->portNum == 2)        
        fd = open( "/dev/ttyMI1", O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if(p1->portNum == 3)        
        fd = open( "/dev/ttyMI2", O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if(p1->portNum == 4)        
        fd = open( "/dev/ttyMI3", O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if(p1->portNum == 5)        
        fd = open( "/dev/ttyMI4", O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if(p1->portNum == 6)        
        fd = open( "/dev/ttyMI5", O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if(p1->portNum == 7)        
        fd = open( "/dev/ttyMI6", O_RDWR | O_NOCTTY | O_NONBLOCK);
    else if(p1->portNum == 8)        
        fd = open( "/dev/ttyMI7", O_RDWR | O_NOCTTY | O_NONBLOCK);
    
    if (fd <0)
    {
        printf( "Device OPEN FAIL\n");
    }
    
    tcgetattr(fd,&oldtio); 
    memset(&newtio, 0, sizeof(newtio));
    
    if(ps[p1->portNum-1].baud_rate == 9600)
    {  
        newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    }
    else if(ps[p1->portNum-1].baud_rate == 19200)
    {
        newtio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;     
    }
    else if(ps[p1->portNum-1].baud_rate == 38400)
    {
        newtio.c_cflag = B38400 | CS8 | CLOCAL | CREAD;     
    }
    else if(ps[p1->portNum-1].baud_rate == 57600)
    {
        newtio.c_cflag = B57600 | CS8 | CLOCAL | CREAD;     
    }
    else if(ps[p1->portNum-1].baud_rate == 115200)
    {   
        newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;    
    }
    else
    {   
        newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    }
    
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; 
    newtio.c_cc[VMIN] = 5; 
  
    switch(p1->portNum)
    {
        case 1:
            loopCount = port1_cnt;
            break;
        case 2:
            loopCount = port2_cnt;
            break;
        case 3:
            loopCount = port3_cnt;
            break;
        case 4:
            loopCount = port4_cnt;
            break;
        case 5:
            loopCount = port5_cnt;
            break;
        case 6:
            loopCount = port6_cnt;
            break;
        case 7:
            loopCount = port7_cnt;
            break;
        case 8:
            loopCount = port8_cnt;
            break;
    }


    while(1)
    {
        for(i=0;i<loopCount;i++)
        {   
		  if(*(holdingResisters+9980)==1) 
		   {
			  int i; 
			  
			  if(port1_cnt>0)
			   {
				for(i=0;i<port1_cnt;i++)
				{
				  if( port1_client[i][3]== *(holdingResisters+9981))
				  {
					port1_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }
			   
			   if(port2_cnt>0)
			   {
				for(i=0;i<port2_cnt;i++)
				{
				  if( port2_client[i][3]== *(holdingResisters+9981))
				  {
					port2_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }
			   
			   if(port3_cnt>0)
			   {
				for(i=0;i<port3_cnt;i++)
				{
				  if( port3_client[i][3]== *(holdingResisters+9981))
				  {
					port3_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }

			   if(port4_cnt>0)
			   {
				for(i=0;i<port4_cnt;i++)
				{
				  if( port4_client[i][3]== *(holdingResisters+9981))
				  {
					port4_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }

			   if(port5_cnt>0)
			   {
				for(i=0;i<port5_cnt;i++)
				{
				  if( port5_client[i][3]== *(holdingResisters+9981))
				  {
					port5_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }

			   if(port6_cnt>0)
			   {
				for(i=0;i<port6_cnt;i++)
				{
				  if( port6_client[i][3]== *(holdingResisters+9981))
				  {
					port6_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }

			   if(port7_cnt>0)
			   {
				for(i=0;i<port7_cnt;i++)
				{
				  if( port7_client[i][3]== *(holdingResisters+9981))
				  {
					port7_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }

			   if(port8_cnt>0)
			   {
				for(i=0;i<port8_cnt;i++)
				{
				  if( port8_client[i][3]== *(holdingResisters+9981))
				  {
					port8_client[i][8] = *(holdingResisters+9982);
				  }
				}
			   }

				*(holdingResisters+9980)=0;
				*(holdingResisters+9981)=0;
				*(holdingResisters+9982)=0;
		   }
		 
			if(p1->portClient[i][8] == 1) continue;
		 
		    t = (time(NULL) + (timeout / 1000));
            tm = *localtime(&t);
            
            memset(trame, 0, 256);

            count = p1->portClient[i][5] - p1->portClient[i][4] + 1; // to get address count
            
            trame[0]=p1->portClient[i][1];
            trame[1]=p1->portClient[i][2];
            trame[2]=(p1->portClient[i][6]-1)>>8;
            trame[3]=(p1->portClient[i][6]-1)&0xFF;
            trame[4]=(p1->portClient[i][5] - p1->portClient[i][4] + 1)>>8;
            trame[5]=(p1->portClient[i][5] - p1->portClient[i][4] + 1)&0xFF;
            calc_crc(trame,6);

            tcflush(fd, TCIOFLUSH);
            tcsetattr(fd,TCSANOW,&newtio);

            ioctl( fd, TIOCMBIS, &modemctlline);
            modemctlline = TIOCM_RTS;
            ioctl( fd, TIOCMBIS, &modemctlline );

            FD_ZERO(&readfds);
            FD_SET(fd, &readfds);
            tv.tv_sec = 0;

            if(ps[0].baud_rate == 9600)
			{
			 tv.tv_usec = 200*1000; 
			}
            else if(ps[0].baud_rate == 19200)
			{
			 tv.tv_usec = 150*1000; 
			}
            else if(ps[0].baud_rate == 38400)
			{
			 tv.tv_usec = 100*1000; 
			}
            else if(ps[0].baud_rate == 57600)
			{
			 tv.tv_usec = 100*1000; 
			}
            else if(ps[0].baud_rate == 115200)
			{
			 tv.tv_usec = 100*1000; 
			}
            else 
			{
			 tv.tv_usec = 200*1000; 
			}

            write(fd, trame, 8);
			
            select(fd, &readfds, (fd_set *)0, (fd_set*)0, &tv);
           
            while( 1 )
            {
                ioctl( fd, TIOCSERGETLSR, &txemptystate);
                if( txemptystate ) break;
            }

            modemctlline = TIOCM_RTS;
            ioctl( fd, TIOCMBIC, &modemctlline );

           memset(buf, 0, 1024); 

           res = read(fd,buf,255);    
           buf[res]=0;

           switch(p1->portClient[i][2])
           {
                case 1:
                case 2:
                    crcLen = 5 + (count/8);
                    if(count%8)
                        crcLen += 1;
                    break;
                case 3:
                case 4:
                    crcLen = 5 + (count*2);
                    break;
                default:
                    break;
           }

		   //printf("%d %d \n", p1->portClient[i][2], crcLen);

           int crcRtn = check_recv_crc(buf, crcLen);
           if(crcRtn)
           {
                tv.tv_sec = 1;
                tv.tv_usec = 10;
                select(fd, &readfds, (fd_set *)0, (fd_set*)0, &tv);
                writeRtuErrlog(p1->portNum, p1->portClient[i][1], p1->portClient[i][2], p1->portClient[i][6], 2);    
                *(holdingResisters + p1->portClient[i][3] -1) = 1;
                continue;
           }
            
           if(res==0)
            {              
                tv.tv_sec = 1;
                tv.tv_usec = 10;
                select(fd, &readfds, (fd_set *)0, (fd_set*)0, &tv);
                writeRtuErrlog(p1->portNum, p1->portClient[i][1], p1->portClient[i][2], p1->portClient[i][6], 2);    
                *(holdingResisters + p1->portClient[i][3] -1) = 1;
                continue;
            }
            
           if(t<time(NULL))
            {
                tv.tv_sec = 1;
                tv.tv_usec = 10;
                select(fd, &readfds, (fd_set *)0, (fd_set*)0, &tv);
                writeRtuErrlog(p1->portNum, p1->portClient[i][1], p1->portClient[i][2], p1->portClient[i][6], 2);    
                *(holdingResisters + p1->portClient[i][3] -1) = 1;
                continue;
            }
          
           //  printf("%x %x %x %x %x %x %x %x %x %x %x %x \n", buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], res);
            //printf("%d :  %d %d %d %d %d \n", p1->portNum, hexToInt(buf[3], buf[4]), hexToInt(buf[5], buf[6]), hexToInt(buf[7], buf[8]), hexToInt(buf[9], buf[10]),res);
               
            if((trame[1]==3)||(trame[1]==4))
               {
                   for(j=0;j<count*2;j=j+2) // for moa
                   {
                     *(holdingResisters + p1->portClient[i][4] -1 + (j/2)) = hexToInt(buf[3+j], buf[4 +j]);
                   }
                   
				   
				   if(p1->portClient[i][7] == 1) // for sids
                   {
                        for(j=0;j<count*2;j=j+2)
                        {
                            *(holdingResisters + p1->portClient[i][4] -1 + (j/2)) = hexToInt(buf[3+j], buf[4 +j]);
                        }
                   }
                   else if(p1->portClient[i][7]== 2) //2byte swap
                   {
                        for(j = 0; j < count*2 ; j = j+4)
                        {
                               *(holdingResisters + p1->portClient[i][4] + (j/2)) = hexToInt(buf[3+j], buf[4 +j]);	
                               *(holdingResisters + p1->portClient[i][4]-1 + (j/2)) = hexToInt(buf[5+j], buf[5 +j]);	
                        }
                        
                   }                
                   else if(p1->portClient[i][7] == 3) // 4byte swap 
                   {
                        for(j = 0; j < count*2 ; j = j+8)
                        {
                               *(holdingResisters +  p1->portClient[i][4]-1 + (j/2)) = hexToInt(buf[9+j], buf[10 +j]);	
                               *(holdingResisters +  p1->portClient[i][4] + (j/2)) = hexToInt(buf[7+j], buf[8 +j]);	
                               *(holdingResisters +  p1->portClient[i][4] +1 + (j/2)) = hexToInt(buf[5+j], buf[6 +j]);	
                               *(holdingResisters +  p1->portClient[i][4] +2 + (j/2)) = hexToInt(buf[3+j], buf[4 +j]);	
                        }

                   }
               }

                *(holdingResisters + p1->portClient[i][3] -1) = 0;
				//printf("%d \n", p1->portClient[i][3]);
				tcsetattr(fd,TCSANOW,&oldtio);
        }
    }
    close( fd );
   
}

int calc_crc(char trame[],int n)
{
    unsigned int crc,i,j,carry_flag,a;
    crc=0xffff;
    for (i=0;i<n;i++)
    {
        crc=crc^trame[i];
        for (j=0;j<8;j++)
        {
            a=crc;
            carry_flag=a&0x0001;
            crc=crc>>1;
            if (carry_flag==1)
                crc=crc^0xa001;
        }
    }
    trame[n+1]=crc>>8;
    trame[n]=crc&255;
    return crc;
}


int check_recv_crc(char trame[],int n)
{
    unsigned int crc,i,j,carry_flag,a;
    crc=0xffff;
    for (i=0;i<n;i++)
    {
        crc=crc^trame[i];
        for (j=0;j<8;j++)
        {
            a=crc;
            carry_flag=a&0x0001;
            crc=crc>>1;
            if (carry_flag==1)
                crc=crc^0xa001;
        }
    }
    if ((trame[n+1]!=(crc>>8)) || (trame[n]!=(crc&255)))
        return 1;
    else
        return 0;
}




void writeRtuErrlog(int port_num, int addr, int fn_code, int slave_start, int gubun)
{
   char msg[30]; 
   FILE *fd;
   char *fileName;
   switch(port_num)
   {
        case 1:
		     fileName = "port1_log";
			//fd = fopen("port1_log", "a");
            break;
        case 2:
		    fileName = "port2_log";
			//fd = fopen("port2_log", "a");
            break;
        case 3:
		    fileName = "port3_log";
			//fd = fopen("port3_log", "a");
            break;
        case 4:
		    fileName = "port4_log";
			//fd = fopen("port4_log", "a");
            break;
        case 5:
		    fileName = "port5_log";
			//fd = fopen("port5_log", "a");
            break;
        case 6:
		    fileName = "port6_log";
			//fd = fopen("port6_log", "a");
            break;
        case 7:
		    fileName = "port7_log";
			//fd = fopen("port7_log", "a");
            break;
        case 8:
		    fileName = "port8_log";
			//fd = fopen("port8_log", "a");
            break;
        default:
            break;
            
   }

	fd = fopen(fileName, "a");
   struct tm* t = nowTime();
  
   if(gubun == 1)
       strcpy(msg, "Connection Error");
   else if(gubun == 2)
       strcpy(msg, "crc Error");
   else if(gubun == 3)
       strcpy(msg, "Time out");
   else
       strcpy(msg, "Error");

   fprintf(fd, "%d-%02d-%02d %02d:%02d:%02d : %d %d %d %d %s\n", t->tm_year + 1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, port_num, addr, fn_code, slave_start,msg);
   fclose(fd);
    
   int size;
   FILE * fp_size = fopen(fileName, "r");	
   
   fseek(fp_size, 0, SEEK_END);
   size = ftell(fp_size);
  
   if(size > 1024000)
	remove(fileName);

   fclose(fp_size);
}
        
