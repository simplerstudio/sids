#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include "hex_info.h"

#define FDCNT 10

unsigned short* holdingResisters;

typedef struct{
	int fd;
	char ip[20];
} CLIENT;


int clientCount = 0;
int sockfd_connect[FDCNT];

void * start_tcp_server();
void * client_thread(CLIENT *clientPtr);

int main()
{
	int shmid;
	void* shmaddr;
	struct shmid_ds shm_stat;

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
	
    memset(holdingResisters, 0, sizeof(unsigned short)*0xFFFF);

    start_tcp_server();
	free(holdingResisters);
    return 0;
}

//void * start_tcp_server(void * arg)
void * start_tcp_server()
{
	int tempfd = 0;
	int clnt_addr_size = 0;
	int sockfd_listen;
	char msg[128];
	char ip[30];
	char port[10];
	struct sockaddr_in server;
	struct sockaddr_in clnt_addr;
	CLIENT client_data[FDCNT] = { 0 };
	pthread_t ptid[FDCNT] = { 0 };
	int i = 0;

	memset(&server, 0, sizeof(server));
	//server.sin_family = AF_INET;
	//server.sin_addr.s_addr = inet_addr = htons(INADDR_ANY);
	//server.sin_addr.s_addr = htonl(INADDR_ANY);
	//server.sin_addr.s_addr = inet_addr("192.168.0.133");
	//server.sin_port = htons(502);
	//server.sin_port = htons((u_short)502);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(atoi("502"));


	if ((sockfd_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{		
		printf("fail to call socket() \n");
		exit(1);
	}

	if (bind(sockfd_listen, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == -1)
	{
		perror("falil to call bind()\n");
		exit(1);
	}

	if (listen(sockfd_listen, 5) == -1)
	{
		printf("falil to call listen()\n");
		exit(1);
	}


	clnt_addr_size = sizeof(clnt_addr);

	while (1)
	{
		tempfd = accept(sockfd_listen, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

		if (clientCount == FDCNT)
		{
			 printf("sockfd full\n");
			continue;
		}

		if (tempfd < 0)
		{
			printf("fail to call accept()\n");
			continue;
		}

		for (i = 0; i < FDCNT; i++)
		{
			if (client_data[i].fd == 0)
			{
				client_data[i].fd = tempfd;
				break;
			}

		}

		strcpy(client_data[i].ip, inet_ntoa(clnt_addr.sin_addr));

		pthread_create(ptid + i, NULL, (void *)client_thread, client_data + i);

		//printf("accepted sockfd:%d, clientCount:%d i:%d\n", client_data[i].fd, clientCount, i);


	}

	close(sockfd_listen);

}



void * client_thread(CLIENT *clientPtr)
{
	int rst;
	char msg[128];
	short reg_count;
	char send_buf[1024];
	char function_code;
	int k = 0, j = 9;
	unsigned short change;
	char buf[128];
	unsigned short start_reg;
	while (1)
	{
		k = 0;
		j = 9;
		change = 0;

		rst = read(clientPtr->fd, buf, sizeof(buf));
		if (rst <= 0)
			break;

	/*	int c=0;
		for( c=0;c<12;c++)
		{
		printf("%x ", buf[c]);
		}
		printf("\n");*/


		function_code = buf[7];

		start_reg = buf[8] << 8 | buf[9];
		reg_count = buf[10] << 8 | buf[11];

		send_buf[0] = buf[0];
		send_buf[1] = buf[1];
		send_buf[2] = buf[2];
		send_buf[3] = buf[3];
		send_buf[4] = buf[4];

		
		if (function_code == 3)
		{
		   if(buf[8]==0xFF && buf[9]==0xFE)
		   {
			send_buf[5] = 3;
			send_buf[6] = 1;
		  	send_buf[7] = 0x83;
			send_buf[8] = 2;		   
		    write(clientPtr->fd, send_buf,  9);		  

		   }
		   else
		   {
			send_buf[5] = 3 + 2 * reg_count;
			send_buf[6] = buf[6];
		  	send_buf[7] = buf[7];
			send_buf[8] = buf[11] * 2;

			while (k < reg_count)
		    {
			  change = *(holdingResisters + start_reg + k);
			  send_buf[j] = change >> 8;
			  j++;
			  send_buf[j] = change & 0xff;
			  j++;
			  k++;
		     }  
		     write(clientPtr->fd, send_buf, (buf[11] * 2) + 9);
		   }
		}
		else if(function_code == 1)
		{
			send_buf[5] = 3;
			send_buf[6] = 1;
		  	send_buf[7] = 0x81;
			send_buf[8] = 1;		   
		    write(clientPtr->fd, send_buf,  9);
		}
		else if(function_code == 2)
		{
			send_buf[5] = 3;
			send_buf[6] = 1;
		  	send_buf[7] = 0x82;
			send_buf[8] = 1;		   
		    write(clientPtr->fd, send_buf,  9);
		}
		else if(function_code == 4)
		{
			send_buf[5] = 3;
			send_buf[6] = 1;
		  	send_buf[7] = 0x84;
			send_buf[8] = 1;		   
		    write(clientPtr->fd, send_buf,  9);
		}
		else if(function_code == 6) //write
		{		 
		 *(holdingResisters + hexToInt(buf[8], buf[9])) = hexToInt(buf[10], buf[11]);
		   send_buf[0] = buf[0];	
		   send_buf[1] = buf[1];	
		   send_buf[2] = buf[2];	
		   send_buf[3] = buf[3];	
		   send_buf[4] = buf[4];	
		   send_buf[5] = buf[5];	
		   send_buf[6] = buf[6];	
		   send_buf[7] = buf[7];	
		   send_buf[8] = buf[8];	
		   send_buf[9] = buf[9];	
		   send_buf[10] = buf[10];	
		   send_buf[11] = buf[11];	
		 
		   write(clientPtr->fd, send_buf,  12);

		}
		else if(function_code == 16)
		{
          int i;
		  int startAddr = hexToInt(buf[8],buf[9]) -1;
		  for(i=0;i<(hexToInt(buf[10], buf[11]))*2;i=i+2)
		  {
		    *(holdingResisters + startAddr++ ) = hexToInt(buf[13+i], buf[14+i]);
		  }
		 
		   send_buf[0] = buf[0];	
		   send_buf[1] = buf[1];	
		   send_buf[2] = buf[2];	
		   send_buf[3] = buf[3];	
		   send_buf[4] = buf[4];	
	       send_buf[5] = 0x06;	
		   send_buf[6] = buf[6];	
		   send_buf[7] = buf[7];	
		   send_buf[8] = buf[8];	
		   send_buf[9] = buf[9];	
		   send_buf[10] = buf[10];	
		   send_buf[11] = buf[11];	
		 
		   write(clientPtr->fd, send_buf,  12);

		}



	  /* int b=0;
	   for(b=0;b<12;b++)
	   {
		printf("%x ", send_buf[b]);
	   }
	   printf("\n");*/

			 
			 
	//	else
	//	if ((reg_count % 8) == 0)
	//		send_buf[5] = 3 + (reg_count / 8);
	//	else
	//		send_buf[5] = 4 + (reg_count / 8);


	}

	close(clientPtr->fd);
	clientCount--;
	clientPtr->fd = 0;
	printf("close(%s) \n", clientPtr->ip);
	return NULL;
}
