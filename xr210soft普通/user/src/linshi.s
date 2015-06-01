
;********************************************************************************
;                        M-298  sbs   回冲吸尘器	   			*
;										*
;			版本号：3.0						*
;			
;		        程序员：张建中  TEL:13923460181 			*
;			2006年7月20日						*
;********************************************************************************
;比2.0增加充电快冲，与258通讯



;比1。0增加遥控时无电碰撞转弯功能
;PCB板,为m-298;CDS短路(光亮)为遥控,CDS开路(黑暗)为回冲

;主要功能
;   	1、可靠、安全开机
;   	2、开机音乐、关机音乐
;	3、左右轮电机正反转，高、低电压电机转速一致
;	4、各电机垛转保护
;	5、红外地检、墙检
;	6、红外左右碰撞
;	7、左右轮转速检测
;	8、离地开关检测
;	9、虚拟墙（红外500Hz信号，38KHz载波）
;	10、红外遥控器ht48r6
;	11、自动回冲
;测试搜寻功能，2组码1111100，1111010,7位数据
;********************************************************************************
;芯片W78E54
;晶振30MHz,12分频
;马达控制全部是低电平
;********************************************************************************
;主要子程序
;    	音乐子程序
;    	各电机垛转检测子程序
;    	左右轮转速检测子程序
;	红外地检、墙检
;	红外左右碰撞
;	离地开关检测
;	虚拟墙（红外500Hz信号，38KHz载波）
;	按键检测子程序
;	led显示子程序;
;	数据串并输入子程序
;---------------------------------------------------------------------------

ON		EQU	20H.0;		机器开
SPOT		EQU	20H.1;		工作状态SPOT,定点清扫
CLEAN		EQU	20H.2;		工作状态CLEAN,30分钟清扫
MAX		EQU	20H.3;		工作状态MAX,最大时间清扫

;---------------------------------------------------------------------------
ERR		EQU	21H.0;
M_PROT		EQU	21H.1;		马达保护
BAT_R		EQU	21H.2;		红色电源LED点亮
BAT_Y		EQU	21H.3;		黄色电源LED点亮;


path		equ	21h.4;		碰撞结束后，新路径不能是转弯路径
KS		EQU	21H.5;		键释放标志
XUNIQ		EQU	21H.6;

;IR_R		EQU	21H.7;		右地检测试标志
REM_START	EQU	21H.7;		遥控接受



;---------------------------------------------------------------------------
IR_GO		EQU	22H.0;		红外的前后左右功能
IR_B		EQU	22H.1;		后退
IR_F		EQU	22H.2;		前进		
IR_ZY		EQU	22H.3;		左右
KS_IR		EQU	22H.4;		红外健释放

CHAR		EQU	22H.5;
CHAR_END	EQU	22H.6;
CHAR_EN		EQU	22H.7;



;T1用了23H和30H、31、32H；
;---------------------------------------------------------------------------

V_TEST_1S	EQU	23H.0;		转速可以检测标志位
M_STAT		EQU	23H.1;		主马达转速测试
M_L_STAT	EQU	23H.2;		左马达前一次状态
M_R_STAT	EQU	23H.3;		右马达前一次状态
M_MAIN		EQU	23H.4;		主马达定义，调速时转速不变

BAT_R_BAK	EQU	23H.5;		电池不够电开始4分钟计数标志

FULL_GO		EQU	23H.6;
NOW_GO		EQU	23H.7;		现GO

;---------------------------------------------------------------------------
M_DIR		EQU	24H;		马达方向
M_L_Z		EQU	24H.0	;	L马达正转标志，1=正转、0=不转
M_R_Z		EQU	24H.1	;	R马达正转标志，1=正转、0=不转
M_L_F		EQU	24H.2	;	L马达反转标志，1=反转、0=不转
M_R_F		EQU	24H.3	;	R马达反转标志，1=反转、0=不转



;---------------------------------------------------------------------------
DIST_BIG	EQU	25H.0;		距离到，判断标志
CH		EQU	25H.1;		路径改变标志
LD_L		EQU	25H.2;		先离地是左边，直线行走1M后清除
LD_R		EQU	25H.3;		先离地是左边，直线行走1M后清除
	
				;只要有效不管左、右离地均一个方向
;----------------------------------------------------------------------------

ANO_BAK		EQU	26H;
XUNIQ_BAK	EQU	26H.0;
IR_R_BAK	EQU	26H.1;		右地检
IR_RM_BAK	EQU	26H.3;		右中地检
IR_LM_BAK	EQU	26H.6;		左中地检
IR_L_BAK	EQU	26H.7;		左地检

PUSH_L_BAK	EQU	26H.4;
PUSH_R_BAK	EQU	26H.5;
WALL_BAK	EQU	26H.2;
;---------------------------------------------------------------------------

YICH		EQU	27H;
YICH1		EQU	27H.0;		旋转
YICH2		EQU	27H.1;		后退；
YICH3		EQU	27H.2;		旋转
YICH4		EQU	27H.3;		前进1
YICH5		EQU	27H.4;		

ANO		EQU	27H.6;		帮助异常判断是否需要停2MS
ANO_DO		EQU	27H.7;		退后到异常消除后的处理

;---------------------------------------------------------------------------
TEST_REG	EQU	28H;	
	
QIANGJIAN	EQU	28H;
Q0		EQU	28H.0;		原地转圈
Q1		EQU	28H.1;		前进100个脉冲
Q2		EQU	28H.2;		小半径转圈
Q3		EQU	28H.3;		1轮不转
Q4		EQU	28H.4;
Q5		EQU	28H.5;
Q6		EQU	28H.6;
QIANGJ		EQU	28H.7;		 要进行墙柱检查标注


;----------------------------------------------------------------------------
PS_DATA		EQU	29H;		串并口数据
SWITCH		EQU	29H.0;
TEMP		EQU	29H.1;
HI_VOL		EQU	29H.2;
EXT_VOL		EQU	29H.3;
IO_M_PROT	EQU	29H.4;
KEY_S		EQU	29H.5;
KEY_M		EQU	29H.6;
KEY_L		EQU	29H.7;

;-------------------------------------------------------------


WORK		EQU	2AH.0;		工作/休眠标志，1=工作    0=休眠
P_MODE		EQU	2AH.1;		SPOT MODE
OFF_ON		EQU	2AH.2;		马达启动标志

test_zjz	equ	2ah.3
;DOLL_180	EQU	2AH.4;		第一次搜寻无信号时，转180度


DOLL_Z		EQU	2BH.2;		检测到的信号
DOLL_Y		EQU	2BH.3;
DOLL_M		EQU	2BH.4;
doll_c		equ	2bh.5;
DOLL_ALL	EQU	2BH.6;		能收到DOLL的信号
DOLL_CD		EQU	2BH.7;		充电由DOLL提供
;---------------------------------
DOLL_ST1	EQU	2CH;
DOLL		EQU	2CH.0;
DOLL_FIND	EQU	2CH.1;		已经发现DOLL的位置
DOLL_OVER1	EQU	2CH.2;		0.3S内无DOLL信号
DOLL_A_F	EQU	2CH.3;
doll_over	equ	2Ch.4;		走路摆动到3次，没发现
DOLL_Z_F	EQU	2CH.5;		已经检测到的信号
DOLL_Y_F	EQU	2CH.6;
DOLL_F		EQU	2CH.7;		该圈内有DOL...,,,
         
;---------------------------------
DOLL_ST		EQU	2DH;
DOLL0		EQU	2DH.0;
DOLL1		EQU	2DH.1;
DOLL2		EQU	2DH.2;
DOLL3		EQU	2DH.3;
DOLL4		EQU	2DH.4;
DOLL5		EQU	2DH.5;
DOLL6		EQU	2DH.6;
DOLL7		EQU	2DH.7;


;-------------------------------------------------------------


PROG		EQU	2EH;		程序数据
LINE		EQU	2EH.7;		直线标志	


;-------------------------------------------------------------

REMOTE		EQU	2FH.0;		是遥控还是回冲器

F_T_STAT	EQU	2FH.1;		前轮工作状态
F_EN		EQU	2FH.2;		前轮有转速
F_ALM		EQU	2FH.3;		前轮报警
F_ALM_BAK	EQU	2FH.4;

WALL_DET_DIS	EQU	2FH.5;		墙检无效标志

BACK		EQU	2fH.6;		机器后退标志

TIME_3M		EQU	2fH.7;		3M搜寻计数


;==============================================================
;===============================================================

T2MS		EQU	30H;		PWM的2MS周期计数
T_L_M		EQU	31H;		左轮打开时间
T_R_M		EQU	32H;		右轮打开时间

DET_L		EQU	33H;
DET_R		EQU	34H;

DIST_L_H	EQU	35H;		计数左马达脉冲个数
DIST_L_L	EQU	36H;
DIST_R_H	EQU	37H;		计数右马达脉冲个数
DIST_R_L	EQU	38H;


CIRCLE		EQU	39H;		运行第几号圈
SUDU		EQU	3AH;
M_ANGLE_H	EQU	3BH;		机器转的角度
M_ANGLE_L	EQU	3CH;
IR_G_SEND	EQU	3DH;		红外发射
IRKEY		EQU	3EH;
IRKEY_BAK	EQU	3FH;

DPTR_PROG	EQU	40H;		主程序指针	
DPTR_XUAN	EQU	41H;		虚拟墙/离地旋转角度指针
DPTR_LINE	EQU	42H;		碰撞旋转角度指针
DPTR_SPOT	EQU	43H;		SPOT路径
MUSIC		EQU	44H;		音乐
P0_BAK		EQU	45H;		P0备份



CNTSUDU1	EQU	48H;
CNTSUDU2	EQU	49H;
DET_M_L		EQU	4AH;		主马达脉冲计数
DET_M_R		EQU	4BH;		主马达脉冲计数
PWM		EQU	4CH;		主动轮的转速		
CNT_IR		EQU	4DH;		红外遥控108MS计数器
;LX_IR		EQU	4EH;		测试IR_G
dir_bak		equ	4Fh;		方向备份


DET_QJ_L	EQU	50H;
DET_QJ_R	EQU	51H;

ANG_H		EQU	52H;		左轮角度
ANG_L		EQU	53H;		右轮角度



CNT_ALARM	EQU	60H;
CNT_RED_H	EQU	61H;		电平反转后延时4分钟
CNT_RED_L	EQU	62H;
CNT_DOLL	EQU	63H;


CNT_LOWBAT	EQU	64H;
CNT_F		EQU	65H;		前轮转速计数
CNT_F_EN	EQU	66H;		前轮转速有效计数

T_PS		EQU	70H;


T_COM1		EQU	72H;		公用

T_CD		EQU	73H;		充电时间计数器
T_CD1		EQU	74H
T_CD2		EQU	75H;
T_PROT		EQU	76H;
T_DLY		EQU	77H;
T_SONG		EQU	78H;
T_SONG1		EQU	79H;



CNT_CLEAN1	EQU	7AH;
CNT_CLEAN2	EQU	7BH;

T_PINBI		equ	7Ch;
T_INI		EQU	7DH;
DIST_Q_H	EQU	7EH;
DIST_Q_L	EQU	7FH;
;===============================================================================
;===============================================================================
;===============================================================================
;数据		数据		数据		数据		数据
;===============================================================================
;===============================================================================
T100US_H	EQU	0FFH
T100US_L	EQU	00;		170;
T10MS_H		EQU	00;		0CBH;		定时10MS内有主马达检测脉冲
T10MS_L		EQU	00;		0EAH;

PWM100		EQU	20;		PWM的比例
PWM25		EQU	5;

DATA_ANGLE	EQU	100;		主动轮100个脉冲，从动轮变化

ANG_LDFH	equ	01h;		无地检时，转的脉冲个数
ANG_LDFL	equ	00h;
ANG_LDH		equ	03h;		无地检时，转的脉冲个数
ANG_LDL		equ	00h;
ANG_XNH		equ	08H;06h;		碰到虚拟墙时，转的脉冲个数
ANG_XNL		equ	0C0H;90h;

ANG_QJH		EQU	01H;	墙检角度
ANG_QJL		EQU	30H;

ANG_90QJH	EQU	08H;
ANG_90QJL	EQU	1CH;

ANG_DOLLH	EQU	00H;
ANG_DOLLL	EQU	60H;

ANG_30_DOLLH	EQU	01H	
ANG_30_DOLLL	EQU	0D0H;


ANG_15H		EQU	01H;
ANG_15L		EQU	18H;

ANG_30H		EQU	02H;
ANG_30L		EQU	30H;

ANG_45H		EQU	03H;
ANG_45L		EQU	48H;

ANG_60H		EQU	04H;
ANG_60L		EQU	60H;

ANG_75H		EQU	05H;
ANG_75L		EQU	78H;

ANG_90H		EQU	06H;
ANG_90L		EQU	0ECH;

ANG_105H	EQU	07H;
ANG_105L	EQU	0A8H;

ANG_120H	EQU	08H;
ANG_120L	EQU	0C0H;

ANG_135H	EQU	09H;
ANG_135L	EQU	0D8H;

ANG_180H	EQU	0DH;
ANG_180L	EQU	0D8H;

ang_200h	equ	10h;
ang_200l	equ	80h;

ANG_270H	EQU	14H
ANG_270L	EQU	0C4H

ANG_360H	EQU	1BH;
ANG_360L	EQU	0B0H;

ANG_800H	EQU	30H;
ANG_800L	EQU	0B0H;

D_Q1_L_H	EQU	02H;	大房间4.6米x6米，需扫63.6米=2532个100脉冲	
D_Q1_L_L	EQU	79H;

D_Q1_M_H	EQU	08H;	中房间4.3米x4.9米，需扫55.2米=2198个100脉冲	
D_Q1_M_L	EQU	96H;

D_Q1_S_H	EQU	06H;	小房间3米x3.7米，需扫40.2米=1600个100脉冲	
D_Q1_S_L	EQU	40H;

D_Q2_L_H	EQU	09H;	大房间4.6米x6米，需扫63.6米=2532个100脉冲/4	
D_Q2_L_L	EQU	0E4H;

D_Q2_M_H	EQU	02H;	中房间4.3米x4.9米，需扫55.2米=2198个100脉冲/4	
D_Q2_M_L	EQU	25H;

D_Q2_S_H	EQU	01H;	小房间3米x3.7米，需扫40.2米=1600个100脉冲/4	
D_Q2_S_L	EQU	90H;

D_Q_H		EQU	01;	非原地9圈的转圈路径，2米距离
D_Q_L		EQU	80;


DATA_01MH	EQU	01H;
DATA_01ML	EQU	20H;

DATA_1MH	EQU	03H;		0.2M距离的脉冲个数
DATA_1ML	EQU	1CH;

DATA_2MH	EQU	20H;		2M距离的脉冲个数
DATA_2ML	EQU	1CH;

DATA_XNH	EQU	03H;
DATA_XNL	EQU	00H;

;************************************************************************************		


;==========================================================================================
		ORG	0000H;
                AJMP	MAIN;		
																				SJMP	MAIN;
		ORG	0003H;		外部中断0
		LJMP	EXINT0;
		NOP;
		ORG	000BH;
		SJMP	MAIN;		定时器0
		NOP;
		NOP;
		ORG	0013H;
		JMP	MAIN;		外部中断1
		NOP	;
		NOP;
		ORG	001BH;
		LJMP	TIMER1;		定时器1
		NOP;
		ORG	0023H;
		JMP	MAIN;		串行口
		NOP	;
		NOP;
		ORG	002BH;
		JMP	MAIN;		定时器2或T2EX负跳变

		ORG	0030H
MAIN:
