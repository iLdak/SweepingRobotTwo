   m=read_bump();
    switch(mode.bump)
    {
	    case 1://E_L		   
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(7,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==1))
				    {
				     return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(8,angle15))
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return 1;
		        case  3:		
	                if(do_action(1,angle180))
		            {
		                mode.bump = 0;
			            if(piv_left == 0)
			            {
			                piv_left = 1;
			            }
			            else
			            {
			                piv_left = 0;
			            }
		            }
				    read_earth();	
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==4))
				    {
				     return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return 1;
		        case  3:		
	                if(do_action(1,angle180))
		            {
		                mode.bump = 0;
			            if(piv_left == 0)
			            {
			                piv_left = 1;
			            }
			            else
			            {
			                piv_left = 0;
			            }
		            }
				    read_earth();	
		            return;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  ;	 
		case 2://E_LM 
	    case 3://E_RM: 
	    case 5://L_BUMP:
	    case 6://R_BUMP:	
	    case 7://W_L:
	    case 8://W_LM:	
	    case 9://W_M:		 		    
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://m_hw
	    case 13://r_hw	
	    case 14://W_R:	
	    case 15://W_RM:
	        default :
	        mode.bump = 0;
	        break;
	} //½áÊø  switch(mode.bump)
    return 0;

