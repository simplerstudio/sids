#include "user_define.h"
#include "tcp_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int device[DEVICE_COUNT];
int total_pthread;
int intervalTime;

int getIntervalTime()
{
	FILE *fp_time;
	int i;

	if ((fp_time = fopen("interval.txt", "r")) == NULL)
	{
	   intervalTime = 10;
	}
	else
	{	
	   fscanf(fp_time, "%d", &intervalTime);
	}
	return intervalTime;
	
}

int checkTcpConfigFile()
{
	char readLine[100];
	int result;
	FILE *fp_sids = fopen("tcp_config.txt",   "r");  // read sids tcp config file	
	FILE *fp_moa  = fopen("mbeclient.config", "r");  // read moa tcp config file

	if ((fp_sids == NULL) && (fp_moa == NULL))
	{		
		result = -1;	
		return result;	
	}
	
	if ((fp_sids != NULL) && (fp_moa != NULL))
	{	
		result = 3;	
		return result;
	}
	
	if (fp_sids != NULL)
	{			
		result = 1;	
		return result;
	}
	
	if (fp_moa != NULL)
	{		
		result = 2;
		return result;
	}
	
	result = -1;
	return result;
	

	if(fp_sids !=  NULL) fclose(fp_sids);
	if(fp_moa !=  NULL)  fclose(fp_moa);
	
	
}


void getMoaTcpConfig()
{
	char readLine[100];
	FILE *fp_moa;
	if ((fp_moa = fopen("mbeclient.config", "r")) == NULL)
	{
		perror("Fail to open mbeclient.config");	
		return;
	}
	else
	{		
	//	puts("Sucess to open mbeclient.config");
	}

	int i=0, j=0, k=0;
	char *ptr;
	char *args[6] = {NULL,};

	while (fgets(readLine, sizeof(readLine), fp_moa) != NULL) // To get total device count..
	{
		i = 0; // splited count
		ptr = strtok(readLine, TOKEN);
		while (ptr != NULL)
		{
			args[i] = ptr;
			ptr = strtok(NULL, TOKEN);
			i++;
		}
		if (i == 6) // if this line include ip, port...
		{
			moaDeviceCount++; // Same as thread count
		}
		if (i == 1) // if this line include address info
		{
			ptr = strtok(readLine, ",");
			j = 0;
			while (ptr != NULL)
			{
				args[j] = ptr;
				ptr = strtok(NULL, ",");
				j++;
			}
		
			if (((int)*args[0] > 48) && ((int)*args[1] > 48) && ((int)*args[2] > 48)) // '0'=>48
			{
				moaTcpLineCount++;
			}
		}
	}
	if(fp_moa != NULL) fclose(fp_moa);

	if (moaDeviceCount < 0)
	{
		puts("There are no line included ip, port..");
		return;
	}
	else
	{
		moaThread = (pthread_t *)calloc(moaDeviceCount, sizeof(pthread_t));		  //Allocate thread array as device count
		moaSock = (int *)calloc(moaDeviceCount, sizeof(int));					  // Allocate sock array as device count
		moaTcpStruct = (tcp_struct *)calloc(moaTcpLineCount, sizeof(tcp_struct)); //Allocate tcp_struct
	}

	mbeclient_struct mbe;	
	fp_moa = fopen("mbeclient.config", "r");

	while (fgets(readLine, sizeof(readLine), fp_moa) != NULL)
	{
		i = 0;
		ptr = strtok(readLine, TOKEN);
		while (ptr != NULL)
		{
			args[i] = ptr;			
			ptr = strtok(NULL, TOKEN);
			i++;
		}
		
		if (i == 6)
		{			
			strcpy(mbe.ip, args[0]);	
			strcpy(mbe.port, "502");						
			mbe.idx = *args[1];
			mbe.err_addr = atoi(args[2]);
			mbe.scan_time = atoi(args[3]);
			mbe.time_out = atoi(args[4]);
			mbe.reset = atoi(args[5]);
		}
		else if (i == 1)
		{		
			ptr = strtok(readLine, ",");
			j = 0;
			while (ptr != NULL)
			{
				args[j] = ptr;									
				ptr = strtok(NULL, ",");
				j++;
			}
		
			
			if ((atoi(args[0]) > 0) && (atoi(args[1]) > 0) && (atoi(args[2]) > 0)) 
			{							
				if (atoi(args[0]) > 10000 && atoi(args[0]) < 20000)
					moaTcpStruct[k].fn_code = 1;
				else if (atoi(args[0]) > 20000 && atoi(args[0]) < 30000)
					moaTcpStruct[k].fn_code = 2;
				else if (atoi(args[0]) > 30000 && atoi(args[0]) < 40000)
					moaTcpStruct[k].fn_code = 4;
				else if (atoi(args[0]) > 40000 && atoi(args[0]) < 50000)
					moaTcpStruct[k].fn_code = 3;
				else
					printf("%s \n", "There is incorrect line at start address");				

				strcpy(moaTcpStruct[k].ip, mbe.ip);			
				strcpy(moaTcpStruct[k].port, mbe.port);
				moaTcpStruct[k].idx = mbe.idx;
				moaTcpStruct[k].err_addr = mbe.err_addr;						 
				
				int first, second, third;
				first = atoi(args[0]);
				second = atoi(args[1]);
				third = atoi(args[2]);			
				moaTcpStruct[k].gw_start = first - 40000;	
				moaTcpStruct[k].gw_end = second - 40000;
				moaTcpStruct[k].slave_start = third - 40000;
				moaTcpStruct[k].scan_time = mbe.scan_time;
				moaTcpStruct[k].time_out = mbe.time_out;
				moaTcpStruct[k].reset = mbe.reset;
				moaTcpStruct[k].swap_code = 0;
				
				//moaTcpStruct[k].gw_start = third - 40000;	
				//moaTcpStruct[k].gw_end = third + second - first - 40000;
				//moaTcpStruct[k].slave_start = atoi(args[0]);
				//moaTcpStruct[k].scan_time = mbe.scan_time;
				//moaTcpStruct[k].time_out = mbe.time_out;
				//moaTcpStruct[k].reset = mbe.reset;
				//moaTcpStruct[k].swap_code = 0;
				k++;
			}
			else if (((int)*args[0] == 48) && ((int)*args[1] == 48) && ((int)*args[2] == 48))
			{
			}			
			
		}
		else
		{
			puts("There is incorrect line in mbeclient.config bbb");
		}
	}

	if(fp_moa != NULL) fclose(fp_moa);

	return;
}

void getSidsTcpConfig()
{
	char readLine[100];
	FILE * fp_tcp1 = fopen("tcp_config.txt", "r");
	if(fp_tcp1 == NULL)
	{
		puts("tcp_config.txt file Error"); 
		fclose(fp_tcp1);
		exit(1);
	}
	else
	{
		char *info;
		while(fgets(readLine, sizeof(readLine), fp_tcp1) != NULL)
     	{
		        if(atoi(readLine)==0) break;
	        	info = strtok(readLine, TOKEN);
			int comp = strcmp(info, ";");
			if(comp < 1)
			{
				tcp_lineCount++;
			}
		}

		fclose(fp_tcp1);
	}

	if(tcp_lineCount > 0)
	{
		sidsTcpStruct = (tcp_struct*)malloc(sizeof(tcp_struct) * tcp_lineCount);
		FILE * fp_tcp2 = fopen("tcp_config.txt", "r");
		char * arg[9];
		char * info = NULL;
		int i=0, j=0;
		while(fgets(readLine, sizeof(readLine), fp_tcp2) != NULL)
		{
			if(atoi(readLine)==0) break;
			info = strtok(readLine, TOKEN);
			int comp = strcmp(info, ";");
			if(comp < 1)
			{
				for(i=0;i<9;i++)
				{
					arg[i] = info;
					info = strtok(NULL, TOKEN);
				}
			}
            //puts(arg[0]);
			strcpy(sidsTcpStruct[j].ip,   arg[0]);
            strcpy(sidsTcpStruct[j].port, arg[1]);								
			sidsTcpStruct[j].fn_code     = atoi(arg[2]);		
			sidsTcpStruct[j].err_addr    = atoi(arg[3]);            
			sidsTcpStruct[j].gw_start    = atoi(arg[4]);
            sidsTcpStruct[j].gw_end      = atoi(arg[5]);		
			sidsTcpStruct[j].slave_start = atoi(arg[6]);
			sidsTcpStruct[j].scan_time   = atoi(arg[7]);		
			sidsTcpStruct[j].swap_code   = atoi(arg[8]);	
            
           // printf("%s %d %d %d \n",sidsTcpStruct[j].ip, sidsTcpStruct[j].gw_start, sidsTcpStruct[j].gw_end, sidsTcpStruct[j].slave_start);
            if(j==tcp_lineCount) break;
            j++;


		}
	
		//fclose(fp_tcp2);
		tcp_struct temp;
		for(i=0;i<tcp_lineCount-1;i++) // sorting by ip
		{
			for(j=0;j<tcp_lineCount-1-i;j++)
			{
				if(strcmp(sidsTcpStruct[j].ip, sidsTcpStruct[j+1].ip) > 0)
				{
					temp = sidsTcpStruct[j];
					sidsTcpStruct[j]=sidsTcpStruct[j+1];
					sidsTcpStruct[j+1] = temp;
				}
			}
		}
		char compare_ip[20];
		for(i=0;i<DEVICE_COUNT;i++) device[i]=9999;
		strcpy(compare_ip, "first");
		for(i=0;i<tcp_lineCount;i++)
		{
			if(strcmp(sidsTcpStruct[i].ip, compare_ip) != 0)
			{
				device[total_pthread] = i;
				strcpy(compare_ip, sidsTcpStruct[i].ip);
				total_pthread++;
			}
		}	

	}
}

struct tm* nowTime()
{
   time_t tnow;
   struct tm* t;
   time(&tnow);
   t = (struct tm*)localtime(&tnow);
   return t;
}



void writeErrLog(char* myIp, int errnnum, int gubun)
{
    int size;
	FILE * fp_size, *fp_write;
	char msg[30];
	
   if(gubun == 1)
       strcpy(msg, "Connection Error");
   else if(gubun == 2)
       strcpy(msg, "Error");
   else if(gubun == 3)
       strcpy(msg, "Time out");
   else if(gubun == 4)
       strcpy(msg, "Exception Error");
   else
       strcpy(msg, "Error");
    
   fp_write = fopen(myIp, "a");
    struct tm* t = nowTime();
    fprintf(fp_write, "%d-%02d-%02d %02d:%02d:%02d : %s, %s\n", t->tm_year + 1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, strerror(errnnum), msg);
    fclose(fp_write);
	
	fp_size = fopen(myIp, "r");
	fseek(fp_size, 0, SEEK_END);
	size = ftell(fp_size);
	if(size > 1024000)
	  remove(myIp);
	fclose(fp_size);
}





