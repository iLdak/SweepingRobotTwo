//======================ͷ�ļ�=================================
#include "hwincept.h"
#include "hardware.h"
#include "time1.h"
//======================˽�ж���===============================
//======================ȫ�ֱ���===============================
INFR_DATA l_hw,r_hw,m_hw,up_hw,dummy_wall;
bool Action_hwincept;
//======================˽�б���================================
//======================ȫ�ֺ���================================
void init_hwincept(void);
void read_hw_data(INFR_DATA *hw_struct);
void clr_hw_effect(INFR_DATA *hw_struct);  
void enable_hwincept(void);
void disable_hwincept(void);

//======================˽�к���================================
void clr_hw_struct(INFR_DATA *hw_struct);
void read_hw_bit(INFR_DATA *hw_struct);		
void read_dummy_wall_bit(INFR_DATA *hw_struct);

//======================����ʵ��===============================
/******************************************************************
���ܣ���ʼ���������
******************************************************************/
void  init_hwincept(void)
{
	l_hw.read_io = Read_L_HW;
	r_hw.read_io = Read_R_HW;
	m_hw.read_io = Read_M_HW;
	up_hw.read_io = Read_Up_HW;	 
	dummy_wall.read_io = Read_Up_HW;
	clr_hw_struct(&l_hw);
	clr_hw_struct(&m_hw);
	clr_hw_struct(&r_hw);
	clr_hw_struct(&up_hw); 
	clr_hw_struct(&dummy_wall);
	disable_hwincept();
}
/******************************************************************
���ܣ���ȡһ���ֽں�������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵�������������жϺ�������
******************************************************************/
void read_hw_data(INFR_DATA *hw_struct)
{
  if(hw_struct->start == 0)     
  {
      if(hw_struct->read_io() == Bit_RESET)  //��ʼ��������λ
	  {
	      hw_struct->start = 1;
	      hw_struct->time = giv_sys_time;
	      hw_struct->bitnumber = 1;
	      hw_struct->tempbit = 0;
	  }
  }
  else
  {
      read_hw_bit(hw_struct);
  }
}
/******************************************************************
���ܣ���ȡһ���ֽں�������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵�������������жϺ�������
******************************************************************/
void read_dummy_wall(INFR_DATA *hw_struct)
{		   
  if(hw_struct->start == 0)      
  {
      if(hw_struct->read_io() == Bit_RESET)  //��ʼ��������λ
	  {
	      hw_struct->start = 1;
	      hw_struct->time = giv_sys_time;    //����ʱ��
	      hw_struct->bitnumber = 1;
	      hw_struct->tempbit = 0;
	  }
  }
  else
  {
      read_dummy_wall_bit(hw_struct) ;
  }
}
/*********************************************************************
���ܣ���ȡһλ��������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵������������read_hw_data����,��ǰ����ͽ��������3�Σ����������5��
*********************************************************************/
void read_hw_bit(INFR_DATA *hw_struct)
{
  uint32 t;
  uint8 bit;
  t = giv_sys_time - hw_struct->time;	    //�ӿ�ʼ��ʱ��Ŀǰ��ʱ�䣬�Զ�ʱΪʱ�䵥λ
  if(hw_struct->read_io() == Bit_RESET)     //����ǰ�ĺ���״̬
  {
      bit = 0;
  }
  else
  {
      bit = 1;
  }
  //=======================����ǰ����=========================  
  if(t > 37)
  {
	  hw_struct->start = 0;	   //ʱ�䳬��Ԥ�Ƶģ����¿�ʼ
	  return;
  }
  switch(t)
  {
     case 1:
	 case 3:  	  
	 case 9: 
	 case 11:
	 case 17: 
	 case 21:  	
	 case 30: 
	 case 31:
	     hw_struct->tempbit += bit;
	     break;
	 case 5:  //�ж�1��3��6 ����ǰ����	  
         hw_struct->tempbit += bit;
	     if(hw_struct->tempbit > 1)  //ǰ���������θ�
	     {
	         hw_struct->start = 0;   //������ձ�־����ʾ��������
	     }
	     hw_struct->tempbit = 0;
	     break;
	   
	 case 25: //�ж�8��14��18��22��26 ���������	 
         hw_struct->tempbit += bit;
	     if(hw_struct->tempbit > 3)  //�����볬��3�θ� ��
	     {
	         hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
	     }
	     else
	     {
	         if(hw_struct->tempbit < 2)  //�����볬��3�ε�
	         {
	             hw_struct->tempdata &=  ~hw_struct->bitnumber;//��ǰ�����ǵ�
	         }
		     else
		     {
		         //�ж����Ӳ�����,Ҫ���¿�ʼ
	             hw_struct->start = 0; //������ձ�־
		     }
	     }
	     hw_struct->tempbit = 0;
	     break;
			 	 	
	 case 32:	 //29��31��34 ���ν�����  
         hw_struct->tempbit += bit;	
	     if(hw_struct->tempbit < 2)  //�����������ε�
	     {
	         hw_struct->start = 0; //������ձ�־ ����ʾ��������
	     }
	     else
	     {
	         if( hw_struct->bitnumber == 0x80)
		     {
		         hw_struct->bitnumber = 1;  //�ִӿ�ʼ��һλ�� 
			     hw_struct->start = 0;   //�����ʼ��־.          
			     hw_struct->data = hw_struct->tempdata;//�����ݴ��������
			     hw_struct->sign = 1;
			     hw_struct->effect_time = giv_sys_time;
			     hw_struct->effect = 1;
		     } 
	     }
	     hw_struct->tempbit = 0;
	     break;
	 case 33:
	 case 34:
	 case 35:
	 case 36: 
	 case 37:
	     if(bit == 0) //������ϵļ��ʱ�䴦��
		 {
		     hw_struct->bitnumber = (hw_struct->bitnumber << 1);
			 hw_struct->time = giv_sys_time;
		 }
	     break;
  }
}	
/*********************************************************************
���ܣ���ȡһλ��������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵������������read_hw_data����,��ǰ����ͽ��������3�Σ����������5��
*********************************************************************/
void read_dummy_wall_bit(INFR_DATA *hw_struct)
{
  uint32 t;
  uint8 bit;
  t = giv_sys_time - hw_struct->time;
  if(hw_struct->read_io() == Bit_RESET)  //����ǰ�ĺ���״̬
  {
      bit = 0;
  }
  else
  {
      bit = 1;
  }
  //=================================����ǰ����=============================  
  if(t > 22)
  {
	  hw_struct->start = 0;
	  return;
  }
      switch( t)
      {			 
          case 1:
          case 2:
          case 3:
	      case 4:
          case 5:  	  
	      case 6:
          case 7:  
          case 11:
          case 12:
          case 13:
	      case 14:
          case 15:  	  
	      case 16:
          case 17:
	          hw_struct->tempbit += bit;
	          break;
	      case 8:  //1��2��3��4��5��6��7��8	 	 
              hw_struct->tempbit += bit;
	          if(hw_struct->tempbit > 1)  //ǰ���������θ�
	          {
	              hw_struct->start = 0; //������ձ�־
	          }
	          hw_struct->tempbit = 0;
	          break;
	      case 18: //11��12��13��14��15��16��17��18 		 
              hw_struct->tempbit += bit;
	          if(hw_struct->tempbit < 7)  //�����볬��3�θ�
	          {
	              hw_struct->start = 0; //������ձ�־
	          }
	          hw_struct->tempbit = 0;
	          break;
	      case 19:	 
	      case 20:	
	      case 21:	
	          if(bit == 0)
		      {
		          hw_struct->bitnumber++;
			      hw_struct->time = giv_sys_time;
				  if(hw_struct->bitnumber > 5)
				  {
			          hw_struct->start = 1;   //�����ʼ��־.     
			          hw_struct->data = 0xdc;//�����ݴ��������  
			          hw_struct->sign = 1; 		   
			          hw_struct->effect_time = giv_sys_time;
			          hw_struct->effect = 1;
					  hw_struct->bitnumber = 1;
				  }
		      }	   	
	          hw_struct->tempbit = 0;
	          break;
      }
} 
/***************************************************************************
����: �������ṹ������
***************************************************************************/
void clr_hw_struct(INFR_DATA *hw_struct)
{ 
		//�������Ĺ������
		  hw_struct->tempbit = 0;
		  hw_struct->bitnumber = 0;
		  hw_struct->effect = 0;
		  hw_struct->effect_time = giv_sys_time;
		  hw_struct->sign = 0;
		  hw_struct->start = 0;
		  hw_struct->data = 0;
		  hw_struct->time = giv_sys_time;
		  hw_struct->tempdata = 0;
}
/***************************************************************************
����:����������Ч��
����:��
���:��
˵��:���������Чʱ����100ms���ڴ�ʱ����û�����յ��������Ч��־
***************************************************************************/
void clr_hw_effect(INFR_DATA *hw_struct)
{ //����������Ч�źź���Ч��־
    if((hw_struct->effect == 1) && ((giv_sys_time - hw_struct->effect_time) > 3000))
	{
	    hw_struct->effect = 0;	
	}
}
/***************************************************************************
ʹ�ܺ������
***************************************************************************/
void enable_hwincept(void)
{	   
    if(Action_hwincept == FALSE)
    {
        clr_hw_struct(&l_hw);
        clr_hw_struct(&m_hw);
        clr_hw_struct(&r_hw);
        clr_hw_struct(&up_hw);
        Reset_HW_Power();        //�򿪺������������
        Action_hwincept = TRUE;  //���Խ����ź�
    }
}
/***************************************************************************
�رպ������																	 
***************************************************************************/
void disable_hwincept(void)
{
		Set_HW_Power();
		Action_hwincept = FALSE;
}
