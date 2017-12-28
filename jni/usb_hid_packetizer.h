#ifndef _peripheral_h
#define _peripheral_h

#include <stdint.h>
#include "command_packet.h"

enum _hid_report_ids
{
    kBootloaderReportID_CommandOut = 1,
    kBootloaderReportID_DataOut = 2,
    kBootloaderReportID_CommandIn = 3,
    kBootloaderReportID_DataIn = 4
};

//! @brief Structure of a bootloader HID header.
typedef struct _bl_hid_header
{
    uint8_t reportID; //!< The report ID.
    uint8_t _padding; //!< Pad byte necessary so that the length is 2-byte aligned and the data is 4-byte aligned. Set
    //! to zero.
    uint8_t packetLengthLsb; //!< Low byte of the packet length in bytes.
    uint8_t packetLengthMsb; //!< High byte of the packet length in bytes.
} bl_hid_header_t;

//! @brief Structure of a bootloader HID report.
typedef struct _bl_hid_report
{
    bl_hid_header_t header;               //!< Header of the report.
    uint8_t packet[kMinPacketBufferSize]; //!< The packet data that is transferred in the report.
} bl_hid_report_t;


//! @addtogroup peripheral
//! @{

////////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////////

//! @brief Peripheral type bit mask definitions.
//!
//! These bit mask constants serve multiple purposes. They are each a unique value that identifies
//! a peripheral type. They are also the mask for the bits used in the bootloader configuration
//! flash region to list available peripherals and control which peripherals are enabled.
enum _peripheral_types
{
    kPeripheralType_UART = (1 << 0),
    kPeripheralType_I2CSlave = (1 << 1),
    kPeripheralType_SPISlave = (1 << 2),
    kPeripheralType_CAN = (1 << 3),
    kPeripheralType_USB_HID = (1 << 4),
    kPeripheralType_USB_CDC = (1 << 5),
    kPeripheralType_USB_DFU = (1 << 6),
    kPeripheralType_USB_MSC = (1 << 7)
};

//! @brief Pinmux types.
typedef enum _pinmux_types
{
    kPinmuxType_Default = 0,
    kPinmuxType_GPIO = 1,
    kPinmuxType_Peripheral = 2
} pinmux_type_t;


//! @brief Packet types.
typedef enum _packet_type
{
    kPacketType_Command, //!< Send or expect a command packet
    kPacketType_Data     //!< Send or expect a data packet
} packet_type_t;



enum _usbhid_contants
{
    kReadFlushTimeoutMs = 100000,
#if defined(BL_WITHOUT_RECEIVE_SB_FILE_CMD)
    kPollAbortTimeoutMs = 0,
#else
    kPollAbortTimeoutMs = 10,
#endif
    kPollPacketMaxRetryCnt = 50,
    kContinuousReadMargin = 2,
};


extern status_t writePacket(const uint8_t *packet, uint32_t byteCount, packet_type_t packetType);
extern status_t readPacket(uint8_t **packet, uint32_t *packetLength, packet_type_t packetType);
#endif // _peripheral_h
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////