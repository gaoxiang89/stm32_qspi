#ifndef __QSPI_H
#define __QSPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//QSPI驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


u8 QSPI_Init(void);												//初始化QSPI
void QSPI_Send_CMD(u32 instruction,u32 address,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode);			//QSPI发送命令
u8 QSPI_Receive(u8* buf,u32 datalen);							//QSPI接收数据
u8 QSPI_Transmit(u8* buf,u32 datalen);							//QSPI发送数据


uint8_t gm25qxx_spi_init(void);


uint8_t gm25qxx_qspi_init(void);
void gm25qxx_qspi_send_cmd(u32 instruction,u32 address,u32 dummy_cycles,u32 instruction_mode,u32 address_mode,u32 address_size,u32 data_mode);
uint32_t gm25qxx_qspi_read(uint8_t *data, uint32_t size);
uint32_t gm25qxx_qspi_write(uint8_t *data, uint32_t size);
#endif
