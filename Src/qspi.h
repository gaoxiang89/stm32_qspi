#ifndef __QSPI_H
#define __QSPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//QSPI��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


u8 QSPI_Init(void);												//��ʼ��QSPI
void QSPI_Send_CMD(u32 instruction,u32 address,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode);			//QSPI��������
u8 QSPI_Receive(u8* buf,u32 datalen);							//QSPI��������
u8 QSPI_Transmit(u8* buf,u32 datalen);							//QSPI��������


uint8_t gm25qxx_spi_init(void);


uint8_t gm25qxx_qspi_init(void);
void gm25qxx_qspi_send_cmd(u32 instruction,u32 address,u32 dummy_cycles,u32 instruction_mode,u32 address_mode,u32 address_size,u32 data_mode);
uint32_t gm25qxx_qspi_read(uint8_t *data, uint32_t size);
uint32_t gm25qxx_qspi_write(uint8_t *data, uint32_t size);
#endif
