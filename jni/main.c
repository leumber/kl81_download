#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "command.h"

#include "kl81_bootdown.h"


const unsigned char path[] = "/mnt/sdcard/ZNGReader_boot.bin";


int main(void)
{
    int ret;

    ret = kl81_boot_device_state();
    if(ret != 0)
    {
    	printf("kl81_boot_device_state err return %d \r\n",ret);
    	return -1;
    }
    ret = kl81_boot_get_property();
    if(ret != 0)
    {
    	printf("kl81_boot_get_property err return %d \r\n",ret);
    	return -1;
    }
    ret = kl81_boot_down_bin(path);
    if(ret != 0)
	{
		printf("kl81_boot_down_bin err! ret = %d errno = %d\r\n",ret,errno);
		return -2;	
	}

    return 0;
}