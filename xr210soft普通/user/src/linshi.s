
;********************************************************************************
;                        M-298  sbs   �س�������	   			*
;										*
;			�汾�ţ�3.0						*
;			
;		        ����Ա���Ž���  TEL:13923460181 			*
;			2006��7��20��						*
;********************************************************************************
;��2.0���ӳ���壬��258ͨѶ



;��1��0����ң��ʱ�޵���ײת�书��
;PCB��,Ϊm-298;CDS��·(����)Ϊң��,CDS��·(�ڰ�)Ϊ�س�

;��Ҫ����
;   	1���ɿ�����ȫ����
;   	2���������֡��ػ�����
;	3�������ֵ������ת���ߡ��͵�ѹ���ת��һ��
;	4���������ת����
;	5������ؼ졢ǽ��
;	6������������ײ
;	7��������ת�ټ��
;	8����ؿ��ؼ��
;	9������ǽ������500Hz�źţ�38KHz�ز���
;	10������ң����ht48r6
;	11���Զ��س�
;������Ѱ���ܣ�2����1111100��1111010,7λ����
;********************************************************************************
;оƬW78E54
;����30MHz,12��Ƶ
;������ȫ���ǵ͵�ƽ
;********************************************************************************
;��Ҫ�ӳ���
;    	�����ӳ���
;    	�������ת����ӳ���
;    	������ת�ټ���ӳ���
;	����ؼ졢ǽ��
;	����������ײ
;	��ؿ��ؼ��
;	����ǽ������500Hz�źţ�38KHz�ز���
;	��������ӳ���
;	led��ʾ�ӳ���;
;	���ݴ��������ӳ���
;---------------------------------------------------------------------------

ON		EQU	20H.0;		������
SPOT		EQU	20H.1;		����״̬SPOT,������ɨ
CLEAN		EQU	20H.2;		����״̬CLEAN,30������ɨ
MAX		EQU	20H.3;		����״̬MAX,���ʱ����ɨ

;---------------------------------------------------------------------------
ERR		EQU	21H.0;
M_PROT		EQU	21H.1;		��ﱣ��
BAT_R		EQU	21H.2;		��ɫ��ԴLED����
BAT_Y		EQU	21H.3;		��ɫ��ԴLED����;


path		equ	21h.4;		��ײ��������·��������ת��·��
KS		EQU	21H.5;		���ͷű�־
XUNIQ		EQU	21H.6;

;IR_R		EQU	21H.7;		�ҵؼ���Ա�־
REM_START	EQU	21H.7;		ң�ؽ���



;---------------------------------------------------------------------------
IR_GO		EQU	22H.0;		�����ǰ�����ҹ���
IR_B		EQU	22H.1;		����
IR_F		EQU	22H.2;		ǰ��		
IR_ZY		EQU	22H.3;		����
KS_IR		EQU	22H.4;		���⽡�ͷ�

CHAR		EQU	22H.5;
CHAR_END	EQU	22H.6;
CHAR_EN		EQU	22H.7;



;T1����23H��30H��31��32H��
;---------------------------------------------------------------------------

V_TEST_1S	EQU	23H.0;		ת�ٿ��Լ���־λ
M_STAT		EQU	23H.1;		�����ת�ٲ���
M_L_STAT	EQU	23H.2;		�����ǰһ��״̬
M_R_STAT	EQU	23H.3;		�����ǰһ��״̬
M_MAIN		EQU	23H.4;		����ﶨ�壬����ʱת�ٲ���

BAT_R_BAK	EQU	23H.5;		��ز����翪ʼ4���Ӽ�����־

FULL_GO		EQU	23H.6;
NOW_GO		EQU	23H.7;		��GO

;---------------------------------------------------------------------------
M_DIR		EQU	24H;		��﷽��
M_L_Z		EQU	24H.0	;	L�����ת��־��1=��ת��0=��ת
M_R_Z		EQU	24H.1	;	R�����ת��־��1=��ת��0=��ת
M_L_F		EQU	24H.2	;	L��ﷴת��־��1=��ת��0=��ת
M_R_F		EQU	24H.3	;	R��ﷴת��־��1=��ת��0=��ת



;---------------------------------------------------------------------------
DIST_BIG	EQU	25H.0;		���뵽���жϱ�־
CH		EQU	25H.1;		·���ı��־
LD_L		EQU	25H.2;		���������ߣ�ֱ������1M�����
LD_R		EQU	25H.3;		���������ߣ�ֱ������1M�����
	
				;ֻҪ��Ч����������ؾ�һ������
;----------------------------------------------------------------------------

ANO_BAK		EQU	26H;
XUNIQ_BAK	EQU	26H.0;
IR_R_BAK	EQU	26H.1;		�ҵؼ�
IR_RM_BAK	EQU	26H.3;		���еؼ�
IR_LM_BAK	EQU	26H.6;		���еؼ�
IR_L_BAK	EQU	26H.7;		��ؼ�

PUSH_L_BAK	EQU	26H.4;
PUSH_R_BAK	EQU	26H.5;
WALL_BAK	EQU	26H.2;
;---------------------------------------------------------------------------

YICH		EQU	27H;
YICH1		EQU	27H.0;		��ת
YICH2		EQU	27H.1;		���ˣ�
YICH3		EQU	27H.2;		��ת
YICH4		EQU	27H.3;		ǰ��1
YICH5		EQU	27H.4;		

ANO		EQU	27H.6;		�����쳣�ж��Ƿ���Ҫͣ2MS
ANO_DO		EQU	27H.7;		�˺��쳣������Ĵ���

;---------------------------------------------------------------------------
TEST_REG	EQU	28H;	
	
QIANGJIAN	EQU	28H;
Q0		EQU	28H.0;		ԭ��תȦ
Q1		EQU	28H.1;		ǰ��100������
Q2		EQU	28H.2;		С�뾶תȦ
Q3		EQU	28H.3;		1�ֲ�ת
Q4		EQU	28H.4;
Q5		EQU	28H.5;
Q6		EQU	28H.6;
QIANGJ		EQU	28H.7;		 Ҫ����ǽ������ע


;----------------------------------------------------------------------------
PS_DATA		EQU	29H;		����������
SWITCH		EQU	29H.0;
TEMP		EQU	29H.1;
HI_VOL		EQU	29H.2;
EXT_VOL		EQU	29H.3;
IO_M_PROT	EQU	29H.4;
KEY_S		EQU	29H.5;
KEY_M		EQU	29H.6;
KEY_L		EQU	29H.7;

;-------------------------------------------------------------


WORK		EQU	2AH.0;		����/���߱�־��1=����    0=����
P_MODE		EQU	2AH.1;		SPOT MODE
OFF_ON		EQU	2AH.2;		���������־

test_zjz	equ	2ah.3
;DOLL_180	EQU	2AH.4;		��һ����Ѱ���ź�ʱ��ת180��


DOLL_Z		EQU	2BH.2;		��⵽���ź�
DOLL_Y		EQU	2BH.3;
DOLL_M		EQU	2BH.4;
doll_c		equ	2bh.5;
DOLL_ALL	EQU	2BH.6;		���յ�DOLL���ź�
DOLL_CD		EQU	2BH.7;		�����DOLL�ṩ
;---------------------------------
DOLL_ST1	EQU	2CH;
DOLL		EQU	2CH.0;
DOLL_FIND	EQU	2CH.1;		�Ѿ�����DOLL��λ��
DOLL_OVER1	EQU	2CH.2;		0.3S����DOLL�ź�
DOLL_A_F	EQU	2CH.3;
doll_over	equ	2Ch.4;		��·�ڶ���3�Σ�û����
DOLL_Z_F	EQU	2CH.5;		�Ѿ���⵽���ź�
DOLL_Y_F	EQU	2CH.6;
DOLL_F		EQU	2CH.7;		��Ȧ����DOL...,,,
         
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


PROG		EQU	2EH;		��������
LINE		EQU	2EH.7;		ֱ�߱�־	


;-------------------------------------------------------------

REMOTE		EQU	2FH.0;		��ң�ػ��ǻس���

F_T_STAT	EQU	2FH.1;		ǰ�ֹ���״̬
F_EN		EQU	2FH.2;		ǰ����ת��
F_ALM		EQU	2FH.3;		ǰ�ֱ���
F_ALM_BAK	EQU	2FH.4;

WALL_DET_DIS	EQU	2FH.5;		ǽ����Ч��־

BACK		EQU	2fH.6;		�������˱�־

TIME_3M		EQU	2fH.7;		3M��Ѱ����


;==============================================================
;===============================================================

T2MS		EQU	30H;		PWM��2MS���ڼ���
T_L_M		EQU	31H;		���ִ�ʱ��
T_R_M		EQU	32H;		���ִ�ʱ��

DET_L		EQU	33H;
DET_R		EQU	34H;

DIST_L_H	EQU	35H;		����������������
DIST_L_L	EQU	36H;
DIST_R_H	EQU	37H;		����������������
DIST_R_L	EQU	38H;


CIRCLE		EQU	39H;		���еڼ���Ȧ
SUDU		EQU	3AH;
M_ANGLE_H	EQU	3BH;		����ת�ĽǶ�
M_ANGLE_L	EQU	3CH;
IR_G_SEND	EQU	3DH;		���ⷢ��
IRKEY		EQU	3EH;
IRKEY_BAK	EQU	3FH;

DPTR_PROG	EQU	40H;		������ָ��	
DPTR_XUAN	EQU	41H;		����ǽ/�����ת�Ƕ�ָ��
DPTR_LINE	EQU	42H;		��ײ��ת�Ƕ�ָ��
DPTR_SPOT	EQU	43H;		SPOT·��
MUSIC		EQU	44H;		����
P0_BAK		EQU	45H;		P0����



CNTSUDU1	EQU	48H;
CNTSUDU2	EQU	49H;
DET_M_L		EQU	4AH;		������������
DET_M_R		EQU	4BH;		������������
PWM		EQU	4CH;		�����ֵ�ת��		
CNT_IR		EQU	4DH;		����ң��108MS������
;LX_IR		EQU	4EH;		����IR_G
dir_bak		equ	4Fh;		���򱸷�


DET_QJ_L	EQU	50H;
DET_QJ_R	EQU	51H;

ANG_H		EQU	52H;		���ֽǶ�
ANG_L		EQU	53H;		���ֽǶ�



CNT_ALARM	EQU	60H;
CNT_RED_H	EQU	61H;		��ƽ��ת����ʱ4����
CNT_RED_L	EQU	62H;
CNT_DOLL	EQU	63H;


CNT_LOWBAT	EQU	64H;
CNT_F		EQU	65H;		ǰ��ת�ټ���
CNT_F_EN	EQU	66H;		ǰ��ת����Ч����

T_PS		EQU	70H;


T_COM1		EQU	72H;		����

T_CD		EQU	73H;		���ʱ�������
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
;����		����		����		����		����
;===============================================================================
;===============================================================================
T100US_H	EQU	0FFH
T100US_L	EQU	00;		170;
T10MS_H		EQU	00;		0CBH;		��ʱ10MS���������������
T10MS_L		EQU	00;		0EAH;

PWM100		EQU	20;		PWM�ı���
PWM25		EQU	5;

DATA_ANGLE	EQU	100;		������100�����壬�Ӷ��ֱ仯

ANG_LDFH	equ	01h;		�޵ؼ�ʱ��ת���������
ANG_LDFL	equ	00h;
ANG_LDH		equ	03h;		�޵ؼ�ʱ��ת���������
ANG_LDL		equ	00h;
ANG_XNH		equ	08H;06h;		��������ǽʱ��ת���������
ANG_XNL		equ	0C0H;90h;

ANG_QJH		EQU	01H;	ǽ��Ƕ�
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

D_Q1_L_H	EQU	02H;	�󷿼�4.6��x6�ף���ɨ63.6��=2532��100����	
D_Q1_L_L	EQU	79H;

D_Q1_M_H	EQU	08H;	�з���4.3��x4.9�ף���ɨ55.2��=2198��100����	
D_Q1_M_L	EQU	96H;

D_Q1_S_H	EQU	06H;	С����3��x3.7�ף���ɨ40.2��=1600��100����	
D_Q1_S_L	EQU	40H;

D_Q2_L_H	EQU	09H;	�󷿼�4.6��x6�ף���ɨ63.6��=2532��100����/4	
D_Q2_L_L	EQU	0E4H;

D_Q2_M_H	EQU	02H;	�з���4.3��x4.9�ף���ɨ55.2��=2198��100����/4	
D_Q2_M_L	EQU	25H;

D_Q2_S_H	EQU	01H;	С����3��x3.7�ף���ɨ40.2��=1600��100����/4	
D_Q2_S_L	EQU	90H;

D_Q_H		EQU	01;	��ԭ��9Ȧ��תȦ·����2�׾���
D_Q_L		EQU	80;


DATA_01MH	EQU	01H;
DATA_01ML	EQU	20H;

DATA_1MH	EQU	03H;		0.2M������������
DATA_1ML	EQU	1CH;

DATA_2MH	EQU	20H;		2M������������
DATA_2ML	EQU	1CH;

DATA_XNH	EQU	03H;
DATA_XNL	EQU	00H;

;************************************************************************************		


;==========================================================================================
		ORG	0000H;
                AJMP	MAIN;		
																				SJMP	MAIN;
		ORG	0003H;		�ⲿ�ж�0
		LJMP	EXINT0;
		NOP;
		ORG	000BH;
		SJMP	MAIN;		��ʱ��0
		NOP;
		NOP;
		ORG	0013H;
		JMP	MAIN;		�ⲿ�ж�1
		NOP	;
		NOP;
		ORG	001BH;
		LJMP	TIMER1;		��ʱ��1
		NOP;
		ORG	0023H;
		JMP	MAIN;		���п�
		NOP	;
		NOP;
		ORG	002BH;
		JMP	MAIN;		��ʱ��2��T2EX������

		ORG	0030H
MAIN:
