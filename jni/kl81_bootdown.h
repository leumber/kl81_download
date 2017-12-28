#ifndef _KL81_BOOTDOWN_H_
#define _KL81_BOOTDOWN_H_


#include <stdio.h>


#define DEBUG	1

#if DEBUG
#define PRINTF(...)		printf(__VA_ARGS__)
#define printf(...)		printf(__VA_ARGS__)

#else 
#define PRINTF(...)	
//#define printf(...)	

#endif


/*
download bin file
return 	-1:not found descriptor device!
		-2:download bin err!
		0: sucess
*/

int kl81_boot_down_bin(const unsigned char *binpath);


/*
get USB descriptor
return 	-1:not found device!
		0: is clear 		15a2:0073
		1: already boot		2012:1402
*/
int kl81_boot_device_state();

/*
get KL81 ROM state
return 	-1:not found device!
		0: sucess
*/
int kl81_boot_get_property();

#endif

