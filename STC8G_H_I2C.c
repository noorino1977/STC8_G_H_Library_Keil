#include	"STC8G_H_I2C.h"

u8 xdata I2C_Buffer[I2C_BUF_LENTH];
 
void I2C_Init(I2C_InitTypeDef *I2Cx)
{
	if(I2Cx->I2C_Mode == I2C_Mode_Master)
	{
		I2C_Master();			 
		I2CMSST = 0x00;		 
		I2C_SetSpeed(I2Cx->I2C_Speed);
		if(I2Cx->I2C_MS_WDTA == ENABLE)		I2C_WDTA_EN();	 
		else									I2C_WDTA_DIS();	 
	}
	else
	{
		I2C_Slave();	 
		I2CSLST = 0x00;		 
		I2C_Address(I2Cx->I2C_SL_ADR);
		if(I2Cx->I2C_SL_MA == ENABLE)		I2C_MATCH_EN();	 
		else									I2C_MATCH_DIS();	 
	}
	
	I2C_Function(I2Cx->I2C_Enable);
}
 
u8 Get_MSBusy_Status(void)
{
	return (I2CMSST & 0x80);
}
 
void Wait()
{
	while (!(I2CMSST & 0x40));
	I2CMSST &= ~0x40;
}
 
void Start()
{
	I2CMSCR = 0x01;                         //发送START命令
	Wait();
}
 
void SendData(char dat)
{
	I2CTXD = dat;                           //写数据到数据缓冲区
	I2CMSCR = 0x02;                         //发送SEND命令
	Wait();
}
 
void RecvACK()
{
	I2CMSCR = 0x03;                         //发送读ACK命令
	Wait();
}
 
char RecvData()
{
	I2CMSCR = 0x04;                         //发送RECV命令
	Wait();
	return I2CRXD;
}
 
void SendACK()
{
	I2CMSST = 0x00;                         //设置ACK信号
	I2CMSCR = 0x05;                         //发送ACK命令
	Wait();
}
 
void SendNAK()
{
	I2CMSST = 0x01;                         //设置NAK信号
	I2CMSCR = 0x05;                         //发送ACK命令
	Wait();
}
 
void Stop()
{
	I2CMSCR = 0x06;                         //发送STOP命令
	Wait();
}
 
void SendCmdData(u8 cmd, u8 dat)
{
	I2CTXD = dat;                           //写数据到数据缓冲区
	I2CMSCR = cmd;                          //设置命令
	Wait();
}
 
void I2C_WriteNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)  /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //发送起始命令
	SendData(dev_addr);                     //发送设备地址+写命令
	RecvACK();
	SendData(mem_addr);                     //发送存储地址
	RecvACK();
	do
	{
		SendData(*p++);
		RecvACK();
	}
	while(--number);
	Stop();                                 //发送停止命令
}
 
void I2C_ReadNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)   /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //发送起始命令
	SendData(dev_addr);                     //发送设备地址+写命令
	RecvACK();
	SendData(mem_addr);                     //发送存储地址
	RecvACK();
	Start();                                //发送起始命令
	SendData(dev_addr|1);                   //发送设备地址+读命令
	RecvACK();
	do
	{
		*p = RecvData();
		p++;
		if(number != 1) SendACK();          //send ACK
	}
	while(--number);
	SendNAK();                              //send no ACK	
	Stop();                                 //发送停止命令
}
