#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <linux/types.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


#include "usb_bulk.h"
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#define USB_TIMEOUT		1000	    // 10 secconds
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	int ret,i;
	unsigned char read_buf[1024];
	int buf_size;
	int timeout_loops;

	ret = USBInit(0,0,USB_TIMEOUT);
printf("UsbInit return :: %08X\r\n",ret);
	if(ret != 0)
	{
printf("not found desnation device!\r\n");
		return -1;	
	}	
	//向端点2发送升级报文 01 00 0c 00 07 00 00 02 05 00 00 00 00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 00 00 00 00 
	{
		unsigned char req_packet[36] = {0x01,0x00,0x0c,0x00,0x07,0x00,0x00,0x02,
						0x05,0x00,0x00,0x00,0xcc,0xcc,0xcc,0xcc,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00};
		ret = USBWriteToBulkEp(0x02,(char *)req_packet,sizeof(req_packet));
printf("\r\n*****************************\r\n");
for(i = 0;i < sizeof(req_packet);i++)
{
printf("%02X",req_packet[i]);
}
printf("\r\n*****************************\r\n");
	}
	//从端点1接受响应报文
	buf_size = sizeof(read_buf);
	timeout_loops = 0;
	do
	{
		usleep(10 * 1000);
		ret = USBReadFromBulkEp(0x81,(char *)read_buf,&buf_size);
		if(ret > 0)
		{
			timeout_loops = 0;
printf("get %08X bytes from endpoint!\r\n",ret);
printf("*****************************\r\n");
for(i = 0;i < ret;i++)
{
printf("%02X",read_buf[i]);
}
printf("\r\n*****************************\r\n");				
		}
		else
		{
printf("read usb endpoint ret = %d errno = %d \r\n",ret,errno);	
			timeout_loops++;
			if(timeout_loops > 50)
			{
printf("read usb endpoint timeout!!!! \r\n");	
				USBRelease();
				return -1;
			}	
		}	
	}while(1);

	USBRelease();
	return 0;	
}
