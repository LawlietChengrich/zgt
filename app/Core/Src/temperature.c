#include "temperature.h"
#include "tim.h"
#include "stm32f4xx_it.h"
//#include "const_data.h"
#include "can_app.h"
#include "math.h"

#define DELAY_MESSURE_DS18B20			(3)

uint8_t DS18B20_ID[MaxSensorNum][8];	// 存检测到的传感器DS18B20_ID的数组,前面的维数代表单根线传感器数量上限
uint8_t DS18B20_SensorNum = 0;			// 检测到的传感器数量(从1开始，例如显示1代表1个，8代表8个)
float TemperatureDDD[MaxSensorNum];
volatile static uint32_t ds18B20_messure_delay_unit = 0;

// 引脚输入
void DS18B20_Mode_IPU(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = DA_18B20_Pin;
	GPIO_InitStructure.Mode = LL_GPIO_MODE_INPUT;
	LL_GPIO_Init(DA_18B20_GPIO_Port, &GPIO_InitStructure);
}

// 引脚输出
void DS18B20_Mode_Out(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = DA_18B20_Pin;
	GPIO_InitStructure.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_LOW;
	LL_GPIO_Init(DA_18B20_GPIO_Port, &GPIO_InitStructure);
}

void DS18B20_DQ_OUT_Low(void)
{
    LL_GPIO_ResetOutputPin(DA_18B20_GPIO_Port, DA_18B20_Pin);
}

void DS18B20_DQ_OUT_High(void)
{
    LL_GPIO_SetOutputPin(DA_18B20_GPIO_Port, DA_18B20_Pin);
}

// 复位，主机给从机发送复位脉冲
void DS18B20_Rst(void)
{
	DS18B20_Mode_Out();
	DS18B20_DQ_OUT_Low();		// 产生至少480us的低电平复位信号
	dh_timer_us_block_delay(480);
	DS18B20_DQ_OUT_High();	// 在产生复位信号后，需将总线拉高
	dh_timer_us_block_delay(15);
}

// 检测从机给主机返回的应答脉冲。从机接收到主机的复位信号后，会在15~60us后给主机发一个应答脉冲
uint8_t DS18B20_Answer_Check(void)
{
	uint8_t delay = 0;
	DS18B20_Mode_IPU(); // 主机设置为上拉输入
	// 等待应答脉冲（一个60~240us的低电平信号 ）的到来
	// 如果100us内，没有应答脉冲，退出函数，注意：从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
	while (LL_GPIO_IsInputPinSet(DA_18B20_GPIO_Port, DA_18B20_Pin)&&delay < 100)
	{
		delay++;
		dh_timer_us_block_delay(1);
	}
	// 经过100us后，如果没有应答脉冲，退出函数
	if (delay >= 100)//Hu200
		return 1;
	else
		delay = 0;
	// 有应答脉冲，且存在时间不超过240us
	while (!LL_GPIO_IsInputPinSet(DA_18B20_GPIO_Port, DA_18B20_Pin)&&delay < 240)
	{
		delay++;
		dh_timer_us_block_delay(1);
	}
	if (delay >= 240)
		return 1;
	return 0;
}

// 从DS18B20读取1个位
uint8_t DS18B20_Read_Bit(void)
{
	uint8_t data;
	DS18B20_Mode_Out();
	DS18B20_DQ_OUT_Low(); // 读时间的起始：必须由主机产生 >1us <15us 的低电平信号
	dh_timer_us_block_delay(2);
	DS18B20_DQ_OUT_High();
	dh_timer_us_block_delay(12);
	DS18B20_Mode_IPU();// 设置成输入，释放总线，由外部上拉电阻将总线拉高

	if (LL_GPIO_IsInputPinSet(DA_18B20_GPIO_Port, DA_18B20_Pin))
		data = 1;
	else
		data = 0;

	dh_timer_us_block_delay(50);
	return data;
}

// 从DS18B20读取2个位
uint8_t DS18B20_Read_2Bit(void)//读二位 子程序
{
	uint8_t i;
	uint8_t dat = 0;
	for (i = 2; i > 0; i--)
	{
		dat = dat << 1;
		DS18B20_Mode_Out();
		DS18B20_DQ_OUT_Low();
		dh_timer_us_block_delay(2);
		DS18B20_DQ_OUT_High();
		DS18B20_Mode_IPU();
		dh_timer_us_block_delay(12);
		if (LL_GPIO_IsInputPinSet(DA_18B20_GPIO_Port, DA_18B20_Pin))	dat |= 0x01;
		dh_timer_us_block_delay(50);
	}
	return dat;
}

// 从DS18B20读取1个字节
uint8_t DS18B20_Read_Byte(void)	// read one byte
{
	uint8_t i, j, dat;
	dat = 0;
	for (i = 0; i < 8; i++)
	{
		j = DS18B20_Read_Bit();
		dat = (dat) | (j << i);
	}
	return dat;
}

// 写1位到DS18B20
void DS18B20_Write_Bit(uint8_t dat)
{
	DS18B20_Mode_Out();
	if (dat)
	{
		DS18B20_DQ_OUT_Low();// Write 1
		dh_timer_us_block_delay(2);
		DS18B20_DQ_OUT_High();
		dh_timer_us_block_delay(60);
	}
	else
	{
		DS18B20_DQ_OUT_Low();// Write 0
		dh_timer_us_block_delay(60);
		DS18B20_DQ_OUT_High();
		dh_timer_us_block_delay(2);
	}
}

// 写1字节到DS18B20
void DS18B20_Write_Byte(uint8_t dat)
{
	uint8_t j;
	uint8_t testb;
	DS18B20_Mode_Out();
	for (j = 1; j <= 8; j++)
	{
		testb = dat & 0x01;
		dat = dat >> 1;
		if (testb)
		{
			DS18B20_DQ_OUT_Low();// 写1
			dh_timer_us_block_delay(10);
			DS18B20_DQ_OUT_High();
			dh_timer_us_block_delay(50);
		}
		else
		{
			DS18B20_DQ_OUT_Low();// 写0
			dh_timer_us_block_delay(60);
			DS18B20_DQ_OUT_High();// 释放总线
			dh_timer_us_block_delay(2);
		}
	}
}

#if 0
void temp_foramt_convert(float temp_f, uint8_t i)
{
	int8_t tmp1;
	uint16_t tmp2;

	tmp1 = round(temp_f);
	temp_data.dh_ds18b20_temp[i].data_z = (uint8_t)tmp1;
	if(temp_f < 0.0f)
	{
		//负数需要变正取余，再变回负数
		temp_f = 0 - temp_f;
		temp_f = temp_f * 100;
		tmp2 = round(temp_f);
		tmp1 = tmp2%100;
		tmp1 = 0 - tmp1;
	}
	else
	{
		temp_f = temp_f * 100;
		tmp2 = round(temp_f);
		tmp1 = tmp2%100;
	}

	temp_data.dh_ds18b20_temp[i].data_f = (uint8_t)tmp1;
}
#endif

// 从ds18b20得到温度值，精度：0.1C，返回温度值（-550~1250），Temperature1返回浮点实际温度
void DS18B20_Get_Temp(uint8_t i)
{
	//uint8_t flag;
	uint8_t j = 0;//匹配的字节
	uint8_t TL, TH;
	short Temperature = 0;
	float Temperature1 = 0;

	DS18B20_Rst();
	DS18B20_Answer_Check();

	DS18B20_Write_Byte(0x55);
	for (j = 0; j < 8; j++)
	{
		DS18B20_Write_Byte(DS18B20_ID[i][j]);
	}


	//DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0x44);// convert
	DS18B20_Rst();
	DS18B20_Answer_Check();

	 //DS18B20_Write_Byte(0xcc);// skip rom
	//匹配ID，i为形参

	DS18B20_Write_Byte(0x55);
	for (j = 0; j < 8; j++)
	{
		DS18B20_Write_Byte(DS18B20_ID[i][j]);
	}

	DS18B20_Write_Byte(0xbe);// convert
	TL = DS18B20_Read_Byte(); // LSB   
	TH = DS18B20_Read_Byte(); // MSB  

	
	Temperature = (TH << 8) | TL;
	if (Temperature>0x7d0)
	{
		//flag=1;
		//Temperature = (TH << 8) | TL;
		Temperature1 = (~Temperature) + 1;
		Temperature1 *= 0.0625f;
	}
	else
	{
		//flag=0;
		Temperature1 = Temperature * 0.0625;
	}
	TemperatureDDD[i] = Temperature1;

	//temp_foramt_convert(Temperature1, i);
}

// 自动搜索ROM
void DS18B20_Search_Rom(void)
{
	uint8_t k = 0, l = 0, chongtuwei = 0, m = 0, n = 0, num = 0;
	uint8_t zhan[5] = {0};
	uint8_t ss[64] = {0};
	uint8_t tempp;
	l = 0;
	num = 0;
	do
	{
		DS18B20_Rst(); //注意：复位的延时不够
		dh_timer_us_block_delay(480); //480、720
		DS18B20_Write_Byte(0xf0);
		for (m = 0; m < 8; m++)
		{
			uint8_t s = 0;
			for (n = 0; n < 8; n++)
			{
				k = DS18B20_Read_2Bit();//读两位数据

				k = k & 0x03;
				s >>= 1;
				if (k == 0x01)//01读到的数据为0 写0 此位为0的器件响应
				{
					DS18B20_Write_Bit(0);
					ss[(m * 8 + n)] = 0;
				}
				else if (k == 0x02)//读到的数据为1 写1 此位为1的器件响应
				{
					s = s | 0x80;
					DS18B20_Write_Bit(1);
					ss[(m * 8 + n)] = 1;
				}
				else if (k == 0x00)//读到的数据为00 有冲突位 判断冲突位
				{
					//如果冲突位大于栈顶写0 小于栈顶写以前数据 等于栈顶写1
					chongtuwei = m * 8 + n + 1;
					if (chongtuwei > zhan[l])
					{
						DS18B20_Write_Bit(0);
						ss[(m * 8 + n)] = 0;
						zhan[++l] = chongtuwei;
					}
					else if (chongtuwei < zhan[l])
					{
						s = s | ((ss[(m * 8 + n)] & 0x01) << 7);
						DS18B20_Write_Bit(ss[(m * 8 + n)]);
					}
					else if (chongtuwei == zhan[l])
					{
						s = s | 0x80;
						DS18B20_Write_Bit(1);
						ss[(m * 8 + n)] = 1;
						l = l - 1;
					}
				}
				else
				{
					//没有搜索到
				}
			}
			tempp = s;
			DS18B20_ID[num][m] = tempp; // 保存搜索到的ID
		}
		num = num + 1;// 保存搜索到的个数
	} while (zhan[l] != 0 && (num < MaxSensorNum));
	DS18B20_SensorNum = num;
}

//初始化DS18B20的IO口，同时检测DS的存在
void DS18B20_Init(void)
{
	DS18B20_Search_Rom();
	ds18B20_messure_delay_unit = DEFAULT_DE18B20_MESSURE_MS * (1000/SYSTICK_HANDLE_US);
}

void dh_ds18b20_process(void)
{
	static uint8_t i = 0;

	if(DS18B20_SensorNum && ds18B20_messure_delay_unit)
	{
		if(flag1ms & (1 << DELAY_MESSURE_DS18B20))
		{
			flag1ms &= ~((1 << DELAY_MESSURE_DS18B20));
			ds18B20_messure_delay_unit--;
		}

		if(ds18B20_messure_delay_unit == 0)
		{
			HAL_TIM_Base_Start_IT(&htim6);
			DS18B20_Get_Temp(i);
			i++;
			if(i >= DS18B20_SensorNum)
			{
				i = 0;
			}
			HAL_TIM_Base_Stop_IT(&htim6);
			ds18B20_messure_delay_unit = DEFAULT_DE18B20_MESSURE_MS * (1000/SYSTICK_HANDLE_US);
		}
	}
}
