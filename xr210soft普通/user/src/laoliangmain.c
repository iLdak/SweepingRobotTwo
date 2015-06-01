
#include "ht48r06a-1.h"
#include "a7105.h"

#define Uint8 unsigned char
#define uint  unsigned int
#define _nop_()  _nop()

/*********************************************************************
**  I/O Declaration
*********************************************************************/
#define SCS       	_pc0		//Ƭѡ
#define SCK      	_pa0		//ʱ��
#define SDIO      	_pc1		//����
#define GPIO1      	_pb0		//GPIO1

//ϵͳID��
const Uint8 ID_Tab[4]={0x54,0x75,0xC5,0x2A}; //ID code
//���������б�


const Uint8  A7105Config[51]=	 //���üĴ���
{
	0x00, //д��ʱоƬ��λ������Ϊ״̬λ
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
 Uint8 tmpbuf[8];                                            //�������ݻ�����
 Uint8  PN9_Tab[8]= {0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,0};  //���͵�����
/**
// **************��ʼ��IO��********************
   _pac=0b00000010;//P0��Ϊ���,������Ϊ����
   _pbc=0xff;//GPIO1��Ϊ����
   _pcc=0;//p0��Ϊ���	 
  	  
//**************��ʼ������оƬ***************
	SCS = 1;
    SCK = 0;
    SDIO = 1;
    GPIO1 = 1;
    A7105_Reset();   //��λоƬ д��ַ0�ȸ�λ������ַ0�õ�״̬	
	A7105_WriteID(); //доƬID����	
	A7105_Config();  //оƬ����	
	
// *************оƬУ׼*********************

	StrobeCmd(CMD_STBY);                   //0XA0  ���ʹ�������
	A7105_WriteReg(CALIBRATION_REG, 0x01); //0X02 У׼ʹ��
	do                                     //�ȴ�У׼���
	{
		a = A7105_ReadReg(CALIBRATION_REG);
		a &= 0x01;
	}
	
	while (a);
	a = A7105_ReadReg(IFCAL1_REG);//0X22�Ĵ���
    a=a&0X10;
	if (a)
	{
	    while(1);    //����У׼ʧ��
	}
	
    A7105_WriteReg(VCOCCAL_REG, 0x13); //0X25 VCO У׼	
	A7105_WriteReg(VCOCAL1_REG, 0x09);//0X26 VCO У׼	
    StrobeCmd(CMD_STBY);           //�������ģʽ
// &&&&&&&&&&&&&&&���ճ���&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//��������˵������Ƶ��2449.501MHz����������
//             �����ݣ�����У��ͣ�
//             �������������ȷ����2450.001MHzƵ�η���У���
   
// ********************************************
       while(1)
        {   
            SetCH(99);                 //Ƶ��2449.501MHz
            StrobeCmd(CMD_RFR);        //���RX FIFO
            StrobeCmd(CMD_RX);         //�������ģʽ
    	    while(GPIO1==1);           //�ȴ�����
   			RxPacket(tmpbuf);		   //��ȡ����,��ڻ����ַ
   			               //У���
   			if(tmpbuf[7]==(tmpbuf[0]+tmpbuf[1]+tmpbuf[2]+tmpbuf[3]+tmpbuf[4]+tmpbuf[5]+tmpbuf[6]));                          
   			   {
		       A7105_WriteFIFO(&tmpbuf[7],1); //�����ݵ����ͻ�����
		       SetCH(100); //                 //��Ƶ�� 2450.001MHz����
   		       StrobeCmd(CMD_TX);             //����
    	       while(GPIO1==1);               //�ȴ��������
    	       SCS=0;
    	       for(a=0;a<200;a++)
    	       _delay(2000);
    	        SCS=1;
    	       
    	       }

			
        }
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
********/
//&&&&&&&&&&&&&&&&&&&&���ͻ�����&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//�������ܣ��������ݼ�У���룬Ȼ�����Ӧ��Ӧ������ûӦ�𣬴ӷ�20��
//
 _pac=0b11111111;//P0��Ϊ���,������Ϊ����
		while(1)		 
		{//clrwdt(); //�忴�Ź�
 //û�а����������
		  if (_pa1==1) 
		      {StrobeCmd(CMD_SLEEP);  //����оƬ����
		      _pcc=0xff;
		      _pbc=0xff;
		      _pac=0xff;//P0��Ϊ���,������Ϊ����
		       _halt();              //û�а����������
		      }
		      
// **************��ʼ��CPU********************
   _pac=0b00000010;//P0��Ϊ���,������Ϊ����
   _pbc=0xff;//GPIO1��Ϊ����
   _pcc=0;//p0��Ϊ���	 
//��������
        StrobeCmd(CMD_STBY);//0XA0  ��������
	     SCS = 1;
         SCK = 0;
         SDIO = 1;
         GPIO1 = 1;
         A7105_Reset();   //��λоƬ д��ַ0�ȸ�λ������ַ0�õ�״̬	
	     A7105_WriteID(); //доƬID����	
	     A7105_Config();  //оƬ����	
	
// *************оƬУ׼*********************

	
	A7105_WriteReg(CALIBRATION_REG, 0x01); //0X02 У׼ʹ��
	do   //�ȴ�У׼���
	{
		a = A7105_ReadReg(CALIBRATION_REG);
		a &= 0x01;
	}
	
	while (a);
	a = A7105_ReadReg(IFCAL1_REG);//0X22�Ĵ���
    a=a&0X10;
	if (a)
	{
	    while(1);    //����У׼ʧ��
	}
	
    A7105_WriteReg(VCOCCAL_REG, 0x13);//0X25 VCO У׼
	
	A7105_WriteReg(VCOCAL1_REG, 0x09);//0X26 VCO У׼
	
    StrobeCmd(CMD_STBY);                    //�������ģʽ		      
//��������		      
	PN9_Tab[7]=PN9_Tab[0]+PN9_Tab[1]+PN9_Tab[2]+PN9_Tab[3]+PN9_Tab[4]+PN9_Tab[5]+PN9_Tab[6];
	a=0;
         while(1)
       		{      
		    A7105_WriteFIFO(PN9_Tab,8);     //д���͵����ݵ�fifo
		    SetCH(100);                     //freq 2450.001MHz
   		    StrobeCmd(CMD_TX);              //���뷢��
    		while(GPIO1);                   //�ȴ��������
    		SetCH(99);                      //freq 2449.501MHz ��Ƶ
    		StrobeCmd(CMD_RX);              //�������
    		a++;
    		time=0;
			while((GPIO1==1)&&(a<20))          //�ȴ�����
			    {time++;
			     if(time>100)
			       {
                   break;         //20��û�н��յ�Ӧ���˳�
			       }
			     _delay(1000);
			    }
			if(GPIO1!=1)             //������
			   {RxPacket(tmpbuf);		   //��ȡ����,��ڻ����ַ
			    if(tmpbuf[0]==PN9_Tab[7])break; //������ȷ �˳�
			   }
			if(a>=20) break;              //��������������˳�
			
			StrobeCmd(CMD_STBY);//�������ģʽ 
			}   			    
		while(_pa1==0);	    
		}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  			 
}

/************************************************************************
**  Reset_RF	��λоƬ
************************************************************************/
void A7105_Reset(void)
{
	A7105_WriteReg(MODE_REG, 0x00); //��Ĵ���0�ĸ�λд�ȸ�λоƬ
}

/************************************************************************
**  WriteID	   //доƬID�����оƬIDΪ4���ֽ�
************************************************************************/
void A7105_WriteID(void)
{
	Uint8 i;
	Uint8 addr;

	addr = IDCODE_REG; //дID���� send address 0x06, bit cmd=0, r/w=0
	SCS = 0;				  //Ƭѡ
	ByteSend(addr);			  //��������
	for (i=0; i < 4; i++)
		ByteSend(ID_Tab[i]);  //����ID
	SCS = 1;

}

/************************************************************************
**  A7105_WriteReg
д�Ĵ��� �Ĵ�����ַ������
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
**  A7105_ReadReg //���Ĵ���
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
    _pcc=0b00000010;//������Ϊ�����
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
   _pcc=0;//������Ϊ�����
    return tmp;
}

/************************************************************************
**  Byte Send	//д���� 8λ
************************************************************************/
void ByteSend(Uint8 src)
{
    Uint8 i;

    for(i = 0; i < 8; i++)
    {
        if(src & 0x80)
            SDIO = 1;	   //����
        else
            SDIO = 0;

		_nop_();
        SCK = 1;		   //ʱ��
        _nop_();
        SCK = 0;
        src = src << 1;
    }
}

/************************************************************************
**  ByteRead	������ 8λ
************************************************************************/
Uint8 ByteRead(void)
{
	Uint8 i,tmp;
  _pcc=0b00000010;//������Ϊ�����
    SDIO = 1; //sdio pull high	 ����������
    for(i = 0; i < 8; i++)
    {
        if(SDIO)				 //������
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

        SCK = 1;				 //ʱ��
        _nop_();
        SCK = 0;
    }
    _pcc=0;//������Ϊ�����
    return tmp;
}

/************************************************************************
**  Send4Bit	 ���ֽ�4λ����  �ֽڵĸ���λд����
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
** A7105_WriteFIFO ����������FIFO�� �ֽڵ�ַ������
*********************************************************************/
void A7105_WriteFIFO(Uint8 p[], Uint8 byte)
{
	Uint8 i;
	Uint8 cmd;

	cmd = FIFO_REG; //send address 0x05, bit cmd=0, r/w=0 ��ַ
    SCS=0;						  //Ƭѡ
	ByteSend(cmd);			      //д����
for(i=0; i <byte; i++)
		ByteSend(p[i]);	  //д64�����ݵ�FIFO��
	SCS=1;						  //Ƭѡ
}

/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCmd(Uint8 cmd)
{
	SCS = 0;
	Send4Bit(cmd);	 //����4λ
	SCS = 1;
}

/*********************************************************************
** Rx Packet   ��FIFO�ж������ϲ��Ա�
*********************************************************************/
void RxPacket(Uint8 a[])
{
    Uint8 i;
    Uint8 recv;
     Uint8 tmp;
    Uint8 cmd;
    
	cmd = FIFO_REG | 0x40; //������ address 0x05, bit cmd=0, r/w=1
    SCS=0;				    //Ƭѡ
	ByteSend(cmd);			//д����
	for(i=0; i <8; i++)
	{
	  a[i]  = ByteRead();	//����		
    }
	SCS=1;
}


/*********************************************************************
** A7105_Config	  //����оƬ
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
	    A7105_WriteReg(i, A7105Config[i]);// д�Ĵ���

	for (i=0x07; i<=0x22; i++)
		A7105_WriteReg(i, A7105Config[i]);

	for (i=0x24; i<=0x31; i++)
		A7105_WriteReg(i, A7105Config[i]);
}

/**********************************************************
* Delay10ms @12MHz			//�ӳٺ���
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
