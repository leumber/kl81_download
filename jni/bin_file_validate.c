#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sha256.h"

#include "des_op.h"

uint8_t m_data_key[24] = {0xCD,0x9F,0xD1,0x81,0x86,0xD8,0x20,0x3C,0xFE,0xE9,0x02,0x55,0xC5,0xC4,0xBE,0x09,0x72,0x35,0xD6,0x2B,0x2F,0x5B,0xF5,0xF7};
uint32_t decrypt_bin_file(unsigned char *destBuf,unsigned char *srcBuf,unsigned int dLen)
{
    unsigned char *binBuf;
    unsigned char tmpKey[24];
    unsigned char keyFactor[24];
    unsigned int tLen,tOffs;

    binBuf = (unsigned char *)malloc(512*1024);

    tLen = ((dLen - TDES_KEY_LEN) / 8) * 8;           //ȥ��ĩβ16�ֽ��Ժ���8�ֽڶ��봦���ĳ��ȣ�Ϊ���ռ��ܵ����ݳ��ȡ�
    tOffs = dLen - TDES_KEY_LEN;                      //ĩβ16�ֽ�Ϊ��Կ��ɢ���ӣ��˴�ȡ��ƫ�ơ�
    memcpy(keyFactor,srcBuf+tOffs,TDES_KEY_LEN);
    des3_ecb_encrypt(tmpKey,keyFactor,TDES_KEY_LEN,m_data_key,TDES_KEY_LEN);  //������ʱ������Կ��
    des3_ecb_decrypt(binBuf,srcBuf,tLen,tmpKey,TDES_KEY_LEN);       //ʹ����ʱ������Կ�������ݡ�
    memcpy(destBuf,binBuf,tLen);

    free(binBuf);
    return 0;
}

#define HASH_DIGEST_LENGTH      32

uint32_t validate_bin_data(uint8_t *data,uint32_t dlen)
{
    uint8_t tmpBuf[512];
    uint32_t m,i;
    uint8_t *p;

    m = dlen - HASH_DIGEST_LENGTH;
    memset(tmpBuf,0,sizeof(tmpBuf));
    mbedtls_sha256(data,m,tmpBuf,0);

    p = data + m;
    for(i = 0;i < HASH_DIGEST_LENGTH;i++)
    {
        if(p[i] != tmpBuf[i])
        {
            return 1;
        }
    }

    return 0;

}

