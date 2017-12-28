#include <stdint.h>
#include "typedefs.h"
#include "command_packet.h"

property_store_t m_boot_property;
data_property m_data_property;
CMD_PACKET m_packet;

void m_packet_init(uint8_t tag, uint8_t flags, uint8_t numArguments)
{
    m_packet.m_header.commandTag = tag;
    m_packet.m_header.flags = flags;
    m_packet.m_header.reserved = 0;
    m_packet.m_header.parameterCount = numArguments;
}

//! @brief Constructor that takes no command arguments.
void CommandPacketNoPara(uint8_t tag, uint8_t flags)
{
    uint8_t m_numArguments;
    flags = 0;
    m_numArguments = 0;
    m_packet_init(tag, flags, m_numArguments);
}

//! @brief Constructor that takes one command argument.
void CommandPacketOnePara(uint8_t tag, uint8_t flags, uint32_t arg)
{
    uint8_t m_numArguments;
    m_numArguments = 1;
    m_packet_init(tag, flags, m_numArguments);
    m_packet.m_arguments[0] = arg;
}

//! @brief Constructor that takes two command arguments.
void CommandPacketTwoPara(uint8_t tag, uint8_t flags, uint32_t arg1, uint32_t arg2)
{
    uint8_t m_numArguments;
    m_numArguments = 2;
    m_packet_init(tag, flags, m_numArguments);
    m_packet.m_arguments[0] = arg1;
    m_packet.m_arguments[1] = arg2;
}

//! @brief Constructor that takes three command arguments.
void CommandPacketThreePara(uint8_t tag, uint8_t flags, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    uint8_t m_numArguments;
    m_numArguments = 3;
    m_packet_init(tag, flags, m_numArguments);
    m_packet.m_arguments[0] = arg1;
    m_packet.m_arguments[1] = arg2;
    m_packet.m_arguments[2] = arg3;
}

//! @brief Constructor that takes four command arguments.
void CommandPacketFourPara(uint8_t tag, uint8_t flags, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    uint8_t m_numArguments;
    m_numArguments = 4;
    m_packet_init(tag, flags, m_numArguments);
    m_packet.m_arguments[0] = arg1;
    m_packet.m_arguments[1] = arg2;
    m_packet.m_arguments[2] = arg3;
    m_packet.m_arguments[3] = arg4;
}




