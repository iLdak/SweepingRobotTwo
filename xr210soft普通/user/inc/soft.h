#ifndef  __SOFT_H
#define  __SOFT_H
//=================下面是PWM驱动脚使用的软定义================
#define   L_FRONT     0x1   //左轮向前
#define   L_BACK      0x2   //左轮向后
#define   R_FRONT     0x3   //右轮向前
#define   R_BACK      0x4   //右轮向后
#define   CHARGE_PWM  0x5   //充电控制
//=================下面是左右轮驱动使用的io脚定义==============
#define   L_FRONTONOFF   GPIO_Pin_0   //左轮向前开关   PE
#define   L_BACKONOFF    GPIO_Pin_1   //左轮向后开关   PE 
//#define   L_CURRENT      GPIO_Pin_2   //左轮电流采样   PC
#define   L_SPEED        GPIO_Pin_6   //左轮速度       PD
#define   L_LIDI         GPIO_Pin_7   //左轮离地开关   PD

#define   R_FRONTONOFF   GPIO_Pin_2   //右轮向前开关   PE   
#define   R_BACKONOFF    GPIO_Pin_3   //右轮向后开关   PE  
//#define   R_CURRENT      GPIO_Pin_3   //右轮点流采样   PC
#define   R_SPEED        GPIO_Pin_4   //右轮速度       PE
#define   R_LIDI         GPIO_Pin_5   //右轮离地开关   PE
//=================下面是显示驱动管脚的定义=====================
#define  LCD_POWER       GPIO_Pin_10	//LCD电源控制   PD
#define  LCD_STB  		 GPIO_Pin_15	//LCD指令控制   PB
#define  LCD_CLK  	 	 GPIO_Pin_8	//LCD时钟       PD
#define  LCD_DATA	  	 GPIO_Pin_9	//LCD数据       PD
#define  LCD_BG 	   	 GPIO_Pin_14	//LCD背光       PD
//=================下面是按键管脚定义=============================
#define  KEY1        	 GPIO_Pin_1 //按键1         PD
#define  KEY2        	 GPIO_Pin_2 //按键1         PD
#define  KEY3         	 GPIO_Pin_3 //按键1         PD
//=================下面是蜂鸣器管脚定义============================
#define  BEEP        	 GPIO_Pin_13   //蜂鸣器     PD

//=================下面是速度检测发射控制管脚定义==================
#define  SPEED_SEND  	 GPIO_Pin_5    //发射灯     PD

//=================下面是地检发射管输出驱动IO脚的定义==============
#define  EARTH_SEND      GPIO_Pin_4   //发射灯     PD

//=================下面是墙检发射管输出驱动IO脚的定义==============
#define  WALL_SEND       GPIO_Pin_12   //发射灯     PA

//=================下面是电池充电输入脚定义=========================
#define  CHARGE_DC   	 GPIO_Pin_12   //DC输入     PD
#define  CHARGE_SEAT 	 GPIO_Pin_11   //回充座输入 PD

//=================下面是中扫输入脚定义==============================
//#define  M_MOTOR_DRIVER  GPIO_Pin_8    //中扫马达控制 PC
#define  M_MOTOR_ZPMOS   GPIO_Pin_10    //中扫马达控制 PB
#define  M_MOTOR_FPMOS   GPIO_Pin_8    //中扫马达控制 PC
#define  M_MOTOR_ZNMOS   GPIO_Pin_14    //中扫马达控制 PE
#define  M_MOTOR_FNMOS   GPIO_Pin_15    //中扫马达控制 PE

//=================下面是边扫输出io脚的定义==========================  
#define  S_MOTOR_DRIVER  GPIO_Pin_8    //边扫马达控制 PA      
#define  S_CURRENT       GPIO_Pin_9    //电流       PC

//=================下面是灰尘盒风机输出IO的定义=======================
#define  DUST_MOTOR_DRIVER  GPIO_Pin_7  //灰尘马达控制         PC

//=================下面是紫外灯输出驱动IO脚的定义=====================
#define  UV_DRIVER       GPIO_Pin_14   //UV输出控制          PB

//=================下面是前轮脉冲检测管脚定义=========================	 
#define  FRONT_WHEEL     GPIO_Pin_5   //脉冲输入          PB

//=================下面是红外接收的管脚定义============================ 
#define  HW_POWER        GPIO_Pin_7   //红外电源输出控制    PE
#define  L_HW            GPIO_Pin_8   //左红外输入          PE
#define  M_HW            GPIO_Pin_9   //中红外输入          PE
#define  R_HW            GPIO_Pin_10  //右红外输入          PE
#define  UP_HW           GPIO_Pin_11  //顶红外输入          PE

//==================下面是碰撞输入的管脚定义=============================	
#define  L_BUMP          GPIO_Pin_12  //左碰撞输入          PE
#define  R_BUMP          GPIO_Pin_13  //右碰撞输入          PE

//==================下面是A7105驱动的管脚定义=============================
#define  SCS             GPIO_Pin_10    //A7105片选信号，PC口。
#define  SCK             GPIO_Pin_11    //A7105时钟信号，PC口。
#define  SDIO            GPIO_Pin_12    //A7105数据信号，PC口。
#define  GIO1            GPIO_Pin_0     //A7105输入信号，PD口。

//==================下面是一些软件定义====================================
#define  BATTERY_VOLTAGE    0   //电池电压通道
#define  BATTERY_TEMP       1   //电池温度通道     //////////////
#define  CHARGE_CURRENT     2   //充电电流通道
#define  E_L                3   //左边地检通道
#define  E_LM               4   //左中地检通道
#define  E_RM               5   //右中地检通道
#define  E_R                6   //右边地检通道
#define  W_L                7   //左边墙检通道    AD转换通道
#define  W_RM               8   //右中墙检通道
#define  W_R                9   //右边墙检通道
#define  M_CURRENT          10  //中扫电流通道
#define  DUST_CURRENT       11  //灰尘电流通道
#define  L_CURRENT          12  //左轮电流通道
#define  R_CURRENT          13  //右轮电流通道
#define  W_LM               14  //左中墙检通道
#define  W_M                15  //中间墙检通道   

//===================下面是电池电压定义======================================
#define NOPOWER             1241   //电池电压11V，认为彻底
#define LOWPOWER            1489   //电压1.1v					
#define FULLPOWER           1895   //电压1.4v
#define ONEPOWER            1624   //电压1.2v
#define TWOPOWER            1760   //电压1.3v
#define FULLPOWER_CHARGE    2058   //充电时的满格电压   1.52V   18.24V 
#define TWOPOWER_CHARGE     1949   //充电时的两格电压   1.44V   17.28V
#define ONEPOWER_CHARGE     1895   //充电时的一格电压   1.40V   16.80V
#define FULLPOWER_CEASE     1692   //待机时的满格电压   1.25V   15.00V
#define TWOPOWER_CEASE      1651   //待机时的两格电压   1.22V   14.64V
#define ONEPOWER_CEASE      1624   //待机时的一格电压   1.20V   14.40V
#define FULLPOWER_WORK      1651   //工作时的满格电压   1.22V   14.64V
#define TWOPOWER_WORK       1624   //工作时的两格电压   1.20V   14.40V
#define ONEPOWER_WORK       1584   //工作时的一格电压   1.17V   14.04V

//=======================下面是红外接收的数据定义========================
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

//============================下面是遥控器的按键定义===============================
#define  YAOKONG_ESC           10	   //退出、取消、关闭电源
#define  YAOKONG_TIMINE_MON	   6	   //定时、星期一
#define  YAOKONG_SETTIME_TUE   9	   //设定时间、星期二
#define  YAOKONG_UV_WED		   3	   //UV、星期三
#define  YAOKONG_AUTO_THU	   1	   //定点清扫、星期四
#define  YAOKONG_SPOT_FRI	   4	   //自动清扫、星期五
#define  YAOKONG_V_SAT		   8	   //速度设定、星期六
#define  YAOKONG_DOCKING_SUN   5	   //回充、星期天
#define  YAOKONG_ED			   2	   //每天
#define  YAOKONG_LEFT		   12	   //向左
#define  YAOKONG_RIGHT		   13	   //向右
#define  YAOKONG_FRONT		   14	   //向前
#define  YAOKONG_BACK		   11	   //向后
#define  YAOKONG_OK			   7	   //确定
                         
//================================下面是地检和墙检的状态定义============================
#define     NEAR            0XFE                              //近
#define     FARN            0XFD                              //远

#define     LEFT             0            //方向是向左
#define     RIGHT            1            //方向是向右

#define     FRONT             0X01                // 向前
#define     BACK              0X02                // 向后

#define     BAD              0X01                //车轮不能动
#define     GOOD             0X00                //车轮正常

#define     LI_ANGLE        0X90         //内墙角
#define     WAI_ANGLE       0XA0         //外墙角
#define     U_ANGLE         0XB0         //U型墙角

#define     POWER_DOWN       0X003        //显示电池没电
#define     POWER_TEMP       0x005        //显示电池温度过高
#define     POWER_NTC        0X006        //电池没有NTC
#define     POWER_CHARGE     0X007        //电池正在充电
#define     CHARGE_ERR       0X009        //充电错误
//=====================================工作模式=================================
#define     CEASE            0X00                //待机模式
#define     SWEEP            0X01                //扫地模式
#define     SPOT             0X02                //定点清扫
#define     TELE             0X03                //遥控模式
#define     DOCKING          0X04                //自动回充模式
#define     CHARGEING        0X05				 //充电模式
#define     SLEEP            0X06                //睡眠模式 
#define     ERR              0X07				 //出错模式
#define     BORDER           0X08				 //??????
//====================================下面是速度的定义=============================
#define     HIGHSPEED        1200                //高速的速度
#define     LOWSPEED         600                 //低速的速度

/*************************************************************
左右轮直径：68mm
传动比  61.6
脉冲数  16
一个脉冲的长度是68*3.14/61.6/16=0.2167mm。
两轮距离  267.6mm。
毛刷   左边82mm  右边  75mm ；  平均毛刷150mm
*************************************************************/
#define     SPEED           1000        // 定义为车轮的最大速度   
#define     FARAWAY         0XFFFFFFFF   //定义为无穷远
#define     LENGTH		    243			 //车轮的宽度  单位是毫米
#define     METE            4615         //1米的脉冲数
#define     METE_H5         2308         //0.5米的距离
#define     METE_H6         2769         //0.6米的距离
#define     METE_H7         3231         //0.7米的距离
#define     METE_H8         3692         //0.8米的距离
#define     METE_H9         4154         //0.9米的距离
#define     METE_TWO		5130         //2米的脉冲数
#define     METE_0			5592		 //1.1米的脉冲数
#define     METE_1          6053         //1.2米的脉冲数	
#define     METE_2			6515		 //1.3米的脉冲数
#define     METE_3			6976		 //1.4米的脉冲数
#define     METE_4			7438		 //1.5米的脉冲数
#define     METE_5			7899		 //1.6米的脉冲数
#define     METE_6			8361		 //1.7米的脉冲数
#define     METE_7			8822		 //1.8米的脉冲数
#define     METE_8			9384		 //1.9米的脉冲数
#define     METE_THREE		13845		 //3米的脉冲数
//=================================定义PID算法的系数========================
#define     KP              4     //8
#define     KI              10    //30
#define     KD              80    //150

#define   radii    3880            //定义为绕中心点一圈的脉冲数
#define   sys_err_p  10			   //正向系统误差
#define   sys_err_n  20			   //反向系统误差
#define   angle5   81
#define   angle15  162			   //旋转15度	   
#define   angle20  200			   //旋转20度
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
//=============================一些宏函数定义==================================
#define   SUBBTIME(t1,t2)   ((t1>t2)?(0xffffffff-t1+t2):(t2-t1))    
#define   DELAY(a)          while(a--);          

typedef struct 			    //按键的结构
{
     uint8 key;		        //键值,当前状态
     uint8 number;          //去抖次数
     uint8 (*read_start)(); //读取得状态函数
     uint8 sign;	        //标志  表示该按键有效，没有被处理
}KEY;

typedef struct 			  //遥控器
{
    uint32 effect_time;   //按键时间
	uint8 old_key;        //旧按键
    uint8 effect;         //按键有效时间
    uint8 key;		      //键值
    uint8 sign;	          //有按键 
	uint8 test;			  //测试的遥控器标志  0表示自己的遥控器；1表示信号是由编码为FFFF的遥控器发出的信号
}YAOKONG;

typedef struct 			  //光敏电阻值的结构
{
 bool uv;		 
 uint8 work_mode;	 
 uint8 speed;
}WORK_MODE;
typedef struct
{
 uint32 time;  //采样垃圾检测的时间    
 uint8  odds;  //判断垃圾的标准
 uint32 samp;  //在一段时间内垃圾脉冲的个数。
 uint8  step;  //垃圾检测的过程步骤
 uint8  sign;  //垃圾检测的标志 0表示垃圾少,1表示垃圾多
}MUCK;
typedef struct             //与串口通讯的数据结构
{
  uint8  rcv_buff[12];	   //接收缓冲
  uint8  rcv_number;	   //接收数据个数
  uint32  time ;		   //发送时的系统时钟
  uint8  send_buff[12];	   //发送缓冲
  uint8  send_number;	   //发送的字结数
  uint8  send_t;		   //当条数据已经发送的次数
  uint8  send;			   //发送数据的标志
} UART;
typedef struct             //墙检地检的数据结构
{
  uint16 brightness;	   //亮发射管时采样的值
  uint16 darkness;         //暗发射管时采样的值
  uint8  near;	           //近地次数
  uint8  farn;	           //远地次数
  uint16 difference;       //测量的反射值
  uint32 low_door;         //门限值	 
  uint32 high_door;        //门限值
  uint8  sign;	           //本灯的标志
  uint8  high_sign;	       //本灯的标志	
  uint8  high_near;	       //近地次数
  uint8  high_farn;	       //远地次数
  uint8  on;               //增加墙检有效标志。0为无效，1为有效
}WALL_DATA;

typedef struct             //墙检地检的数据结构
{
  uint8  (*read_io)();
  uint8   data;	           //红外接收的数据
  uint32  time;	           //接收开始的时间
  uint8   tempdata;        //临时接收的数据
  uint8   tempbit ;	       //采样的当前位的值
  uint8   bitnumber;	   //红外接收位
  uint8   start;           //红外信号数据开始标志
volatile    uint32   sign;	      //红外接收到的标志
volatile    uint32  effect_time;  //红外有效时间
volatile    uint32  effect;       //红外有效
}INFR_DATA;

typedef struct                    //车轮速度的数据结构
{ 
  int32  ek[3];                   //最近的三次系统速度偏差
  volatile    uint32   length;	   //车轮所走的脉冲数
  volatile    uint32   dq_time;      //轮子当前时间，在下降沿的时间
  uint32  rising_time;               //当前测到的上升沿时间
  uint32  old_time;                  //调整周期前的时间
  uint8  state;                      //车轮状态  BAD;车轮不动 GOOD车轮正常
  uint32  odds;                      //车轮停下转动的时间次数

//  uint16 rising_captime;           //当前测到的上升沿时间
//  uint32 rising_dqtime;            //确定测到的上升沿时间
//  uint16 rising_dqcaptime;         //确定测到的上升沿时间
//  volatile    uint16   dq_captime; //轮子捕获的定时器计数，在下降沿的时间    
//  uint32 falling_time;             //当前测到的下降沿时间
//  uint16 falling_captime;          //当前测到的下降沿时间
//  uint16  old_captime;
}RING;
typedef struct	                  //车轮的速度信息
{
uint8    master;                  //主动轮标志   ；1为主动轮，0为从动轮
uint8    old_time;                //测速的起始时间
uint8    ori   ;                  //车轮的前后方向         
uint16   pwm   ;                  //车轮的pwm值
int32    rap  ;                   //车轮的速度，表示为每秒的脉冲数
uint32   length  ;                //车轮应当走过的距离
uint32   time   ;                 //测量速度的时间
uint32   pulse  ;                 //测量速度用的脉冲数
uint8    sign   ;                 //标志 0:车轮停止 1:车轮启动，方式为到点停车 2:阶段到点 3:车轮启动，到点不停
}RAP;
typedef  struct
{
uint32 temp   ;	     //电池温度 
uint32 voltage;		 //电池电压   
uint32 current;      //充电电流
uint8  charge_dc ;	 //外部DC座输入信号
uint8  charge_seat ; //外部充电座输入信号
uint16 pwm;         //充电PWM脉冲
uint8  err;         //电池错误  0 没有错误，1没有NTC，2 NTC短路，3 没有电池， 4 电池短路
uint8 step;         //充电步骤  0 小电流  1 大电流  2维持电流
uint32 time;        //充电开始时间
}POWER;

typedef  struct
{
uint8 data;          //开关信号的数据 0:表示没有动作 1:表示其已经动作
uint32 time;         //读取开关信号的时间
}SWITCH;

typedef  struct
{
uint32 current;	       //当前采样的电流值
uint32 old_current;    //前一次的采样电流
uint32 sum_current;    //电流的和
uint32 linshi_current; //临时存放的采样电流值
uint32 temp;	       //电流的标准值
uint32 time;		   //采样时间
uint32 typo;		   //采样超标次数
uint8  sign;		   //电流标志         0:表示未超标 1:表示超标
uint8 sign_l;
}CURRENT;

typedef  struct
{
uint32 * music;	    //需要播放的音乐段
uint32 temp;	    //电流的标准值
uint32 time;		//音乐播放的时间
uint32 typo;		//音乐播放的起始位置
volatile  uint8  sign;		//信号标志   0:表示不唱 1:表示唱
}mySOUND;

typedef struct	    //机器坐标参数
{
uint32 x;		    //机器的x坐标
uint32 y;			//机器的y坐标
uint32 angle;		//机器的角度
}POSITION;

typedef struct		//机器系统的工作状态
{
 uint8  mode ;	    //机器的运行模式  0：静止模式；1：扫地模式；2：自动回充模式；3；遥控模式	4:出错模式
 uint32 action;		//动作   0:停止  1:原地左转  2:原地右转 3:前进   4:后退   5:旋转1  6：旋转2  。。。18旋转14	，19走螺旋线
 uint8  fangxiang;  // 0 左转； 1右转
 uint8  sign;       //动作标志 
 uint8  *road;		//路径指针

 uint32 time;       //动作的起始时间
 uint32 run_time;   //扫地时需要运行时间
 uint8  step;		//路径执行的步骤
 uint32  abnormity; //异常号码  0表示没有异常
 uint8 step_abn;    //异常时的步骤
 uint32 bump ;      //碰撞的标志
 uint8  step_bp;    //碰撞时的步骤标志
 uint8  times_bp;   //碰撞的次数
 uint32 angle_wall; //墙检的角度
 uint32 long_wall;  //墙检的距离
 uint8  start_wall; //墙检开始
 uint8  muck;       //多垃圾清扫路径开始
 uint8  step_mk;    //多垃圾清扫步骤
 uint8  times_mk;   //垃圾检测时连续左转次数
 uint8  ybs_sign;   //沿边扫标志
 uint8  step_yaokong; // 在遥控时的步骤
 uint8 sum_bump;    //在累计的碰撞次数。由ACTION_BUMP()函数和DO_SWEEP()函数修改和使用。
}MODE;
typedef struct
{
  uint8 step;	    //步骤
  uint8 sign;	    //开始标志   0表示没有开始，1表示已经开始
  uint32 time ;     //动作开始的时间
  uint32 length ;   //长度或者所转圈数
}ACTION;

typedef	 union		 //显示数据
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

