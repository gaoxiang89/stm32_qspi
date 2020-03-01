#ifndef __W25QXX_H
#define __W25QXX_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������ 
//W25QXX QPIģʽ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/18
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//W25Xϵ��/Qϵ��оƬ�б�	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
//W25Q256 ID  0XEF18
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

extern u16 W25QXX_TYPE;					//����W25QXXоƬ�ͺ�		   
 
////////////////////////////////////////////////////////////////////////////////// 
//ָ���
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 //0x09  //0x35
#define W25X_ReadStatusReg3		0x15 //0x95  //0x15
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9
#define W25X_SetReadParam		0xC0 
#define W25X_EnterQPIMode       0x38
#define W25X_ExitQPIMode        0xFF

#define GM25_RSTEN              0x66
#define GM25_RST                0x99


//#define NOR_FLASH_WRITE_ENABLE             0x06
//#define NOR_FLASH_WRITE_DISABLE            0x04
//#define NOR_FLASH_WRITE_ENABLE_VOLATILE    0x50
//
//#define NOR_FLASH_ENABLE_RESET             0x66
//#define NOR_FLASH_RESET                    0x99
//
//#define NOR_FLASH_READ_STATUS_REG_1        0x05
//#define NOR_FLASH_READ_STATUS_REG_2        0x35
//#define NOR_FLASH_READ_STATUS_REG_3        0x15
//#define NOR_FLASH_WRITE_STATUS_REG_1       0x01
//#define NOR_FLASH_WRITE_STATUS_REG_2       0x31
//#define NOR_FLASH_WRITE_STATUS_REG_3       0x11
//
//#define NOR_FLASH_READ_DATA                0x03
//#define NOR_FLASH_FAST_READ                0x0b
//#define NOR_FLASH_FAST_READ_DUAL_O         0x3B
//#define NOR_FLASH_FAST_READ_QUAD_O         0x6B
//#define NOR_FLASH_FAST_READ_DUAL_IO        0xBB
//#define NOR_FLASH_FAST_READ_QUAD_IO        0xEB
//#define NOR_FLASH_WORD_READ_QUAD_IO        0xE7   // XIP
//
//#define NOR_FLASH_PAGE_PROGRAM             0x02
//#define NOR_FLASH_QUAD_I_PAGE_PROGRAM      0x32
//#define NOR_FLASH_SECTOR_ERASE             0x20
//#define NOR_FLASH_32K_BLOCK_ERASE          0x52
//#define NOR_FLASH_64K_BLOCK_ERASE          0xD8
//#define NOR_FLASH_CHIP_ERASE               0x60 // or 0xC7
//
//#define NOR_FLASH_ERASE_PP_SUSPEND         0x75
//#define NOR_FLASH_ERASE_PP_RESUME          0x7A
//
//#define NOR_FLASH_POWER_DOWN               0xB9
//#define NOR_FLASH_POWER_DOWN_RELEASE       0xAB
//
//#define NOR_FLASH_MANUFACTURER_ID          0x90
//
//#define NOR_FLASH_READ_JEDEC_ID            0x9F
//#define NOR_FLASH_READ_SFDP                0x5A
//
//#define NOR_FLASH_ERASE_SECURITY_REG       0x44
//#define NOR_FLASH_PROG_SECURITY_REG        0x42
//#define NOr_FLASH_READ_SECURITY_REG        0x48



void W25QXX_Init(void);					//��ʼ��W25QXX
void W25QXX_Qspi_Enable(void);			//ʹ��QSPIģʽ
void W25QXX_Qspi_Disable(void);			//�ر�QSPIģʽ
u16  W25QXX_ReadID(void);  	    		//��ȡFLASH ID
u8 W25QXX_ReadSR(u8 regno);             //��ȡ״̬�Ĵ��� 
void W25QXX_4ByteAddr_Enable(void);     //ʹ��4�ֽڵ�ַģʽ
void W25QXX_Write_SR(u8 regno,u8 sr);   //д״̬�Ĵ���
void W25QXX_Write_Enable(void);  		//дʹ�� 
void W25QXX_Write_Disable(void);		//д����
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//дflash,��У��
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void W25QXX_Erase_Chip(void);    	  	//��Ƭ����
void W25QXX_Erase_Sector(u32 Dst_Addr);	//��������
void W25QXX_Wait_Busy(void);           	//�ȴ�����

void GM25XX_QspiRest(void);
void GM25XX_EraseSector(uint32_t sectorNum);
void GM25XX_WritePage(uint8_t *Data, uint32_t size, uint32_t addr);
void GM25XX_ReadPage(uint8_t *Data, uint32_t size, uint32_t addr);
void GM32XX_TEST(void);
#endif
