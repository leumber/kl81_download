
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "command_packet.h"
#include "usb_hid_packetizer.h"
#include "command.h"
#include "bootloader_flasher.h"

#include "bin_file_validate.h"

#include "sha256.h"



uint8_t m_file_buf[512*1024];
//uint8_t m_file_enc[512*1024];
//uint8_t m_file_dec[512*1024];


uint32_t flasher_connect_init(void)
{
    cmd_get_property();
    cmd_erase_all_unsecure();
    cmd_erase_region();
    return 0;
}

uint32_t flasher_download(const unsigned char *filepath,uint32_t startaddr,flasher_progress_callback *callback)
{
    FILE *file;
    uint32_t m;
    uint32_t ret;
    

    file = fopen((const char *)filepath,"rb");
    if(file == NULL)
    {
        printf("\r\n");
        printf("read file error.\r\n");
        return 1;
    }
    
    fseek(file, 0L, SEEK_END);
    m = ftell(file);
    rewind(file);

    fread(m_file_buf,sizeof(uint8_t),sizeof(m_file_buf),file);

    decrypt_bin_file(m_file_buf,m_file_buf,m);
    ret = validate_bin_data(m_file_buf,m);
    if(ret != 0)
    {
        fclose(file);
        printf("hash check failed!\r\n");
        return 1;
    }

    flasher_connect_init();
    cmd_write_memory(startaddr,m);
    send_data_check_response(m_file_buf,startaddr,m,callback);
    cmd_reset();

    fclose(file);
    return 0;
}




