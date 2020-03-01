#ifndef __NOR_FLASH_H
#define __NOR_FLASH_H

#include "stdint.h"

#define GM25Q128A_ID 0x1C17


#define NOR_FLASH_WRITE_ENABLE                    0x06
#define NOR_FLASH_WRITE_DISABLE                   0x04
#define NOR_FLASH_WRITE_ENABLE_VOLATILE           0x50

#define NOR_FLASH_ENABLE_RESET                    0x66
#define NOR_FLASH_RESET                           0x99

#define NOR_FLASH_READ_STATUS_REG_1               0x05
#define NOR_FLASH_READ_STATUS_REG_2               0x35
#define NOR_FLASH_READ_STATUS_REG_3               0x15
#define NOR_FLASH_WRITE_STATUS_REG_1              0x01
#define NOR_FLASH_WRITE_STATUS_REG_2              0x31
#define NOR_FLASH_WRITE_STATUS_REG_3              0x11

#define NOR_FLASH_READ_DATA                       0x03
#define NOR_FLASH_FAST_READ                       0x0B
#define NOR_FLASH_FAST_READ_DUAL_O                0x3B
#define NOR_FLASH_FAST_READ_QUAD_O                0x6B
#define NOR_FLASH_FAST_READ_DUAL_IO               0xBB
#define NOR_FLASH_FAST_READ_QUAD_IO               0xEB
#define NOR_FLASH_WORD_READ_QUAD_IO               0xE7

// #define NOR_FLASH_OCTAL_WORD_READ_QUAD_IO     

#define NOR_FLASH_PAGE_PROGRAM                    0x02
#define NOR_FLASH_QUAD_I_PAGE_PROGRAM             0x32
#define NOR_FLASH_SECTOR_ERASE                    0x20
#define NOR_FLASH_32K_BLOCK_ERASE                 0x52
#define NOR_FLASH_64K_BLOCK_ERASE                 0xD8
#define NOR_FLASH_CHIP_ERASE                      0x60

#define NOR_FLASH_ERASE_PP_SUSPEND                0x75
#define NOR_FLASH_ERASE_PP_RESUME                 0x7A

#define NOR_FLASH_POWER_DOWN                      0xB9
#define NOR_FLASH_POWER_DOWN_RELEASE              0xAB

#define NOR_FLASH_MANUFACTURER_DEVICE_ID          0x90
#define NOR_FLASH_READ_JEDEC_ID                   0x9F
#define NOR_FLASH_READ_SFDP                       0x5A

// #define NOR_FLASH_READ_UID
#define NOR_FLASH_ERASE_SECURITY_REG              0x44
#define NOR_FLASH_PROG_SECURITY_REG               0x42
#define NOR_FLASH_READ_SECURITY_REG               0x48

// #define NOR_FLASH_EQPI
// #define NOR_FLASH_RSTQPI
// #define NOR_FLASH_ENTER_OTP
#define NOR_FLASH_SET_BURST_WITH_WRAP             0x77
// #define NOR_FLASH_BURST_READ_WITH_WRAP
// #define NOR_FLASH_GLOBAL_BLOCK_LOCK
// #define NOR_FLASH_GLOBAL_BLOCK_UNLOCK
// #define NOR_FLASH_INDIVIDUAL_BLOCL_LOCK
// #define NOR_FLASH_INDIVIDUAL_BLOCL_UNLOCK
// #define NOR_FLASH_READ_BLOCL_LOCK
// #define NOR_FLASH_SET_READ_PARAMS


#define WAIT_BUSY_BIT_MASK                       0x01
#define ENABLE_QSPI_BIT_MASK                     0x02

#define PAGE_SIZE                                256
#define SECTOR_SIZE                              4096
#define PAGE_NUM_PER_SECTOR                      SECTOR_SIZE/PAGE_SIZE

typedef enum {
    NOR_SPI_MODE = 0,
    NOR_QSPI_MODE,      // GM25Q128A support Fast read quad output
    // NOR_QPI_MODE     // GM25Q128A do not support QPI mode
}nor_flash_mode_t;


#define	GM25Q128A_DRV_100  0x00
#define	GM25Q128A_DRV_75   0x02
#define	GM25Q128A_DRV_50   0x40
#define	GM25Q128A_DRV_25   0x60



void gm25qxx_init(void);
void gm25qxx_reset(void);

uint16_t gm25qxx_read_id(void);

uint8_t gm25qxx_read_sr(uint8_t index);
void gm25qxx_write_sr(uint8_t index, uint8_t sr);

void gm25qxx_qspi_enable(void);
void gm25qxx_qspi_disable(void);
void gm25qxx_get_mode(void);



void gm25qxx_write_enable(void);
void gm25qxx_write_disable(void);
void gm25qxx_write_enable_volatile_sr(void);

void gm25qxx_wait_busy(void);
void gm25qxx_erase_sector(uint32_t sectorNum);
void gm25qxx_write(uint8_t *p_data, uint32_t addr, uint16_t size);
void gm25qxx_read(uint8_t *p_data, uint32_t addr, uint16_t size);

void nor_flash_spi_loop_test(void);
void nor_flash_qspi_loop_test(void);
void nor_flash_spi_loop_test_2(void);

#endif
