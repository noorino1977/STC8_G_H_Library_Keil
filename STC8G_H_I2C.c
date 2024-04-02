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
	I2CMSCR = 0x01;                         //����START����
	Wait();
}
 
void SendData(char dat)
{
	I2CTXD = dat;                           //д���ݵ����ݻ�����
	I2CMSCR = 0x02;                         //����SEND����
	Wait();
}
 
void RecvACK()
{
	I2CMSCR = 0x03;                         //���Ͷ�ACK����
	Wait();
}
 
char RecvData()
{
	I2CMSCR = 0x04;                         //����RECV����
	Wait();
	return I2CRXD;
}
 
void SendACK()
{
	I2CMSST = 0x00;                         //����ACK�ź�
	I2CMSCR = 0x05;                         //����ACK����
	Wait();
}
 
void SendNAK()
{
	I2CMSST = 0x01;                         //����NAK�ź�
	I2CMSCR = 0x05;                         //����ACK����
	Wait();
}
 
void Stop()
{
	I2CMSCR = 0x06;                         //����STOP����
	Wait();
}
 
void SendCmdData(u8 cmd, u8 dat)
{
	I2CTXD = dat;                           //д���ݵ����ݻ�����
	I2CMSCR = cmd;                          //��������
	Wait();
}
 
void I2C_WriteNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)  /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //������ʼ����
	SendData(dev_addr);                     //�����豸��ַ+д����
	RecvACK();
	SendData(mem_addr);                     //���ʹ洢��ַ
	RecvACK();
	do
	{
		SendData(*p++);
		RecvACK();
	}
	while(--number);
	Stop();                                 //����ֹͣ����
}
 
void I2C_ReadNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)   /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //������ʼ����
	SendData(dev_addr);                     //�����豸��ַ+д����
	RecvACK();
	SendData(mem_addr);                     //���ʹ洢��ַ
	RecvACK();
	Start();                                //������ʼ����
	SendData(dev_addr|1);                   //�����豸��ַ+������
	RecvACK();
	do
	{
		*p = RecvData();
		p++;
		if(number != 1) SendACK();          //send ACK
	}
	while(--number);
	SendNAK();                              //send no ACK	
	Stop();                                 //����ֹͣ����
}
