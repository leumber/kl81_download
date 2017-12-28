#ifndef __TYPE_DEFS_H__
#define __TYPE_DEFS_H__

#include <stdint.h>

//�ص��������ͣ����ڽ�����ʾ���ⲿ����
typedef void flasher_progress_callback(unsigned int current,unsigned int total);

//! @brief Format of bootloader configuration data on Flash.
typedef struct BootloaderConfigurationData
{
    uint32_t tag; //!< [00:03] Tag value used to validate the bootloader configuration data. Must be set to 'kcfg'.
    uint32_t crcStartAddress;              //!< [04:07]
    uint32_t crcByteCount;                 //!< [08:0b]
    uint32_t crcExpectedValue;             //!< [0c:0f]
    uint8_t enabledPeripherals;            //!< [10:10]
    uint8_t i2cSlaveAddress;               //!< [11:11]
    uint16_t peripheralDetectionTimeoutMs; //!< [12:13] Timeout in milliseconds for peripheral detection before jumping
    //! to application code
    uint16_t usbVid;                    //!< [14:15]
    uint16_t usbPid;                    //!< [16:17]
    uint32_t usbStringsPointer;         //!< [18:1b]
    uint8_t clockFlags;                 //!< [1c:1c] High Speed and other clock options
    uint8_t clockDivider;               //!< [1d:1d] One's complement of clock divider, zero divider is divide by 1
    uint8_t bootFlags;                  //!< [1e:1e] One's complemnt of direct boot flag, 0xFE represents direct boot
    uint8_t pad0;                       //!< [1f:1f] Reserved, set to 0xFF
    uint32_t mmcauConfigPointer;        //!< [20:23] Holds a pointer value to the MMCAU configuration
    uint32_t keyBlobPointer;            //!< [24:27] Holds a pointer value to the key blob array used to configure OTFAD
    uint8_t pad1;                       //!< [28:28] reserved
    uint8_t canConfig1;                 //!< [29:29] ClkSel[1], PropSeg[3], SpeedIndex[4]
    uint16_t canConfig2;                //!< [2a:2b] Pdiv[8], Pseg1[3], Pseg2[3],  rjw[2]
    uint16_t canTxId;                   //!< [2c:2d] txId
    uint16_t canRxId;                   //!< [2e:2f] rxId
    uint32_t qspi_config_block_pointer; //!< [30:33] QSPI config block pointer.
} bootloader_configuration_data_t;

typedef union StandardVersion
{
    struct
    {
        uint8_t bugfix; //!< bugfix version [7:0]
        uint8_t minor;  //!< minor version [15:8]
        uint8_t major;  //!< major version [23:16]
        char name;      //!< name [31:24]
    };
    uint32_t version; //!< combined version numbers
} standard_version_t;

enum _property_constants
{
    kProperty_ReservedRegionsCount = 2,
    kProperty_FlashReservedRegionIndex = 0,
    kProperty_RamReservedRegionIndex = 1,

    kProperty_FlashVersionIdSizeInBytes = 8,
};

//! @brief Structure of a reserved regions entry.
typedef struct ReservedRegion
{
    uint32_t startAddress;
    uint32_t endAddress;
} reserved_region_t;

//! @brief Structure of a unique device id.
typedef struct UniqueDeviceId
{
    uint32_t uidl;
    uint32_t uidml;
    uint32_t uidmh;
    uint32_t uidh;
} unique_device_id_t;

//! @brief External Memory Properties tag
enum _external_memory_property_tags
{
    kExternalMemoryPropertyTag_InitStatus = 0,         //!< Init status tag
    kExternalMemoryPropertyTag_StartAddress = 1,       //!< Start address tag
    kExternalMemoryPropertyTag_MemorySizeInKbytes = 2, //!< Memory size tag
    kExternalMemoryPropertyTag_PageSize = 3,           //!< Pag size tag
    kExternalMemoryPropertyTag_SectorSize = 4,         //!< Sector size tag
    kExternalMemoryPropertyTag_BlockSize = 5,          //!< Block size tag

    kExternalMemoryPropertyTag_Start = kExternalMemoryPropertyTag_StartAddress,
    kExternalMemoryPropertyTag_End = kExternalMemoryPropertyTag_BlockSize,
};

//! @brief Exernal Memory attribute store
typedef struct
{
    uint32_t availableAttributesFlag; //!< Available Atrributes, bit map
    uint32_t startAddress;            //!< start Address of external memory
    uint32_t flashSizeInKB;           //!< flash size of external memory
    uint32_t pageSize;                //!< page size of external memory
    uint32_t sectorSize;              //!< sector size of external memory
    uint32_t blockSize;               //!< block size of external memory
} external_memory_property_store_t;

enum _ram_constants
{
    kRAMCount = 1,
};
typedef struct PropertyStore
{
    standard_version_t bootloaderVersion;     //!< Current bootloader version.
    standard_version_t serialProtocolVersion; //!< Serial protocol version number.
    standard_version_t targetVersion;         //!< Target version number.
    uint32_t availablePeripherals;            //!< The set of peripherals supported available on this chip. See enum
                                              //!_peripheral_types in bl_peripheral.h.
    uint32_t flashStartAddress;               //!< Start address of program flash.
    uint32_t flashSizeInBytes;                //!< Size in bytes of program flash.
    uint32_t flashSectorSize; //!< The size in bytes of one sector of program flash. This is the minimum erase size.
    uint32_t flashBlockSize;  //!< The size in bytes of one block of program flash.
    uint32_t flashBlockCount; //!< Number of blocks in the flash array.
    uint32_t ramStartAddress[kRAMCount]; //!< Start address of RAM
    uint32_t ramSizeInBytes[kRAMCount];  //!< Size in bytes of RAM
    uint32_t crcCheckStatus;             //!< Status code from the last CRC check operation.
    uint32_t verifyWrites; //!< Boolean controlling whether the bootloader will verify writes to flash. Non-zero enables
    //! verificaton. Writable by host.
    uint32_t availableCommands;        //!< Bit mask of the available commands.
    unique_device_id_t UniqueDeviceId; //!< Unique identification for the device.
    uint32_t flashFacSupport;          //!< Boolean indicating whether the FAC feature is supported
    uint32_t flashAccessSegmentSize;   //!< The size in bytes of one segment of flash
    uint32_t flashAccessSegmentCount;  //!< The count of flash access segment within flash module
    uint32_t flashReadMargin;          //!< The margin level setting for flash erase and program Verify CMDs
    uint32_t qspiInitStatus;           //!< Result of QSPI+OTFAD init during bootloader startup
    reserved_region_t reservedRegions[kProperty_ReservedRegionsCount]; //!< Flash and Ram reserved regions.
    bootloader_configuration_data_t
        configurationData; //!< Configuration data from flash address 0x3c0-0x3ff in sector 0 (64 bytes max)
    external_memory_property_store_t externalMemoryPropertyStore; //!< Property store for external memory
    uint32_t reliableUpdateStatus;                                //!< Status of reliable update
} property_store_t;

typedef struct _data_property {
    uint32_t start_addr;
    uint32_t data_len;
    uint32_t start_sector_addr;     //!< start address of sector for data storage>
    uint32_t data_sector_len;       //!< length of sectors in bytes needed for data storage>
}data_property;

#endif
