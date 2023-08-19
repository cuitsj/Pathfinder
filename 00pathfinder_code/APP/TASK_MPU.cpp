#include "TASK_MPU.hpp"

#define   uchar unsigned char
#define   uint unsigned int	
uint8_t accel[6],gyro[6],Temperature[2];

// 定义MPU9250内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

		
#define MAG_XOUT_L		0x03
#define MAG_XOUT_H		0x04
#define MAG_YOUT_L		0x05
#define MAG_YOUT_H		0x06
#define MAG_ZOUT_L		0x07
#define MAG_ZOUT_H		0x08


#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		  0x75	//IIC地址寄存器(默认数值0x68，只读)


//****************************

#define	GYRO_ADDRESS   0xD0	  //陀螺地址
#define MAG_ADDRESS    0X18   //磁场地址
#define ACCEL_ADDRESS  0xD0 

unsigned char TX_DATA[4];  	 //显示据缓存区
unsigned char BUF[10];       //接收数据缓存区
char  test=0; 				 //IIC用到
short T_X,T_Y,T_Z,T_T;		 //X,Y,Z轴，温度

//************************************
/*模拟IIC端口输出输入定义*/
#define SCL_H         gBSP->MPU_SCL->Set()
#define SCL_L         gBSP->MPU_SCL->Reset()
   
#define SDA_H         gBSP->MPU_SDA->Set()
#define SDA_L         gBSP->MPU_SDA->Reset()

#define SCL_read      gBSP->MPU_SCL->Gets()
#define SDA_read      gBSP->MPU_SDA->Gets()

/* 函数申明 -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART1_Configuration(void);
void WWDG_Configuration(void);
/* 变量定义 ----------------------------------------------*/

  /*******************************/
void DATA_printf(uchar *s,short temp_data)
{
	if(temp_data<0){
	temp_data=-temp_data;
    *s='-';
	}
	else *s=' ';
    *++s =temp_data/100+0x30;
    temp_data=temp_data%100;     //取余运算
    *++s =temp_data/10+0x30;
    temp_data=temp_data%10;      //取余运算
    *++s =temp_data+0x30; 	
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{
		
   uint8_t i=30; //这里可以优化速度	，经测试最低到5还能写入
   while(i) 
   { 
     i--; 
   }  
}

void delay5ms(void)
{
		
   int i=5000;  
   while(i) 
   { 
     i--; 
   }  
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return 0;	//SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_delay();
	return 1;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
bool I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
	  I2C_delay();
      return 0;
	}
	SCL_L;
	I2C_delay();
	return 1;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(uint8_t SendByte) //数据从高位到低位//
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char I2C_RadeByte(void)  //数据从高位到低位//
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
} 
//ZRX          
//单字节写入*******************************************

bool Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!I2C_Start())return 0;
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return 0;}
    I2C_SendByte(REG_Address );   //设置低起始地址      
    I2C_WaitAck();	
    I2C_SendByte(REG_data);
    I2C_WaitAck();   
    I2C_Stop(); 
    delay5ms();
    return 1;
}

//单字节读取*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
	if(!I2C_Start())return 0;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop();test=1; return 0;}
    I2C_SendByte((uint8_t) REG_Address);   //设置低起始地址      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);
    I2C_WaitAck();

	REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return 1;
	return REG_data;

}						      

//初始化MPU9250，根据需要请参考pdf进行修改************************
void Init_MPU9250(void)
{
/*
   Single_Write(GYRO_ADDRESS,PWR_M, 0x80);   //
   Single_Write(GYRO_ADDRESS,SMPL, 0x07);    //
   Single_Write(GYRO_ADDRESS,DLPF, 0x1E);    //±2000°
   Single_Write(GYRO_ADDRESS,INT_C, 0x00 );  //
   Single_Write(GYRO_ADDRESS,PWR_M, 0x00);   //
*/
  Single_Write(GYRO_ADDRESS,PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_Write(GYRO_ADDRESS,SMPLRT_DIV, 0x07);
	Single_Write(GYRO_ADDRESS,CONFIG, 0x06);
	Single_Write(GYRO_ADDRESS,GYRO_CONFIG, 0x18);
	Single_Write(GYRO_ADDRESS,ACCEL_CONFIG, 0x01);
  //----------------
//	Single_Write(GYRO_ADDRESS,0x6A,0x00);//close Master Mode	

}
	
//******读取MPU9250数据****************************************
void READ_MPU9250_ACCEL(void)
{ 

   accel[0]=Single_Read(ACCEL_ADDRESS,ACCEL_XOUT_H); 
   accel[1]=Single_Read(ACCEL_ADDRESS,ACCEL_XOUT_L);

   accel[2]=Single_Read(ACCEL_ADDRESS,ACCEL_YOUT_H);
   accel[3]=Single_Read(ACCEL_ADDRESS,ACCEL_YOUT_L);

   accel[4]=Single_Read(ACCEL_ADDRESS,ACCEL_ZOUT_H);
   accel[5]=Single_Read(ACCEL_ADDRESS,ACCEL_ZOUT_L);

 
}

void READ_MPU9250_GYRO(void)
{ 

   gyro[0]=Single_Read(GYRO_ADDRESS,GYRO_XOUT_H); 
   gyro[1]=Single_Read(GYRO_ADDRESS,GYRO_XOUT_L);


   gyro[2]=Single_Read(GYRO_ADDRESS,GYRO_YOUT_H);
   gyro[3]=Single_Read(GYRO_ADDRESS,GYRO_YOUT_L);

   gyro[4]=Single_Read(GYRO_ADDRESS,GYRO_ZOUT_H);
   gyro[5]=Single_Read(GYRO_ADDRESS,GYRO_ZOUT_L);

 
 
	 Temperature[0]=Single_Read(GYRO_ADDRESS,TEMP_OUT_H); 
	 Temperature[1]=Single_Read(GYRO_ADDRESS,TEMP_OUT_L); 
}


void READ_MPU9250_MAG(void)
{ 
   Single_Write(GYRO_ADDRESS,0x37,0x02);//turn on Bypass Mode 
   gOS->delay_ms(10);
   Single_Write(MAG_ADDRESS,0x0A,0x01);
   gOS->delay_ms(10);
   BUF[0]=Single_Read (MAG_ADDRESS,MAG_XOUT_L);
   BUF[1]=Single_Read (MAG_ADDRESS,MAG_XOUT_H);
   T_X=(BUF[1]<<8)|BUF[0];

   BUF[2]=Single_Read(MAG_ADDRESS,MAG_YOUT_L);
   BUF[3]=Single_Read(MAG_ADDRESS,MAG_YOUT_H);
   T_Y=	(BUF[3]<<8)|BUF[2];
   						   //读取计算Y轴数据
	 
   BUF[4]=Single_Read(MAG_ADDRESS,MAG_ZOUT_L);
   BUF[5]=Single_Read(MAG_ADDRESS,MAG_ZOUT_H);
   T_Z=	(BUF[5]<<8)|BUF[4];
 					       //读取计算Z轴数据
}

 //********串口发送数据***************************************
 void Send_data(uchar MAG,uchar axis)
 {
	 uchar i;
	 uchar fuhao1 = ':';
	 uchar fuhao2 = ' ';
	 
	 gBSP->UART2_BT->Write(&MAG,1);
	 gBSP->UART2_BT->Write(&axis,1);
	 gBSP->UART2_BT->Write(&fuhao1,1);
   gBSP->UART2_BT->Write(TX_DATA,4);
	 gBSP->UART2_BT->Write(&fuhao2,1);
	 gBSP->UART2_BT->Write(&fuhao2,1);
 }

/*************结束***************/

void TASK_MPU::MPU_FUN(void* parameter)
{
	int Voltage;
	int16_t Encoder_Left=0,Encoder_Right=0;
	uint8_t t[20]={0};
	
  Init_MPU9250();		     //初始化MPU9250
	for(;;)
	{
		READ_MPU9250_ACCEL();  //加速度
//		DATA_printf(TX_DATA,T_X);//转换X轴数据到数组
//		Send_data('A','X');			 //发送X轴数
//		DATA_printf(TX_DATA,T_Y);//转换Y轴数据到数组
//		Send_data('A','Y');			 //发送Y轴数
//		DATA_printf(TX_DATA,T_Z);//转换Z轴数据到数组
//		Send_data('A','Z');			 //发送Z轴数
//		 
		READ_MPU9250_GYRO();      //陀螺
//		DATA_printf(TX_DATA,T_X);//转换X轴数据到数组
//		Send_data('G','X');			 //发送X轴数
//		DATA_printf(TX_DATA,T_Y);//转换Y轴数据到数组
//		Send_data('G','Y');			 //发送Y轴数
//		DATA_printf(TX_DATA,T_Z);//转换Z轴数据到数组
//		Send_data('G','Z');			 //发送Z轴数
//		 
//		READ_MPU9250_MAG();	      //磁场
//		DATA_printf(TX_DATA,T_X);//转换X轴数据到数组
//		Send_data('M','X');			 //发送X轴数
//		DATA_printf(TX_DATA,T_Y);//转换Y轴数据到数组
//		Send_data('M','Y');			 //发送Y轴数
//		DATA_printf(TX_DATA,T_Z);//转换Z轴数据到数组
//		Send_data('M','Z');			 //发送Z轴数
		/*
		DATA_printf(TX_DATA,T_T);//转换温度数据到数组
		Send_data('T');			 //发送温度数据
		*/
		
				
//				Encoder_Left = gBSP->TIM2_LENCODER->ReadCnt();
//				Encoder_Right = gBSP->TIM3_RENCODER->ReadCnt();
//				Voltage=gBSP->ADC0_VOL->VOLIN11_BAT;

			 	t[0] = accel[0];
				t[1] = accel[1];
				t[2] = accel[2];
				t[3] = accel[3];
				t[4] = accel[4];
				t[5] = accel[5];
			  t[6] = gyro[0];
				t[7] = gyro[1];
				t[8] = gyro[2];
				t[9] = gyro[3];
				t[10] = gyro[4];
				t[11] = gyro[4];
//				t[12] = Temperature[0];
//				t[13] = Temperature[1];
//				t[14] = Encoder_Left>>8;
//				t[15] = Encoder_Left;
//				t[16] = Encoder_Right>>8;
//				t[17] = Encoder_Right;
//				t[18] = Voltage>>8;
//				t[19] = Voltage;
				
		gBSP->UART2_BT->Write(t,20);
		gOS->delay_ms(100);
	}
	
}

TASK_MPU::TASK_MPU()
{
	func = (mainFun_t)MPU_FUN;
	stackSize = 1024;
	name = "TASK_MPU";
	thread_prioroty = 1;
}
