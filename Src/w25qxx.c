#include "w25qxx.h"
#include "qspi.h"
#include "stdio.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//W25QXX QPIģʽ��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/18
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

u16 W25QXX_TYPE = W25Q64;	//Ĭ����W25Q256
u8 W25QXX_QPI_MODE = 0;		//QSPIģʽ��־:0,SPIģʽ;1,QPIģʽ.

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q256
//����Ϊ32M�ֽ�,����512��Block,8192��Sector

void qspi_test(void) {
	uint8_t reg;
	uint32_t i, j, addr;
	uint8_t Data[256], RxData[256];
	for (i = 1; i < 100; i++) {

		W25QXX_Write_Enable();

		QSPI_Send_CMD(W25X_SectorErase, i * 4096, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE); //QPI,���ö�����ָ��,��ַΪ0,4�ߴ�����_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,1���ֽ�����
		printf("erasing %lx sector\n", 4096 * i);
		reg = W25QXX_ReadSR(1);
		printf("After Erase Sector: SR1 0x%02x  ", reg);
		reg = W25QXX_ReadSR(2);
		printf("SR2 0x%02x  ", reg);
		reg = W25QXX_ReadSR(3);
		printf("SR3 0x%02x\n", reg);
		W25QXX_Wait_Busy();

		for (j = 0; j < 16; j++) {
			addr = i * 4096 + 256 * j;
			memset(Data, j, 256);
			W25QXX_Write_Enable();
			QSPI_Send_CMD(W25X_PageProgram, addr, 0, QSPI_INSTRUCTION_4_LINES,
			QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS,
			QSPI_DATA_4_LINES);
			QSPI_Transmit(Data, 256);
			reg = W25QXX_ReadSR(1);
			printf("After Program Page: SR1 0x%02x  ", reg);
			reg = W25QXX_ReadSR(2);
			printf("SR2 0x%02x  ", reg);
			reg = W25QXX_ReadSR(3);
			printf("SR3 0x%02x\n", reg);
			printf("QSPI Program Page addr %lx, write %02x  ", addr, Data[0]);
			W25QXX_Wait_Busy();
//			HAL_Delay(1000);

			//TODO  �Ա� W25X_FastReadData
			QSPI_Send_CMD(W25X_FastReadData, addr, 8, QSPI_INSTRUCTION_4_LINES,
			QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS,
			QSPI_DATA_4_LINES);	//QPI,���ٶ�����,��ַΪReadAddr,4�ߴ�������_32λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,8������,NumByteToRead������
			QSPI_Receive(RxData, 256);
			printf("QSPI read %02x\n", RxData[0]);
			HAL_Delay(100);
		}
	}
}

void spi_test(void) {
	uint8_t reg;
	uint32_t i, j, addr;
	uint8_t TxData[256], RxData[256];
	for (i = 0; i < 100; i++) {

		W25QXX_Write_Enable();

		QSPI_Send_CMD(W25X_SectorErase, i * 4096, 0, QSPI_INSTRUCTION_1_LINE,
		QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE); //QPI,���ö�����ָ��,��ַΪ0,4�ߴ�����_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,1���ֽ�����
		printf("erasing %lx sector\n", 4096 * i);
		reg = W25QXX_ReadSR(1);
		printf("SR1 0x%02x  ", reg);
		reg = W25QXX_ReadSR(2);
		printf("SR2 0x%02x  ", reg);
		reg = W25QXX_ReadSR(3);
		printf("SR3 0x%02x\n", reg);
		W25QXX_Wait_Busy();

		for (j = 0; j < 16; j++) {
			addr = i * 4096 + 256 * j;
			memset(TxData, j + 4, 256);
			W25QXX_Write_Enable();
			QSPI_Send_CMD(W25X_PageProgram, addr, 0, QSPI_INSTRUCTION_1_LINE,
			QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS,
			QSPI_DATA_1_LINE);
			QSPI_Transmit(TxData, 256);
			printf("SPI pp addr %lx, write %02x  ", addr, TxData[0]);
			W25QXX_Wait_Busy();

			QSPI_Send_CMD(W25X_FastReadData, addr, 8, QSPI_INSTRUCTION_1_LINE,
			QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS,
			QSPI_DATA_1_LINE); //QPI,���ٶ�����,��ַΪReadAddr,4�ߴ�������_32λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,8������,NumByteToRead������
			memset(RxData, 0,256);
			QSPI_Receive(RxData, 256);
			printf("SPI read %02x\n", RxData[0]);
			if (memcmp(TxData,RxData,256) == 0)
			{
				printf("SPI PP addr %lx read&write OK\n", addr);
			}else{
				printf("SPI PP addr %lx read&write Fail\n", addr);
			}
			HAL_Delay(100);
		}
	}
}
//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void) {

	GM25XX_QspiRest();
	W25QXX_TYPE = W25QXX_ReadID();	//��ȡFLASH ID.
//	printf("ID:%x\r\n", W25QXX_TYPE);
	spi_test();

	GM25XX_QspiRest();
	W25QXX_Qspi_Enable();			//ʹ��QSPIģʽ
	W25QXX_TYPE = W25QXX_ReadID();	//��ȡFLASH ID.
//	printf("ID:%x\r\n", W25QXX_TYPE);
//	qspi_test();

}

void GM32XX_TEST() {
	int i, j;
	uint32_t addr;
	uint8_t tx_buf[256], rx_buf[256];
	for (i = 0; i < 1; i++) {

		GM25XX_EraseSector(i);
		printf("erasing %d sector\n", i);
		for (j = 0; j < 16; j++) {
			memset(tx_buf, j, 256);
			addr = i * 4096 + j * 256;
			GM25XX_WritePage(tx_buf, 256, addr);
			GM25XX_ReadPage(rx_buf, 256, addr);
			printf("page addr %lx, write data %02x, read data %02x\n", addr,
					tx_buf[0], rx_buf[0]);
			if (memcmp(tx_buf, rx_buf, 256) == 0) {
				printf("Compare write data and read data: consistent\n");
			} else {
				printf("Compare write data and read data: inconsistent\n");
			}
			HAL_Delay(100);
		}
	}
}
void GM25XX_EraseSector(uint32_t sectorNum) {

	uint32_t Dst_Addr = sectorNum * 4096;
	W25QXX_Write_Enable();
	QSPI_Send_CMD(W25X_SectorErase, Dst_Addr, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES); //QPI,���ö�����ָ��,��ַΪ0,4�ߴ�����_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,1���ֽ�����
	W25QXX_Wait_Busy();

}

void GM25XX_WritePage(uint8_t *Data, uint32_t size, uint32_t addr) {
	W25QXX_Write_Enable();
	QSPI_Send_CMD(W25X_PageProgram, addr, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES);
	if (size > 256) {
		printf("WritePage error, invalid params: size is bigger than 256\n");
		return;
	}
	QSPI_Transmit(Data, size);
	W25QXX_Wait_Busy();
}

void GM25XX_ReadPage(uint8_t *data, uint32_t size, uint32_t addr) {
	QSPI_Send_CMD(W25X_FastReadData, addr, 8, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES);	//QPI,���ٶ�����,��ַΪReadAddr,4�ߴ�������_32λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,8������,NumByteToRead������
	QSPI_Receive(data, size);
}
void GM25XX_QspiRest(void) {
	QSPI_Send_CMD(GM25_RSTEN, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//дcommandָ��,��ַΪ0,������_8λ��ַ_�޵�ַ_���ߴ���ָ��,�޿�����,0���ֽ�����

	QSPI_Send_CMD(GM25_RST, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	QSPI_Send_CMD(GM25_RSTEN, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	QSPI_Send_CMD(GM25_RST, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	W25QXX_Wait_Busy();
	printf("nor flash reset\n");
}
//W25QXX����QSPIģʽ
void W25QXX_Qspi_Enable(void) {
//	u8 stareg2;
//	stareg2=W25QXX_ReadSR(2);		//�ȶ���״̬�Ĵ���2��ԭʼֵ
//	if((stareg2&0X02)==0)			//QEλδʹ��
//	{
//		W25QXX_Write_Enable();		//дʹ��
//		stareg2|=1<<1;				//ʹ��QEλ
//		W25QXX_Write_SR(2,stareg2);	//д״̬�Ĵ���2
//	}

	QSPI_Send_CMD(W25X_EnterQPIMode, 0, 0, QSPI_INSTRUCTION_1_LINE,
	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//дcommandָ��,��ַΪ0,������_8λ��ַ_�޵�ַ_���ߴ���ָ��,�޿�����,0���ֽ�����
	W25QXX_QPI_MODE = 1;				//���QSPIģʽ
}

//W25QXX�˳�QSPIģʽ
void W25QXX_Qspi_Disable(void) {
	QSPI_Send_CMD(W25X_ExitQPIMode, 0, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//дcommandָ��,��ַΪ0,������_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,0���ֽ�����
	W25QXX_QPI_MODE = 0;				//���SPIģʽ
}

//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
u8 W25QXX_ReadSR(u8 regno) {
	u8 byte = 0, command = 0;
	switch (regno) {
	case 1:
		command = W25X_ReadStatusReg1;    //��״̬�Ĵ���1ָ��
		break;
	case 2:
		command = W25X_ReadStatusReg2;    //��״̬�Ĵ���2ָ��
		break;
	case 3:
		command = W25X_ReadStatusReg3;    //��״̬�Ĵ���3ָ��
		break;
	default:
		command = W25X_ReadStatusReg1;
		break;
	}
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_4_LINES);	//QPI,дcommandָ��,��ַΪ0,4�ߴ�����_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,1���ֽ�����
	else
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
		QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);	//SPI,дcommandָ��,��ַΪ0,���ߴ�����_8λ��ַ_�޵�ַ_���ߴ���ָ��,�޿�����,1���ֽ�����
	QSPI_Receive(&byte, 1);
	return byte;
}

//дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(u8 regno, u8 sr) {
	u8 command = 0;
	switch (regno) {
	case 1:
		command = W25X_WriteStatusReg1;    //д״̬�Ĵ���1ָ��
		break;
	case 2:
		command = W25X_WriteStatusReg2;    //д״̬�Ĵ���2ָ��
		break;
	case 3:
		command = W25X_WriteStatusReg3;    //д״̬�Ĵ���3ָ��
		break;
	default:
		command = W25X_WriteStatusReg1;
		break;
	}
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_4_LINES);	//QPI,дcommandָ��,��ַΪ0,4�ߴ�����_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,1���ֽ�����
	else
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
		QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);	//SPI,дcommandָ��,��ַΪ0,���ߴ�����_8λ��ַ_�޵�ַ_���ߴ���ָ��,�޿�����,1���ֽ�����
	QSPI_Transmit(&sr, 1);
}

//W25QXXдʹ��
//��S1�Ĵ�����WEL��λ
void W25QXX_Write_Enable(void) {
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(W25X_WriteEnable, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//QPI,дʹ��ָ��,��ַΪ0,������_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,0���ֽ�����
	else
		QSPI_Send_CMD(W25X_WriteEnable, 0, 0, QSPI_INSTRUCTION_1_LINE,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//SPI,дʹ��ָ��,��ַΪ0,������_8λ��ַ_�޵�ַ_���ߴ���ָ��,�޿�����,0���ֽ�����
}

//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(void) {
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(W25X_WriteDisable, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//QPI,д��ָֹ��,��ַΪ0,������_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,0���ֽ�����
	else
		QSPI_Send_CMD(W25X_WriteDisable, 0, 0, QSPI_INSTRUCTION_1_LINE,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//SPI,д��ָֹ��,��ַΪ0,������_8λ��ַ_�޵�ַ_���ߴ���ָ��,�޿�����,0���ֽ�����
}

//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
//0XEF18,��ʾоƬ�ͺ�ΪW25Q256
u16 W25QXX_ReadID(void) {
	u8 temp[5];
	u16 deviceid;
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES);	//QPI,��id,��ַΪ0,4�ߴ�������_24λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,�޿�����,2���ֽ�����
	else
		QSPI_Send_CMD(W25X_ManufactDeviceID, 2, 0, QSPI_INSTRUCTION_1_LINE,
		QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);//SPI,��id,��ַΪ0,���ߴ�������_24λ��ַ_���ߴ����ַ_���ߴ���ָ��,�޿�����,2���ֽ�����
	QSPI_Receive(temp, 3);
	printf("ID %02x%02x%02x\n",temp[0],temp[1],temp[2]);
	deviceid = (temp[0] << 8) | temp[1];
	return deviceid;
}

//��ȡSPI FLASH,��֧��QPIģʽ
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(���32bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead) {
	QSPI_Send_CMD(W25X_FastReadData, ReadAddr, 8, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, QSPI_DATA_4_LINES);	//QPI,���ٶ�����,��ַΪReadAddr,4�ߴ�������_32λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,8������,NumByteToRead������
	QSPI_Receive(pBuffer, NumByteToRead);
}

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(���32bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) {
	W25QXX_Write_Enable();					//дʹ��
	QSPI_Send_CMD(W25X_PageProgram, WriteAddr, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, QSPI_DATA_4_LINES);	//QPI,ҳдָ��,��ַΪWriteAddr,4�ߴ�������_32λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,�޿�����,NumByteToWrite������
	QSPI_Transmit(pBuffer, NumByteToWrite);
	W25QXX_Wait_Busy();					   //�ȴ�д�����
}

//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(���32bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) {
	u16 pageremain;
	pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite; //������256���ֽ�
	while (1) {
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; //д�������
		else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if (NumByteToWrite > 256)
				pageremain = 256; //һ�ο���д��256���ֽ�
			else
				pageremain = NumByteToWrite; 	  //����256���ֽ���
		}
	}
}

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(���32bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) {
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u8 * W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096; 	  //������ַ
	secoff = WriteAddr % 4096; 	  //�������ڵ�ƫ��
	secremain = 4096 - secoff; 	  //����ʣ��ռ��С
	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
	if (NumByteToWrite <= secremain)
		secremain = NumByteToWrite; 	//������4096���ֽ�
	while (1) {
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); 	//������������������
		for (i = 0; i < secremain; i++) 	//У������
				{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break; 	//��Ҫ����
		}
		if (i < secremain) 	//��Ҫ����
				{
			W25QXX_Erase_Sector(secpos); 	//�����������
			for (i = 0; i < secremain; i++)	   //����
					{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);	  //д����������

		} else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		if (NumByteToWrite == secremain)
			break;	   //д�������
		else	   //д��δ����
		{
			secpos++;	   //������ַ��1
			secoff = 0;	   //ƫ��λ��Ϊ0

			pBuffer += secremain;  //ָ��ƫ��
			WriteAddr += secremain;  //д��ַƫ��
			NumByteToWrite -= secremain;				//�ֽ����ݼ�
			if (NumByteToWrite > 4096)
				secremain = 4096;	//��һ����������д����
			else
				secremain = NumByteToWrite;			//��һ����������д����
		}
	};
}

//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void) {
	W25QXX_Write_Enable();					//SET WEL 
	W25QXX_Wait_Busy();
	QSPI_Send_CMD(W25X_ChipErase, 0, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//QPI,дȫƬ����ָ��,��ַΪ0,������_8λ��ַ_�޵�ַ_4�ߴ���ָ��,�޿�����,0���ֽ�����
	W25QXX_Wait_Busy();						//�ȴ�оƬ��������
}

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr) {

	//printf("fe:%x\r\n",Dst_Addr);			//����falsh�������,������
	Dst_Addr *= 4096;
	W25QXX_Write_Enable();                  //SET WEL 	 
	W25QXX_Wait_Busy();
	QSPI_Send_CMD(W25X_SectorErase, Dst_Addr, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, QSPI_DATA_NONE); //QPI,д��������ָ��,��ַΪ0,������_32λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,�޿�����,0���ֽ�����
	W25QXX_Wait_Busy();   				    //�ȴ��������
}

//�ȴ�����
void W25QXX_Wait_Busy(void) {
	while ((W25QXX_ReadSR(1) & 0x01) == 0x01)
		;   // �ȴ�BUSYλ���
}
