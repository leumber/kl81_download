#ifndef __BIN_FILE_VALIDATE_H__
#define __BIN_FILE_VALIDATE_H__
#include <stdint.h>

extern uint32_t decrypt_bin_file(unsigned char *destBuf,unsigned char *srcBuf,unsigned int dLen);
extern uint32_t validate_bin_data(uint8_t *data,uint32_t dlen);
#endif

