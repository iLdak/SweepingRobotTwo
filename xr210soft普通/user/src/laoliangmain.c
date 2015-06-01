
#include "ht48r06a-1.h"
#include "a7105.h"

#define Uint8 unsigned char
#define uint  unsigned int
#define _nop_()  _nop()

/*********************************************************************
**  I/O Declaration
*********************************************************************/
#define SCS       	_pc0		//片选
#define SCK      	_pa0		//时钟
#define SDIO      	_pc1		//数据
#define GPIO1      	_pb0		//GPIO1

//系统ID号
const Uint8 ID_Tab[4]={0x54,0x75,0xC5,0x2A}; //ID code
//发送数据列表


const Uint8  A7105Config[51]=	 //配置寄存器
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


/*********************************************************************
**  function Declaration
*********************************************************************/
void A7105_Reset(void);
void A7105_WriteReg(Uint8, Uint8);
Uint8 A7105_ReadReg(Uint8);
void ByteSend(Uint8 src);
Uint8 ByteRead(void);
void A7105_WriteID(void);
void initRF(void);
void A7105_Config(void);
void A7105_Cal(void);
void RxPacket(Uint8 a[]);
void StrobeCmd(Uint8);
void SetCH(Uint8);
void A7105_WriteFIFO(Uint8 p[], Uint8 byte);

/*********************************************************************/
void main(void)
{uint time;
 Uint8 a;
 Uint8 tmpbuf[8];                                            //接收数据缓冲区
 Uint8  PN9_Tab[8]= {0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,0};  //发送的数据
/**
// **************初始化IO口********************
   _pac=0b00000010;//P0口为输出,按键口为输入
   _pbc=0xff;//GPIO1口为输入
   _pcc=0;//p0口为输出	 
  	  
//**************初始化无线芯片***************
	SCS = 1;
    SCK = 0;
    SDIO = 1;
    GPIO1 = 1;
    A7105_Reset();   //复位芯片 写地址0既复位，读地址0得到状态	
	A7105_WriteID(); //写芯片ID号码	
	A7105_Config();  //芯片配置	
	
// *************芯片校准*********************

	StrobeCmd(CMD_STBY);                   //0XA0  发送待机命令
	A7105_WriteReg(CALIBRATION_REG, 0x01); //0X02 校准使能
	do                                     //等待校准完成
	{
		a = A7105_ReadReg(CALIBRATION_REG);
		a &= 0x01;
	}
	
	while (a);
	a = A7105_ReadReg(IFCAL1_REG);//0X22寄存器
    a=a&0X10;
	if (a)
	{
	    while(1);    //错误，校准失败
	}
	
    A7105_WriteReg(VCOCCAL_REG, 0x13); //0X25 VCO 校准	
	A7105_WriteReg(VCOCAL1_REG, 0x09);//0X26 VCO 校准	
    StrobeCmd(CMD_STBY);           //进入待机模式
// &&&&&&&&&&&&&&&接收程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//函数过程说明：在频率2449.501MHz处接收数据
//             有数据，验算校验和，
//             如果接收数据正确则在2450.001MHz频段返回校验和
   
// ********************************************
       while(1)
        {   
            SetCH(99);                 //频率2449.501MHz
            StrobeCmd(CMD_RFR);        //清除RX FIFO
            StrobeCmd(CMD_RX);         //进入接收模式
    	    while(GPIO1==1);           //等待数据
   			RxPacket(tmpbuf);		   //读取数据,入口缓冲地址
   			               //校验和
   			if(tmpbuf[7]==(tmpbuf[0]+tmpbuf[1]+tmpbuf[2]+tmpbuf[3]+tmpbuf[4]+tmpbuf[5]+tmpbuf[6]));                          
   			   {
		       A7105_WriteFIFO(&tmpbuf[7],1); //送数据到发送缓冲区
		       SetCH(100); //                 //调频到 2450.001MHz发送
   		       StrobeCmd(CMD_TX);             //发送
    	       while(GPIO1==1);               //等待发送完成
    	       SCS=0;
    	       for(a=0;a<200;a++)
    	       _delay(2000);
    	        SCS=1;
    	       
    	       }

			
        }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
********/
//&&&&&&&&&&&&&&&&&&&&发送机程序&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//函数功能：发射数据及校验码，然后接受应答，应答错误或没应答，从发20次
//
 _pac=0b11111111;//P0口为输出,按键口为输入
		while(1)		 
		{//clrwdt(); //清看门狗
 //没有按键进入掉电
		  if (_pa1==1) 
		      {StrobeCmd(CMD_SLEEP);  //发射芯片掉电
		      _pcc=0xff;
		      _pbc=0xff;
		      _pac=0xff;//P0口为输出,按键口为输入
		       _halt();              //没有按键进入掉电
		      }
		      
// **************初始化CPU********************
   _pac=0b00000010;//P0口为输出,按键口为输入
   _pbc=0xff;//GPIO1口为输入
   _pcc=0;//p0口为输出	 
//按键唤醒
        StrobeCmd(CMD_STBY);//0XA0  发送命令
	     SCS = 1;
         SCK = 0;
         SDIO = 1;
         GPIO1 = 1;
         A7105_Reset();   //复位芯片 写地址0既复位，读地址0得到状态	
	     A7105_WriteID(); //写芯片ID号码	
	     A7105_Config();  //芯片配置	
	
// *************芯片校准*********************

	
	A7105_WriteReg(CALIBRATION_REG, 0x01); //0X02 校准使能
	do   //等待校准完成
	{
		a = A7105_ReadReg(CALIBRATION_REG);
		a &= 0x01;
	}
	
	while (a);
	a = A7105_ReadReg(IFCAL1_REG);//0X22寄存器
    a=a&0X10;
	if (a)
	{
	    while(1);    //错误，校准失败
	}
	
    A7105_WriteReg(VCOCCAL_REG, 0x13);//0X25 VCO 校准
	
	A7105_WriteReg(VCOCAL1_REG, 0x09);//0X26 VCO 校准
	
    StrobeCmd(CMD_STBY);                    //进入待机模式		      
//发射数据		      
	PN9_Tab[7]=PN9_Tab[0]+PN9_Tab[1]+PN9_Tab[2]+PN9_Tab[3]+PN9_Tab[4]+PN9_Tab[5]+PN9_Tab[6];
	a=0;
         while(1)
       		{      
		    A7105_WriteFIFO(PN9_Tab,8);     //写发送的数据到fifo
		    SetCH(100);                     //freq 2450.001MHz
   		    StrobeCmd(CMD_TX);              //进入发送
    		while(GPIO1);                   //等待发送完成
    		SetCH(99);                      //freq 2449.501MHz 调频
    		StrobeCmd(CMD_RX);              //进入接收
    		a++;
    		time=0;
			while((GPIO1==1)&&(a<20))          //等待接收
			    {time++;
			     if(time>100)
			       {
                   break;         //20次没有接收到应答，退出
			       }
			     _delay(1000);
			    }
			if(GPIO1!=1)             //有数据
			   {RxPacket(tmpbuf);		   //读取数据,入口缓冲地址
			    if(tmpbuf[0]==PN9_Tab[7])break; //数据正确 退出
			   }
			if(a>=20) break;              //发射次数超过，退出
			
			StrobeCmd(CMD_STBY);//进入待机模式 
			}   			    
		while(_pa1==0);	    
		}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  			 
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
	Uint8 i;
	Uint8 addr;

	addr = IDCODE_REG; //写ID命令 send address 0x06, bit cmd=0, r/w=0
	SCS = 0;				  //片选
	ByteSend(addr);			  //发送命令
	for (i=0; i < 4; i++)
		ByteSend(ID_Tab[i]);  //发送ID
	SCS = 1;

}

/************************************************************************
**  A7105_WriteReg
写寄存器 寄存器地址，数据
************************************************************************/
void A7105_WriteReg(Uint8 addr, Uint8 dataByte)
{
    Uint8 i;

	SCS = 0;
    addr |= 0x00; //bit cmd=0,r/w=0
    for(i = 0; i < 8; i++)
    {
        if(addr & 0x80)
            SDIO = 1;
        else
            SDIO = 0;

        SCK = 1;
        _nop_();
        SCK = 0;
        addr = addr << 1;
    }
    _nop_();

    //send data byte
    for(i = 0; i < 8; i++)
    {
        if(dataByte & 0x80)
            SDIO = 1;
        else
            SDIO = 0;

        SCK = 1;
        _nop_();
        SCK = 0;
        dataByte = dataByte<< 1;
    }
    SCS = 1;
}

/************************************************************************
**  A7105_ReadReg //读寄存器
************************************************************************/
Uint8 A7105_ReadReg(Uint8 addr)
{
    Uint8 i;
    Uint8 tmp;

	SCS = 0;
    addr |= 0x40; //bit cmd=0,r/w=1
    for(i = 0; i < 8; i++)
    {

        if(addr & 0x80)
            SDIO = 1;
        else
            SDIO = 0;

		_nop_();
        SCK = 1;
        _nop_();
        SCK = 0;

        addr = addr << 1;
    }
    _pcc=0b00000010;//数据线为输入口
    _nop_();
    SDIO = 1;

    //read data
    for(i = 0; i < 8; i++)
    {
        if(SDIO)
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

        SCK = 1;
        _nop_();
        SCK = 0;
    }
    SCS = 1;
   _pcc=0;//数据线为输入口
    return tmp;
}

/************************************************************************
**  Byte Send	//写数据 8位
************************************************************************/
void ByteSend(Uint8 src)
{
    Uint8 i;

    for(i = 0; i < 8; i++)
    {
        if(src & 0x80)
            SDIO = 1;	   //数据
        else
            SDIO = 0;

		_nop_();
        SCK = 1;		   //时钟
        _nop_();
        SCK = 0;
        src = src << 1;
    }
}

/************************************************************************
**  ByteRead	读数据 8位
************************************************************************/
Uint8 ByteRead(void)
{
	Uint8 i,tmp;
  _pcc=0b00000010;//数据线为输入口
    SDIO = 1; //sdio pull high	 数据线上拉
    for(i = 0; i < 8; i++)
    {
        if(SDIO)				 //读数据
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

        SCK = 1;				 //时钟
        _nop_();
        SCK = 0;
    }
    _pcc=0;//数据线为输入口
    return tmp;
}

/************************************************************************
**  Send4Bit	 半字节4位发送  字节的高四位写命令
************************************************************************/
void Send4Bit(Uint8 src)
{
    Uint8 i;

    for(i = 0; i < 4; i++)
    {
        if(src & 0x80)
            SDIO = 1;
        else
            SDIO = 0;

		_nop_();
        SCK = 1;
        _nop_();
        SCK = 0;
        src = src << 1;
    }
}

/*********************************************************************
** SetCH
*********************************************************************/
void SetCH(Uint8 ch)
{
    //RF freq = RFbase + (CH_Step * ch)
	A7105_WriteReg(PLL1_REG, ch);
}


/*********************************************************************
** A7105_WriteFIFO 把数据送入FIFO中 字节地址，个数
*********************************************************************/
void A7105_WriteFIFO(Uint8 p[], Uint8 byte)
{
	Uint8 i;
	Uint8 cmd;

	cmd = FIFO_REG; //send address 0x05, bit cmd=0, r/w=0 地址
    SCS=0;						  //片选
	ByteSend(cmd);			      //写命令
for(i=0; i <byte; i++)
		ByteSend(p[i]);	  //写64个数据到FIFO中
	SCS=1;						  //片选
}

/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCmd(Uint8 cmd)
{
	SCS = 0;
	Send4Bit(cmd);	 //发送4位
	SCS = 1;
}

/*********************************************************************
** Rx Packet   从FIFO中读出资料并对比
*********************************************************************/
void RxPacket(Uint8 a[])
{
    Uint8 i;
    Uint8 recv;
     Uint8 tmp;
    Uint8 cmd;
    
	cmd = FIFO_REG | 0x40; //读命令 address 0x05, bit cmd=0, r/w=1
    SCS=0;				    //片选
	ByteSend(cmd);			//写命令
	for(i=0; i <8; i++)
	{
	  a[i]  = ByteRead();	//读数		
    }
	SCS=1;
}


/*********************************************************************
** A7105_Config	  //配置芯片
*********************************************************************/
void A7105_Config(void)
{
	Uint8 i;

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

/**********************************************************
* Delay10ms @12MHz			//延迟函数
**********************************************************/

void Delay10ms(Uint8 n)
{
    Uint8 i,j;
    while(n--)
    {
        for(i=0;i<22;i++)
            for(j=0;j<250;j++);
    }
}
