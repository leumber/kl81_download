#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "command.h"
#include "bootloader_flasher.h"

#include "usb_bulk.h"
#include "kl81_bootdown.h"

#define USB_TIMEOUT		1000	    // 10 secconds




int  hid_read_timeout(unsigned char *data, size_t length, uint32_t *actualBytes,int milliseconds)
{
	int ret;
    uint32_t i;
    PRINTF("hid_read::\r\n");

    ret = USBReadFromBulkEp(0x81,(char *)data,(int *)&length);
	PRINTF("length = %d\r\n",ret);
    if(ret > 0)
	{
		*actualBytes = ret;
		PRINTF("get %08X bytes from endpoint!\r\n",ret);
		PRINTF("*****************************\r\n");
		for(i = 0;i < ret;i++)
		{
			PRINTF("%02X",data[i]);
		}
		PRINTF("\r\n*****************************\r\n");				
	}
	else
	{
		return -1;
	}	
    return 0;
}
int  hid_write_timeout( const unsigned char *data,size_t length,int milliseconds)
{
	int ret;
    uint32_t i;
	PRINTF("hid_write:: %d\r\n",length);
    for(i = 0;i < length;i++)
    {
        PRINTF("%02X",data[i]);
    }
    PRINTF("\r\n");
    ret = USBWriteToBulkEp(0x02,(char *)data,length);
    
    return ret;
}

void flasher_callback(uint32_t cur,uint32_t total)
{
	
    PRINTF("percent:%02d\r\n",cur*100/total);
}

int kl81_boot_down_bin(const unsigned char *binpath)
{
	int ret;
	ret = USBInit(0,0,USB_TIMEOUT);
    if(ret != 0)
	{
		printf("not found desnation device! ret = %d errno = %d\r\n",ret,errno);
		return -1;	
	}
	
    printf("*****flasher download start*****\r\n");
    ret = flasher_download(binpath,0,flasher_callback);
    if(ret != 0)
    {
    	printf("\r\n-----flasher download err-----\r\n");
    	return -1;
    }
    printf("\r\n-----flasher download end-----\r\n");
    USBRelease();
    return 0;
}
/*
return 	-1:not found device!
		0: clear 	15a2:0073
		1: boot		2012:1402
*/
int kl81_boot_device_state()
{
	int ret;
	ret = USBGetState();
	
	return ret;
}
int kl81_boot_get_property()
{
	int ret;
	ret = USBInit(0,0,USB_TIMEOUT);
    if(ret != 0)
	{
		printf("not found desnation device! ret = %d errno = %d\r\n",ret,errno);
		return -1;	
	}
	printf("kl81_boot_get_property:: \r\n");
	cmd_get_property();
    USBRelease();
    return 0;

}
