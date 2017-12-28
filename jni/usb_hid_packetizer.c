#include "usb_hid_packetizer.h"
#include "command_packet.h"
#include "bootloader_flasher.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint32_t pollForAbortPacket();
extern int  hid_read_timeout(unsigned char *data, size_t length, uint32_t *actualBytes,int milliseconds);
extern int  hid_write_timeout( const unsigned char *data,size_t length,int milliseconds);
enum _bl_status_groups
{
    kStatusGroup_Bootloader = kStatusGroup_ApplicationRangeStart, //!< Bootloader status group number (100).
    kStatusGroup_SBLoader,                                        //!< SB loader status group number (101).
    kStatusGroup_MemoryInterface,                                 //!< Memory interface status group number (102).
    kStatusGroup_PropertyStore,                                   //!< Property store status group number (103).
    kStatusGroup_AppCrcCheck,                                     //!< Application crc check status group number (104).
    kStatusGroup_Packetizer,                                      //!< Packetizer status group number (105).
    kStatusGroup_ReliableUpdate                                   //!< Reliable Update status groupt number (106).
};
enum _packetizer_status
{
    kStatus_NoPingResponse = MAKE_STATUS(kStatusGroup_Packetizer, 0),
    kStatus_InvalidPacketType = MAKE_STATUS(kStatusGroup_Packetizer, 1),
    kStatus_InvalidCRC = MAKE_STATUS(kStatusGroup_Packetizer, 2),
    kStatus_NoCommandResponse = MAKE_STATUS(kStatusGroup_Packetizer, 3)
};

uint16_t m_options;           //!< Framing protocol options bitfield.
uint32_t m_startTime;          //!< Beginning time of packet transaction.
uint32_t m_packetTimeoutMs;
uint32_t m_isAbortEnabled; //!< True if allowing abort packet. Not used by all packetizers.


bl_hid_report_t m_report;      //!< Used for building and receiving the report.
bl_hid_report_t m_abortReport; //!< Used for received abort report.
uint32_t m_continuousReadCount; //!< Used for distinguish abort report for write-memory/receive-sb-file or read-memory

// See usb_hid_packetizer.h for documentation of this method.
status_t writePacket(const uint8_t *packet, uint32_t byteCount, packet_type_t packetType)
{
    //assert(m_peripheral);
    //if (byteCount)
    //{
    //    assert(packet);
    //}

    // Determine report ID based on packet type.
    uint8_t reportID;
    switch (packetType)
    {
        case kPacketType_Command:
            reportID = kBootloaderReportID_CommandOut;
            break;
        case kPacketType_Data:
            reportID = kBootloaderReportID_DataOut;
            break;
        default:
            printf("usbhid: unsupported packet type %d\n", (int)packetType);
            return kStatus_Fail;
    };

    //if (m_isAbortEnabled && (reportID == kBootloaderReportID_DataOut))
//    if ((reportID == kBootloaderReportID_DataOut))
//    {
//        // Check if the target has sent an abort report.
//        if (pollForAbortPacket())
//        {
//            printf("usb hid detected receiver data abort\n");
//            return -1;
//        }
//    }

    // Construct report contents.
    memset(&m_report, 0, sizeof(m_report));
    m_report.header.reportID = reportID;
    m_report.header.packetLengthLsb = byteCount & 0xff;
    m_report.header.packetLengthMsb = (byteCount >> 8) & 0xff;

    // If not a zero-length report, copy in packet data.
    if (byteCount)
    {
        memcpy(m_report.packet, packet, byteCount);
    }

    m_continuousReadCount = 0;

    return hid_write_timeout((uint8_t *)&m_report, sizeof(m_report), m_packetTimeoutMs);
}

// See usb_hid_packetizer.h for documentation of this method.
status_t readPacket(uint8_t **packet, uint32_t *packetLength, packet_type_t packetType)
{
        // Read report.
    uint32_t actualBytes = 0;
    uint16_t lengthInPacket = 0;
    uint32_t retryCnt = 0;
    uint8_t reportID;

    //assert(m_peripheral);
    //assert(packet);
    //assert(packetLength);
    *packet = NULL;
    *packetLength = 0;

    // Determine report ID based on packet type.
    
    switch (packetType)
    {
        case kPacketType_Command:
            reportID = kBootloaderReportID_CommandIn;
            break;
        case kPacketType_Data:
            reportID = kBootloaderReportID_DataIn;
            break;
        default:
            printf("usbhid: unsupported packet type %d\n", (int)packetType);
            return kStatus_Fail;
    };


    do
    {
        status_t retVal =
            hid_read_timeout((unsigned char *)&m_report, sizeof(m_report), &actualBytes, m_packetTimeoutMs);
        if (retVal != kStatus_Success)
        {
            return retVal;
        }

        if (!retryCnt)
        {
            m_continuousReadCount++;
        }

        // Check the report ID.
        if (m_report.header.reportID != reportID)
        {
            //printf("usbhid: received unexpected report=%x\n", m_report.header.reportID);
            return kStatus_Fail;
        }

        // Extract the packet length encoded as bytes 1 and 2 of the report. The packet length
        // is transferred in little endian byte order.
        lengthInPacket = m_report.header.packetLengthLsb | (m_report.header.packetLengthMsb << 8);

        // See if we received the data abort packet,
        // if the data abort packet was received, try to read pending generic response packet.
        if (lengthInPacket == 0)
        {
            //printf("usbhid: received data phase abort\n");

            // If continuous read occurs, that means this is a read-memory command,
            // host tool shouldn't wait for pending packet because there are no pending packets.
            if (m_continuousReadCount >= kContinuousReadMargin)
            {
                break;
            }
            retryCnt++;
        }
    } while (actualBytes && !lengthInPacket && retryCnt < kPollPacketMaxRetryCnt);

    // Make sure we got all of the packet. Target will send the maximum
    // report size, so there may be extra trailing bytes.
    if ((actualBytes - sizeof(m_report.header)) < lengthInPacket)
    {
        //printf("usbhid: received only %ld bytes of packet with length %d\n", actualBytes - sizeof(m_report.header),
        //           lengthInPacket);
        return kStatus_Fail;
    }

    // Return results.
    *packet = m_report.packet;
    *packetLength = lengthInPacket;

    return kStatus_Success;
}

// See usb_hid_packetizer.h for documentation of this method.
void flushInput()
{
    uint32_t actualBytes = 0;
    do
    {
        hid_read_timeout((unsigned char *)&m_report, sizeof(m_report), &actualBytes, m_packetTimeoutMs);
    } while (actualBytes > 0);
}

// See usb_hid_packetizer.h for documentation of this method.
void abortPacket()
{
    // Abort data phase by writing a zero-length command packet.
    writePacket(NULL, 0, kPacketType_Command);
    flushInput();
}

uint32_t pollForAbortPacket()
{
    // Just check to see if there is data to be read from hid device.
    // No reason to wait (milliseconds = 0), because we aren't really expecting anything.
    uint32_t actualBytes = 0;
    hid_read_timeout((unsigned char *)&m_abortReport, sizeof(m_abortReport), &actualBytes, kPollAbortTimeoutMs);
    if (actualBytes == 0)
    {
        // No abort packet
        return 0;
    }
    else
    {
        // Got an abort packet
        return 1;
    }
}

// See usb_hid_packetizer.h for documentation of this method.
uint32_t getMaxPacketSize()
{
    return kMinPacketBufferSize;
}