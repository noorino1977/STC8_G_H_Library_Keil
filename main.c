#include	"config.h"
#include	"STC8G_H_GPIO.h"
#include	"STC8G_H_ADC.h"
#include	"STC8G_H_I2C.h"
#include	"STC8G_H_Timer.h"
#include	"STC8G_H_UART.h"
#include	"STC8G_H_SPI.h"
#include	"STC8G_H_NVIC.h"
#include	"STC8G_H_Switch.h"
#include	"STC8G_H_Delay.h"
#include	"STC8G_H_Switch.h"

bit B_1ms;      

#define BAND1 P20
#define BAND2 P37
#define BAND3 P36
#define BAND4 P35
#define BAND5 P34
#define BAND6 P33
#define PTT   P11
#define SMTR  P12

#define TOMBOL_MODE1  P21
#define TOMBOL_MODE6  P26

#define SS_    P22
#define MOSI_  P23
#define MISO_  P24
#define SCLK_  P25

#define SCL_  P32
#define SDA_  P33

u16 msecond;
u8 i;
u16	j;
				
void	GPIO_config(void)
{
	
	P2_MODE_IO_PU(GPIO_Pin_1 | GPIO_Pin_6 ); 
	P1_MODE_OUT_PP(GPIO_Pin_1 | GPIO_Pin_2);
	P2_MODE_OUT_PP(GPIO_Pin_0);
	P3_MODE_OUT_PP(GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 );		 	 
	P1_MODE_IO_PU(GPIO_Pin_4);
		
}

 
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;						 
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;         //指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;              //是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000UL);   //初值,
	TIM_InitStructure.TIM_Run       = ENABLE;               //是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);               //初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer0_Init(ENABLE,Priority_0);    //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

 
void	I2C_config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;
	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//主从选择   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C功能使能,   ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;			//主机使能自动发送,  ENABLE, DISABLE
	I2C_InitStructure.I2C_Speed     = 16;				//总线速度=Fosc/2/(Speed*2+4),      0~63
	I2C_Init(&I2C_InitStructure);
	NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_1);	//主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	I2C_SW(I2C_P33_P32);					//I2C_P14_P15, I2C_P24_P25, I2C_P33_P32
}
 
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		 
	ADC_InitStructure.ADC_SMPduty   = 31;		 
	ADC_InitStructure.ADC_CsSetup   = 0;		 
	ADC_InitStructure.ADC_CsHold    = 1;		 
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X16T;		 
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	 
	ADC_Inilize(&ADC_InitStructure);	 
	ADC_PowerControl(ENABLE);				//ENABLE?DISABLE
	NVIC_ADC_Init(DISABLE,Priority_2);		//ENABLE/DISABLE; Priority_0,Priority_1,Priority_2,Priority_3
}
 
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;				

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;		// BRT_Timer1, BRT_Timer2 
	COMx_InitStructure.UART_BaudRate  = 115200ul;		//110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;			//ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;		//ENABLE或DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//UART1,UART2,UART3,UART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//ENABLE/DISABLE; Priority_0,Priority_1,Priority_2,Priority_3

	UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

/*
         mcu                          lcd
  |-----------------|           |-----------------|
  |            MISO |-----------| MISO            |
--| TX         MOSI |-----------| MOSI         TX |--
  |            SCLK |-----------| SCLK            |
--| RX           SS |-----------| SS           RX |--
  |-----------------|           |-----------------|
*/

void	SPI_config(void)
{
	SPI_InitTypeDef		SPI_InitStructure;

	SPI_InitStructure.SPI_Enable    = ENABLE;				//  ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;				//   ENABLE, DISABLE
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;				//  SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Slave;		//   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;			//   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;		//    SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Speed     = SPI_Speed_4;			//   SPI_Speed_4,SPI_Speed_16,SPI_Speed_64,SPI_Speed_2/SPI_Speed_32
	SPI_Init(&SPI_InitStructure);
	NVIC_SPI_Init(ENABLE,Priority_3);		//ENABLE/DISABLE; Priority_0,Priority_1,Priority_2,Priority_3
	
	SPI_SW(SPI_P22_P23_P24_P25);	//SPI_P12_P13_P14_P15,SPI_P22_P23_P24_P25,SPI_P54_P40_P41_P43,SPI_P35_P34_P33_P32
	SPI_SS_2 = 1;
}


void main(void)
{	
	EAXSFR();		
	GPIO_config();
	Timer_config();
	I2C_config();
  UART_config();
  ADC_config();
	EA = 1;
  PTT =1;
	
	PrintString1("UART1 Test !\r\n");
	
	while (1)
	{
      PTT = !PTT;//LED KEDIP KEDIP
		
			delay_ms(1000);
			j = Get_ADCResult(4);
			TX1_write2buff('A');
			TX1_write2buff('D');
			TX1_write2buff(i+'0');
			TX1_write2buff('=');
			TX1_write2buff(j/1000 + '0');
			TX1_write2buff(j%1000/100 + '0');
			TX1_write2buff(j%100/10 + '0');
			TX1_write2buff(j%10 + '0');
			TX1_write2buff(' ');
			TX1_write2buff(' ');
 
	
			
		/*
		if(B_1ms)
		{
			B_1ms = 0;           
            if(++msecond >= 2000)    
            {
                msecond = 0;       
                I2C_ReadNbyte(DEV_ADDR, 0, I2C_Buffer, 8);
                printf("Read=");    
                for(i=0;i<8;i++)
                {
                    printf("0x%02bx ",I2C_Buffer[i]);
                    if(I2C_Buffer[i] == 0xff) I2C_Buffer[i] = i;  
                    else I2C_Buffer[i]++;    
                }
                printf("\r\n");

                I2C_WriteNbyte(DEV_ADDR, 0, I2C_Buffer, 8);
            }
		}
		*/
	
	}
}
