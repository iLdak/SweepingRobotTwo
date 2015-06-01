//======================头文件=================================
#include "hwincept.h"
#include "hardware.h"
#include "time1.h"
//======================私有定义===============================
//======================全局变量===============================
INFR_DATA l_hw,r_hw,m_hw,up_hw,dummy_wall;
bool Action_hwincept;
//======================私有变量================================
//======================全局函数================================
void init_hwincept(void);
void read_hw_data(INFR_DATA *hw_struct);
void clr_hw_effect(INFR_DATA *hw_struct);  
void enable_hwincept(void);
void disable_hwincept(void);

//======================私有函数================================
void clr_hw_struct(INFR_DATA *hw_struct);
void read_hw_bit(INFR_DATA *hw_struct);		
void read_dummy_wall_bit(INFR_DATA *hw_struct);

//======================函数实体===============================
/******************************************************************
功能：初始化红外接收
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
功能：读取一个字节红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由中断函数调用
******************************************************************/
void read_hw_data(INFR_DATA *hw_struct)
{
  if(hw_struct->start == 0)     
  {
      if(hw_struct->read_io() == Bit_RESET)  //开始有启动的位
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
功能：读取一个字节红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由中断函数调用
******************************************************************/
void read_dummy_wall(INFR_DATA *hw_struct)
{		   
  if(hw_struct->start == 0)      
  {
      if(hw_struct->read_io() == Bit_RESET)  //开始有启动的位
	  {
	      hw_struct->start = 1;
	      hw_struct->time = giv_sys_time;    //管制时间
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
功能：读取一位红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由read_hw_data调用,在前导码和结束码采样3次，数据码采样5次
*********************************************************************/
void read_hw_bit(INFR_DATA *hw_struct)
{
  uint32 t;
  uint8 bit;
  t = giv_sys_time - hw_struct->time;	    //从开始计时到目前的时间，以定时为时间单位
  if(hw_struct->read_io() == Bit_RESET)     //读当前的红外状态
  {
      bit = 0;
  }
  else
  {
      bit = 1;
  }
  //=======================采样前导码=========================  
  if(t > 37)
  {
	  hw_struct->start = 0;	   //时间超出预计的，重新开始
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
	 case 5:  //判断1，3，6 三次前导吗	  
         hw_struct->tempbit += bit;
	     if(hw_struct->tempbit > 1)  //前导码有两次高
	     {
	         hw_struct->start = 0;   //清除接收标志，表示接收有误
	     }
	     hw_struct->tempbit = 0;
	     break;
	   
	 case 25: //判断8，14，18，22，26 五次数据码	 
         hw_struct->tempbit += bit;
	     if(hw_struct->tempbit > 3)  //数据码超过3次高 ，
	     {
	         hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
	     }
	     else
	     {
	         if(hw_struct->tempbit < 2)  //数据码超过3次低
	         {
	             hw_struct->tempdata &=  ~hw_struct->bitnumber;//当前数据是低
	         }
		     else
		     {
		         //判断是杂波干扰,要重新开始
	             hw_struct->start = 0; //清除接收标志
		     }
	     }
	     hw_struct->tempbit = 0;
	     break;
			 	 	
	 case 32:	 //29，31，34 三次结束码  
         hw_struct->tempbit += bit;	
	     if(hw_struct->tempbit < 2)  //结束码有两次低
	     {
	         hw_struct->start = 0; //清除接收标志 ，表示接收有误
	     }
	     else
	     {
	         if( hw_struct->bitnumber == 0x80)
		     {
		         hw_struct->bitnumber = 1;  //又从开始第一位读 
			     hw_struct->start = 0;   //清除开始标志.          
			     hw_struct->data = hw_struct->tempdata;//把数据存入最后结果
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
	     if(bit == 0) //发送完毕的间隔时间处理
		 {
		     hw_struct->bitnumber = (hw_struct->bitnumber << 1);
			 hw_struct->time = giv_sys_time;
		 }
	     break;
  }
}	
/*********************************************************************
功能：读取一位红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由read_hw_data调用,在前导码和结束码采样3次，数据码采样5次
*********************************************************************/
void read_dummy_wall_bit(INFR_DATA *hw_struct)
{
  uint32 t;
  uint8 bit;
  t = giv_sys_time - hw_struct->time;
  if(hw_struct->read_io() == Bit_RESET)  //读当前的红外状态
  {
      bit = 0;
  }
  else
  {
      bit = 1;
  }
  //=================================采样前导码=============================  
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
	      case 8:  //1，2，3，4，5，6，7，8	 	 
              hw_struct->tempbit += bit;
	          if(hw_struct->tempbit > 1)  //前导码有两次高
	          {
	              hw_struct->start = 0; //清除接收标志
	          }
	          hw_struct->tempbit = 0;
	          break;
	      case 18: //11，12，13，14，15，16，17，18 		 
              hw_struct->tempbit += bit;
	          if(hw_struct->tempbit < 7)  //数据码超过3次高
	          {
	              hw_struct->start = 0; //清除接收标志
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
			          hw_struct->start = 1;   //清除开始标志.     
			          hw_struct->data = 0xdc;//把数据存入最后结果  
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
功能: 清除红外结构的数据
***************************************************************************/
void clr_hw_struct(INFR_DATA *hw_struct)
{ 
		//清除红外的管理变量
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
功能:清除红外的有效性
输入:无
输出:无
说明:红外接收有效时间是100ms，在此时间内没有再收到则清除有效标志
***************************************************************************/
void clr_hw_effect(INFR_DATA *hw_struct)
{ //清除红外的有效信号和有效标志
    if((hw_struct->effect == 1) && ((giv_sys_time - hw_struct->effect_time) > 3000))
	{
	    hw_struct->effect = 0;	
	}
}
/***************************************************************************
使能红外接收
***************************************************************************/
void enable_hwincept(void)
{	   
    if(Action_hwincept == FALSE)
    {
        clr_hw_struct(&l_hw);
        clr_hw_struct(&m_hw);
        clr_hw_struct(&r_hw);
        clr_hw_struct(&up_hw);
        Reset_HW_Power();        //打开红外接收三极管
        Action_hwincept = TRUE;  //可以接收信号
    }
}
/***************************************************************************
关闭红外接收																	 
***************************************************************************/
void disable_hwincept(void)
{
		Set_HW_Power();
		Action_hwincept = FALSE;
}
