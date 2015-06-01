#ifndef  __SOFT_H
#define  __SOFT_H
//=================������PWM������ʹ�õ�����================
#define   L_FRONT     0x1   //������ǰ
#define   L_BACK      0x2   //�������
#define   R_FRONT     0x3   //������ǰ
#define   R_BACK      0x4   //�������
#define   CHARGE_PWM  0x5   //������
//=================����������������ʹ�õ�io�Ŷ���==============
#define   L_FRONTONOFF   GPIO_Pin_0   //������ǰ����   PE
#define   L_BACKONOFF    GPIO_Pin_1   //������󿪹�   PE 
//#define   L_CURRENT      GPIO_Pin_2   //���ֵ�������   PC
#define   L_SPEED        GPIO_Pin_6   //�����ٶ�       PD
#define   L_LIDI         GPIO_Pin_7   //������ؿ���   PD

#define   R_FRONTONOFF   GPIO_Pin_2   //������ǰ����   PE   
#define   R_BACKONOFF    GPIO_Pin_3   //������󿪹�   PE  
//#define   R_CURRENT      GPIO_Pin_3   //���ֵ�������   PC
#define   R_SPEED        GPIO_Pin_4   //�����ٶ�       PE
#define   R_LIDI         GPIO_Pin_5   //������ؿ���   PE
//=================��������ʾ�����ܽŵĶ���=====================
#define  LCD_POWER       GPIO_Pin_10	//LCD��Դ����   PD
#define  LCD_STB  		 GPIO_Pin_15	//LCDָ�����   PB
#define  LCD_CLK  	 	 GPIO_Pin_8	//LCDʱ��       PD
#define  LCD_DATA	  	 GPIO_Pin_9	//LCD����       PD
#define  LCD_BG 	   	 GPIO_Pin_14	//LCD����       PD
//=================�����ǰ����ܽŶ���=============================
#define  KEY1        	 GPIO_Pin_1 //����1         PD
#define  KEY2        	 GPIO_Pin_2 //����1         PD
#define  KEY3         	 GPIO_Pin_3 //����1         PD
//=================�����Ƿ������ܽŶ���============================
#define  BEEP        	 GPIO_Pin_13   //������     PD

//=================�������ٶȼ�ⷢ����ƹܽŶ���==================
#define  SPEED_SEND  	 GPIO_Pin_5    //�����     PD

//=================�����ǵؼ췢����������IO�ŵĶ���==============
#define  EARTH_SEND      GPIO_Pin_4   //�����     PD

//=================������ǽ�췢����������IO�ŵĶ���==============
#define  WALL_SEND       GPIO_Pin_12   //�����     PA

//=================�����ǵ�س������Ŷ���=========================
#define  CHARGE_DC   	 GPIO_Pin_12   //DC����     PD
#define  CHARGE_SEAT 	 GPIO_Pin_11   //�س������� PD

//=================��������ɨ����Ŷ���==============================
//#define  M_MOTOR_DRIVER  GPIO_Pin_8    //��ɨ������ PC
#define  M_MOTOR_ZPMOS   GPIO_Pin_10    //��ɨ������ PB
#define  M_MOTOR_FPMOS   GPIO_Pin_8    //��ɨ������ PC
#define  M_MOTOR_ZNMOS   GPIO_Pin_14    //��ɨ������ PE
#define  M_MOTOR_FNMOS   GPIO_Pin_15    //��ɨ������ PE

//=================�����Ǳ�ɨ���io�ŵĶ���==========================  
#define  S_MOTOR_DRIVER  GPIO_Pin_8    //��ɨ������ PA      
#define  S_CURRENT       GPIO_Pin_9    //����       PC

//=================�����ǻҳ��з�����IO�Ķ���=======================
#define  DUST_MOTOR_DRIVER  GPIO_Pin_7  //�ҳ�������         PC

//=================������������������IO�ŵĶ���=====================
#define  UV_DRIVER       GPIO_Pin_14   //UV�������          PB

//=================������ǰ��������ܽŶ���=========================	 
#define  FRONT_WHEEL     GPIO_Pin_5   //��������          PB

//=================�����Ǻ�����յĹܽŶ���============================ 
#define  HW_POWER        GPIO_Pin_7   //�����Դ�������    PE
#define  L_HW            GPIO_Pin_8   //���������          PE
#define  M_HW            GPIO_Pin_9   //�к�������          PE
#define  R_HW            GPIO_Pin_10  //�Һ�������          PE
#define  UP_HW           GPIO_Pin_11  //����������          PE

//==================��������ײ����ĹܽŶ���=============================	
#define  L_BUMP          GPIO_Pin_12  //����ײ����          PE
#define  R_BUMP          GPIO_Pin_13  //����ײ����          PE

//==================������A7105�����ĹܽŶ���=============================
#define  SCS             GPIO_Pin_10    //A7105Ƭѡ�źţ�PC�ڡ�
#define  SCK             GPIO_Pin_11    //A7105ʱ���źţ�PC�ڡ�
#define  SDIO            GPIO_Pin_12    //A7105�����źţ�PC�ڡ�
#define  GIO1            GPIO_Pin_0     //A7105�����źţ�PD�ڡ�

//==================������һЩ�������====================================
#define  BATTERY_VOLTAGE    0   //��ص�ѹͨ��
#define  BATTERY_TEMP       1   //����¶�ͨ��     //////////////
#define  CHARGE_CURRENT     2   //������ͨ��
#define  E_L                3   //��ߵؼ�ͨ��
#define  E_LM               4   //���еؼ�ͨ��
#define  E_RM               5   //���еؼ�ͨ��
#define  E_R                6   //�ұߵؼ�ͨ��
#define  W_L                7   //���ǽ��ͨ��    ADת��ͨ��
#define  W_RM               8   //����ǽ��ͨ��
#define  W_R                9   //�ұ�ǽ��ͨ��
#define  M_CURRENT          10  //��ɨ����ͨ��
#define  DUST_CURRENT       11  //�ҳ�����ͨ��
#define  L_CURRENT          12  //���ֵ���ͨ��
#define  R_CURRENT          13  //���ֵ���ͨ��
#define  W_LM               14  //����ǽ��ͨ��
#define  W_M                15  //�м�ǽ��ͨ��   

//===================�����ǵ�ص�ѹ����======================================
#define NOPOWER             1241   //��ص�ѹ11V����Ϊ����
#define LOWPOWER            1489   //��ѹ1.1v					
#define FULLPOWER           1895   //��ѹ1.4v
#define ONEPOWER            1624   //��ѹ1.2v
#define TWOPOWER            1760   //��ѹ1.3v
#define FULLPOWER_CHARGE    2058   //���ʱ�������ѹ   1.52V   18.24V 
#define TWOPOWER_CHARGE     1949   //���ʱ�������ѹ   1.44V   17.28V
#define ONEPOWER_CHARGE     1895   //���ʱ��һ���ѹ   1.40V   16.80V
#define FULLPOWER_CEASE     1692   //����ʱ�������ѹ   1.25V   15.00V
#define TWOPOWER_CEASE      1651   //����ʱ�������ѹ   1.22V   14.64V
#define ONEPOWER_CEASE      1624   //����ʱ��һ���ѹ   1.20V   14.40V
#define FULLPOWER_WORK      1651   //����ʱ�������ѹ   1.22V   14.64V
#define TWOPOWER_WORK       1624   //����ʱ�������ѹ   1.20V   14.40V
#define ONEPOWER_WORK       1584   //����ʱ��һ���ѹ   1.17V   14.04V

//=======================�����Ǻ�����յ����ݶ���========================
#define  L_CHARGE         	0xe0		
#define  M_CHARGE         	0xb0
#define  R_CHARGE         	0xd0
#define  UP_CHARGE         	0x70
#define  L_UP_CHARGE        0x60 
#define  M_UP_CHARGE        0x30 
#define  R_UP_CHARGE  		0x50
#define  L_M_CHARGE         0xa0
#define  L_R_CHARGE         0xc0
#define  R_M_CHARGE         0x90 
#define  L_M_UP_CHARGE      0x20   
#define  L_R_UP_CHARGE      0x40   
#define  R_M_UP_CHARGE   	0x10
#define  L_M_R_CHARGE 		0x80
#define  L_M_R_UP_CHARGE 	0x00
#define  V_DUMMYWALL        0xdc 
#define  UP__DUMMYWALL      0xec    
#define  V_UP_DUMMYWALL     0xcc                             

//============================������ң�����İ�������===============================
#define  YAOKONG_ESC           10	   //�˳���ȡ�����رյ�Դ
#define  YAOKONG_TIMINE_MON	   6	   //��ʱ������һ
#define  YAOKONG_SETTIME_TUE   9	   //�趨ʱ�䡢���ڶ�
#define  YAOKONG_UV_WED		   3	   //UV��������
#define  YAOKONG_AUTO_THU	   1	   //������ɨ��������
#define  YAOKONG_SPOT_FRI	   4	   //�Զ���ɨ��������
#define  YAOKONG_V_SAT		   8	   //�ٶ��趨��������
#define  YAOKONG_DOCKING_SUN   5	   //�س䡢������
#define  YAOKONG_ED			   2	   //ÿ��
#define  YAOKONG_LEFT		   12	   //����
#define  YAOKONG_RIGHT		   13	   //����
#define  YAOKONG_FRONT		   14	   //��ǰ
#define  YAOKONG_BACK		   11	   //���
#define  YAOKONG_OK			   7	   //ȷ��
                         
//================================�����ǵؼ��ǽ���״̬����============================
#define     NEAR            0XFE                              //��
#define     FARN            0XFD                              //Զ

#define     LEFT             0            //����������
#define     RIGHT            1            //����������

#define     FRONT             0X01                // ��ǰ
#define     BACK              0X02                // ���

#define     BAD              0X01                //���ֲ��ܶ�
#define     GOOD             0X00                //��������

#define     LI_ANGLE        0X90         //��ǽ��
#define     WAI_ANGLE       0XA0         //��ǽ��
#define     U_ANGLE         0XB0         //U��ǽ��

#define     POWER_DOWN       0X003        //��ʾ���û��
#define     POWER_TEMP       0x005        //��ʾ����¶ȹ���
#define     POWER_NTC        0X006        //���û��NTC
#define     POWER_CHARGE     0X007        //������ڳ��
#define     CHARGE_ERR       0X009        //������
//=====================================����ģʽ=================================
#define     CEASE            0X00                //����ģʽ
#define     SWEEP            0X01                //ɨ��ģʽ
#define     SPOT             0X02                //������ɨ
#define     TELE             0X03                //ң��ģʽ
#define     DOCKING          0X04                //�Զ��س�ģʽ
#define     CHARGEING        0X05				 //���ģʽ
#define     SLEEP            0X06                //˯��ģʽ 
#define     ERR              0X07				 //����ģʽ
#define     BORDER           0X08				 //??????
//====================================�������ٶȵĶ���=============================
#define     HIGHSPEED        1200                //���ٵ��ٶ�
#define     LOWSPEED         600                 //���ٵ��ٶ�

/*************************************************************
������ֱ����68mm
������  61.6
������  16
һ������ĳ�����68*3.14/61.6/16=0.2167mm��
���־���  267.6mm��
ëˢ   ���82mm  �ұ�  75mm ��  ƽ��ëˢ150mm
*************************************************************/
#define     SPEED           1000        // ����Ϊ���ֵ�����ٶ�   
#define     FARAWAY         0XFFFFFFFF   //����Ϊ����Զ
#define     LENGTH		    243			 //���ֵĿ��  ��λ�Ǻ���
#define     METE            4615         //1�׵�������
#define     METE_H5         2308         //0.5�׵ľ���
#define     METE_H6         2769         //0.6�׵ľ���
#define     METE_H7         3231         //0.7�׵ľ���
#define     METE_H8         3692         //0.8�׵ľ���
#define     METE_H9         4154         //0.9�׵ľ���
#define     METE_TWO		5130         //2�׵�������
#define     METE_0			5592		 //1.1�׵�������
#define     METE_1          6053         //1.2�׵�������	
#define     METE_2			6515		 //1.3�׵�������
#define     METE_3			6976		 //1.4�׵�������
#define     METE_4			7438		 //1.5�׵�������
#define     METE_5			7899		 //1.6�׵�������
#define     METE_6			8361		 //1.7�׵�������
#define     METE_7			8822		 //1.8�׵�������
#define     METE_8			9384		 //1.9�׵�������
#define     METE_THREE		13845		 //3�׵�������
//=================================����PID�㷨��ϵ��========================
#define     KP              4     //8
#define     KI              10    //30
#define     KD              80    //150

#define   radii    3880            //����Ϊ�����ĵ�һȦ��������
#define   sys_err_p  10			   //����ϵͳ���
#define   sys_err_n  20			   //����ϵͳ���
#define   angle5   81
#define   angle15  162			   //��ת15��	   
#define   angle20  200			   //��ת20��
#define   angle30  323
#define   angle45  485
#define   angle60  647
#define   angle75  808
#define   angle90  970
#define   angle105  1131
#define   angle120  1293
#define   angle135  1455
#define   angle150  1616
#define   angle165  1778
#define   angle180  1940
#define   angle195  2101
#define   angle210  2263
#define   angle225  2424
#define   angle240  2586
#define   angle255  2748
#define   angle270  2910
#define   angle285  3071
#define   angle300  3233
#define   angle315  3395
#define   angle330  3556
#define   angle345  3717
#define   angle360  3880
#define   angle720  7760
//=============================һЩ�꺯������==================================
#define   SUBBTIME(t1,t2)   ((t1>t2)?(0xffffffff-t1+t2):(t2-t1))    
#define   DELAY(a)          while(a--);          

typedef struct 			    //�����Ľṹ
{
     uint8 key;		        //��ֵ,��ǰ״̬
     uint8 number;          //ȥ������
     uint8 (*read_start)(); //��ȡ��״̬����
     uint8 sign;	        //��־  ��ʾ�ð�����Ч��û�б�����
}KEY;

typedef struct 			  //ң����
{
    uint32 effect_time;   //����ʱ��
	uint8 old_key;        //�ɰ���
    uint8 effect;         //������Чʱ��
    uint8 key;		      //��ֵ
    uint8 sign;	          //�а��� 
	uint8 test;			  //���Ե�ң������־  0��ʾ�Լ���ң������1��ʾ�ź����ɱ���ΪFFFF��ң�����������ź�
}YAOKONG;

typedef struct 			  //��������ֵ�Ľṹ
{
 bool uv;		 
 uint8 work_mode;	 
 uint8 speed;
}WORK_MODE;
typedef struct
{
 uint32 time;  //������������ʱ��    
 uint8  odds;  //�ж������ı�׼
 uint32 samp;  //��һ��ʱ������������ĸ�����
 uint8  step;  //�������Ĺ��̲���
 uint8  sign;  //�������ı�־ 0��ʾ������,1��ʾ������
}MUCK;
typedef struct             //�봮��ͨѶ�����ݽṹ
{
  uint8  rcv_buff[12];	   //���ջ���
  uint8  rcv_number;	   //�������ݸ���
  uint32  time ;		   //����ʱ��ϵͳʱ��
  uint8  send_buff[12];	   //���ͻ���
  uint8  send_number;	   //���͵��ֽ���
  uint8  send_t;		   //���������Ѿ����͵Ĵ���
  uint8  send;			   //�������ݵı�־
} UART;
typedef struct             //ǽ��ؼ�����ݽṹ
{
  uint16 brightness;	   //�������ʱ������ֵ
  uint16 darkness;         //�������ʱ������ֵ
  uint8  near;	           //���ش���
  uint8  farn;	           //Զ�ش���
  uint16 difference;       //�����ķ���ֵ
  uint32 low_door;         //����ֵ	 
  uint32 high_door;        //����ֵ
  uint8  sign;	           //���Ƶı�־
  uint8  high_sign;	       //���Ƶı�־	
  uint8  high_near;	       //���ش���
  uint8  high_farn;	       //Զ�ش���
  uint8  on;               //����ǽ����Ч��־��0Ϊ��Ч��1Ϊ��Ч
}WALL_DATA;

typedef struct             //ǽ��ؼ�����ݽṹ
{
  uint8  (*read_io)();
  uint8   data;	           //������յ�����
  uint32  time;	           //���տ�ʼ��ʱ��
  uint8   tempdata;        //��ʱ���յ�����
  uint8   tempbit ;	       //�����ĵ�ǰλ��ֵ
  uint8   bitnumber;	   //�������λ
  uint8   start;           //�����ź����ݿ�ʼ��־
volatile    uint32   sign;	      //������յ��ı�־
volatile    uint32  effect_time;  //������Чʱ��
volatile    uint32  effect;       //������Ч
}INFR_DATA;

typedef struct                    //�����ٶȵ����ݽṹ
{ 
  int32  ek[3];                   //���������ϵͳ�ٶ�ƫ��
  volatile    uint32   length;	   //�������ߵ�������
  volatile    uint32   dq_time;      //���ӵ�ǰʱ�䣬���½��ص�ʱ��
  uint32  rising_time;               //��ǰ�⵽��������ʱ��
  uint32  old_time;                  //��������ǰ��ʱ��
  uint8  state;                      //����״̬  BAD;���ֲ��� GOOD��������
  uint32  odds;                      //����ͣ��ת����ʱ�����

//  uint16 rising_captime;           //��ǰ�⵽��������ʱ��
//  uint32 rising_dqtime;            //ȷ���⵽��������ʱ��
//  uint16 rising_dqcaptime;         //ȷ���⵽��������ʱ��
//  volatile    uint16   dq_captime; //���Ӳ���Ķ�ʱ�����������½��ص�ʱ��    
//  uint32 falling_time;             //��ǰ�⵽���½���ʱ��
//  uint16 falling_captime;          //��ǰ�⵽���½���ʱ��
//  uint16  old_captime;
}RING;
typedef struct	                  //���ֵ��ٶ���Ϣ
{
uint8    master;                  //�����ֱ�־   ��1Ϊ�����֣�0Ϊ�Ӷ���
uint8    old_time;                //���ٵ���ʼʱ��
uint8    ori   ;                  //���ֵ�ǰ����         
uint16   pwm   ;                  //���ֵ�pwmֵ
int32    rap  ;                   //���ֵ��ٶȣ���ʾΪÿ���������
uint32   length  ;                //����Ӧ���߹��ľ���
uint32   time   ;                 //�����ٶȵ�ʱ��
uint32   pulse  ;                 //�����ٶ��õ�������
uint8    sign   ;                 //��־ 0:����ֹͣ 1:������������ʽΪ����ͣ�� 2:�׶ε��� 3:�������������㲻ͣ
}RAP;
typedef  struct
{
uint32 temp   ;	     //����¶� 
uint32 voltage;		 //��ص�ѹ   
uint32 current;      //������
uint8  charge_dc ;	 //�ⲿDC�������ź�
uint8  charge_seat ; //�ⲿ����������ź�
uint16 pwm;         //���PWM����
uint8  err;         //��ش���  0 û�д���1û��NTC��2 NTC��·��3 û�е�أ� 4 ��ض�·
uint8 step;         //��粽��  0 С����  1 �����  2ά�ֵ���
uint32 time;        //��翪ʼʱ��
}POWER;

typedef  struct
{
uint8 data;          //�����źŵ����� 0:��ʾû�ж��� 1:��ʾ���Ѿ�����
uint32 time;         //��ȡ�����źŵ�ʱ��
}SWITCH;

typedef  struct
{
uint32 current;	       //��ǰ�����ĵ���ֵ
uint32 old_current;    //ǰһ�εĲ�������
uint32 sum_current;    //�����ĺ�
uint32 linshi_current; //��ʱ��ŵĲ�������ֵ
uint32 temp;	       //�����ı�׼ֵ
uint32 time;		   //����ʱ��
uint32 typo;		   //�����������
uint8  sign;		   //������־         0:��ʾδ���� 1:��ʾ����
uint8 sign_l;
}CURRENT;

typedef  struct
{
uint32 * music;	    //��Ҫ���ŵ����ֶ�
uint32 temp;	    //�����ı�׼ֵ
uint32 time;		//���ֲ��ŵ�ʱ��
uint32 typo;		//���ֲ��ŵ���ʼλ��
volatile  uint8  sign;		//�źű�־   0:��ʾ���� 1:��ʾ��
}mySOUND;

typedef struct	    //�����������
{
uint32 x;		    //������x����
uint32 y;			//������y����
uint32 angle;		//�����ĽǶ�
}POSITION;

typedef struct		//����ϵͳ�Ĺ���״̬
{
 uint8  mode ;	    //����������ģʽ  0����ֹģʽ��1��ɨ��ģʽ��2���Զ��س�ģʽ��3��ң��ģʽ	4:����ģʽ
 uint32 action;		//����   0:ֹͣ  1:ԭ����ת  2:ԭ����ת 3:ǰ��   4:����   5:��ת1  6����ת2  ������18��ת14	��19��������
 uint8  fangxiang;  // 0 ��ת�� 1��ת
 uint8  sign;       //������־ 
 uint8  *road;		//·��ָ��

 uint32 time;       //��������ʼʱ��
 uint32 run_time;   //ɨ��ʱ��Ҫ����ʱ��
 uint8  step;		//·��ִ�еĲ���
 uint32  abnormity; //�쳣����  0��ʾû���쳣
 uint8 step_abn;    //�쳣ʱ�Ĳ���
 uint32 bump ;      //��ײ�ı�־
 uint8  step_bp;    //��ײʱ�Ĳ����־
 uint8  times_bp;   //��ײ�Ĵ���
 uint32 angle_wall; //ǽ��ĽǶ�
 uint32 long_wall;  //ǽ��ľ���
 uint8  start_wall; //ǽ�쿪ʼ
 uint8  muck;       //��������ɨ·����ʼ
 uint8  step_mk;    //��������ɨ����
 uint8  times_mk;   //�������ʱ������ת����
 uint8  ybs_sign;   //�ر�ɨ��־
 uint8  step_yaokong; // ��ң��ʱ�Ĳ���
 uint8 sum_bump;    //���ۼƵ���ײ��������ACTION_BUMP()������DO_SWEEP()�����޸ĺ�ʹ�á�
}MODE;
typedef struct
{
  uint8 step;	    //����
  uint8 sign;	    //��ʼ��־   0��ʾû�п�ʼ��1��ʾ�Ѿ���ʼ
  uint32 time ;     //������ʼ��ʱ��
  uint32 length ;   //���Ȼ�����תȦ��
}ACTION;

typedef	 union		 //��ʾ����
{
 uint32 data;
 uint8  dis_data[4];
}DIS_DATA;
typedef struct
{
  uint8 sign;
  uint8 step;
  volatile  uint8 start;
  uint32 time;
} DUST;
#endif

