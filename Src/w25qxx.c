#include "w25qxx.h"
#include "qspi.h"
#include "stdio.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//W25QXX QPI模式驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/7/18
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

u16 W25QXX_TYPE = W25Q64;	//默认是W25Q256
u8 W25QXX_QPI_MODE = 0;		//QSPI模式标志:0,SPI模式;1,QPI模式.

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q256
//容量为32M字节,共有512个Block,8192个Sector

void qspi_test(void) {
	uint8_t reg;
	uint32_t i, j, addr;
	uint8_t Data[256], RxData[256];
	for (i = 1; i < 100; i++) {

		W25QXX_Write_Enable();

		QSPI_Send_CMD(W25X_SectorErase, i * 4096, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE); //QPI,设置读参数指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
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

			//TODO  对比 W25X_FastReadData
			QSPI_Send_CMD(W25X_FastReadData, addr, 8, QSPI_INSTRUCTION_4_LINES,
			QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS,
			QSPI_DATA_4_LINES);	//QPI,快速读数据,地址为ReadAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,8空周期,NumByteToRead个数据
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
		QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE); //QPI,设置读参数指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
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
			QSPI_DATA_1_LINE); //QPI,快速读数据,地址为ReadAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,8空周期,NumByteToRead个数据
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
//初始化SPI FLASH的IO口
void W25QXX_Init(void) {

	GM25XX_QspiRest();
	W25QXX_TYPE = W25QXX_ReadID();	//读取FLASH ID.
//	printf("ID:%x\r\n", W25QXX_TYPE);
	spi_test();

	GM25XX_QspiRest();
	W25QXX_Qspi_Enable();			//使能QSPI模式
	W25QXX_TYPE = W25QXX_ReadID();	//读取FLASH ID.
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
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES); //QPI,设置读参数指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
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
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES);	//QPI,快速读数据,地址为ReadAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,8空周期,NumByteToRead个数据
	QSPI_Receive(data, size);
}
void GM25XX_QspiRest(void) {
	QSPI_Send_CMD(GM25_RSTEN, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//写command指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据

	QSPI_Send_CMD(GM25_RST, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	QSPI_Send_CMD(GM25_RSTEN, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	QSPI_Send_CMD(GM25_RST, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
	QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	W25QXX_Wait_Busy();
	printf("nor flash reset\n");
}
//W25QXX进入QSPI模式
void W25QXX_Qspi_Enable(void) {
//	u8 stareg2;
//	stareg2=W25QXX_ReadSR(2);		//先读出状态寄存器2的原始值
//	if((stareg2&0X02)==0)			//QE位未使能
//	{
//		W25QXX_Write_Enable();		//写使能
//		stareg2|=1<<1;				//使能QE位
//		W25QXX_Write_SR(2,stareg2);	//写状态寄存器2
//	}

	QSPI_Send_CMD(W25X_EnterQPIMode, 0, 0, QSPI_INSTRUCTION_1_LINE,
	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//写command指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
	W25QXX_QPI_MODE = 1;				//标记QSPI模式
}

//W25QXX退出QSPI模式
void W25QXX_Qspi_Disable(void) {
	QSPI_Send_CMD(W25X_ExitQPIMode, 0, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//写command指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	W25QXX_QPI_MODE = 0;				//标记SPI模式
}

//读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
//状态寄存器1：
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//状态寄存器2：
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//状态寄存器3：
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:状态寄存器号，范:1~3
//返回值:状态寄存器值
u8 W25QXX_ReadSR(u8 regno) {
	u8 byte = 0, command = 0;
	switch (regno) {
	case 1:
		command = W25X_ReadStatusReg1;    //读状态寄存器1指令
		break;
	case 2:
		command = W25X_ReadStatusReg2;    //读状态寄存器2指令
		break;
	case 3:
		command = W25X_ReadStatusReg3;    //读状态寄存器3指令
		break;
	default:
		command = W25X_ReadStatusReg1;
		break;
	}
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_4_LINES);	//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
	else
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
		QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);	//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
	QSPI_Receive(&byte, 1);
	return byte;
}

//写W25QXX状态寄存器
void W25QXX_Write_SR(u8 regno, u8 sr) {
	u8 command = 0;
	switch (regno) {
	case 1:
		command = W25X_WriteStatusReg1;    //写状态寄存器1指令
		break;
	case 2:
		command = W25X_WriteStatusReg2;    //写状态寄存器2指令
		break;
	case 3:
		command = W25X_WriteStatusReg3;    //写状态寄存器3指令
		break;
	default:
		command = W25X_WriteStatusReg1;
		break;
	}
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_4_LINES);	//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
	else
		QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
		QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);	//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
	QSPI_Transmit(&sr, 1);
}

//W25QXX写使能
//将S1寄存器的WEL置位
void W25QXX_Write_Enable(void) {
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(W25X_WriteEnable, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//QPI,写使能指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	else
		QSPI_Send_CMD(W25X_WriteEnable, 0, 0, QSPI_INSTRUCTION_1_LINE,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//SPI,写使能指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
}

//W25QXX写禁止
//将WEL清零
void W25QXX_Write_Disable(void) {
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(W25X_WriteDisable, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	else
		QSPI_Send_CMD(W25X_WriteDisable, 0, 0, QSPI_INSTRUCTION_1_LINE,
		QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
}

//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
//0XEF18,表示芯片型号为W25Q256
u16 W25QXX_ReadID(void) {
	u8 temp[5];
	u16 deviceid;
	if (W25QXX_QPI_MODE)
		QSPI_Send_CMD(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_4_LINES,
		QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES);	//QPI,读id,地址为0,4线传输数据_24位地址_4线传输地址_4线传输指令,无空周期,2个字节数据
	else
		QSPI_Send_CMD(W25X_ManufactDeviceID, 2, 0, QSPI_INSTRUCTION_1_LINE,
		QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);//SPI,读id,地址为0,单线传输数据_24位地址_单线传输地址_单线传输指令,无空周期,2个字节数据
	QSPI_Receive(temp, 3);
	printf("ID %02x%02x%02x\n",temp[0],temp[1],temp[2]);
	deviceid = (temp[0] << 8) | temp[1];
	return deviceid;
}

//读取SPI FLASH,仅支持QPI模式
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(最大32bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead) {
	QSPI_Send_CMD(W25X_FastReadData, ReadAddr, 8, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, QSPI_DATA_4_LINES);	//QPI,快速读数据,地址为ReadAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,8空周期,NumByteToRead个数据
	QSPI_Receive(pBuffer, NumByteToRead);
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25QXX_Write_Page(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) {
	W25QXX_Write_Enable();					//写使能
	QSPI_Send_CMD(W25X_PageProgram, WriteAddr, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, QSPI_DATA_4_LINES);	//QPI,页写指令,地址为WriteAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,无空周期,NumByteToWrite个数据
	QSPI_Transmit(pBuffer, NumByteToWrite);
	W25QXX_Wait_Busy();					   //等待写入结束
}

//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) {
	u16 pageremain;
	pageremain = 256 - WriteAddr % 256; //单页剩余的字节数
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite; //不大于256个字节
	while (1) {
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; //写入结束了
		else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain;			  //减去已经写入了的字节数
			if (NumByteToWrite > 256)
				pageremain = 256; //一次可以写入256个字节
			else
				pageremain = NumByteToWrite; 	  //不够256个字节了
		}
	}
}

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) {
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u8 * W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096; 	  //扇区地址
	secoff = WriteAddr % 4096; 	  //在扇区内的偏移
	secremain = 4096 - secoff; 	  //扇区剩余空间大小
	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
	if (NumByteToWrite <= secremain)
		secremain = NumByteToWrite; 	//不大于4096个字节
	while (1) {
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); 	//读出整个扇区的内容
		for (i = 0; i < secremain; i++) 	//校验数据
				{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break; 	//需要擦除
		}
		if (i < secremain) 	//需要擦除
				{
			W25QXX_Erase_Sector(secpos); 	//擦除这个扇区
			for (i = 0; i < secremain; i++)	   //复制
					{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);	  //写入整个扇区

		} else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);//写已经擦除了的,直接写入扇区剩余区间.
		if (NumByteToWrite == secremain)
			break;	   //写入结束了
		else	   //写入未结束
		{
			secpos++;	   //扇区地址增1
			secoff = 0;	   //偏移位置为0

			pBuffer += secremain;  //指针偏移
			WriteAddr += secremain;  //写地址偏移
			NumByteToWrite -= secremain;				//字节数递减
			if (NumByteToWrite > 4096)
				secremain = 4096;	//下一个扇区还是写不完
			else
				secremain = NumByteToWrite;			//下一个扇区可以写完了
		}
	};
}

//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(void) {
	W25QXX_Write_Enable();					//SET WEL 
	W25QXX_Wait_Busy();
	QSPI_Send_CMD(W25X_ChipErase, 0, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);//QPI,写全片擦除指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	W25QXX_Wait_Busy();						//等待芯片擦除结束
}

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr) {

	//printf("fe:%x\r\n",Dst_Addr);			//监视falsh擦除情况,测试用
	Dst_Addr *= 4096;
	W25QXX_Write_Enable();                  //SET WEL 	 
	W25QXX_Wait_Busy();
	QSPI_Send_CMD(W25X_SectorErase, Dst_Addr, 0, QSPI_INSTRUCTION_4_LINES,
	QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, QSPI_DATA_NONE); //QPI,写扇区擦除指令,地址为0,无数据_32位地址_4线传输地址_4线传输指令,无空周期,0个字节数据
	W25QXX_Wait_Busy();   				    //等待擦除完成
}

//等待空闲
void W25QXX_Wait_Busy(void) {
	while ((W25QXX_ReadSR(1) & 0x01) == 0x01)
		;   // 等待BUSY位清空
}

