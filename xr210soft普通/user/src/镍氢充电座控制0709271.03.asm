  	;*********************************************************************;
	;  TILTE:       吸尘车充电座电路MCU程序                                 ;
	;  FUNCTION:    1、控制充电电流，充电电压，完成电压监测，防止过充过放   ； 
    ;               2、控制LED灯闪烁，指示电路工作状态                      ；
    ;               3、适合镍氢电池和镍铬电池两种电池的充电管理。           ； 
	;  COMPANY:     深圳市银星智能电器有限公司 研发部                       ;
	;***********************************************************************;
	/*
	STEPS:
	1. SET P6.0 HI AND SET P6.0 LOW AFTER DELAY SOMETIME
	2. PRESCALER TO TCC,TCCA,TCCB,TCCC,SET PRESCALER 分频器分配给TCC...
	3. ENABLE TCC,TCCA,TCCB,TCCC COUNT 
	4. DISABLE I/O INTERRUPT
	*/

	;==============================================
	; Include  "259headfile\em78P259.inc"
	;=============================================
	R0      ==    0x00
	
	R1      ==    0x01
	TCC     ==    0x01
	RTCC    ==    0x01
	;;;;;;;;;;;;;;;;;;;;;
	R2      ==    0x02
	PC      ==    0x02
	;;;;;;;;;;;;;;;;;;;;;
	R3      ==    0x03
	PSR     ==    0x03
	STATUS  ==    0x03
        RST     ==  0x07    ; Bit for reset type
        IOCS    ==  0x06    ; Select the Segment of the control register
        PS0     ==  0X05    ; Page select bits
                            ; -------------------------------------;
                            ;   PS0  Program memory page [Address] ;
                            ;   0      Page 0 [000-3FF]            ;
                            ;   1      Page 1 [400-7FF]            ;
                            ;--------------------------------------;
        T       ==  0x04    ; Time-out bit
        P       ==  0x03    ; Power down bit
        Z       ==  0x02    ; Zero flag
        DC      ==  0x01    ; Auxiliary carry flag
        C       ==  0x00    ; Carry flag
	;;;;;;;;;;;;;;;;;;;;;
    R4     ==    0x04
        
	;;;;;;;;;;;;;;;;;;;;;
	R8     ==    0x08
        ADE3   ==  0x03     ; 0 = Disable ADC4, P53 act as I/O pin
                            ; 1 = Enable ADC4 act as analog input pin
        ADE2   ==  0x02     ; 0 = Disable ADC3, P52 act as I/O pin
                            ; 1 = Enable ADC3 act as analog input pin
        ADE1   ==  0x01     ; 0 = Disable ADC2, P51 act as I/O pin
                            ; 1 = Enable ADC2 act as analog input pin
        ADE0   ==  0x00     ; 0 = Disable ADC1, P50 act as I/O pin
                            ; 1 = Enable ADC1 act as analog input pin
	;;;;;;;;;;;;;;;;;;;;;
	R9      ==    0x09
        VREFS    ==  0X07   ; The input source of the Vref of the ADC
                            ; 0 = The Vref of the ADC is connected to Vdd (default value)
                            ; 1 = The Vref of the ADC is connected to P54/VREF
        CKR1     ==  0X06   ; The prescaler of oscillator clock rate of ADC
        CKR0     ==  0X05   ; ---------------------------------------------------;
                            ; CLK1  CLK0  Operation Mode  Max Operation frequency;
                            ;   0     0       Fsco/16        4 MHz               ;
                            ;   0     1       Fsco/4         1 MHz               ;
                            ;   1     0       Fsco/64        16 MHz              ;
                            ;   1     1       Internal RC    1 MHz               ;
                            ;----------------------------------------------------;
        ADRUN    ==  0X04   ; ADC starts to RUN
                            ; 1 = an A/D conversion is started
                            ; 0 = reset on completion of the conversion                        
        ADPD     ==  0X03   ; ADC Power-down mode 
                            ; 1 = ADC is operating;
                            ; 0 = switch off the resistor reference to save 
                            ;     power even while the CPU is operating.
        ADIS1    ==  0X01   ; Analog Input Select
        ADIS0    ==  0X00   ; --------------------------------------;
                            ; ADIS1  ADIS0  Analog Input Select     ;
                            ;   0     0       AN0/P50               ;
                            ;   0     1       AN1/P51               ;
                            ;   1     0       AN2/P52               ;
                            ;   1     1       AN3/P53               ;
                            ;---------------------------------------;
	;;;;;;;;;;;;;;;;;;;;;
	RA      ==    0x0A
        CALI  ==  0x07      ; Calibration enable bit for A/D offset
                            ; 0 = Calibration disable
                            ; 1 = Calibration enable
        SIGN  ==  0x06  	; Polarity bit of offset voltage 
                            ; 0 = Negative voltage
                            ; 1 = Positive voltage
        VOF2  ==  0x05  	; Offset voltage bits
        VOF1  ==  0x04  	; Offset voltage bits
        VOF0  ==  0x03  	; Offset voltage bits
	;;;;;;;;;;;;;;;;;;;;;
	RB      ==    0x0B
        					;AD11 AD10 AD9 AD8 AD7 AD6 AD5 AD4
	RC      ==    0x0C
               			 ;AD11 AD10 AD9 AD8
	RD      ==    0x0D
               			 ;AD7 AD6 AD5 AD4 AD3 AD2 AD1 AD0
	;;;;;;;;;;;;;;;;;;;;;
	RE      ==    0x0E
	ISR2    ==    0X0E
        ADIF  ==  0x05  	; Interrupt flag for analog to digital conversion
        CMPIF ==  0x04  	; Comparator interrupt flag
        ADWE  ==  0x03  	; ADC wake-up enable bit
                            ; 0 = Disable ADC wake-up
                            ; 1 = Enable ADC wake-up
        CMPWE ==  0x02  	; Comparator wake-up enable bit
                            ; 0 = Disable Comparator wake-up
                            ; 1 = Enable Comparator wake-up
        ICWE  ==  0x01 	 ; Port 5 input status change wake-up enable bit
                            ; 0 = Disable Port 5 input status change wake-up
                            ; 1 = Enable Port 5 input status change wake-up
	;;;;;;;;;;;;;;;;;;;;;
	RF      ==    0x0F
	ISR1    ==     0x0F
        LPWTIF==  0X07 	 ; Internal low-pulse width timer underflow interrupt 
                               ; flag for IR/PWM function, reset by software
	    HPWTIF==  0X06	  ; Internal high-pulse width timer underflow interrupt
                               ; flag for IR/PWM function, reset by software
        TCCCIF==  0X05 	 ; TCCC overflowing interrupt flag
                               ; Set when TCCC overflow, reset by software
        TCCBIF==  0X04 	 ; TCCB overflowing interrupt flag
                               ; Set when TCCB overflow, reset by software
        TCCAIF==  0X03 	 ; TCCA overflowing interrupt flag
                               ; Set when TCCA overflow, reset by software
        EXIF  ==  0X02 	 ; External interrupt flag
	    ICIF  ==  0X01 	 ; Port 5 input status changed interrupt flag
        TCIF  ==  0X00 	 ; TCC overflowing interrupt flag
	;;;;;;;;;;;;;;;;;;;;;
	; (R10~R1F and R20 ~ R3F): General Purpose Register
	R10     ==    0x10
	R11     ==    0x11
	R12     ==    0x12 
	R13     ==    0x13
	R14     ==    0x14
	R15     ==    0x15
	R16     ==    0x16
	R17     ==    0x17
	R18     ==    0x18
	R19     ==    0x19
	R1A     ==    0x1A
	R1B     ==    0x1B
	R1C     ==    0x1C
	R1D     ==    0x1D
	R1E     ==    0x1E
	R1F     ==    0x1F
	;;;;;;;;;;;;;;;;;;;;;
	R20     ==    0x20
	R21     ==    0x21
	R22     ==    0x22
	R23     ==    0x23
	R24     ==    0x24
	R25     ==    0x25
	R26     ==    0x26
	R27     ==    0x27
	R28     ==    0x28
	R29     ==    0x29
	R2A     ==    0x2A
	R2B     ==    0x2B
	R2C     ==    0x2C
	R2D     ==    0x2D
	R2E     ==    0x2E
	R2F     ==    0x2F
	;;;;;;;;;;;;;;;;;;;;;
	R30     ==    0x30
	R31     ==    0x31
	R32     ==    0x32
	R33     ==    0x33
	R34     ==    0x34
	R35     ==    0x35
	R36     ==    0x36
	R37     ==    0x37
	R38     ==    0x38
	R39     ==    0x39
	R3A     ==    0x3A
	R3B     ==    0x3B
	R3C     ==    0x3C
	R3D     ==    0x3D
	R3E     ==    0x3E
	R3F     ==    0x3F
	;======================================================;
	; Special Purpose Registers Define                     ;
	;======================================================;
	; CONT: Control Register
		
        INTE    ==  0X07    ; INT signal edge
                            ; 0: interrupt occurs at the rising edge on the INT pin
                            ; 1: interrupt occurs at the falling edge on the INT pin
        ;INT     ==  0X06    ; Interrupt enable 
                            ; 0: masked by DISI or hardware interrupt
                            ; 1: enabled by ENI/RETI instructions
        TS      ==  0X05    ; TCC signal source
                            ; 0: internal instruction cycle clock, P54 is a bi-directional I/O pin
                            ; 1: transition on TCC pin
        TE      ==  0X04    ; TCC signal edge
                            ; 0: increment if the transition from low to high takes place on TCC pin
                            ; 1: increment if the transition from high to low takes place on TCC pin
        PSTE    ==  0X03    ; Prescaler enable bit for TCC
                            ; 0 : prescaler disable bit, TCC rate is 1:1 
                            ; 1 : prescaler enable bit, TCC rate is set as Bit 2~Bit 0
        PST2    ==  0X02    ; TCC prescaler bits
        PST1    ==  0X01    ; TCC prescaler bits
        PST0    ==  0X00    ; ------------------------------- ;
                            ; PST2  PST1  PST0    TCC Rate    ;
                            ;   0     0     0       1:2       ;
                            ;   0     0     1       1:4       ;
                            ;   0     1     0       1:8       ;
                            ;   0     1     1       1:16      ;
                            ;   1     0     0       1:32      ;
                            ;   1     0     1       1:64      ;
                            ;   1     1     0       1:128     ;
                            ;   1     1     1       1:256     ;
                            ; ------------------------------- ; 
	;;;;;;;;;;;;;;;;;;;;;
	IOC50   ==    0x05      ;IOC50~IOC70: I/O Port Control Register
							; Bit[n]=1, Set P5[n] as input pin, n=0~7
                            ; Bit[n]=0, Set P5[n] as output pin, n=0~7
	IOC60   ==    0x06      ; Bit[n]=1, Set P6[n] as input pin, n=0~3
                            ; Bit[n]=0, Set P6[n] as output pin, n=0~3
	IOC70   ==    0x07      ; Bit[n]=1, Set P7[n] as input pin, n=0~1
                            ; Bit[n]=0, Set P7[n] as output pin, n=0~1  
	;;;;;;;;;;;;;;;;;;;;;
	IOC80   ==    0X08	  ; IOC80: ( TCC Control Register 1)
    	CMPOUT  ==  0X05    ; the result of the comparator output
        COS1    ==  0X04    ; Comparator/OP Select bits
        COS2    ==  0X03    ; Comparator/OP Select bits
                            ;---------------------------------------------------------------------------;
                            ; COS1	COS0  Function description                                          ;
                            ;  0	 0    Comparator and OP not use, P64 act as normal I/O pin          ;
                            ;  0	 1    Act as an Comparator and P64 act as normal I/O pin            ;
                            ;  1	 0    Act as an Comparator and P64 act as Comparator output pin(CO) ;
                            ;  1	 1    Act as an OP and P64 act as OP output pin(CO)                 ;
                            ;---------------------------------------------------------------------------;
        TCCAEN  ==  0X02    ; TCCA enable bit
                            ; 0: disable TCCA 
                            ; 1: enable TCCA as a counter
        TCCATS  ==  0X01    ; TCCA signal source
                            ; 0: internal instruction cycle clock, P61 is a bi-directional I/O pin
                            ; 1: transition on the TCCA pin
        TCCATE  ==  0X00    ; TCCA signal edge
                            ; 0: increment if the transition from low to high takes place on the TCCA pin
                            ; 1: increment if the transition from high to low takes place on the TCCA pin 
	;;;;;;;;;;;;;;;;;;;;;
	IOC90    ==  0X09	   ; IOC90:  ( TCC Control Register 2)
        TCCBHE  ==  0X07    ; Control bit is used to enable the most significant byte of counter
                            ; 1: Enable the most significant byte of TCCBH. TCCB is a 16-bits counter
                            ; 0: Disable the most significant byte of TCCBH (default value)
        TCCBEN  ==  0X06    ; TCCB enable bit ; 0: disable TCCB 
                            ; 1: enable TCCB as a counter
        TCCBTS  ==  0X05    ; TCCB signal source
                            ; 0: internal instruction cycle clock, P62 is a bi-directional I/O pin
                            ; 1: transition on the TCCB pin
        TCCBTE  ==  0X04    ; TCCB signal edge
                            ; 0: increment if the transition from low to high takes place on the TCCB pin
                            ; 1: increment if the transition from high to low takes place on the TCCB pin
        TCCCEN  ==  0X02    ; TCCC enable bit
                            ; 0: disable TCCC ; 1: enable TCCC as a counter
        TCCCTS  ==  0X01    ; TCCC signal source
                            ; 0: internal instruction cycle clock, P63 is a bi-directional I/O pin.
                            ; 1: transition on the TCCC pin
        TCCCTE  ==  0X00    ; TCCC signal edge
                            ; 0: increment if the transition from low to high takes place on the TCCC pin
                            ; 1: increment if the transition from high to low takes place on the TCCC pin 
	;;;;;;;;;;;;;;;;;;;;;
	IOCA0    ==  0X0A       ; IOCA0:  (IR and TCCC scale Control Register)
        TCCCSE  ==  0X07    ; Scale enable bit for TCCC
                            ; 0 : scale disable bit, TCCC rate is 1:1
                            ; 1 : scale enable bit, TCCC rate is set as Bit 6~Bit 4
        TCCCS2  ==  0X06    ; TCCC scale bits
        TCCCS1  ==  0X05    ;;------------------------------------;
        TCCCS0  ==  0X04    ;; TCCCS2  TCCCS1  TCCCS0  TCCC Rate  ;
                            ;  0	    0	    0	    1:2     ;
                            ;  0	    0	    1	    1:4     ;
                            ;  0	    1	    0	    1:8     ;
                            ;  0	    1	    1	    1:16    ;
                            ;  1	    0	    0	    1:32    ;
                            ;  1	    0	    1	    1:64    ;
                            ;  1	    1	    0	    1:128   ;
                            ;  1	    1	    1	    1:256   ;
                            ;-------------------------------------;
        IRE     ==  0x03    ; Infrared Remote Enable bit
                            ; 0: Disable IRE
                            ; 1: Enable IRE
        HF      ==  0x02    ; High Frequency
                            ; 0: PWM application
                            ; 1: IR application mode
        LGP     ==  0x01    ; Long Pulse
                            ; 0: The high-time register and low-time register is valid
                            ; 1: The high-time register is ignored
        IROUTE  ==  0x00    ; control bit is used to define the function of P67 (IROUT) pin
                            ; 0: P67, bi-directional I/O pin.
                            ; 1: IROUT,the I/O control bit of P67 must be set to “0” 
	;;;;;;;;;;;;;;;;;;;;;
	IOCB0   ==    0x0B	  ; IOCB0: (Pull-down Control Register)
					        ;Bit 7 (/PD57) Use to enable the pull-down of P57 pin(for EM78P259NB only)
                            ;0: Enable internal pull-down
        					;1: Disable internal pull-down
          				  ;Bit 6 (/PD56) Use to enable the pull-down of P56 pin(for EM78P259NB only)
                            ;Bit 5 (/PD55) Use to enable the pull-down of P55 pin
                            ;Bit 4 (/PD54) Use to enable the pull-down of P54 pin
                            ;Bit 3 (/PD53) Use to enable the pull-down of P53 pin
                            ;Bit 2 (/PD52) Use to enable the pull-down of P52 pin
                            ;Bit 1 (/PD51) Use to enable the pull-down of P51 pin
                            ;Bit 0 (/PD50) Control bit is used to enable the pull-down of P50 pin 
	;;;;;;;;;;;;;;;;;;;;;
	IOCC0   ==    0x0C  	; IOCC0: (Open-drain Control Register) 
	IOCD0   ==    0x0D  	; IOCD0: (Pull-high Control Register) 
	IOCE0   ==    0x0E  	; IOCE0: (WDT Control Register) 
          WDTE   ==  0X07   ; Control bit is used to enable Watchdog timer
                            ; 0: Disable WDT
                            ; 1: Enable WDT
          EIS    ==  0X06   ; Control bit is used to define the function of P60(/INT) pin
                            ; 0: P60, bi-directional I/O pin
                            ; 1: /INT, external interrupt pin
          ADIE   ==  0X05   ; ADIF interrupt enable bit
                            ; 0 = disable ADIF interrupt
                            ; 1 = enable ADIF interrupt 
          CMPIE  ==  0X04   ; CMPIF interrupt enable bit
                            ; 0 = disable CMPIF interrupt
                            ; 1 = enable CMPIF interrupt
          PSWE   ==  0X03   ; Prescaler enable bit for WDT
                            ; 0 = prescaler disable bit, WDT rate is 1:1
                            ; 1 = prescaler enable bit, WDT rate is set as Bit0~Bit2
          PSW2   ==  0X02   ; WDT prescaler bits
          PSW1   ==  0X01   ; WDT prescaler bits
          PSW0   ==  0X00   ; WDT prescaler bits
                            ;---------------------------------;
                            ; PSW2  PSW1	 PSW0	WDT Rate ;
                            ;  0	    0	  0	 1:2      ;
                            ;  0	    0	  1	 1:4      ;
                            ;  0	    1	  0	 1:8      ;
                            ;  0	    1	  1	 1:16     ;
                            ;  1	    0	  0	 1:32     ;
                            ;  1	    0	  1	 1:64     ;
                            ;  1	    1	  0	 1:128    ;
                            ;  1	    1	  1	 1:256    ;
                            ;---------------------------------;  
	;;;;;;;;;;;;;;;;;;;;;
	IOCF0   ==    0x0F	  ; IOCF0: (Interrupt Mask Register)
        LPWTIE   ==  0X07   ; LPWTIF interrupt enable bit
                            ; 0: disable the LPWTIF interrupt
                            ; 1: enable the LPWTIF inter
        HPWTIE   ==  0X06   ; HPWTIF interrupt enable bit
                            ; 0: disable the HPWTIF interrupt
                            ; 1: enable the HPWTIF interrupt
        TCCCIE   ==  0X05   ; TCCCIF interrupt enable bit
                            ; 0: disable the TCCCIF interrupt
                            ; 1: enable the TCCCIF interrupt
        TCCBIE   ==  0X04   ; TCCBIF interrupt enable bit
                            ; 0: disable the TCCBIF interrupt
                            ; 1: enable the TCCBIF interrupt
        TCCAIE   ==  0X03   ; TCCAIF interrupt enable bit
                            ; 0: disable TCCAIF interrupt
                            ; 1: enable TCCAIF interrupt
        EXIE     ==  0X02   ; EXIF interrupt enable bit
                            ; 0: disable EXIF interrupt
                            ; 1: enable EXIF interrupt
        ICIE     ==  0X01   ; ICIF interrupt enable bit
                            ; 0: disable ICIF interrupt
                            ; 1: enable ICIF interrupt
        TCIE     ==  0X00   ; TCIF interrupt enable bit
                            ; 0: disable TCIF interrupt
                            ; 1: enable TCIF interrupt 
	;;;;;;;;;;;;;;;;;;;;; 
	IOC51   ==     0x05	 ; IOC51:  ( TCCA Counter )  
	IOC61   ==     0x06 	; IOC61:  ( TCCB Counter) /LSB Counter  
	IOC71   ==     0x07	 ; IOC71:  (TCCBH Counter) /MSB Counter  
	IOC81   ==     0x08	 ; IOC81:  (TCCC Counter )  
	IOC91   ==     0x09	 ; IOC91: ( Low-time Register )  
	IOCA1   ==     0x0A	 ; IOCA1: ( High-time Register )  
	IOCB1   ==     0x0B	 ; IOCB1: (High-Time and Low-Time Scale control register) 
        HTSE   ==   0X07    ; High-time scale enable bit
                            ; 0 : scale disable bit, High-time rate is 1:1
                            ; 1 : scale enable bit, High-time rate is set as Bit 6~Bit 4
        HTS2   ==   0X06    ; High-time scale bits
        HTS1   ==   0X05    ;-------------------------------------;
        HTS0   ==   0X04    ; HTS2  HTS1  HTS0  High-time Rate    ; 
                            ;   0     0     0       1:2           ;
                            ;   0     0     1       1:4           ;
                            ;   0     1     0       1:8           ;
                            ;   0     1     1       1:16          ;
                            ;   1     0     0       1:32          ;
                            ;   1     0     1       1:64          ;
                            ;   1     1     0       1:128         ;
                            ;   1     1     1       1:258         ;
                            ;-------------------------------------;
        LTSE   ==  0x03     ; Low-time scale enable bit
                            ; 0 : scale disable bit, Low-time rate is 1:1
                            ; 1 : scale enable bit, Low-time rate is set as Bit 2~Bit 0
        LTS2   ==  0x02     ; Low-time scale bits
        LTS1   ==  0x01     ;-------------------------------------;
        LTS0   ==  0x00     ; LTS2  LTS1  LTS0  LOW-time Rate     ; 
                            ;   0     0     0       1:2           ;
                            ;   0     0     1       1:4           ;
                            ;   0     1     0       1:8           ;
                            ;   0     1     1       1:16          ;
                            ;   1     0     0       1:32          ;
                            ;   1     0     1       1:64          ;
                            ;   1     1     0       1:128         ;
                            ;   1     1     1       1:258         ;
                            ;-------------------------------------; 

	IOCC1   ==    0x0C      ; IOCC1:  (TCC Prescaler Counter) 
	;======================================================;
	; Others Define                                        ;
	;======================================================; 
	; Bit set use BC or BS 
	B0         ==    0x00
	B1         ==    0x01
	B2         ==    0x02
	B3         ==    0x03
	B4         ==    0x04
	B5         ==    0x05
	B6         ==    0x06
	B7         ==    0x0
	;;;;;;;;;;;;;;;;;;;;; 
    
    FLAG       EQU    0X21
    TCCBHT     EQU    0X22
    TCCAT      EQU    0X23
    TCCCT      EQU    0X24
    TCCT       EQU    0X25

    TCCA       EQU    0X5
    TCCBL      EQU    0X6
    TCCBH      EQU    0X7
    TCCC       EQU    0X8 
	;======================================================;
	; Prot Define                                          ;
	;======================================================; 
    P5         equ    0x05 
    R5         equ    0x05 
        AD0    ==     0x00
        AD1    ==     0x01
        AD2    ==     0x02
        AD3    ==     0x03
        Disch  ==     0x04
        Sle2   ==     0x05
        
     
    P6         equ    0x06 
    R6         equ    0x06
        LED2   ==     0x00              ;Clock
        LED1   ==     0x01
        LED3   ==     0x06
        Pwm_Out==     0x07
     
    P7         equ    0x07 
    R7         equ    0x07
        LED4   ==     0x00
    
	;======================================================;
	; RAM Define                                           ;
	;======================================================; 
	Dely_W     equ    0x10                 ;DC-V
	Dely_W1    equ    0x11                 ;EX-V
	Adc_P      equ    0x12
	Add_Bak    equ    0x13
	R3_Bak     equ    0x14
	R4_Bak     equ    0x15
    Flag1      equ    0x16
    Flag2      equ    0x17 
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	BatV1      equ    0x18
	BatV2      equ    0x19
	AC_IN1     equ    0x1A
	AC_IN2     equ    0x1B
	Curnt1     equ    0x1C
	Curnt2     equ    0x1D
	Temp1      equ    0x1E
	Temp2      equ    0x1F
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	General1   equ    0x18
	General2   equ    0x19
	General3   equ    0x1A
	General4   equ    0x1B
	General5   equ    0x1C
	General6   equ    0x1D
	General7   equ    0x1E
	General8   equ    0x1F
	;======================================================;
    HavBa_Tim  equ    0x20              ;Have Bat  count
    Work_Ten   equ    0x21              ;tenor 
    NoNt_Tim   equ    0x22              ;No Ntc  count
    Time_C1    equ    0x23              ;ms count 0--250ms
    Time_C2    equ    0x24              ;250ms count 
    Tim_cnt    equ    0x25              ;
    Charg_C1   equ    0x26              ;
    Charg_C2   equ    0x27              ;Charg current
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    Minut_Cnt  equ    0x28              ;count
    C_Reckn1   equ    0x29              ;Current Reckon
    C_Reckn2   equ    0x2A
    C_Reckn3   equ    0x2B
    Ext_Count1 equ    0x2C              ;have ac count
    Ext_Count2 equ    0x2D              ;no ac count
    
    Dect_C     equ    0x2D
    Temp_Bak2  equ    0x2E
    Com_Tim    equ    0x2F
    ;======================================================; 
    Byte1      equ    0x30
    Byte2      equ    0x31
    Temp_Bak   equ    0x32
    Comm_W     equ    0x33              ;communication
    Temp_NB    equ    0x34
    FlashH     equ    0x35
    FlashL     equ    0x36
    FlashNb    equ    0x37
    
	BatV21     equ    0x38
	BatV22     equ    0x39
	AC_IN21    equ    0x3A
	AC_IN22    equ    0x3B
	Curnt21    equ    0x3C
	Curnt22    equ    0x3D
	Temp21     equ    0x3E
	Temp22     equ    0x3F
	;======================================================;
	; Flag Define                                          ;
	;======================================================;
	;============= Flag1 ==================================; 
    Time_ms    equ    0x00
    Time_mut   equ    0x01
    Ad_Ok      equ    0x02
    Wait       equ    0x03
    Inc_P      equ    0x04     
    Time_F     equ    0x05
    AD_str     equ    0x06
    Pag        equ    0x07
	;============= Flag2 ==================================;
    Flash_F    equ    0x00
    Scond_F    equ    0x01
    Time1      equ    0x02
    Temp_Ok    equ    0x03
    Wdt_rst    equ    0x04
    No_Ntc     equ    0x05
    No_Bat     equ    0x06
    LEDF       equ    0x07
    
	;============== Comm_W ================================;
    Fls_D      equ    0x00
    Fh         equ    0x01
    Ac1        equ    0x02
    Alm        equ    0x03
    Charg_ok   equ    0x04
    Temp_ov    equ    0x05
    Charg_Ov   equ    0x06
    ;     equ    0x07
    ;======================================================;
	; constant Define                                          ;
	;======================================================;
    init_t     equ    100               ;初始检测等待次数 
    
    Ext_Hav    equ    0x0a              ;有AC检测次数 
    Ext_No     equ    0x0a              ;无AC检测次数 
    
    FD_Tim     equ    180 
    
    JC_Time    equ    10                ;涓充时间 
    FD_Time    equ    10                ;放电时间 
    
  ;  Start_PL   equ    0x0FF
  ;  Start_PH   equ    0x04
    Max_Plus   equ    0xC0
    Min_Plus   equ    0x02
    
    JC_Curt11  equ    0x01              ;120mA
    JC_Curt12  equ    0x40
    
    JC_Curt21  equ    0x01              ;140mA
    JC_Curt22  equ    0x60
    
    Larg_C1    equ    0x09              ;900mA 
    Larg_C2    equ    0x5E
    
    Farg_C1    equ    0x00              ;100mA
    Farg_C2    equ    0xCE
    
    Small_C1   equ    0x04              ;400mA
    Small_C2   equ    0x34
    
    Wait1      equ    10                ;100ms 
    Max_Temp   equ    124               ;最高温度  124/2=62
    Step_T     equ    5                 ;2分钟 温度差  4/2=2.0
    
    ;;;;;;;;;;;  AC_IN Define   ;;;;;;;;;
    Ext_V13A   equ    0x06
    Ext_V13B   equ    0xEF              ;13V
    
    ;;;;;;;;;;;   Bat_V Define   ;;;;;;;;;
    Bat_060A   equ    0x04
    Bat_060B   equ    0x6E              ;6.0V
    
    Bat_123A   equ    0x09
    Bat_123B   equ    0x15              ;12.3V
    
    
    ;======================================================; 
            ORG     0X000                         ;Reset
            
            mov     a, status
            mov     R3_Bak, a
            JMP     START
            
    ;============================================= 
            ORG     0X003                         ;Eint
            
            bc      status, PS0
            mov     Add_bak, a
            JMP     Ext_INT
    ;=============================================  
            ORG     0X009                         ;Tcc
             
            bc      status, PS0
            mov     Add_bak, a
            JMP     TCC_INT
            
    ;=============================================  
            ORG     0X00C                         ;ADC
            
            bc      status, PS0
            mov     Add_bak, a
            JMP     ADC_INT
    ;=============================================   
            ORG     0X012                         ;HTIME_INT
            
            bc      status, PS0
            bc      RF,HPWTIF
            JMP     HTIME_INT
            
    ;============================================  
            ORG     0X015                         ;LTIME_INT
            
            bc      status, PS0
            bc      RF,LPWTIF
            JMP     LTIME_INT
            
    ;============================================  
            ORG     0X01E                         ;TCCC_INT Page
   
            bc      status, PS0                   ;定时启动AD转换 
            
            bs      R9, ADRun  
            bc      RF, Tcccif
            
            jbc     Flag1, Pag
            bs      status, PS0
            reti
    ;============================================
    
    ;============================================  
 HTIME_INT:
            jbc     Flag1, Pag
            bs      status, PS0
            reti
    ;============================================   
 LTIME_INT:
            jbc     Flag1, Pag
            bs      status, PS0
            reti
    ;============================================ 
     
    ;============================================
        
    ;        ORG     0X010
    ;--------------------------------------------------;
 TCC_INT:                                         ;1ms 定时，提供时钟基准 
            mov     a, status
            mov     R3_Bak, a
            mov     a, R4
            mov     R4_bak, a
            
         ;   mov    a, @0B00010000                 ;00010000b
         ;   xor    P5, a
 Tcc_Int1:
            bs      Flag1, Time_ms
            bc      R4, B6
            mov     a, @0x85                      ;83H
            add     tcc, a
            inc     Time_C1
            jbs     status, Z
            jmp     Tcc_end 
            bs      Flag1, Time_mut
            inc     Time_C2
            jbc     status, Z
            bs      Flag1, Time_F                 ;约1分钟 
            bs      Flag2, Flash_F
            jbc     Flag2, No_Ntc
            jmp     Tcc_Int3
            jbs     Flag2, LEDF
            jmp     Tcc_Int2
            bc      Flag2, LEDF
 Tcc_Int3:
            mov     a,  @0x03                     ;00000011b
            xor     R6, a
            jmp     Tcc_Int21
 Tcc_Int2:
            bs      Flag2, LEDF
 Tcc_Int21:
            mov     a, @0x01                      ;00000001b
            jbc     Comm_W, Alm
            xor     R7, a
 Tcc_end:
            mov     a, R4_Bak
            mov     R4, a
            mov     a, R3_Bak
            mov     Status, a
            mov     a, Add_Bak 
            bc      RF, Tcif
            jbc     Flag1, Pag
            bs      status, PS0
            reti
    ;--------------------------------------------------; 
 ADC_INT:                                         ;AD 转换 
            mov     a, status
            mov     R3_Bak, a
            mov     a, R4
            mov     R4_bak, a
            
            bc      RE, Adif
            
            jbc     Flag1, AD_str
            jmp     ADC_INT1
            bs      Flag1, AD_str
            jmp     ADC_End
 ADC_INT1:
            bc      Flag1, AD_str
            inca    R9
            and     a, @0xFB
            mov     R9, a
            and     a, @0x03
            jbc     status, Z
            bs      Flag1, Ad_Ok
            
            mov     a, Adc_P                      ;缓存指针 
            mov     R4, a
            
            mov     a, RD
            mov     R0, a
            inc     R4
            mov     a, RC
            and     a, @0x0f
            mov     R0, a
            inca    R4
            mov     Adc_P, a
            xor     a, @0x80
            
            jbs     status, z
            jmp     ADC_End
            mov     a, @0x48
            mov     R9, a
            mov     a, @0x60
            mov     Adc_P, a
 ADC_End:           
            mov     a, R4_Bak
            mov     R4, a
            mov     a, R3_Bak
            mov     Status, a
            mov     a, Add_Bak
         ;   bs      R9, ADRun
            
            jbc     Flag1, Pag
            bs      status, PS0
            reti
    ;--------------------------------------------------;
 Ext_INT:                                              ;No Used
            
 Ext_End:
            mov     a, Add_Bak 
            bc      RF, Exif
            jbc     Flag1, Pag
            bs      status, PS0
            reti
    ;--------------------------------------------------;
    
    ;--------------------------------------------------;
    
    ;--------------------------------------------------;
START:
            add     a, @0xff
            disi
            ;;;;;;;;Port seting;;;;;;;;;;         ;端口初始化 
            bc      Status, Iocs
            mov     a, @0xCF           
            iow     Ioc50   
            mov     a, @0xFF 
            mov     R5, a        
            
            mov     a, @0x3C            ;00111100b
            iow     Ioc60
            mov     a, @0xFF            ;11111111b
            mov     R6, a
            
            mov     a, @0xFE      
            iow     Ioc70
            mov     a, @0xF9
            mov     R7, a
            
            bs      P6, Pwm_Out
            ;;;;;;;Proc Wdt;;;;;;;;;;;;;;         ;复位模式判断 
            
            mov     a, R3_Bak
            and     a, @0x98            
            xor     a, @0x08
            jbc     status,z
            bs      Flag2, Wdt_rst
            
            
            jbs     Flag2, Wdt_rst
            jmp     Clear_Ram
            clr    Work_Ten
            clr    Time_C1
            clr    Time_C2
            mov    a, @0x20 
            mov    General2, a
 SDelay:                                          ;延时 
            call   dely
            djz    General2
            jmp    SDelay
            
            jmp    Main_Prog
 Clear_Ram:
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;         ;清内存 
            mov    a, @0x10             ;00010000b 
            mov    R4, a
 Clr_GenRam:                            ;clr 20h-3fh
            clr    R0
            inc    R4
            mov    a, @0x40
            sub    a, R4 
            jbs    Status, c
            jmp    Clr_GenRam 
            add    a, @0xff
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            mov    a, @0x60  
            mov    R4, a
 Clr_ADValue:
            clr    R0
            inc    R4
            mov    a, @0x7f
            sub    a, R4 
            jbs    Status, c
            jmp    Clr_ADValue
             
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
 Main_Prog: 
            mov    a, @0x60                       ;ADC 缓存指针 
            mov	Adc_P, a
            bc     R4, B6
            ;;;;;;; 外设设置 ;;;;;;;;;;;;;;;
            ;;;;;;;Open Wdt;;;;;;;;;;;;;;;
      ;      Ior    Ioce0                ;
            mov    a, @0x8B              ;10001010b
            Iow    Ioce0                 ;Prescaler on  1:8
            
            ;;;;;;; Ext_Int ;;;;;;;;;;;;;;;;
            ;;  CONT, IocE0, IOCF0        ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
      ;      Ior    Ioce0                ;
      ;      or     a, @0x40
      ;      iow    IocE0
            
            ;;;;;;;Open Tcc ;;;;;;;;;;;;;;;;
            ;;  CONT, IOCF0               ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            mov     a, @0x8D               ;10001101b 64分频 
            contw 
            clr     Tcc
            ;;;;;;;  Open ADC  ;;;;;;;;;;;;;
            ;;   R8, R9, IOCE0            ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            mov    a, @0b01001000
            mov    R9, a
            Ior    Ioce0
            or     a, @0x20             ;00100000b
            Iow    Ioce0
            bs     R9, ADRun
            mov    a, @0x0f
            mov    R8, a
           
            ;;;;;;;    Seting PWM   ;;;;;;;;;;;
            ;; IOCA0,IOCF0,IOC91,IOCA1,IOCB1 ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            bs     status,iocs          ;select ioc bank1
            mov    a, @0x00
            iow    iocb1                ;H/L Time Scale Control
            mov    a, @0x70             ;mov    LTimB, a
            iow    ioc91                ;Low-Time
            mov    a, @0xf0             ;mov    HTimB, a
            iow    iocA1                ;High Time
            
            ;;;;;;;    Open TccC   ;;;;;;;;;;;;
            ;; IOC90, IOCA0, IOCF0, IOC81    ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            clra
            iow    ioc81
            
            bc     status,iocs          ;select ioc bank0
            mov    a, @0x04             ;00000100b TcccEN
            iow    ioc90
            mov    a, @0xA0             ;10101011,IRE=1,HF,LGP=0 ??
            iow    ioca0                ;IROUT=1  
            mov    a, @0x21             ;00100001b  TccCIE,Exie,TCIE   
            Iow    Iocf0                ;
            Eni
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            bc      Flag2, Wdt_rst
 WaitFor:                               ;延时 
            call    dely
            Wdtc
            djz     General2
            jmp     WaitFor
            clr     General3            ;初始化 
            clr     General4
            clr     General5
            
            clr     HavBa_Tim           ;No Bat  count
            clr     NoNt_Tim
            clr     Dect_C              ;Detect  count
            bs      P6, Pwm_Out
            bs      Flag2, No_Bat
            bs      Flag2, No_Ntc
            bc      P6, LED1
            bs      P6, LED2
            bs      P7, LED4
 MainM:
            mov     a, @0x01
            mov     FlashH, a
            mov     a, @0x04
            mov     FlashL, a
            mov     FlashNb, a
            
            ;;;;;;;Program Main ;;;;;;;;;;;;主程序 
 Main:             
            clr     R4
            jbs     Flag2, Flash_F
            jmp     Main2
            bc      Flag2, Flash_F
            
            jbs     Comm_W, Fls_D                 ; LED 闪烁 
            jmp     Main2
            jbs     Comm_W, Fh
            jmp     Main1
            mov     a, @0b01000000
            xor     P6, a
            dec     FlashNb
            jbs     status, Z
            jmp     Main2
            bc      Comm_W, Fh
            mov     a, FlashL
            mov     FlashNb, a
            jmp     Main2
 Main1:
            dec     FlashNb
            jbs     status, Z
            jmp     Main2
            bs      P6, Led3
            bs      Comm_W, Fh
            rlca    FlashH
            and      a, @0xfe
            mov     FlashNb, a
 Main2:
            ;;;;;;;;;;;;;;;;;;;;;;
            jbs     Flag1, Ad_Ok
            jmp     Main
            bc      Flag1, Ad_Ok
            Wdtc
            
            
            jbs     Flag1, Time_mut
            jmp     Main3
            bc      Flag1, Time_mut
            bc      status, iocs
            mov     a, @0xFF
            iow     iocB0
            iow     iocC0
            iow     iocD0
            
 Main3:
            Disi
            bs      status, PS0
            call    Get_Dat             ;Dat-->38h-3Fh BatV AC_IN Curnt Temp
            Eni
            ;;;;;;; Bat Detect ;;;;;;;;;;;;是否有电池 ?
            jbs     Flag2, No_Bat
            jmp     Pro_Dat
            
            bs      P5, Disch           ;No_Ba=1
            Disi
            bs      status, PS0
            call    Detect_Bat
            Eni
            jbc     Flag2, No_Bat
            jmp     Main
            
            bc      P5, Disch
            bs      Comm_W, Fls_D
            jbs     Comm_W, Temp_ov
            bc      Comm_W, Alm
            bs      P7, LED4
            clr     Work_Ten
            ;;;;;;; AC in Detect ;;;;;;;;输入电压检测 
 Pro_Dat: 
            Disi
            bs      status, PS0         ;No_Ba=0
            call    Detect_AC           ;AC-->Dely_W1
            Eni
            
            jbc     Comm_W, Ac1
            jmp     Pro_Dat2
            mov     a, @0x14            ; 18.4V
            sub     a, Dely_W1
            jbs     status, C
            jmp     Pro_Dat3
            inc     Ext_Count1          ; AC_In> 18.4V
            mov     a, @Ext_Hav
            sub     a, Ext_Count1
            jbs     status, C
            jmp     Pro_Dat4
            bs      Comm_W, Ac1
            mov     a, @0x90
            mov     Work_Ten, a
            clr     Ext_Count1
            jmp     Pro_Dat4
 Pro_Dat2:
            mov     a, @0x12            ; 17.9V
            sub     a, Dely_W1
            jbc     status, C
            jmp     Pro_Dat3
            inc     Ext_Count1
            mov     a, @Ext_No
            sub     a, Ext_Count1
            jbs     status, C
            jmp      Pro_Dat4
            bc      Comm_W, Ac1         ; AC_In< 17.2V no Ac
            clr     Ext_Count1
            jmp      Pro_Dat4
 Pro_Dat3:
            clr     Ext_Count1
            ;;;;;;; Bat_V Detect ;;;;;;;;电池电压检测 
 Pro_Dat4:
            Disi
            bs      status, PS0
            call    Detect_B            ;Bat_V --> Dely_W
            Eni
            
            mov     a, Work_Ten
            and     a, @0xF0
            xor     a, @0x10
            jbc     status, Z
            jmp     Pro_Dat5
            mov     a, @0x1E            ;15.67V
            sub     a, Dely_W
            jbs     status, c
            bc      Comm_W, Charg_ok    ;Bat_V<15.67V
 Pro_Dat5:
            swapa   Work_Ten            ;16.67V
            and     a, @0x0F
            mov     FlashH, a           ;Bat_V<13.3V
            inc     FlashH
 Pro_Dat54:                             ;Temp
            Disi
            bs      status, PS0
            call    Get_Temp            ;Temperature --> Temp_Nb
            Eni
            mov     a, General2
            mov     Temp_NB, a
            
            mov     a, @Max_Temp
            sub     a, Temp_NB
            jbs     status, c
            jmp     Pro_Dat55
            
            jbc     Comm_W, Temp_ov
            jmp     Pro_Dat56
            bs      Comm_W, Temp_ov     ;Temp>Max_T
            jmp     Pro_Dat6
 Pro_Dat56:
            jbc     Comm_W, Alm
            jmp     Pro_Dat6
            bs      Comm_W, Alm
            mov     a, @0x70 
            mov     Work_Ten, a         ; -->70 进入均衡、补充电 
            jmp     Charg35
 Pro_Dat55:
            mov     a, @Max_Temp-5
            sub     a, Temp_NB
            jbc     status, c           ;Temp>Max_T
            jmp     Pro_Dat6
            bc      Comm_W, Temp_ov     ;Temp>Max_T
            jbs     Flag2, No_Bat
            bc      Comm_W, Alm
            bs      P7, Led4
            ;;;;;;;;;;;;;;;;;;
 Pro_Dat6:                              ;程序分支 
            swapa   Work_Ten
            and     a, @0x0F
            sub     a, @0x09
            jbc     status, C
            jmp     Pro_Dat61
            clr     Work_Ten
 Pro_Dat61:
            swapa   Work_Ten
            and     a, @0x0F
            add     R2, a
            ;;;;;;;;;;;;;;;;;;
            jmp     Work_Start          ;00
            jmp     Charg1              ;10 start
            jmp     Charg2              ;20 FD1
            jmp     Charg3              ;30 FD2
            jmp     Charg4              ;40 JCong
            jmp     Charg5              ;50 CDian1 start
            jmp     Charg6              ;60 CDian2 Large Current
            jmp     Charg7              ;70 JCong2
            jmp     Charg8              ;80
            jmp     Slep_Pro            ;90
            jmp     Labor               ;A0
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Work_Start:                            ;Work_Ten=00 充电前准备 
            mov     a, @init_t          ; 100
            sub     a, Time_C1
            jbs     status, C
            jmp     Main
            mov     a, @0x10
            mov     Work_Ten, a
            jmp     Main
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Charg1:                                ;10 接触不良处理 
            jbc     Flag2, No_Ntc       ;无NTC
            jmp     Charg23
            jbc     Flag2, No_Bat       ;无电池 
            jmp     Charg23
            
            mov     a, @0x20
            mov     Work_Ten, a         ;10--20 Comm_W, Fls_D
            
            mov     a, @0x04
            mov     FlashL, a
            bc      P5, Disch
            jmp     Main
            ;;;;;;;;;;;;;;;;;;
 Charg2:                                ;20 充电流程分配 
            mov     a, Dely_W
            jbc     status, Z
            jmp     Charg23
            mov     a, @0x14            ;12.3V
            sub     a, Dely_W
            jbs     status, C 
            jmp     Charg34             ;BatV<12.3V 进入初充 
 Charg21: 
            mov     a, @0x1B            ;14.67V
            sub     a, Dely_W
            jbc     status, C
            jmp     Charg22
            mov     a, @0x30
            mov     Work_Ten, a         ;14.8V>BatV>12.3V
            clr     Tim_Cnt             ;20-->30; 进入放电 
            bs      P5, Disch           ;FD
            jmp     Charg4B
 Charg22:                               ;BatV>14.67V
            mov     a, @0x23            ;17.3V
            sub     a, Dely_W
            jbs     status, C
            jmp     Charg34             ; 14.67V<BatV<17.0V 进入涓充1
 Charg23:
            mov     a, @0x70            ;BatV>17.0V
            mov     Work_Ten, a         ; 20-->60 进入均衡补充 
            mov     a, @0x08
            mov     FlashL, a
            jmp     Charg35
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Charg3:                                ;30 初始放电 
            mov     a, @FD_Tim
            sub     a, Time_C1
            jbc     status, C
            jmp     Charg31
            bs      P5, Disch 
            jmp     Charg32
 Charg31: 
            bc      P5, Disch
 Charg32:
            jbs     Flag1, Time_F
            jmp     Charg33
            bc      Flag1, Time_F
            inc     Tim_Cnt             ;minute count
            mov     a, @FD_Time
            sub     a, Tim_Cnt
            jbc     status, C
            jmp     Charg34             ;all the 10minute 
 Charg33:
            mov     a, @0x15            ;12.7V
            sub     a, Dely_W 
            jbc     status, C 
            jmp     Main                ;BatV>12.7V
 Charg34:
            mov     a, @0x40            ;BatV<12.7V
            mov     Work_Ten, a         ;30-->40 进入初充 
            mov     a, @0x05
            mov     FlashL, a
 Charg35:
            bc      P5, Disch
            bs      status,iocs         ;select ioc bank1
            mov     a, @0x00
            iow     iocb1               ;H/L Time Scale Control
            mov     a, @Min_Plus        ;mov     LTimB, a
            iow     ioc91               ;Low-Time
            mov     a, @Max_Plus        ;mov     HTimB, a
            iow     iocA1               ;High Time
            bc      status, iocs        ;select ioc bank1
            ior     IocA0
            and     a, @0xF0            ;开放PWM输出 
            add     a, @0x09
            iow     IocA0 
 ;Charg36:           
            clr     Tim_Cnt
            clr     C_Reckn1
            clr     C_Reckn2
            clr     C_Reckn3
            clr     Minut_Cnt           
            bc      Flag1, Time_F
            jmp     Main
            ;;;;;;;;;;;;;;;;;;
 Charg4:                                ;40 初充 
            jbs     Flag1, Time_ms
            jmp     Charg46
            bc      Flag1, Time_ms
            jbc     Flag1, Wait
            jmp     Charg45
            
            Disi
            bs      status, PS0
            call    Count_Curnt
            Eni
            inc     Tim_cnt
            mov     a, Tim_cnt
            xor     a, @128
            jbs     status, Z
            jmp     Charg46
            bs      Flag1, Wait
            clr     Tim_cnt
            call    Get_Icc
            mov     a, C_Reckn1
            mov     General1, a
            ;;;;;;;;;;;;;;;;;;
            mov     a, @JC_Curt12
            sub     a, C_Reckn2
            jbc     status, C
            jmp     Charg41
            mov     a, @0x01
            sub     General1, a
            jbs     status, C
            jmp     Charg44   
 Charg41:
            mov     a, @JC_Curt11
            sub     a, General1
            jbs     status, C
            jmp     Charg44
            ;;;;;;;;;;;;;;;;
            mov     a, C_Reckn1
            mov     General1, a
            mov     a, @JC_Curt22       ;55mA
            sub     a, C_Reckn2
            jbc     status, C
            jmp     Charg42
            mov     a, @0x01
            sub     General1, a
            jbs     status, C
            jmp     Charg46
 Charg42:                               ;C>55mA
            mov     a, @JC_Curt21       ;55mA
            sub     a, General1
            jbs     status, C
            jmp     Charg46
            Disi
            bs      status, PS0
            call    Inc_Pulse           ;HTim-1
            Eni
            jmp     Charg46
 Charg44:
            Disi
            bs      status, PS0
            call    Dec_Pulse           ;HTim+1
            Eni
            jmp     Charg46
 Charg45:
            inc     Tim_cnt
            mov     a, @Wait1
            sub     a, Tim_cnt
            jbs     status, C
            jmp     Charg46
            bc      Flag1, Wait
            clr     Tim_cnt
            clr     C_Reckn3
            clr     C_Reckn2
            clr     C_Reckn1  
 Charg46:
            jbs     Flag1, Time_F
            jmp     Charg48
            bc      Flag1, Time_F
            inc     Minut_Cnt
            mov     a, @JC_Time
            sub     a, Minut_Cnt
            jbc     status, C
            jmp     Charg49
 Charg48:
            mov     a, @0x1B            ;14.6V
            sub     a, Dely_W
            jbs     status, C
            jmp     Charg47
 Charg49:
            mov     a, @0x50
            mov     Work_Ten, a         ;40-->50 进入大电流充电 
            mov     a, @0x06
            mov     FlashL, a
            bc      P5, Disch
            bc      Flag1, Wait
            clr     Tim_cnt
            clr     C_Reckn3
            clr     C_Reckn2
            clr     C_Reckn1  
            jmp     Main 
 Charg47:                               ;充电出口 
            Disi
            bs      status, Ps0
            call    Pro_Bat             ;检测电池 
            Eni
            jbs     Flag2, No_Bat
            jmp     Main                ;have Ext_power
            bs      Comm_W, Alm         ;报警 
            bc      Comm_W, Fls_D       ;停止闪烁 
            bs      P6, Led3
 Charg4A:   
            mov     a, @0x00            ;
            mov     Work_Ten, a         ;
            clr     Time_C1
 Charg4B:
            bc      status, iocs        ;select ioc bank1
            ior     IocA0
            and     a, @0xF0 
            iow     IocA0               ;关PWM输出 
            bc      P5, Disch
            bs      status,iocs         ;select ioc bank1
            mov     a, @Min_Plus        ;mov     LTimB, a
            iow     ioc91               ;Low-Time
            mov     a, @Max_Plus        ;mov     HTimB, a
            iow     iocA1               ;High Time
            bc      status, iocs        ;select ioc bank1
            
            bs      P6, Pwm_Out
            clr     Tim_Cnt
            clr     C_Reckn1
            clr     C_Reckn2
            clr     C_Reckn3
            jmp     Main 
            ;;;;;;;;;;;;;;;;;;
 Charg5:                                ; 大电流充电准备 
            mov     a, @Larg_C1         ; 电流参数获得 
            mov     Charg_C1, a
            mov     a, @Larg_C2
            mov     Charg_C2, a 
            jbc     P5, Sle2
            jmp     Charg51
            
            mov     a, @Small_C1
            mov     Charg_C1, a
            mov     a, @Small_C2
            mov     Charg_C2, a         ;
 Charg51:
            mov     a, @0x60
            mov     Work_Ten, a         ;50-->60  进入大电流充电 
 Charg55:
            bc      P5, Disch
            clr     Tim_Cnt
            clr     Ext_Count1
            clr     Ext_Count2
            clr     C_Reckn1
            clr     C_Reckn2
            clr     C_Reckn3
            bc      Flag1, Time_ms
            bc      Flag1, Wait
            jmp     Main
            ;;;;;;;;;;;;;;;;;;
 Charg6:                                ;60  大电流充电 
            jbs     Flag1, Time_ms
            jmp     Charg65
            bc      Flag1, Time_ms
            jbs     Flag1, Wait
            jmp     Charg61
            inc     Tim_Cnt
            mov     a, @Wait1
            sub     a, Tim_cnt
            jbs     status, C
            jmp     Charg65
            bc      Flag1, Wait
            clr     Tim_Cnt
            clr     C_Reckn1
            clr     C_Reckn2
            clr     C_Reckn3 
            jmp     Charg65
 Charg61:
            Disi
            bs      status, PS0
            call    Count_Curnt             ;电流累加 
            Eni
            inc     Tim_cnt
            mov     a, Tim_cnt
            xor     a, @128
            jbs     status, Z
            jmp     Charg65
            bs      Flag1, Wait
            clr     Tim_cnt
            call    Get_Icc             ;电流平均 
            
            mov     a, Charg_C2
            sub     C_Reckn2, a
            jbc     status, c
            jmp     Charg62
            mov     a, @0x01
            sub     C_Reckn1, a
            jbs     status, c
            jmp     Charg64             ;current< seting 
 Charg62:
            mov     a, Charg_C1
            sub     C_Reckn1, a
            jbs     status, c
            jmp     Charg64             ;current< seting 
            mov     a, @0x40            ;回滞系数 
            sub     C_Reckn2, a
            jbc     status, c
            jmp     Charg63
            mov     a, @0x01
            sub     C_Reckn1, a
            jbs     status, c
            jmp     Charg65
 Charg63:                               ;current> seting
            Disi
            bs      status, PS0
            call    Inc_Pulse           ;DEC
            Eni
            jmp     Charg65
 Charg64:                               ;current> seting
            Disi
            bs      status, PS0
            call    Dec_Pulse           ;INC
            Eni
            ;;;;;;;;;;;;;;;;;;
 Charg65:                               ;电池电压超过19.67V？ 
            mov     a, @0x2A            ;19.67V
            sub     a, Dely_W
            jbs     status, c
            jmp     Charg67             ;BatV<19.67V
            jbc     Comm_W, Charg_Ov
            jmp     Charg66
            bs      Comm_W, Charg_Ov
            jmp     Charg670
 Charg66:
            mov     a, @0x70            ;BatV>19.67V goto JC2
            mov     Work_Ten, a 
            clr     Minut_Cnt
            mov     a, @0x08
            mov     FlashL, a
            jmp     Charg47
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Charg67:                               ;BatV<19.67V
            bc      Comm_W, Charg_Ov
 Charg670:
            jbs     Flag1, Time_F
            jmp     Charg47
            bc      Flag1, Time_F
            
            jbs     Flag2, Time1
            jmp     Charg671
            bc      Flag2, Time1        ;add5-11
            mov     a, Temp_Bak
            mov     General2, a
            jmp     Charg672
 Charg671:
            bs      Flag2, Time1        ;add5-11
            mov     a, Temp_Bak2
            mov     General2, a
 Charg672:   
            jbc     status, Z
            jmp     Charg68
            sub     a, Temp_Nb
            jbs     status, c
            jmp     Charg68
            mov     General1, a
            mov     a, @Step_T          ;温度差 
            sub     a, General1
            jbs     status, c
            jmp     Charg68
            jbc     Flag2, Temp_Ok
            jmp     Charg66
            bs      Flag2, Temp_Ok
            jmp     Charg681
 Charg68: 
            bc      Flag2, Temp_Ok
 Charg681:
            jbs     Flag2, Time1
            jmp     Charg69
            mov     a, Temp_Nb
            mov     Temp_Bak2, a        ;温度值备份2
            jmp     Main
 Charg69:
            mov     a, Temp_Nb          ;温度值备份1
            mov     Temp_Bak, a
            jmp     Main
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Charg7:                                ;70 补充、均衡 
            jbs     Flag1, Time_ms
            jmp     Charg76
            bc      Flag1, Time_ms
            jbc     Flag1, Wait
            jmp     Charg75
            
            Disi
            bs      status, PS0
            call    Count_Curnt
            Eni
            inc     Tim_cnt
            mov     a, Tim_cnt
            xor     a, @128
            jbs     status, Z
            jmp     Charg76
            bs      Flag1, Wait
            clr     Tim_cnt
            call    Get_Icc
            ;;;;;;;;;;;;;;;;;;
            mov     a, C_Reckn1         ;电流检测 
            mov     General1, a
            mov     a, @JC_Curt12
            sub     a, C_Reckn2
            jbc     status, C
            jmp     Charg71
            mov     a, @0x01
            sub     General1, a
            jbs     status, C
            jmp     Charg74             ;C < setting
 Charg71:
            mov     a, @JC_Curt11
            sub     a, General1
            jbs     status, C
            jmp     Charg74
            ;;;;;;;;;;;;;;;;
            mov     a, C_Reckn1
            mov     General1, a
            mov     a, @JC_Curt22       ;55mA
            sub     a, C_Reckn2
            jbc     status, C
            jmp     Charg72
            mov     a, @0x01
            sub     General1, a
            jbs     status, C
            jmp     Charg76
 Charg72:                               ;C>55mA
            mov     a, @JC_Curt21       ;55mA
            sub     a, General1
            jbs     status, C
            jmp     Charg76
            Disi
            bs      status, PS0         ; 电流调整 
            call    Inc_Pulse           ;HTim-2 
            Eni
            jmp     Charg76
 Charg74:
            Disi
            bs      status, PS0
            call    Dec_Pulse           ;HTim+2 
            Eni
            jmp     Charg76
 Charg75:                               ;Wait
            inc     Tim_cnt
            mov     a, @Wait1
            sub     a, Tim_cnt
            jbs     status, C
            jmp     Charg76
            bc      Flag1, Wait
            clr     Tim_cnt
            clr     C_Reckn3
            clr     C_Reckn2
            clr     C_Reckn1  
 Charg76:
            jbs     Flag1, Time_F
            jmp     Charg77
            bc      Flag1, Time_F       ;256 X 250ms约60s 
            inc     Minut_Cnt
            mov     a, @JC_Time
            sub     a, Minut_Cnt
            jbs     status, C
            jmp     Main
            bs      Comm_W, Charg_ok
            bc      Comm_W, Fls_D
            bc      P6, Led3
            mov     a, @0x80
            mov     Work_Ten, a
            jmp     Charg55
 Charg77:
            jbs     Flag2, No_Ntc
            jmp     Charg47
            Disi
            bs      status, Ps0
            call    Det_Ntc
            Eni
            jbc     Flag2, No_Ntc
            jmp     Charg47
            jbs     Flag2, No_Bat
            bc      Comm_W, Alm
            bs      Comm_W, Ac1
            clr     Work_Ten
            jmp     Main
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Charg8:                                ;80 小电流维持 
            jbs     Flag1, Time_ms
            jmp     Main
            bc      Flag1, Time_ms
            jbc     Flag1, Wait
            jmp     Charg85
            Disi
            bs      status, PS0
            call    Count_Curnt
            Eni
            inc     Tim_cnt
            mov     a, Tim_cnt
            xor     a, @128
            jbs     status, Z
            jmp     Slep_Pro
            bs      Flag1, Wait
            clr     Tim_cnt
            call    Get_Icc
            ;;;;;;;;;;;;;;;;;;
            mov     a, C_Reckn1
            mov     General1, a
            mov     a, @JC_Curt12
            sub     a, C_Reckn2
            jbc     status, C
            jmp     Charg81
            mov     a, @0x01
            sub     General1, a
            jbs     status, C
            jmp     Charg84             ;C<35mA
 Charg81:
            mov     a, @JC_Curt11
            sub     a, General1
            jbs     status, C
            jmp     Charg84
            ;;;;;;;;;;;;;;;;
            mov     a, C_Reckn1
            mov     General1, a
            mov     a, @JC_Curt22       ;55mA
            sub     a, C_Reckn2
            jbc     status, C
            jmp     Charg82
            mov     a, @0x01
            sub     General1, a
            jbs     status, C
            jmp     Slep_Pro
 Charg82:                               ;C>55mA
            mov     a, @JC_Curt21       ;55mA
            sub     a, General1
            jbs     status, C
            jmp     Slep_Pro
            Disi
            bs      status, PS0
            call    Inc_Pulse           ;HTim-2 
            Eni
            jmp     Slep_Pro
 Charg84:
            Disi
            bs      status, PS0
            call    Dec_Pulse           ;HTim+2 
            Eni
            jmp     Slep_Pro
 Charg85:                               ;Wait
            inc     Tim_cnt
            mov     a, @Wait1
            sub     a, Tim_cnt
            jbs     status, C
            jmp     Slep_Pro
            bc      Flag1, Wait
            clr     Tim_cnt
            clr     C_Reckn3
            clr     C_Reckn2
            clr     C_Reckn1
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Slep_Pro:                              ;Work_Ten=90 检测，等待 
 Labor:                                 ;Work_Ten=A0
            Disi
            bs      status, Ps0
            call    Pro_Bat
            Eni
            jbs     Flag2, No_Bat
            jmp     Labor1
            bs      Comm_W, Alm
            bc      Comm_W, Fls_D
            bs      P6, Led3
            jmp     Charg4A
 Labor1:
            jbs     Flag2, No_Ntc
            jmp     Labor2
            Disi
            bs      status, Ps0
            call    Det_Ntc
            Eni
            jbc     Flag2, No_Ntc
            jmp     Labor2
            jbs     Flag2, No_Bat
            bc      Comm_W, Alm
            bs      Comm_W, Ac1
            clr     Work_Ten
            jmp     Main
 Labor2:
            mov     a, @0x1E            ;电池电压<15.67V?
            sub     a, Dely_W
            jbc     status, c
            jmp     Main
            jbc     Comm_W, Temp_ov     ;Temp<Max_T?
            jmp     Main
            mov     a, @0x10
            mov     Work_Ten, a         ;重新开始充电 
            jmp     Main 
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 dely:
            nop
            djz     General1
            jmp     dely
            ret
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Get_Icc:                               ;电流平均 
            rlc     C_Reckn3
            rlc     C_Reckn2
            rlc     C_Reckn1
            jbs     C_Reckn3, B7
            jmp     Get_Icc1
            mov     a, @0x01
            add     C_Reckn2, a
            jbc     status, C
            inc     C_Reckn1
 Get_Icc1:
            mov     a, @0x0F
            and     C_Reckn1, a
            ret
    ;==============================================;
    ;        ORG     0X400                         ;Sub Prog
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ;=============================================;
    ;        A                       ;
    ;=============================================;
    
    
    
    ;=============================================;
            ORG     0X400                         ;Sub Prog
             
             
 Table:
            add     R2, a
            retl    0x48
            retl    0x0C                ;0--00 C
            
            retl    0x0B
            retl    0x0B                ;1--08 C
            
            retl    0xA5
            retl    0x09                ;2--16 C
            
            retl    0x2F
            retl    0x08                ;3--24 C
            
            retl    0xCD
            retl    0x06                ;4--32 C
            
            retl    0x8D
            retl    0x05                ;5--40 C
            
            retl    0x77
            retl    0x04                ;6--48 C
            
            retl    0x8F
            retl    0x03                ;7--56 C
            
            retl    0xD5
            retl    0x02                ;8--64 C
            
            retl    0x3B
            retl    0x02                ;9--72 C
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Get_Temp:                              ;计算温度 
            bs      Flag1, Pag
            Eni
            clr     General1
            clr     General2
            clr     General3
            clr     General4
            clr     General5
            clr     General6
            clr     General7
            clr     General8
 Get_Temp1:
            mov     a, General1
            call    Table
            inc     General1
            mov     General3, a
            mov     General5, a         ;low-->small
            mov     a, General1
            call    Table
            inc     General1
            mov     General4, a
            mov     General6, a         ;high-->large
            mov     a, Temp21
            sub     a, General3
            jbc     status, c
            jmp     Get_Temp2
            mov     a, @0x01
            sub     General4, a
            jbs     status, c
            jmp     Get_Temp3
 Get_Temp2:
            mov     a, Temp22
            sub     a, General4
            jbs     status, c
            jmp     Get_Temp3
            mov     a, General5         ;data>temp continue
            mov     General7, a
            mov     a, General6
            mov     General8, a
            inc     General2
            mov     a, General2
            xor     a, @0x0A
            jbs     status, z
            jmp     Get_Temp1
 Get_Temp3:
            mov     a, General2
            jbc     status, Z
            jmp     Get_End
            dec     General2
            mov     a, General8 
            mov     General4, a
            mov     a, General5
            sub     a, General7
            mov     General3, a
            jbc     status, c
            jmp     Get_Temp4
            mov     a, @0x01
            sub     General4, a
 Get_Temp4:           
            mov     a, General6
            sub     General4, a         ;General8,7-General6,5-->General4,3
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;DT
            mov     a, Temp21
            sub     General7, a
            jbs     status, c
            dec     General8
            mov     a, Temp22
            sub     General8, a         ;General8,7-Temp-->General8,7
            
            mov     a, @0x04
            mov     General1, a
 Get_Temp5:                             ;;;;;
            mov     a, @General3
            mov     R4, a
            call    Data_Proc
            mov     a, General8
            mov     General6, a
            mov     a, General7
            mov     General5, a
            
            mov     a, Byte1
            sub     General5, a
            jbc     status, c
            jmp     Get_Temp6
            mov     a, @0x01
            sub     General6, a
            jbs     status, c
            jmp     Get_Temp7           ;General8,7<Byte2,1
 Get_Temp6:
            mov     a, Byte2
            sub     General6, a
            jbs     status, c
            jmp     Get_Temp7
            
            rlca    General2            ;General8,7>Byte2,1
            or      a, @0x01
            mov     General2, a
            mov     a, General5
            mov     General7, a
            mov     a, General6
            mov     General8, a
            
            mov     a, Byte1
            sub     General3, a 
            jbs     status, c
            dec     General4 
            mov     a, Byte2
            sub     General4, a 
            jmp     Get_Temp8
 Get_Temp7:                             ;General8,7<Byte2,1
            rlca    General2 
            and     a, @0xFE
            mov     General2, a 
            
            mov     a, Byte1
            mov     General3, a 
            mov     a, Byte2
            mov     General4, a 
 Get_Temp8:
            djz     General1
            jmp     Get_Temp5
           ; bc      Status, c
           ; rrc     General2
           ; jbc     status, C
           ; inc     General2
 Get_End: 
            disi
            bc      status, Ps0 
            bc      Flag1, Pag 
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Data_Proc:                            ;数据差补 
            rlca    R0
            mov     Byte1, a
            inc     R4
            rlca    R0
            mov     Byte2, a
            rlc     Byte1
            rlc     Byte2
            rlc     Byte1
            rlc     Byte2
            mov     a, @0xF8
            and     Byte1, a
            dec     R4
            mov     a, R0
            add     Byte1, a
            inc     R4
            jbc     status, c
            inc     Byte2
            mov     a, R0
            add     Byte2, a
            swapa   Byte1
            and     a, @0x0F
            mov     Byte1, a
            swapa   Byte2
            and     a, @0xF0
            add     Byte1, a
            swapa   Byte2
            and     a, @0x0F
            mov     Byte2, a
            ret
    ;=============================================;
    ;        Increase H_Pulse Width               ;
    ;=============================================;
 Inc_Pulse:                             ;增加PWM占空比 
            bs      Flag1, Pag
            Eni
            mov     a, R4
            mov     General3, a
            
            bs      status, Iocs
            jbc     Flag1, Inc_P
            jmp     Inc_Pulse2
            bs      Flag1, Inc_P
            
            ior     IocA1               ;Low Time
            xor     a, @Max_Plus
            jbc     status, Z
            jmp     Inc_Pulse3
            ior     IocA1
            add     a, @0x01
            iow     IocA1
            jmp     Inc_Pulse3
 Inc_Pulse2:
            bc      Flag1, Inc_P
            ior     ioc91
            xor     a, @Min_Plus
            jbc     status, Z
            jmp     Inc_Pulse3
            ior     Ioc91
            add     a, @0xFF
            iow     Ioc91
 Inc_Pulse3:
            mov     a, General3
            mov     R4, a 
            jmp     Get_End
    ;=============================================;
    ;        Decrease H_Pulse Width               ;
    ;=============================================;
 Dec_Pulse:                             ;减小PWM占空比 
            bs      Flag1, Pag
            Eni
            mov     a, R4
            mov     General3, a
            bs      status, Iocs
            
            jbc     Flag1, Inc_P
            jmp     Dec_Pulse1
            bs      Flag1, Inc_P
            ior     Ioc91               ;Low Time
            xor     a, @Max_Plus
            jbc     status, Z
            jmp     Dec_Pulse3
            ior     Ioc91
            add     a, @0x01
            iow     Ioc91
            jmp     Dec_Pulse3
 Dec_Pulse1:
            bc      Flag1, Inc_P
            ior     iocA1
            xor     a, @Min_Plus
            jbc     status, Z
            jmp     Dec_Pulse3       ;2
            ior     IocA1
            add     a, @0xFF
            iow     IocA1
 Dec_Pulse3:
            mov     a, General3
            mov     R4, a 
            jmp     Get_End
    ;=============================================;
    ;        Account Current                      ;
    ;=============================================;
 Count_Curnt:                           ;电流累计 
            bs      Flag1, Pag
            Eni
            mov     a, Curnt21
            add     C_Reckn3, a
            jbs     status, c
            jmp     Count_Curnt1
            mov     a, @0x01
            add     C_Reckn2, a
            jbs     status, c
            jmp     Count_Curnt1
            mov     a, @0x01
            add     C_Reckn1, a
 Count_Curnt1:
            mov     a, Curnt22
            add     C_Reckn2, a
            jbs     status, c
            jmp     Count_Curnt2
            mov     a, @0x01
            add     C_Reckn1, a
 Count_Curnt2:
            jmp     Get_End
    ;=============================================;
    ;        detect Bat_V                         ;
    ;  12.3, 12.8, 13.3, 13.8, 14.3, 14.8, 15.3   ;
    ;  15.8, 16.3, 16.8, 17.3, 17.8, 18.3, 18.8   ;
    ;=============================================;
 Detect_B:                              ;电池电压计算 
            bs      Flag1, Pag
            Eni
            bc      R4, B6              ;select bank1
            clr     Dely_W
            mov     a, BatV21
            mov     General1, a
            mov     a, BatV22
            mov     General2, a
            mov     a, @Bat_060B
            sub     General1, a
            jbc     status, C
            jmp     Detect_B1
            mov     a, @0x01
            sub     General2, a
            jbs     status, C
            jmp     Detect_B3
 Detect_B1:
            mov     a, @Bat_060A
            sub     General2, a
            jbs     status, C
            jmp     Detect_B3 
 Detect_B2:
            inc     Dely_W
            mov     a, @0x3F
            sub     General1, a
            jbc     status, C
            jmp     Detect_B2
            mov     a, @0x01
            sub     General2, a
            jbc     status, C
            jmp     Detect_B2
 Detect_B3:
            jmp     Get_End
    ;=============================================;
    ;        detect AC_IN                         ;
    ;  13,   13.6, 14.2, 14.8, 15.4, 16,   16.6   ;
    ;  17.2, 17.8, 18.4, 19,   19.6, 20.2, 20.8   ;
    ;  21.4, 22,   22.6, 23.2, 23.8, 24.4, 25     ;
    ;=============================================;
 Detect_AC:                             ;交流电压计算 
            bs      Flag1, Pag
            Eni
            bc      R4, B6              ;select bank1
            clr     Dely_W1
            mov     a, AC_In21
            mov     General1, a
            mov     a, AC_In22
            mov     General2, a
            mov     a, @Ext_V13B
            sub     General1, a
            jbc     status, C
            jmp     Detect_AC1
            mov     a, @0x01
            sub     General2, a
            jbs     status, C
            jmp     Detect_AC3
 Detect_AC1:
            mov     a, @Ext_V13A
            sub     General2, a
            jbs     status, C
            jmp     Detect_AC3 
 Detect_AC2:
            inc     Dely_W1
            mov     a, @0x27
            sub     General1, a
            jbc     status, C
            jmp     Detect_AC2
            mov     a, @0x01
            sub     General2, a
            jbc     status, C
            jmp     Detect_AC2
 Detect_AC3:
            jmp     Get_End
    ;=============================================;
    ;        detect AC_IN                         ;
    ;  13,   13.6, 14.2, 14.8, 15.4, 16,   16.6   ;
    ;  17.2, 17.8, 18.4, 19,   19.6, 20.2, 20.8   ;
    ;  21.4, 22,   22.6, 23.2, 23.8, 24.4, 25     ;
    ;=============================================;
 Get_Dat:                               ;取样数据整理 
            bs      Flag1, Pag
            Eni
            clr     BatV1 
            clr     BatV2 
            clr     AC_IN1 
            clr     AC_IN2 
            clr     Curnt1 
            clr     Curnt2 
            clr     Temp1 
            clr     Temp2 
            
            mov     a, @0x04
            mov     Dely_W, a
            mov     a, @0x60
            mov     R4, a               ;Ram Bank1 R20
 Get_Dat1:
            mov     a, R0
            add     BatV1, a
            jbc     status, c
            inc     BatV2
            inc     R4
            mov     a, R0
            add     BatV2, a 
            inc     R4
            
            mov     a, R0
            add     AC_IN1, a
            jbc     status, c
            inc     AC_IN2
            inc     R4
            mov     a, R0
            add     AC_IN2, a
            inc     R4
            
            mov     a, R0
            add     Curnt1, a
            jbc     status, c
            inc     Curnt2
            inc     R4
            mov     a, R0
            add     Curnt2, a
            inc     R4
            
            mov     a, R0
            add     Temp1, a
            jbc     status, c
            inc     Temp2
            inc     R4
            mov     a, R0
            add     Temp2, a
            inc     R4
            djz     Dely_W 
            jmp     Get_Dat1
            ;;;;;;;;;;;;;;;;
            bc      R4, B6               ;Ram Bank0
            
            bc      status, C
            rrc     BatV2 
            rrc     BatV1 
            bc      status, C
            rrca    BatV2
            mov     BatV22, a
            rrca    BatV1 
            mov     BatV21, a
            jbs     status, c
            jmp     Get_Dat2
            mov     a, @0x01
            add     BatV21, a
            jbc     status, c
            inc     BatV22
 Get_Dat2: 
            bc      status, C
            rrc     AC_IN2 
            rrc     AC_IN1 
            bc      status, C
            ;;;;;;;;;;;;;;;;;;
            rrca    AC_IN2
            mov     AC_IN22, a
            rrca    AC_IN1 
            mov     AC_IN21, a
            jbs     status, c
            jmp     Get_Dat3
            mov     a, @0x01
            add     AC_IN21, a
            jbc     status, c
            inc     AC_IN22
 Get_Dat3:
            bc      status, C
            rrc     Curnt2 
            rrc     Curnt1 
            bc      status, C
            ;;;;;;;;;;;;;;;;;;
            rrca    Curnt2 
            mov     Curnt22, a
            rrca    Curnt1 
            mov     Curnt21, a
            jbs     status, c
            jmp     Get_Dat4
            mov     a, @0x01
            add     Curnt21, a
            jbc     status, c
            inc     Curnt22
 Get_Dat4:
            bc      status, C
            rrc     Temp2 
            rrc     Temp1
            bc      status, C
            ;;;;;;;;;;;;;;;;;;
            rrca    Temp2 
            mov     Temp22, a
            rrca    Temp1 
            mov     Temp21, a
            jbs     status, c
            jmp     Get_Dat5
            mov     a, @0x01
            add     Temp21, a
            jbc     status, c
            inc     Temp22
 Get_Dat5:        ;
            jmp     Get_End
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Detect_Bat:                            ;有电池检测 
            bs      Flag1, Pag
            Eni
            mov     a, @0x04
            sub     a, BatV22
            jbs     status, C
            jmp     Detect_Bat2
            inc     HavBa_Tim
            jbs     status, Z
            jmp     DetBat_End
            bc      Flag2, No_Bat
            jmp     DetBat_End
            ;;;;;;;;;;;;;;;;;;
 Detect_Bat2:
            clr     HavBa_Tim
 DetBat_End:
            jmp     Get_End
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Pro_Bat:                                         ;无电池检测 
            bs      Flag1, Pag
            Eni
            mov     a, Curnt22
            jbs     status, Z                     ;No_Bat?
            jmp     Pro_Bat1
            mov     a, @0x20
            sub     a, Curnt21
            jbc     status, C
            jmp     Pro_Bat1
            bs      status, Iocs
            ior     Ioc91                         ;Low_time -->a
            bc      status, Iocs
            sub     a, @0x80     
            jbc     status, C
            jmp     Pro_Bat1
            dec     HavBa_Tim                     ;mov     a, HavBa_Tim
            jbs     status, Z
            jmp     ProBat_End
            bs      Flag2, No_Bat
            jmp     ProBat_End
 Pro_Bat1:
            clr     HavBa_Tim
 ProBat_End:
            jmp     Get_End
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Pro_Ntc:                                         ;无NTC检测 
            bs      Flag1, Pag
            Eni
            mov     a, @0x0B
            sub     a, Temp22
            jbs     status, C
            jmp     Pro_Ntc1                      ;Temp22 <0Bh
            inc     NoNt_Tim
            jbc     status, Z
            bs      Flag2, No_Ntc
            jmp     Pro_Ntc2
 Pro_Ntc1:
            clr     NoNt_Tim
 Pro_Ntc2:
            jmp     Get_End
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Det_Ntc:                                         ;有NTC检测 
            bs      Flag1, Pag
            Eni
            mov     a, @0x0B
            sub     a, Temp22
            jbc     status, C
            jmp     Det_Ntc1                      ;Temp22 >0Bh
            dec     NoNt_Tim
            jbc     status, Z
            bc      Flag2, No_Ntc
            jmp     Det_Ntc2
 Det_Ntc1:
            clr     NoNt_Tim
 Det_Ntc2:
            jmp     Get_End
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
            
            END