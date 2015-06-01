
//==================头文件=============================
#include   "a7105.h"  
#include   "soft.h"
#include   "hardware.h"
#include   "flash.h"
#include   "time1.h"
#include   "lcdkey.h"
#include   "key.h"
//==================私有定义=========================
const uint8 ID_Tab[4]={'X','R',210,'A'}; //ID code
 uint8  A7105Config[51]=	 //配置寄存器
{
	0x00, //写入时芯片复位，读出为状态位
	0x42, //MODE register,
	0x00, //CALIBRATION register,only read, not use on config
	0x08, //FIFO1 register,
	0x00, //FIFO2 register,
	0x00, //FIFO register,		for fifo read/write
	0x00, //IDDATA register,	for idcode
	0x00, //RCOSC1 register,
	0x00, //RCOSC2 register,
	0x00, //RCOSC3 register,
	0x00, //CKO register,
	0x01, //GPIO1 register
	0x21, //GPIO2 register,
	0x05, //CLOCK register,
	0x00, //DATARATE register,
	0x50, //PLL1 register,
	0x9E, //PLL2 register, 		RFbase 2400MHz
	0x4B, //PLL3 register,
	0x00, //PLL4 register,
	0x02, //PLL5 register,
	0x16, //TX1 register,
	0x2B, //TX2 register,
	0x12, //DELAY1 register,
	0x00, //DELAY2 register,
	0x62, //RX register,
	0x80, //RXGAIN1 register,
	0x80, //RXGAIN2 register,
	0x00, //RXGAIN3 register,
	0x0a, //RXGAIN4 register,
	0x32, //RSSI register,
	0xC3, //ADC register,
	0x07, //CODE1 register,
	0x16, //CODE2 register,
	0x00, //CODE3 register,
	0x00, //IFCAL1 register,
	0x00, //IFCAL2 register,    only read
	0x00, //VCOCCAL register,
	0x00, //VCOCAL1 register,
	0x3b, //VCOCAL2 register,
	0x00, //BATTERY register,
	0x17, //TXTEST register,
	0x47, //RXDEM1 register,
	0x80, //RXDEM2 register,
	0x03, //CPC register,
	0x01, //CRYSTAL register,
	0x45, //PLLTEST register,
	0x18, //VCOTEST1 register,
	0x00, //VCOTEST2 register,
	0x01, //IFAT register,
	0x0F, //RSCALE register,
	0x00  //FILTERTEST
};
//====================全局变量=======================
YAOKONG gsv_yaokong;
bool gbv_yaokong_time;
bool gbv_a7105_reset;
//====================私有变量=======================
uint32 Telecontrol_ID;//遥控ID号
//====================全局函数=======================
uint8 initRF(void);
//====================私有函数=======================
void init_a7105hardware(void);
void A7105_Reset(void);
void A7105_WriteReg(uint8, uint8);
uint8 A7105_ReadReg(uint8);
void ByteSend(uint8 src);
uint8 ByteRead(void);
void A7105_WriteID(void);
void A7105_Config(void);
void A7105_Cal(void);
void RxPacket(uint8 a[]);
void StrobeCmd(uint8);
void SetCH(uint8);
void A7105_WriteFIFO(uint8 p[], uint8 byte);
void A7105_ReadFIFO(uint8 a[],uint8 number);
void A7105_ReadID(uint8 *id);
void A7105_rx_en(void);
void set_SDIO_in(void);
void set_SDIO_out(void);
//====================函数实体=======================
/****************************************************
功能：初始化与a7105芯片通讯的io口
输入：无
输出：无
描述：将GIO1初始化为悬空输入；将 SCS、SCK、SDIO初始化
      为推挽输出。
	  置SCS SDIO高，置SCK低
****************************************************/
void init_a7105hardware(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;   //GPIO 结构类型
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC, ENABLE); 
  
  GPIO_InitStructure.GPIO_Pin = GIO1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOC, SDIO);
  GPIO_SetBits(GPIOC, SCS);
  GPIO_ResetBits(GPIOC, SCK);
  GPIO_InitStructure.GPIO_Pin = SCS | SCK | SDIO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void init_a7105(void)
{
uint8 a;
  A7105_Reset();
  A7105_WriteID();
  A7105_Config();
  //===============下面是校准A7105================
  StrobeCmd(CMD_STBY);                      //0XA0  发送待机命令
  A7105_WriteReg(CALIBRATION_REG, 0x01);    //0X02 校准使能
  DelayMs(3);//delay(10000);
  a = A7105_ReadReg(CALIBRATION_REG);
  a &= 0x01;
  if(a!=0)return ;                          //校准失败，返回0
  a = A7105_ReadReg(IFCAL1_REG);            //0X22寄存器
  a=a&0X10;								    //校准失败，返回0
  if (a)return ;
  A7105_WriteReg(VCOCCAL_REG, 0x13);        //0X25 VCO 校准	
  A7105_WriteReg(VCOCAL1_REG, 0x09);        //0X26 VCO 校准	
  StrobeCmd(CMD_STBY);                      //进入待机模式 
  A7105_rx_en();
  gbv_a7105_reset = FALSE;
}
/************************************************************
功能: 初始化7105模块
输入: 无
输出: 无
************************************************************/
uint8 initRF(void)
{
uint8 a;
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
  init_a7105hardware();	
  init_flash();	   	               //初始化flash
  Telecontrol_ID = 	read_flash();  //从flash中读出遥控器的id号

  init_a7105();
  gsv_yaokong.key = 0;
  gsv_yaokong.sign = 0;
  //=====================调试==================
  for(a=0; a<100;a++)
  {
      if(dui_Telecontrol_ID() == 1)
	  {
		  Set_Beep();
		  delay(1000000);
		  Reset_Beep();
		  delay(1000000);
	      break;
	  }
	  delay(30000);
  }
   //================配置GIO1脚为下降沿中断。 
 
  StrobeCmd(CMD_RFR);        //清除RX FIFO
  //----exteral interrupt configure
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  StrobeCmd(CMD_RX);         //进入接收模式  
  return 1;
}
/************************************************************************
**  Reset_RF	复位芯片
************************************************************************/
void A7105_Reset(void)
{
	A7105_WriteReg(MODE_REG, 0x00); //向寄存器0的高位写既复位芯片
}
/************************************************************************
**  WriteID	   //写芯片ID，设计芯片ID为4个字节
************************************************************************/
void A7105_WriteID(void)
{
	uint8 i;
	uint8 addr;
	uint8 id[4];
	addr = IDCODE_REG; //写ID命令 send address 0x06, bit cmd=0, r/w=0
	GPIO_ResetBits(GPIOC, SCS);//片选
	ByteSend(addr);			  //发送命令 0X06
	for (i=0; i < 4; i++)
		ByteSend(ID_Tab[i]);  //发送ID
    GPIO_SetBits(GPIOC, SCS);

	addr = IDCODE_REG|0x40;	//尽当时间延迟而已
	GPIO_ResetBits(GPIOC, SCS);
	A7105_ReadID(id);	   
	for (i=0; i < 4; i++)
	{
	    if(id[i] !=  ID_Tab[i])
		break;
	}
}
/************************************************************************
**  A7105_WriteReg
写寄存器 寄存器地址，数据
************************************************************************/
void A7105_WriteReg(uint8 addr, uint8 dataByte)
{
    uint8 i;
	GPIO_ResetBits(GPIOC, SCS);
    addr |= 0x00; //bit cmd=0,r/w=0
    for(i = 0; i < 8; i++)
    {
        if(addr & 0x80)
		  {
		   GPIO_SetBits(GPIOC, SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, SDIO);
		  }
        delay(10);
	    GPIO_SetBits(GPIOC, SCK);
	    delay(10);
	    GPIO_ResetBits(GPIOC,SCK);
        addr = addr << 1;
    }
   delay(10);
    for(i = 0; i < 8; i++)
    {
        if(dataByte & 0x80)
		   {
		   GPIO_SetBits(GPIOC, SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, SDIO);
		  }
        delay(10);
	   GPIO_SetBits(GPIOC, SCK);
	   delay(10);
	   GPIO_ResetBits(GPIOC,SCK);
        dataByte = dataByte<< 1;
    }
	GPIO_SetBits(GPIOC, SCS);
}
/************************************************************************
**  A7105_ReadReg //读寄存器
************************************************************************/
uint8 A7105_ReadReg(uint8 addr)
{
    uint8 i;
    uint8 tmp=0;
	GPIO_ResetBits(GPIOC,SCS);
    addr |= 0x40; //bit cmd=0,r/w=1
    for(i = 0; i < 8; i++)
    {
        if(addr & 0x80)
          {
		   GPIO_SetBits(GPIOC, SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, SDIO);
		  }
		delay(10);
		 GPIO_SetBits(GPIOC, SCK);
		delay(10);
        GPIO_ResetBits(GPIOC, SCK);
        addr = addr << 1;
    }
    delay(10);
	set_SDIO_in();//data transminator io change 
    for(i = 0; i < 8; i++)
    {
        if(GPIO_ReadInputDataBit(GPIOC,SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
	   GPIO_SetBits(GPIOC, SCK);
	   delay(10);
	   GPIO_ResetBits(GPIOC, SCK);
	   delay(10);
    }
	set_SDIO_out();
    return tmp;
}
/************************************************************************
**  Byte Send	//写数据 8位
************************************************************************/
void ByteSend(uint8 src)
{
    uint8 i;

    for(i = 0; i < 8; i++)
    {
        if(src & 0x80)
          {
		   GPIO_SetBits(GPIOC, SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, SDIO);
		  }
		delay(10);
		GPIO_SetBits(GPIOC, SCK);
		delay(10);
		GPIO_ResetBits(GPIOC, SCK);
        src = src << 1;
    }
}
/************************************************************************
**  ByteRead	读数据 8位
************************************************************************/
uint8 ByteRead(void)
{
	uint8 i,tmp;
    tmp = 0;
	set_SDIO_in() ;
    for(i = 0; i < 8; i++)
    {
        if(GPIO_ReadInputDataBit(GPIOC,SDIO))				 //读数据
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
		GPIO_SetBits(GPIOC, SCK);
		delay(10);
		GPIO_ResetBits(GPIOC, SCK);
    }
	set_SDIO_out() ;
    return tmp;
}
/************************************************************************
**  Send4Bit	 半字节4位发送  字节的高四位写命令
************************************************************************/
void Send4Bit(uint8 src)
{  //发送一个字节的高四位
    uint8 i;
    for(i = 0; i < 4; i++)
    {
        if(src & 0x80)
          {
		   GPIO_SetBits(GPIOC, SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, SDIO);
		  }
		delay(10);
		GPIO_SetBits(GPIOC, SCK);
		delay(10);
		GPIO_ResetBits(GPIOC, SCK);
        src = src << 1;
    }
}
/*********************************************************************
** SetCH
*********************************************************************/
void SetCH(uint8 ch)
{
	A7105_WriteReg(PLL1_REG, ch); //PLL1_REG=0X0F ,ch=99;
}
/*********************************************************************
** A7105_WriteFIFO 把数据送入FIFO中 字节地址，个数
*********************************************************************/
void A7105_WriteFIFO(uint8 p[], uint8 byte)
{
	uint8 i;
	uint8 cmd;
	cmd = FIFO_REG; //send address 0x05, bit cmd=0, r/w=0 地址
	GPIO_ResetBits(GPIOC, SCS);					  //片选
	ByteSend(cmd);			      //写命令
    for(i=0; i <byte; i++)
		ByteSend(p[i]);	  //写64个数据到FIFO中
	GPIO_SetBits(GPIOC, SCS);
	
}
/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCmd(uint8 cmd)
{
	GPIO_ResetBits(GPIOC, SCS);					  //片选
	Send4Bit(cmd);	 //发送4位
	GPIO_SetBits(GPIOC, SCS);
}
/*********************************************************************
** Rx Packet   从FIFO中读出资料并对比
*********************************************************************/
void A7105_ReadFIFO(uint8 a[],uint8 number)
{
    uint8 i;
    uint8 cmd;
	cmd = FIFO_REG | 0x40; //读命令 address 0x05, bit cmd=0, r/w=1
	GPIO_ResetBits(GPIOC, SCS);					  //片选
	ByteSend(cmd);			//写命令
	for(i=0; i <number; i++)
	{
	  a[i]  = ByteRead();	//读数		
    }	
	GPIO_SetBits(GPIOC, SCS);
}
/********************************************************************
功能：读取芯片的ID
输出：将芯片ID存入输入的数组中
********************************************************************/
void A7105_ReadID(uint8 *id)
{
   uint8 i;
   uint8 cmd;
   cmd = IDCODE_REG | 0x40;	  //0X46，读为0X46，写为0X06
   GPIO_ResetBits(GPIOC, SCS);					  //片选
   ByteSend(cmd);			//写命令
   for(i=0; i<4; i++)
   {
	  id[i] = ByteRead();
   }	
   GPIO_SetBits(GPIOC, SCS);
}
/*********************************************************************
** A7105_Config	  //配置芯片
*********************************************************************/
void A7105_Config(void)
{
	uint8 i;

    //0x00 mode register, for reset
    //0x05 fifo data register
    //0x06 id code register
    //0x23 IF calibration II, only read
    //0x32 filter test register

    for (i=0x01; i<=0x04; i++)
	    A7105_WriteReg(i, A7105Config[i]);// 写寄存器

	for (i=0x07; i<=0x22; i++)
		A7105_WriteReg(i, A7105Config[i]);

	for (i=0x24; i<=0x31; i++)
		A7105_WriteReg(i, A7105Config[i]);
}

/***************************************************************
功能：接收使能
****************************************************************/
void A7105_rx_en(void)
{
 SetCH(99);                 //频率2449.501MHz
 StrobeCmd(CMD_RFR);        //清除RX FIFO
 StrobeCmd(CMD_RX);         //进入接收模式 	
}
/****************************************************************
功能：与遥控器对接遥控器的id，成功返回1，失败返回0
****************************************************************/
uint8 dui_Telecontrol_ID(void)
{
uint8 A7105_rx_data[8],a;
uint32 x;
if(GPIO_ReadInputDataBit(GPIOD,GIO1) == 0)
{
    RxPacket(A7105_rx_data); //读接收数据包
	//数据校验														   
	if(A7105_rx_data[0]==0x33)//是对遥控器ID码
	{	
	   a=(A7105_rx_data[0]+A7105_rx_data[1]+A7105_rx_data[2]+A7105_rx_data[3]+A7105_rx_data[4]); //去四个字节作为对码
	   if(A7105_rx_data[7]==a)//接收正确 ，打头码
	   {
	       x = (A7105_rx_data[1]<<24)+(A7105_rx_data[2]<<16)+(A7105_rx_data[3]<<8)+A7105_rx_data[4]; //将四个8位数，调整为1个32位数
		   if(x != Telecontrol_ID)//如果有更新，那么就重新写入
		   {
		      Telecontrol_ID = x;
			  write_flash( Telecontrol_ID);
		   }
		   StrobeCmd(CMD_RFR);        //清除RX FIFO
           StrobeCmd(CMD_RX);         //进入接收模式 
		   return 1;
	   }
	}
}
StrobeCmd(CMD_RFR);        //清除RX FIFO
StrobeCmd(CMD_RX);         //进入接收模式 
return 0;
}
/*********************************************************************
** Rx Packet   从FIFO中读出资料
*********************************************************************/
void RxPacket(uint8 a[])
{
    uint8 i;
    uint8 cmd;
    
	cmd = FIFO_REG | 0x40; //读命令 address 0x05, bit cmd=0, r/w=1
	
	GPIO_ResetBits(GPIOC,SCS);
	ByteSend(cmd);			//写命令
	for(i=0; i <8; i++)
	{
	  a[i]  = ByteRead();	//读数		
    }
	GPIO_SetBits(GPIOC,SCS);
}
/**********************************************
遥控器接收程序
**********************************************/
void A7105_rx(void)
{   //-----------------接收前相关量的处理
    if(gbv_yaokong_time == FALSE)
	{
	    return;
	}
	gbv_yaokong_time = FALSE;
	if(((giv_sys_time - gsv_yaokong.effect_time) > 5000) && (gsv_yaokong.effect == 1))
	{
	    gsv_yaokong.effect = 0;
		gsv_yaokong.sign = 0;
	}
	if(((giv_sys_time - gsv_yaokong.effect_time) > 50000) && (gsv_yaokong.effect == 0))
	{									
	    gbv_a7105_reset = TRUE;	 
		gsv_yaokong.effect_time = giv_sys_time;
		EXTI_GenerateSWInterrupt(EXTI_Line0);//开启软件中断
	} 
}
/*************************************************
功能：设置SDIO为输入
*************************************************/
void set_SDIO_in(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;   //GPIO 结构类型
   GPIO_InitStructure.GPIO_Pin =  SDIO;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*************************************************
功能：设置SDIO为输出
*************************************************/
void set_SDIO_out(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;   //GPIO 结构类型
   GPIO_InitStructure.GPIO_Pin =  SDIO;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
}
