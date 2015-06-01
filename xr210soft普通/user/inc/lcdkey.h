#ifndef __LCDKEY_H
#define __LCDKEY_H
/////////////////*****INCLUDE *************************
#include "stm32f10x_lib.h"

////////////////ȫ�ֱ�������//////////////////////////
extern bool Dis_On;            //��ʾ��־��
extern bool Dis_ErrOn ;    	//����ͼ����
extern bool Dis_UvOn ;			//uvͼ����
extern bool Dis_PrOn ;			//ԤԼ������
extern bool Dis_AuOn ;			//�Զ���ɨ��
extern bool Dis_SpOn ;			//�ֲ���ɨ��
extern bool Dis_HourOn ;		//Сʱ��
extern bool Dis_MinuteOn ;		//������
extern bool Dis_ColOn ;		//ʱ�����
extern bool Dis_WeekOn ;		//������
extern bool Dis_SpeedOn ;		//�ٶ���
extern bool Dis_PowerOn ;		//���ͼ����
extern bool Dis_ChargeOn ;		//���ͼ����

extern bool Dis_SpeedGo;       //����ͼ��ת��

extern bool Dis_ErrGlint ;		//����ͼ����˸
extern bool Dis_UvGlint ;		//uvͼ����˸
extern bool Dis_PrGlint ;		//ԤԼ������˸
extern bool Dis_AuGlint ;		//�Զ���ɨ��˸
extern bool Dis_SpGlint ;		//�ֲ���ɨ��˸
extern bool Dis_HourGlint ;	//Сʱ��˸
extern bool Dis_MinuteGlint ;	//������˸
extern bool Dis_ColGlint ;		//ʱ�����˸
extern bool Dis_WeekGlint ;	//������˸
extern bool Dis_SpeedGlint ;	//�ٶ���˸
extern bool Dis_PowerGlint ;	//���ͼ����˸
extern bool Dis_ChargeGlint ;	//���ͼ����˸

extern uint8 Dis_Preengage;	//ԤԼ����
extern uint8 Dis_Hour; 		//��ʾ��Сʱ
extern uint8 Dis_Minute;  		//��ʾ�ķ���
extern uint8 Dis_Week;			//��ʾ����������  ��Χ 1����7
extern uint8 Dis_Speed;		//��ʾ���ٶȣ���Χ1����3
extern uint8 Dis_Power;		//��ʾ�ĵ�Դ���� ����Χ0����3

extern bool dis_updata;        //��ʾ���±�־

extern uint8 Dis_Data[10] ; 	//��ʾ��������
/////////////////***FUC*******************************
void Init_Lcd(void);	//��ʼ��оƬ��IO��
uint8 lcdkey_read(void);     // ��һ����
void Lcd_WriteCommand(uint16 command);
void  lcdkey_disdata(uint8  addr, uint8*  str, uint8  number);
void  lcdkey_bgoff(void);
void  lcdkey_bgon(void);
void  lcdkey_disoff(void);
void  lcdkey_dison(void);
void  AutoDisplay(void);
void clr_display(void);
void delay(uint32 delaynumber);
 void Clr_DisplayData(void);
#endif /* __MAIN_H */
