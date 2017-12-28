/*
 * Copyright (c) 2013-2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "typedefs.h"
#include "command_packet.h"
#include "command.h"
#include "usb_hid_packetizer.h"

#include "kl81_bootdown.h"



//#define TEST_SENDER_ABORT
//#define TEST_RECEIVER_ABORT

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

uint32_t m_memoryId = 0;

uint32_t response_process(generic_response_packet_t *packet, uint8_t commandTag)
{
    
    if (!packet)
    {
        printf("processResponse: null packet\r\n");
        return 1;
    }

    if (packet->commandPacket.commandTag != kCommandTag_GenericResponse)
    {
        printf("Error: expected kCommandTag_GenericResponse (0x%x), received 0x%x\n", kCommandTag_GenericResponse,
                   packet->commandPacket.commandTag);
        return 1;
    }
    if (packet->commandTag != commandTag)
    {
        printf("Error: expected commandTag 0x%x, received 0x%x\n", commandTag, packet->commandTag);
        return 1;
    }
    PRINTF("Generic response command tag=0x%02X!\r\n",packet->commandPacket.commandTag);
    return 0;
}

uint32_t send_cmd_get_response(uint32_t byteCount, packet_type_t packetType)
{
    uint8_t *responsePacket;
    uint32_t responseLength;

    writePacket((const unsigned char *)&m_packet,byteCount,packetType);
    readPacket((uint8_t **)&responsePacket,&responseLength,packetType);
    property_response_process((get_property_response_packet_t *)responsePacket);

    return 0;
}

uint32_t send_data_check_response(uint8_t *data_buf,uint32_t startaddr,uint32_t lens,flasher_progress_callback *callback)
{
    uint8_t *responsePacket;
    uint32_t responseLength;
    uint32_t i,n;
    uint32_t sector_len;
    uint32_t mask32;

    m_data_property.start_addr = startaddr;
    m_data_property.data_len = lens;
    
    sector_len = m_boot_property.flashSectorSize;
    mask32 = ~(uint32_t)(sector_len - 1);
    m_data_property.start_sector_addr = startaddr & mask32;              //sector size aligned.
    m_data_property.data_sector_len = ((startaddr + lens)  & mask32) - m_data_property.start_sector_addr + sector_len;//sectors to be erased.


    i = 0;
    while(i < lens)
    {
        n = lens - i;
        if(n > 0x20)
            n = 0x20;
        writePacket(data_buf+i,n,kPacketType_Data);
        i += n;
        if((i % 0x100) == 0)
        {
            if(callback != (void *)0)
            {
                callback(i,lens);
            }
        }
    }

    readPacket((uint8_t **)&responsePacket,&responseLength,kPacketType_Data);
    if(callback != (void *)0)
    {
        callback(lens,lens);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Reset command
////////////////////////////////////////////////////////////////////////////////
uint32_t cmd_reset(void)
{
    CommandPacketNoPara(kCommandTag_Reset, kCommandFlag_None);
    send_cmd_get_response(4,kPacketType_Command);
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// GetProperty command
////////////////////////////////////////////////////////////////////////////////
uint32_t property_response_process(const get_property_response_packet_t *packet)
{
    uint32_t peripherals = 0;
    uint32_t position = 0;
    uint32_t commands = 0;
    uint32_t flashLength = 0;
    uint32_t ramLength = 0;
    uint32_t i;

    if (!packet)
    {
        printf("processResponse: null packet\n");
        return 1;
    }

    // Handle generic response, which would be returned if command is not supported.
    if (packet->commandPacket.commandTag == kCommandTag_GenericResponse)
    {
        return response_process((generic_response_packet_t *)packet,packet->propertyValue[0]);
    }

    if (packet->commandPacket.commandTag != kCommandTag_GetPropertyResponse)
    {
        printf("Error: expected kCommandTag_GetPropertyResponse (0x%x), received 0x%x\n",
                   kCommandTag_GetPropertyResponse, packet->commandPacket.commandTag);
        return 1;
    }

    if (packet->status != kStatus_Success)
    {
        return 1;
    }

    PRINTF("Successful response to command '%02X'\n",packet->commandPacket.commandTag);



    // Format the returned property details.
    //switch (packet->commandPacket.commandTag)
    switch (m_packet.m_arguments[0])
    {
        case kPropertyTag_BootloaderVersion:
            printf("Bootloader Version = %c%x.%x.%x\r\n", (packet->propertyValue[0] & 0xff000000) >> 24,
                              (packet->propertyValue[0] & 0x00ff0000) >> 16, (packet->propertyValue[0] & 0x0000ff00) >> 8,
                              packet->propertyValue[0] & 0x000000ff);
            m_boot_property.bootloaderVersion.version = packet->propertyValue[0];
            break;
        case kPropertyTag_TargetVersion:
            
            printf("Current Version = %c%x.%x.%x\r\n", (packet->propertyValue[0] & 0xff000000) >> 24,
                              (packet->propertyValue[0] & 0x00ff0000) >> 16, (packet->propertyValue[0] & 0x0000ff00) >> 8,
                              packet->propertyValue[0] & 0x000000ff);
            m_boot_property.targetVersion.version = packet->propertyValue[0];
            break;
        case kPropertyTag_AvailablePeripherals:
        {
            peripherals = packet->propertyValue[0];
            position = 0;
            printf("\r\n ");
            while (peripherals)
            {
                if (peripherals & 0x00000001)
                {
                    printf("peripheral: %d\r\n",position);
                    if (peripherals >> 1)
                        printf(",\r\n ");
                }
                peripherals >>= 1;
                ++position;
            }
            printf("\r\n ");
            m_boot_property.availablePeripherals = peripherals;
            break;
        }
        case kPropertyTag_FlashStartAddress:
            m_boot_property.flashStartAddress = packet->propertyValue[0];
            printf("FlashStartAddress:0x%08X\r\n",packet->propertyValue[0]);
            break;
        case kPropertyTag_FlashSizeInBytes:
            m_boot_property.flashSizeInBytes = packet->propertyValue[0];
            printf("Flash Size = 0x%08X\r\n", packet->propertyValue[0]);
            break;
        case kPropertyTag_FlashSectorSize:
            m_boot_property.flashSectorSize = packet->propertyValue[0];
            printf("Flash Sector Size = %d\r\n", packet->propertyValue[0]);
            break;
        case kPropertyTag_FlashBlockCount:
            m_boot_property.flashBlockCount =  packet->propertyValue[0];
            printf("Flash Block Count = %d\r\n",packet->propertyValue[0]);
            break;
        case kPropertyTag_AvailableCommands:
        {
            printf("Available Commands = ");
            commands = packet->propertyValue[0];
            position = 0;
            while (commands)
            {
                if (commands & 0x00000001)
                {
                    printf("%d",position);
                    if (commands >> 1)
                        printf(", ");
                }
                commands >>= 1;
                ++position;
            }
            m_boot_property.availableCommands = commands;
            printf("\r\n");
            break;
        }
        case kPropertyTag_CrcCheckStatus:
            m_boot_property.crcCheckStatus = packet->propertyValue[0];
            printf("CRC Check Status = %08X",packet->propertyValue[0]);
            printf("\r\n");
            break;
        case kPropertyTag_VerifyWrites:
            m_boot_property.verifyWrites = packet->propertyValue[0];
            printf("Verify Writes Flag = %s", packet->propertyValue[0] ? "ON" : "OFF");
            printf("\r\n");
            break;
        case kPropertyTag_MaxPacketSize:
            printf("Max Packet Size = %d", packet->propertyValue[0]);
            printf("\r\n");
            break;
        case kPropertyTag_ReservedRegions:
        {
            flashLength = packet->propertyValue[1] - packet->propertyValue[0] + 1;
            if (flashLength == 1)
                flashLength = 0;
            ramLength = packet->propertyValue[3] - packet->propertyValue[2] + 1;
            if (ramLength == 1)
                ramLength = 0;
            printf("Reserved Regions = ");
            printf("Flash: 0x%X-0x%X, ",packet->propertyValue[0], packet->propertyValue[1]);
            printf("RAM: 0x%X-0x%X", packet->propertyValue[2],packet->propertyValue[3]);
            m_boot_property.reservedRegions[0].startAddress = packet->propertyValue[0];
            m_boot_property.reservedRegions[0].endAddress = packet->propertyValue[1];
            m_boot_property.reservedRegions[1].startAddress = packet->propertyValue[2];
            m_boot_property.reservedRegions[1].endAddress = packet->propertyValue[3];
            printf("\r\n");
            break;
        }
        case kPropertyTag_RAMStartAddress:
            printf("RAM Start Address = 0x%08X",packet->propertyValue[0]);
            m_boot_property.ramStartAddress[0] = packet->propertyValue[0];
            printf("\r\n");
            break;
        case kPropertyTag_RAMSizeInBytes:
            printf("RAM Size = 0x%08X", packet->propertyValue[0]);
            m_boot_property.ramSizeInBytes[0] = packet->propertyValue[0];
            printf("\r\n");
            break;
        case kPropertyTag_SystemDeviceId:
            printf("System Device ID = 0x%08X", packet->propertyValue[0]);
            printf("\r\n");
            break;
        case kPropertyTag_FlashSecurityState:
            printf("Flash Security State = %s", packet->propertyValue[0] ? "SECURE" : "UNSECURE");
            printf("\r\n");
            break;
        case kPropertyTag_UniqueDeviceId:
            printf("Unique Device ID =");
            for (i = 0; i < 4; ++i)
            {
                printf(" %08X", packet->propertyValue[i]);
            }
            printf("\r\n");
            m_boot_property.UniqueDeviceId.uidl = packet->propertyValue[0];
            m_boot_property.UniqueDeviceId.uidml = packet->propertyValue[1];
            m_boot_property.UniqueDeviceId.uidmh = packet->propertyValue[2];
            m_boot_property.UniqueDeviceId.uidh = packet->propertyValue[3];
            break;
        case kPropertyTag_FacSupport:
            printf("Flash Access Controller (FAC) Support Flag = %s",
                                              packet->propertyValue[0] ? "SUPPORTED" : "UNSUPPORTED");
            printf("\r\n");
            break;
        case kPropertyTag_FlashAccessSegmentSize:
            printf("Flash Access Segment Size = %08X\r\n", packet->propertyValue[0]);
            break;
        case kPropertyTag_FlashAccessSegmentCount:
            printf("Flash Access Segment Count = %d\r\n", packet->propertyValue[0]);
            break;
        case kPropertyTag_FlashReadMargin:
            printf( "Flash read margin level = ");
            if (packet->propertyValue[0] == 0)
                printf("NORMAL");
            else if (packet->propertyValue[0] == 1)
                printf("USER");
            else if (packet->propertyValue[0] == 2)
                printf("FACTORY");
            else
                printf("UNKNOWN (%d)", packet->propertyValue[0]);
            printf("\r\n");
            break;
        case kPropertyTag_QspiInitStatus:
        case kPropertyTag_ExternalMemoryAttributes:
        case kPropertyTag_ReliableUpdateStatus:
        case kPropertyTag_Reserved9:
        case kPropertyTag_InvalidProperty:
        default:
            break;
    }

    return 0;
}

uint32_t cmd_get_property(void)
{
	CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_SystemDeviceId,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_FlashSectorSize,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_FlashStartAddress,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_FlashSizeInBytes,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_FlashBlockCount,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_FlashSecurityState,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_RAMSizeInBytes,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_ReservedRegions,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_AvailableCommands,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_BootloaderVersion,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    CommandPacketTwoPara(kCommandTag_GetProperty, kCommandFlag_None,kPropertyTag_TargetVersion,m_memoryId);
    send_cmd_get_response(0x0C,kPacketType_Command);
    

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// SetProperty command
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// FlashEraseRegion command
////////////////////////////////////////////////////////////////////////////////

uint32_t cmd_erase_region(void)
{
    uint32_t addr;
    uint32_t len;

    addr = m_data_property.start_sector_addr;
    len = m_data_property.data_sector_len;

    CommandPacketTwoPara(kCommandTag_FlashEraseRegion, kCommandFlag_None,addr,len);
    send_cmd_get_response(0x0C,kPacketType_Command);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// FlashEraseAll command
////////////////////////////////////////////////////////////////////////////////

uint32_t cmd_erase_all(void)
{
    CommandPacketNoPara(kCommandTag_FlashEraseAll, kCommandFlag_None);
    send_cmd_get_response(4,kPacketType_Command);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// FlashEraseAllUnsecure command
////////////////////////////////////////////////////////////////////////////////
uint32_t cmd_erase_all_unsecure(void)
{
    CommandPacketNoPara(kCommandTag_FlashEraseAllUnsecure, kCommandFlag_None);
    send_cmd_get_response(4,kPacketType_Command);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// ReadMemory command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// WriteMemory command
////////////////////////////////////////////////////////////////////////////////
uint32_t cmd_write_memory(uint32_t startaddr,uint32_t lens)
{

    CommandPacketTwoPara(kCommandTag_WriteMemory, kCommandFlag_None,startaddr,lens);
    send_cmd_get_response(0x0C,kPacketType_Command);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// FillMemory command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// ReceiveSbFile command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Execute command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Call command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FlashSecurityDisable command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FlashProgramOnce command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FlashReadOnce command
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// FlashReadResource command
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Configure QuadSpi command
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Configure Peripheral command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// ReliableUpdate command
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// FlashImage command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Configure I2C command
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Configure SPI command
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Configure CAN command
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
