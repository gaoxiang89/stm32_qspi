#include "gm25qxx.h"
#include "qspi.h"
#include "stdio.h"
#include "string.h"

static nor_flash_mode_t m_work_mode;
static uint16_t m_device_id;

void gm25qxx_read_all_sr(void)
{
	uint8_t reg[3];
	reg[0] = gm25qxx_read_sr(1);
	reg[1] = gm25qxx_read_sr(2);
	reg[2] = gm25qxx_read_sr(3);
	printf("SR1 0x%02x  SR2 0x%02x  SR3 0x%02x\n", reg[0], reg[1], reg[2]);
}
/**
 * @brief 初始化 gm25qxx
 */
void gm25qxx_init(void)
{
	// send 0x66 and 0x99 reset flash
	gm25qxx_reset();

	// send 0x90 to read manufactory id and device id
	gm25qxx_read_id();

	// read 1 2 3 status register
	gm25qxx_read_all_sr();

	// enable QE bit in status register 2. 0x02
	gm25qxx_qspi_enable();

	// check QE bit
	gm25qxx_get_mode();

	// do a loop:  erase  program and read test
	nor_flash_spi_loop_test();
}

/**
 * @brief 复位 gm25qxx
 */
void gm25qxx_reset(void)
{
	QSPI_Send_CMD(NOR_FLASH_ENABLE_RESET, 0, 0, QSPI_INSTRUCTION_1_LINE,
				  QSPI_ADDRESS_NONE,
				  QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	QSPI_Send_CMD(NOR_FLASH_RESET, 0, 0, QSPI_INSTRUCTION_1_LINE,
				  QSPI_ADDRESS_NONE,
				  QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);

	gm25qxx_wait_busy();
	printf("nor flash reset\n");
	m_work_mode = NOR_SPI_MODE; // default mode
}

/**
 * @brief 读取Manufacture ID和Device ID.
 * @retval 返回uint16_t格式ID. 高8位为Manufacture ID，低8位为Device ID
 */
uint16_t gm25qxx_read_id(void)
{
	uint8_t buf[5];
	/**
	 *  指令        0x90    1线
	 *  地址        0x00    1线
	 *  DummyCycles 0x00
	 *  数据                1线
	 */
	QSPI_Send_CMD(NOR_FLASH_MANUFACTURER_DEVICE_ID, 0, 0,
				  QSPI_INSTRUCTION_1_LINE,
				  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
	/**
	 *  接收两字节数据
	 *  第一个Byte: Manufacture ID
	 *  第二个Byte: Device ID
	 */
	QSPI_Receive(buf, 2);
	printf("ID %02x%02x\n", buf[0], buf[1]);
	/* 合并成uint6_t */
	m_device_id = (buf[0] << 8) | buf[1];
	return m_device_id;
}

/**
 * @brief 读取状态寄存器.
 * @param index 状态寄存器编号 1 2 3
 * @retval 状态寄存器值
 */
uint8_t gm25qxx_read_sr(uint8_t index)
{
	uint8_t byte = 0, command = NOR_FLASH_READ_STATUS_REG_1;
	switch (index)
	{
	case 1:
		command = NOR_FLASH_READ_STATUS_REG_1; //读状态寄存器1指令
		break;
	case 2:
		command = NOR_FLASH_READ_STATUS_REG_2; //读状态寄存器2指令
		break;
	case 3:
		command = NOR_FLASH_READ_STATUS_REG_3; //读状态寄存器3指令
		break;
	default:
		break;
	}
	/**
	 *  指令        0x05/0x35/0x15    1线
	 *  地址        0x00              1线
	 *  DummyCycles 0x00
	 *  数据                          1线
	 */
	QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
				  QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);

	/* */
	QSPI_Receive(&byte, 1);
	return byte;
}
void gm25qxx_write_sr(uint8_t index, uint8_t sr)
{
	uint8_t command = 0;
	switch (index)
	{
	case 1:
		command = NOR_FLASH_WRITE_STATUS_REG_1; //写状态寄存器1指令
		break;
	case 2:
		command = NOR_FLASH_WRITE_STATUS_REG_2; //写状态寄存器2指令
		break;
	case 3:
		command = NOR_FLASH_WRITE_STATUS_REG_3; //写状态寄存器3指令
		break;
	default:
		command = NOR_FLASH_WRITE_STATUS_REG_1;
		break;
	}

	gm25qxx_write_enable(); //对于non-volatile的SR，需要先WRITE_ENABLE
	printf("\nwrite sr cmd %02x  reg %02x\n", command, sr);
	QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE,
				  QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
	QSPI_Transmit(&sr, 1);
	gm25qxx_wait_busy(); //使用WRITE_ENABLE后，相对应的需要等待busy完成
}

/**
 * @brief 使能QSPI模式，设置状态寄存器2中EQ位
 */
void gm25qxx_qspi_enable(void)
{
	uint8_t sr, sr1;
	sr = gm25qxx_read_sr(2);
	if ((sr & ENABLE_QSPI_BIT_MASK) != ENABLE_QSPI_BIT_MASK)
	{
		sr1 = sr | ENABLE_QSPI_BIT_MASK; // 设置SR2中QE为1
		printf("enable qspi. sr %02x\n", sr1);
		gm25qxx_write_sr(2, sr1);
	}
	m_work_mode = NOR_QSPI_MODE;
}

/**
 * @brief 禁止QSPI模式，清除状态寄存器2中EQ位
 */
void gm25qxx_qspi_disable(void)
{
	uint8_t sr, sr1;
	sr = gm25qxx_read_sr(2);
	if ((sr & ENABLE_QSPI_BIT_MASK) == ENABLE_QSPI_BIT_MASK)
	{
		sr1 = sr & (~sr);
		printf("disable qspi. sr %02x\n", sr1);
		gm25qxx_write_sr(2, sr1);
	}
	m_work_mode = NOR_SPI_MODE;
}

void gm25qxx_get_mode(void)
{
	uint8_t reg;
	reg = gm25qxx_read_sr(2);
	if ((reg & ENABLE_QSPI_BIT_MASK) == ENABLE_QSPI_BIT_MASK)
	{
		m_work_mode = NOR_QSPI_MODE;
		printf("gm25qxx work mode NOR_QSPI_MODE\n");
	}
	else
	{
		m_work_mode = NOR_SPI_MODE;
		printf("gm25qxx work mode NOR_SPI_MODE\n");
	}
}

void gm25qxx_set_output_drv_strength(uint8_t strength)
{
	uint8_t reg, reg1;
	reg = gm25qxx_read_sr(3); // read status register 3
	reg1 = (reg & strength) | (reg & 0x0F);
	gm25qxx_write_sr(3, reg1);
}

void gm25qxx_write_enable(void)
{
	/**
	 *  发送指令 0x05/0x35/0x15，单线模式
	 *  地址        0x00              1线
	 *  DummyCycles 0x00
	 *  数据                          NONE
	 */
	QSPI_Send_CMD(NOR_FLASH_WRITE_ENABLE, 0, 0, QSPI_INSTRUCTION_1_LINE,
				  QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}
void gm25qxx_write_disable(void)
{
	/**
	 *  指令        0x05/0x35/0x15    1线
	 *  地址        0x00              NONE
	 *  DummyCycles 0x00
	 *  数据                          NONE
	 */
	QSPI_Send_CMD(NOR_FLASH_WRITE_DISABLE, 0, 0, QSPI_INSTRUCTION_1_LINE,
				  QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE); //SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
}

void gm25qxx_write_enable_volatile_sr(void)
{
	QSPI_Send_CMD(NOR_FLASH_WRITE_ENABLE_VOLATILE, 0, 0, QSPI_INSTRUCTION_1_LINE,
				  QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

void gm25qxx_wait_busy(void)
{
	while ((gm25qxx_read_sr(1) & WAIT_BUSY_BIT_MASK) == WAIT_BUSY_BIT_MASK)
		; // 等待BUSY位清空

	// printf("no busy\n");
}
void gm25qxx_erase_sector(uint32_t sectorNum)
{
	uint32_t addr = sectorNum * 4096;
	gm25qxx_write_enable();
	QSPI_Send_CMD(NOR_FLASH_SECTOR_ERASE, addr, 0, QSPI_INSTRUCTION_1_LINE,
				  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE); //QPI,设置读参数指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
	gm25qxx_wait_busy();
}
void gm25qxx_write(uint8_t *p_data, uint32_t addr, uint16_t size)
{
	if (m_work_mode == NOR_QSPI_MODE)
	{
	}
	else
	{
	}
}
void gm25qxx_read(uint8_t *p_data, uint32_t addr, uint16_t size)
{
	if (m_work_mode == NOR_QSPI_MODE)
	{
		//QSPI Fast read quad output,快速读数据,地址为ReadAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,8空周期,size个数据
		QSPI_Send_CMD(NOR_FLASH_FAST_READ_QUAD_O, addr, 8,
					  QSPI_INSTRUCTION_1_LINE,
					  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES);
		QSPI_Receive(p_data, size);
	}
	else
	{
		//SPI Fast read,快速读数据,地址为ReadAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,8空周期,size个数据
		QSPI_Send_CMD(NOR_FLASH_FAST_READ, addr, 0, QSPI_INSTRUCTION_1_LINE,
					  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
		QSPI_Receive(p_data, size);
	}
}

/**
 * 
 **/
void nor_flash_spi_loop_test(void)
{
	uint8_t reg;
	uint32_t i, j, addr;
	uint8_t TxData[256], RxData[256];
	uint8_t RxDataDual[256], RxDataQuad[256];
	for (i = 0; i < 100; i++)
	{
		// Write Enable
		gm25qxx_write_enable();
		// Send 0x20 CMD. Erase No.xx sector
		QSPI_Send_CMD(NOR_FLASH_SECTOR_ERASE, i * SECTOR_SIZE, 0, QSPI_INSTRUCTION_1_LINE,
					  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE);
		// Wait for busy bit clear
		gm25qxx_wait_busy();
		printf("erasing No. %lx sector\n", i * SECTOR_SIZE);

		for (j = 0; j < PAGE_NUM_PER_SECTOR; j++)
		{
			// Calculate page starting address
			addr = i * SECTOR_SIZE + j * PAGE_SIZE;
			memset(TxData, j, 256);

			// Send PP/0x02 CMD to program whole page
			gm25qxx_write_enable();
			QSPI_Send_CMD(NOR_FLASH_PAGE_PROGRAM, addr, 0, QSPI_INSTRUCTION_1_LINE,
						  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS,
						  QSPI_DATA_1_LINE);
			QSPI_Transmit(TxData, 256);

			gm25qxx_wait_busy();

			// Send Fast_Read(1 line) CMD to read whole page
			QSPI_Send_CMD(NOR_FLASH_FAST_READ, addr, 8, QSPI_INSTRUCTION_1_LINE,
						  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS,
						  QSPI_DATA_1_LINE);
			memset(RxData, 0, 256);
			QSPI_Receive(RxData, 256);
			printf("SPI pp addr %lx, write %02x  and SPI/1 line read %02x", addr, TxData[0], RxData[0]);
			if (memcmp(TxData, RxData, 256) == 0)
			{
				printf("SPI PP & SPI FAST READ  addr %lx read&write OK\n", addr);
			}
			else
			{
				printf("SPI PP & SPI FAST READ  addr %lx read&write Fail\n", addr);
			}

			// Send Fast_Read_Dual_Output(2 line) CMD to read whole page
			QSPI_Send_CMD(NOR_FLASH_FAST_READ_DUAL_O, addr, 8, QSPI_INSTRUCTION_1_LINE,
						  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS,
						  QSPI_DATA_2_LINES);
			memset(RxDataDual, 0, 256);
			QSPI_Receive(RxDataDual, 256);
			printf("SPI pp addr %lx, write %02x  and Dual/2 line read %02x", addr, TxData[0], RxDataDual[0]);
			if (memcmp(TxData, RxDataDual, 256) == 0)
			{
				printf("SPI PP & DUAL FAST READ  addr %lx read&write OK\n", addr);
			}
			else
			{
				printf("SPI PP & DUAL FAST READ  addr %lx read&write Fail\n", addr);
			}

			// Send Fast_Read_Quad_Output(4 line) CMD to read whole page
			HAL_Delay(100); // 需要延时，解决高低位翻转问题，单独使用没问题
			QSPI_Send_CMD(NOR_FLASH_FAST_READ_QUAD_O, addr, 8, QSPI_INSTRUCTION_1_LINE,
						  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS,
						  QSPI_DATA_4_LINES);
			memset(RxDataQuad, 0, 256);
			QSPI_Receive(RxDataQuad, 256);
			printf("SPI pp addr %lx, write %02x  and Quad/4 line read %02x", addr, TxData[0], RxDataDual[0]);
			if (memcmp(TxData, RxDataQuad, 256) == 0)
			{
				printf("SPI PP & Quad FAST READ  addr %lx read&write OK\n", addr);
			}
			else
			{
				printf("SPI PP & Quad FAST READ  addr %lx read&write Fail\n", addr);
			}

			// TODO  test Fast read quad input and output
			// QSPI_Send_CMD(NOR_FLASH_FAST_READ_QUAD_IO, addr, 8, QSPI_INSTRUCTION_1_LINE,
			// 			  QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS,
			// 			  QSPI_DATA_4_LINES);
			// memset(RxDataQuad_IO, 0, 256);
			// QSPI_Receive(RxDataQuad_IO, 256);

			HAL_Delay(100);
		}
	}
}