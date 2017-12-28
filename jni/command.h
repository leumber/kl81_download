/*
 * Copyright (c) 2013-2015, Freescale Semiconductor, Inc.
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

#ifndef _Command_h_
#define _Command_h_

#include <assert.h>
#include <stdint.h>
#include "typedefs.h"
#include "command_packet.h"
    //! @brief Bootloader command mask.
enum COMMAND_MASK
{
    kCommandMask_FlashEraseAll = 0x00000001,
    kCommandMask_ReadMemory = 0x00000004,
    kCommandMask_WriteMemory = 0x00000008,
    kCommandMask_FillMemory = 0x00000010,
    kCommandMask_FlashSecurityDisable = 0x00000020,
    kCommandMask_GetProperty = 0x00000040,
    kCommandMask_ReceiveSBFile = 0x00000080,
    kCommandMask_Execute = 0x00000100,
    kCommandMask_Call = 0x00000200,
    kCommandMask_Reset = 0x00000400,
    kCommandMask_SetProperty = 0x00000800,
    kCommandMask_FlashEraseAllUnsecure = 0x00001000,
    kCommandMask_FlashProgramOnce = 0x00002000,
    kCommandMask_FlashReadOnce = 0x00004000,
    kCommandMask_FlashReadResource = 0x00008000,
    kCommandMask_ConfigureQuadSpi = 0x00010000,
};

//! @brief Bootloader property.
enum PROPERTY_TAG
{
    kPropertyTag_ListProperties = 0x00,
    kPropertyTag_BootloaderVersion = 0x01,
    kPropertyTag_AvailablePeripherals = 0x02,
    kPropertyTag_FlashStartAddress = 0x03,
    kPropertyTag_FlashSizeInBytes = 0x04,
    kPropertyTag_FlashSectorSize = 0x05,
    kPropertyTag_FlashBlockCount = 0x06,
    kPropertyTag_AvailableCommands = 0x07,
    kPropertyTag_CrcCheckStatus = 0x08,
    kPropertyTag_Reserved9 = 0x09,
    kPropertyTag_VerifyWrites = 0x0a,
    kPropertyTag_MaxPacketSize = 0x0b,
    kPropertyTag_ReservedRegions = 0x0c,
    kPropertyTag_Reserved13 = 0x0d,
    kPropertyTag_RAMStartAddress = 0x0e,
    kPropertyTag_RAMSizeInBytes = 0x0f,
    kPropertyTag_SystemDeviceId = 0x10,
    kPropertyTag_FlashSecurityState = 0x11,
    kPropertyTag_UniqueDeviceId = 0x12,
    kPropertyTag_FacSupport = 0x13,
    kPropertyTag_FlashAccessSegmentSize = 0x14,
    kPropertyTag_FlashAccessSegmentCount = 0x15,
    kPropertyTag_FlashReadMargin = 0x16,
    kPropertyTag_QspiInitStatus = 0x17,
    kPropertyTag_TargetVersion = 0x18,
    kPropertyTag_ExternalMemoryAttributes = 0x19,
    kPropertyTag_ReliableUpdateStatus = 0x1a,
    kPropertyTag_InvalidProperty = 0xFF,
};

extern uint32_t cmd_reset(void);
extern uint32_t cmd_get_property(void);
extern uint32_t cmd_erase_region(void);
extern uint32_t cmd_erase_all(void);
extern uint32_t cmd_erase_all_unsecure(void);
extern uint32_t cmd_write_memory(uint32_t startaddr,uint32_t lens);

extern uint32_t send_data_check_response(uint8_t *data_buf,uint32_t startaddr,uint32_t lens,flasher_progress_callback *callback);
extern uint32_t property_response_process(const get_property_response_packet_t *packet);
#endif // _Command_h_

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
