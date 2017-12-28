#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <linux/types.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <libusb.h>

#include "kl81_bootdown.h"

#define USB_TIMEOUT		1000	    // 10 secconds
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define DEV_VID		0X15A2
#define DEV_PID		0X0073

#define DEV_VID_R		0X2012
#define DEV_PID_R		0X1402


libusb_context *u_context = NULL;
libusb_device_handle *dev_handle = NULL;
libusb_device **devs, *dev;

unsigned int usb_time_out;
struct libusb_device_descriptor desc;
const unsigned char *device_descriptor;
const struct libusb_interface *usb_interface = NULL;
int interface = 0;

int USBInit(int *pendpoint,int counts,int usb_timeout_ms)
{
	int ret,cnt,i;
	if(NULL == u_context)
	{
		ret = libusb_init(&u_context);
		if (ret != 0)
		{
			printf("libusb_init failed,return value = %d", ret);
			return -1;
		}
	}
	cnt = libusb_get_device_list(u_context, &devs);
	if(cnt < 0)
	{
		printf("libusb_get_device_list() failed\n");
		return -2;
	}
	i = 0;
	while ((dev = devs[i++]) != NULL)
	{
			int r = libusb_get_device_descriptor(dev, &desc);
			if (r < 0)
			{
				PRINTF("libusb_get_device_descriptor failed\r\n");
				continue;
			}
			PRINTF("idVendor = %08X,idProduct = %08X\r\n",desc.idVendor,desc.idProduct);			
			if (desc.idVendor == DEV_VID && desc.idProduct == DEV_PID)
				break;
	}
	if(dev == NULL)
	{
		printf("no found device!\r\n");
		return -3;	
	}
	
	ret = libusb_open(dev, &dev_handle);
	if (ret < 0)
	{
		printf("libusb_open dev failed\r\n");
		return -4;
	}
	if(dev_handle == NULL)
	{
		libusb_free_device_list(devs, 1);
		libusb_exit(u_context);	
	}
	
	libusb_free_device_list(devs, 1);
	
	ret = libusb_kernel_driver_active(dev_handle,0);
	PRINTF("libusb_kernel_driver_active ret = %d\r\n",ret);
	
	if (ret < 0)
	{
		printf("libusb_kernel_driver_active return %d\r\n",ret);
		libusb_set_configuration(dev_handle,-1);
		libusb_close(dev_handle);
		return -6;
	}
	if(ret > 0)
	{
		ret = libusb_detach_kernel_driver(dev_handle,0);
		PRINTF("libusb_detach_kernel_driver return %d\r\n",ret);
	}

	ret = libusb_set_configuration(dev_handle, 1);
	if (ret < 0)
	{
		PRINTF("libusb_set_configuration return %d\r\n",ret);
		libusb_close(dev_handle);
		return -5;
	}
//	ret = libusb_release_interface(dev_handle, 0);
//	PRINTF("libusb_release_interface 0 return %d\r\n",ret);
	ret = libusb_claim_interface(dev_handle,0);
	if (ret < 0)
	{
		PRINTF("libusb_claim_interface return %d\r\n",ret);
		libusb_set_configuration(dev_handle,-1);
		libusb_close(dev_handle);
		return -6;
	}
	
	usb_time_out = usb_timeout_ms;
	printf("OPEN DEVICE SUCCESS!\r\n");
	return 0;
}

int USBReadFromBulkEp(int ep,char *pbuf,int *buf_size)
{
	int ret,actual_length;
	ret = libusb_bulk_transfer(dev_handle, ep, (unsigned char *)pbuf, *buf_size, &actual_length, usb_time_out);
	if(ret < 0)
	{
		return ret;
	}
	*buf_size = actual_length;
	return actual_length;
}


int USBWriteToBulkEp(int ep,char *pbuf,int size)
{
	int ret,actual_length;
	ret = libusb_bulk_transfer(dev_handle, ep, (unsigned char *)pbuf, size,&actual_length, usb_time_out);

	return ret;
}

int USBReadFromIntrEp(int ep,char *pbuf,int *buf_size)
{
	int ret,actual_length;
	ret = libusb_interrupt_transfer(dev_handle, ep, (unsigned char *)pbuf, *buf_size,&actual_length, usb_time_out);
	if(ret < 0)
	{
		return ret;
	}
	*buf_size = actual_length;
	return ret;
}
int USBWriteToIntrEp(int ep,char *pbuf,int size)
{
	int ret,actual_length;
	ret = libusb_interrupt_transfer(dev_handle, ep, (unsigned char *)pbuf, size,&actual_length, usb_time_out);

	return ret;
}

int USBRelease(void)
{
	int ret;
	ret = libusb_set_configuration(dev_handle,-1);
	libusb_close(dev_handle);
	return ret;
}

int USBGetState()
{
	int ret,cnt,i;
	libusb_context *u_ctx = NULL;
	libusb_device_handle *u_dev_handle = NULL;
	libusb_device **u_devs, *u_dev;
	struct libusb_device_descriptor u_desc;
	
	if(NULL == u_ctx)
	{
		ret = libusb_init(&u_ctx);
		if (ret != 0)
		{
			printf("libusb_init failed,return value = %d", ret);
			return -1;
		}
	}
	cnt = libusb_get_device_list(u_ctx, &u_devs);
	if(cnt < 0)
	{
		printf("libusb_get_device_list() failed\n");
		return -2;
	}
	i = 0;
	while ((u_dev = u_devs[i++]) != NULL)
	{
			int r = libusb_get_device_descriptor(u_dev, &u_desc);
			if (r < 0)
			{
				printf("libusb_get_device_descriptor failed\r\n");
				continue;
			}
			
			if (u_desc.idVendor == DEV_VID && u_desc.idProduct == DEV_PID)
			{
				printf("idVendor = %08X,idProduct = %08X\r\n",u_desc.idVendor,u_desc.idProduct);
				ret = 0;
				break;
			}
			if (u_desc.idVendor == DEV_VID_R && u_desc.idProduct == DEV_PID_R)
			{
				printf("idVendor = %08X,idProduct = %08X\r\n",u_desc.idVendor,u_desc.idProduct);
				ret = 1;
				break;
			}
	}
	
	libusb_free_device_list(u_devs, 1);
	libusb_exit(u_ctx);	
	
	if(u_dev == NULL)
	{
		printf("no found device!\r\n");
		return -1;	
	}
	return ret;
}