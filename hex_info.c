#include "hex_info.h"

char intToHex(int num)
{
	char hexValue = hexaVal[num];
	return hexValue;
}

int hexToInt(const char first, const char second)
{
	char hexaString[2];
	sprintf(hexaString, "%02x%02x", first, second);
	int num = (int)strtol(hexaString, NULL, 16); //like FF
	//int num = (int)strtol(hexaString, NULL, 0); // like 0xFF
	return num;
}

void print_hex(int input, int *rtnVal )
{
	int var, i;
    int len = 0;
    int buf[255];
  
	do{
        var = input % 16;
        input /= 16;
 
		/* try to convert ascii code */
        if( var<10 ) buf[len] = var+48;
        else buf[len] = var+55;
        len++;
    }while( input>0 ); /* bigger than 16? then try again */
 
     /* print reversely */
     for ( i=len; i>0; i-- )
      {
        rtnVal[4-i] = buf[i-1];
      }

    rtnVal[4]='\0';
}
