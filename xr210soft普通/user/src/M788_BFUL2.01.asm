	;*********************************************************************;
	;  TILTE:     M788回充座控制电路MCU程序                               ;
	;  FUNCTION:  1、在未进行充电时发送引导红外编码信号，引导M788寻找回充 ;
	;                座，实现自动充电。                                   ； 
    ;             2、在有负载机器充电的时候，根据时间及设置的情况向M788及 ;
    ;                时的发出FULLGO命令，实现自动清扫功能。               ； 
    ;             3、在没有负载充电的时候，能够通过按键对参数进行设置。   ; 
    ;             4、在没有馈电的情况下，能够通过按FullGo键启动充电过程。 ; 
	;  COMPANY:       深圳市银星智能电器有限公司 研发部               ;
	;                2008-10-09更新                                       ;
	;*********************************************************************;

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
        IR2    ==     0x00
        Curent ==     0x01
        D_CS   ==     0x02
        D_wr   ==     0x03
        Ref    ==     0x04
        OSI    ==     0x05
        
     
    P6         equ    0x06 
    R6         equ    0x06
        Data   ==     0x00              ;Clock
        Key0   ==     0x01
        Key1   ==     0x02
        Key2   ==     0x03
        ExVot  ==     0x04
        JD1    ==     0x05
        IR1    ==     0x06
        IR3    ==     0x07
     
    P7         equ    0x07 
    R7         equ    0x07
        OSCO   ==     0x00
    
	;======================================================;
	; RAM Define                                           ;
	;======================================================; 
    Rb1        equ    0x10                        ;通用寄存器，用于延时 
	Rb2        equ    0x11                        ;通用寄存器，用于长延时，临时计数 
	TempW      equ    0x11                        ;重复使用，临时计数 
	R4_Bak     equ    0x12                        ;间接指针备份 
	Add_Bak    equ    0x13                        ;累加器备份 
	
	R3_Bak     equ    0x14                        ;状态备份 
	Flag1      equ    0x15                        ;标志1
    Flag2      equ    0x16                        ;标志2
    Flag3      equ    0x17                        ;标志3
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    General1   equ    0x18                        ;通用寄存器 
    General2   equ    0x19                        ;通用寄存器 
    General3   equ    0x1A                        ;通用寄存器 
    General4   equ    0x1B                        ;通用寄存器 
    
    P5_Dat     equ    0x1C
	P6_Dat     equ    0x1D
    Setg_T     equ    0x1E
    Set_Nb     equ    0x1F
    ;========== Bank 0 ===========================;
    ;          equ    0x20                        ;程序进程 
    ExV_Cnt    equ    0x21                        ;
    LoadCnt    equ    0x22                        ;显示命令 
    KeyVol     equ    0x23                       ;毫秒计数 
    
    Key_Cunt   equ    0x24                        ;分钟计数 
    ;Tim_Det   equ    0x25                        ;测试电流计数 ?
    ;          equ    0x26                        ;有回馈电压计时 
    ;          equ    0x27                        ;发现负载即计时 
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    Adc_Bas0   equ    0x28                        ;电流零值 
    Adc_Bas1   equ    0x29                        ;
    ;VCutH      equ    0x2A
    ;VCutL      equ    0x2B
    
    ADC_Buf0   equ    0x2C 
    ADC_Buf1   equ    0x2D 
    ADC_Buf2   equ    0x2E 
    ADC_Buf3   equ    0x2F 
    ;========== Bank 0 ====================================;
    Bdata1     equ    0x30                        ;IR数据位值  30h
    Bdata2     equ    0x31                        ;
    Bdata3     equ    0x32                        ;
    Wdata1     equ    0x33                        ;IR数据字值 
    
    Wdata2     equ    0x34                        ;
    Wdata3     equ    0x35                        ;
    Bcont      equ    0x36                        ;IR位计数 
    Wcont      equ    0x37                        ;IR字计数 
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    IR_Cnt     equ    0x38                        ;IR脉冲计数 38h
    ;           equ    0x39                        ;
    ;           equ    0x3A                        ;
    ;           equ    0x3B                        ;
    
    ;           equ    0x3C                        ;
    ;           equ    0x3D                        ;
    ;           equ    0x3E                        ;
    ;           equ    0x3F                        ;
	;========== Bank 1 ====================================;
    ;Bdata1     equ    0x20                        ;IR数据位值  68h
    ;Bdata2     equ    0x21                        ;
    ;Bdata3     equ    0x22                        ;
    ;Wdata1     equ    0x23                        ;IR数据字值 
    
    ;Wdata2     equ    0x24                        ;
    ;Wdata3     equ    0x25                        ;
    ;Bcont      equ    0x26                        ;IR位计数 
    ;Wcont      equ    0x27                        ;IR字计数 
    
    SWeek_Cnt  equ    0x28                        ;设置星期计数 
    FulWeek    equ    0x29                        ;定时星期设置 
    Gweek      equ    0x2A                        ;定时星期 
    Addr       equ    0x2B                        ;
    Second     equ    0x2C                        ;
    Minut      equ    0x2D                        ;分 
    Hour       equ    0x2E                        ;小时 
    Week       equ    0x2F                        ;
    ;======================================================; 
    Str_Nb     equ    0x30                        ;70H
    
    Disp_B0    equ    0x31 
    Disp_B1    equ    0x32 
    Disp_B2    equ    0x33 
    Disp_B3    equ    0x34 
    Disp_B4    equ    0x35 
    Disp_B5    equ    0x36 
    Disp_B6    equ    0x37 
    
    ;======================================================; 
    Comad      equ    0x38                        ;78H
    Gminut     equ    0x39                        ;
    Ghour      equ    0x3A 
    Str_Val    equ    0x3B 
	
    Time_C1    equ    0x3C                        ;5ms计数 
    Sminut     equ    0x3D                        ;显示分钟 
    Shour      equ    0x3E                        ;显示小时 
    Sweek      equ    0x3F                        ;显示星期 
	;======================================================;
	; Flag Define                                          ;
	;======================================================;
	;==============Flag1===================================; 
    Tim_5ms    equ    0x00                        ;5毫秒标志 
    Flash_F    equ    0x01                        ;500毫秒标志，闪烁标志 
    Pow_Off    equ    0x02                        ;设备处于有电关闭状态 
    TccSig     equ    0x03                        ;Tcc中断单次标志（IR脉冲用） 
    Wdt_rst    equ    0x04                        ;WDT复位 
    FSecond    equ    0x05                        ;时钟秒标志，秒到 
    AD_Ok      equ    0x06                        ;AD转换完成 
    GoCmd      equ    0x07                        ;定时有效 
    ;==============Flag2==========================;
    IRstop     equ    0x00                        ;停止IR发送标志 
    HavLoad    equ    0x01                        ;有负载标志 
    FGCmd      equ    0x02                        ;发送FullGo命令标志 
    GoVer      equ    0x03                        ;今天已经打扫过的标志 
    Seting     equ    0x04                        ;正在设定过程中标志 
    FullGo     equ    0x05                        ;有FullGo状态标志 
    Hav_disp   equ    0x06                        ;有显示更新标志 
    Can_Det    equ    0x07                        ;可以测试负载状态标志 
    
    ;============== Flag3 ========================;
    Timing     equ    0x00                        ;有定时标志 
    HaveKey    equ    0x01                        ;有键操作标志 
    Set_Go     equ    0x02                        ;
    FlashB     equ    0x03                        ;
    Week_G     equ    0x04                        ;
    PageF      equ    0x05                        ;
    Set_Wb     equ    0x06                        ;
    ;Temp_ov    equ    0x07                       ;超温 
    ;=============================================;
	; constant Define                             ;
    ;=============================================;
    IR_Stp     equ    140                         ;初始检测等待次数 
    ADC_Nb     equ    0x10
    Init_t     equ    100
    Ex_Load    equ    0x20
    
    KeyNb1     equ    10                          ;10*5 =50ms
    KeyNb2     equ    200                         ;200*5=1000ms
    step_Key   equ    50                          ;50*5 =250ms
    
    Set_Time   equ    100
    DetectNb   equ    10
    
    
    ;=============================================; 
    ;============================================== 
            ORG     0X000                         ;Reset
            
            ;;;;;;;;;;;;;;;;;
            mov     a, status
            mov     R3_Bak, a
            disi
            bs      status, PS0
            call    PortInit
            call    delay_10ms
            bs      status, PS0
            call    PortInit
            bs      status, PS0
            call    ClearERam
            JMP     START
         ;   add     a, @0xff
            
    ;==============================================  
    ;============================================== 
    ;        ORG     0X009                         ;Tcc
            
    ;        nop
    ;        nop
    ;=============================================== 
    ; ADC中断： 根据要求在有负载时进行转换，为检测 ;
    ;           电流提供数据。数据处理在主程序进行 ;
    ;===============================================
            ORG     0X00C                         ;ADC
            
            
            bs      Flag1, Ad_Ok
            bc      RE, ADif
            jbc     Flag3, PageF
            bs      status, PS0
            reti
    ;==============================================
            ORG     0X0011
 Tccb1:
            bc      RF, TccBif
            jbc     Flag3, PageF
            bs      status, PS0
            reti
    ;============================================== 
            ORG     0X001B                        ;TccB
            
            ;;;;;;;;;;;;;;;;;
            bs      Flag1, Tim_5ms
            bc      Status, Ps0
            jmp     Tccb1
    ;==============================================
    ;==============================================
    ;===============================================
    ;TccC中断:用于产生38K载波频率，发送红外编码信号;
    ;                                              ;
    ;=============================================== 
            ORG     0X001E                        ;TccC
            
            ;;;;;;;;;;;;;;;;;
 TccC_INT:                                        ;1/38K  定时，提供时钟基准 
            mov     Add_bak, a
            mov     a, Status
            mov     R3_Bak, a
            bc      status, PS0
            mov     a, R4
            mov     R4_bak, a
            bc      RF, TccCif
            
            bc      R4, B6                        ;select Bank0   
            jbc     Flag2, IRstop
            jmp     Tcc_IR4
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;IR
            bc      P6_Dat, IR1
            bc      P5_Dat, IR2
            bc      P6_Dat, IR3                   ;13
            
            jbs     Flag1, TccSig
            jmp     TccC_IR1
            bc      Flag1, TccSig
            
            jbc     Bdata1, 6                     ;IR1 Top
            bs      P6_Dat, IR1                   ;P6_Bak, IR1
            jbc     Bdata2, 6                     ;IR2 Right
            bs      P5_Dat, IR2                   ;P5_Bak, IR2
            jbc     Bdata3, 6                     ;IR3 Left
            bs      P6_Dat, IR3                   ;P6_Bak, IR3
            jmp     TccC_IR7
 TccC_IR1:
            bs      Flag1, TccSig
            
            djz     IR_Cnt
            jmp     TccC_IR7
            mov     a, @27                        ;27/38k=0.7mS
            mov     IR_Cnt, a
            rlc     Bdata1                        ;Top
            rlc     Bdata2                        ;Right
            rlc     Bdata3                        ;left
            
            djz     Bcont
            jmp     TccC_IR7
            mov     a, @05
            mov     Bcont, a
            
            rlc     Wdata1                        ;Top
            mov     a, @0b01000000
            jbc     Wdata1, 7
            mov     a, @0b01111000                ;
            mov     Bdata1, a
            
            rlc     Wdata2                        ;Right
            mov     a, @0b01000000
            jbc     Wdata2, 7
            mov     a, @0b01111000                ;
            mov     Bdata2, a
            
            rlc     Wdata3                        ;Left
            mov     a, @0b01000000
            jbc     Wdata3, 7
            mov     a, @0b01111000                ;
            mov     Bdata3, a
            
            djz     WCont
            jmp     TccC_IR7
            bs      Flag2, IRstop
            mov     a, @IR_STP                    ;140*0.72=100mS
            mov     BCont, a
            jmp     TccC_IR7
            ;;;;;;;;;;;;;;;;;;;;;;;
 Tcc_IR4:                                         ;间隔 
            bc      P6_Dat, IR1
            bc      P5_Dat, IR2
            bc      P6_Dat, IR3
            
            djz     IR_Cnt
            jmp     TccC_IR7
            mov     a, @27
            mov     IR_Cnt, a
            djz     Bcont
            jmp     TccC_IR7
            ;;;;;;;;;;;;;;;;;;;;;;;               ;IR重新开始 
            mov     a, @05
            mov     Bcont, a
            bc      Flag2, IRstop
            mov     a, @08
            mov     Wcont, a
            
            mov     a, @0b01111000
            mov     Bdata1, a
            mov     Bdata2, a
            mov     Bdata3, a
            
            jbs     Flag1, GoCmd
            jmp     TccC_IR5
            mov     a, @0b11111010                ;Go Command
            mov     Wdata1, a
            mov     Wdata2, a
            mov     Wdata3, a
            jmp     TccC_IR7
            ;;;;;;;;;;;;;;;;;;;;;;;
 TccC_IR5:
            jbs     Flag2, FGCmd
            jmp     TccC_IR6
            mov     a, @0b11111110                ;FG Command
            mov     Wdata1, a
            mov     Wdata2, a
            mov     Wdata3, a
            jmp     TccC_IR7
 TccC_IR6:
            mov     a, @0b11110001
            mov     Wdata1, a
            mov     a, @0b11111000
            mov     Wdata2, a
            mov     a, @0b11110100
            mov     Wdata3, a
 TccC_IR7:
            mov     a, P5_Dat
            mov     P5, a
            mov     a, P6_Dat                     ;P6_bak
            mov     P6, a
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 TccC_end:
            mov     a, R4_Bak
            mov     R4, a
            mov     a, R3_Bak
            mov     Status, a
            mov     a, Add_Bak 
            jbc     Flag3, PageF
            bs      status, PS0
            reti
    ;--------------------------------------------------;
    ;	   
    ;--------------------------------------------------;
 START:
            ;;;;;;;Proc Wdt;;;;;;;;;;;;;;         ;复位模式判断 
            bs      status, PS0
            call    ClearGRam
            disi
            bs      status, PS0
            call    SysInit
            bs      status, PS0
            call    ParaInit
            bc      Flag1, Wdt_rst
            bs      status, PS0
            call    PortInit
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
         ;   call    delay_10ms
         ;   clr     General3            ;初始化 
         ;   clr     General4
            
  ;;;;;;;;;;;;  Inital LCD Display  ;;;;;;;;;;;;;;;;
            disi
            bs      P5, D_Cs
            bs      P5, D_Wr
            bs      P6, Data
            call    delay_10ms
            bs      R4, B6                        ;bank1
            
            mov     a, @0b00000010                ;off Lcd
            mov     Comad, a
            call    sent_comd
            call    delay_10ms
            
            mov     a, @0b00000001                ;SYS ON
            mov     Comad, a
            call    sent_comd
            call    delay_10ms
            
            mov     a, @0b00101001                ;Bias 1/3  4Common
            mov     Comad, a
            call    sent_comd
            call    delay_10ms
            
            mov     a, @0b00011000                ;RC 256KHz
            mov     Comad, a
            call    sent_comd
            call    delay_10ms
            
            mov     a, @0b00000011                ;on Lcd
            mov     Comad, a
            call    sent_comd
            call    delay_10ms
            eni
            
            bs      R4, B6
            bs      Disp_B5, 3                    ;L22 CLK
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            mov     a, @50                        ;进入设置模式检测 
            mov     General3, a
            bc      R4, B6
 Detect_FG:
            call    delay                         ;_10ms
            disi
            bs      status, PS0
            call    Key_scan
            eni
            jbc     Flag3, HaveKey
            jmp     Detect_FG1
            djz     General3
            jmp     Detect_FG
            jmp     MainStr
 Detect_FG1:
            bc      Flag3, HaveKey
            mov     a, KeyVol
            xor     a, @05                        ;FullG键?
            jbs     Status, Z
            jmp     Detect_FG
            
            disi
            bs      status, PS0
            call    Disp_all                      ;??????
            eni
 Detect_FG2:                                      ;wdtc
            call    delay_10ms
            jmp     Detect_FG2
            ;;;;;;;;;;;;;;;;;;;;;;;
 MainStr:                                         ;电流基值采样 
            bc      R4, B6                        ;Bank0
            call    delay1ms
            djz     LoadCnt                       ;等待256*1ms=256ms
            jmp     MainStr
            ;;;;;;;;;;;;;;;;;;;;;;;
            clr     ADC_Buf0
            clr     ADC_Buf1
            clr     ADC_Buf2
            clr     ADC_Buf3
            bc      Flag1, Ad_Ok
 Adc_Int:                                         ;启动AD转换 
            bc      Re, Adif                      ;屏蔽AD中断 
            bs      R9, Adrun                     ;启动AD转换 
            ;;;;;;;;;;;;;;;;;;;;;;;
 Adc_Int1:                                        ;启动AD转换 
            jbs     Flag1, Ad_Ok                  ;检测有负载后，负载离去 
            jmp     Adc_Int1
            bc      Flag1, Ad_Ok
            
            mov     a, RD
            add     ADC_Buf0, a
            jbc     status, C
            inc     ADC_Buf1
            mov     a, RC
            and     a, @0x0f
            add     ADC_Buf1, a
            ;;;;;;;;;
            inc     LoadCnt
            mov     a, @DetectNb
            xor     a, LoadCnt                    ;延时3*256*5ms=3840ms
            jbs     Status, Z
            jmp     Adc_Int
            clr     LoadCnt
            
            mov     a, ADC_Buf1
            and     a, @0x0f
            mov     Adc_Bas1, a
            mov     a, ADC_Buf0
            mov     Adc_Bas0, a
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ;;;              主程序，主循环                ;;;
 ;;;;;;;;;;;;;;;;;  Program Main  ;;;;;;;;;;;;;;;;;
 Main:             
            jbs     Flag1, Tim_5ms
            jmp     Main
            bc      Flag1, Tim_5ms
            Wdtc
            
            bs      R4, B6                        ;Bank1
            jbs     Flag2, Hav_disp
            jmp     Main11
            jbc     Flag1, Pow_off
            jmp     Main11
            jbs     Flag2, IRstop                 ;发射间隔时，修改显示 
            jmp     Main11
            bc      Flag2, Hav_disp
            disi
            bs      status, PS0
            call    Writ_Dat                      ;写显示 
            eni
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main11:                                          ;计时  ;jmp     main
            bs      R4, B6                        ;Bank1
            inc     Time_C1
            mov     a, @50                        ;250mS
            xor     a, Time_C1
            jbc     Status, Z
            jmp     RunTime1
            ;;;;;;;;;;;;;;;;;;;;;;;
            mov     a, @150                       ;750mS
            xor     a, Time_C1
            jbc     Status, Z
            jmp     RunTime1
            ;;;;;;;;;;;;;;;;;;;;;;;
            mov     a, @100                       ;500mS
            xor     a, Time_C1
            jbs     Status, Z
            jmp     RunTime2
            bc      Disp_B2, 6                    ;秒点灭 
            
 RunTime1:
            bs      Flag1, Flash_F
            jmp     Disp_Tim3
            ;;;;;;;;;;;;;;;;;;;;;;;
 RunTime2:
            mov     a, @200
            xor     a, Time_C1
            jbs     Status, Z
            jmp     Main12
            clr     Time_C1
            
            bs      Flag1, FSecond
            bs      Flag1, Flash_F
            bs      Disp_B2, 6                    ;秒点亮 
            bs      status, PS0
            disi
            call    Time_Add
            eni
            jbc     Flag2, Seting                 ;非设定状态 ?
            jmp     main3                         ;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Disp_Tim2:                                       ;写显缓 
            mov     a, week
            mov     Sweek, a
            mov     a, hour
            mov     Shour, a
            mov     a, minut
            mov     Sminut, a
            ;;;;;;;;;;;;;;;;;;;;;;;
 Disp_Tim3:                                       ;
            bs      R4, B6                        ;Bank1
            clr     TempW
            swapa   Shour
            and     a, @0x0f
            mov     Str_Val, a
            call    Disp_Str                      ;显示时 
           
            inc     TempW
            mov     a, Shour
            and     a, @0x0f
            mov     Str_Val, a
            call    Disp_Str
            ;;;;;;;;;;;;;;;;;;;;;;;
            inc     TempW
            swapa   Sminut
            and     a, @0x0f
            mov     Str_Val, a
            call    Disp_Str                      ;显示分 
           
            inc     TempW
            mov     a, Sminut
            and     a, @0x0f
            mov     Str_Val, a
            call    Disp_Str
            ;;;;;;;;;;;;;;;;;;;;;;;
            jbs     Flag3, Set_Go
            call    Disp_Week                     ;显示星期 
            bs      Flag2, Hav_disp
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main12:
            jbc     Flag2, Seting                 ;非设定状态 ?
            jmp     main3                         ;
            bc      R4, B6
            disi
            bs      status, PS0
            call    Key_scan                      ;键扫描 
            eni
            jbc     Flag3, HaveKey
            jmp     Main2
;*****************************************************
 Detect_Go:                                       ;检测是不是到了FullGo时间?
            jbs     Flag1, FSecond
            jmp     Detect_Load                   ;Main
            bc      Flag1, FSecond                ;每秒一次 
            
            jbc     Flag2, HavLoad                ;无负载？ 
            jmp     $+4
            bc      Flag1, GoCmd                  ;无负载 
            bc      Flag2, FGCmd
            jmp     Detect_Load
            ;;;;;;;;;;;;;;;;;;;;;;;               ;有负载 
            bs      R4, B6                        ;Bank1
            ;;;;;;;;;;;;;;;;;;;;;;;               ;定时清扫检测 
            jbc     Flag3, Timing                 ;无定时标志 ？ 
            jmp     Detect_Go3
            add     a, @0xff                      ;无定时标志 
            bc      Flag1, GoCmd                  ;清时间到标志 
            jmp     Detect_Go1
            ;;;;;;;;;;;;;;;;;;;;;;;
 Detect_Go3:                                      ;有定时清扫标志 
            jbc     Flag1, GoCmd                  ;已经有定时清扫 ？ 
            jmp     Detect_Load
            mov     a, Ghour
            xor     a, Hour
            jbs     Status, Z
            jmp     Detect_Go1                    ;Detect_Load
            mov     a, Gminut
            xor     a, minut
            jbs     Status, Z
            jmp     Detect_Go1                    ;Detect_Load
            
            disi
            bs      status, PS0
            call    Proc_Week                      ;写显示 
            eni
            jbs     Flag3, Week_G
            jmp     Detect_Go1
            
            bs      Flag1, GoCmd                  ;时间到 
            jmp     Detect_Load
 Detect_Go1:
            jbc     Flag2, FullGo
            jmp     Detect_Go2
            bc      Flag2, FGCmd
            jmp     Detect_Load
 Detect_Go2:
            bs      Flag2, FGCmd
;***************************************************** 
 Detect_Load:                                     ;检测负载 
            bc      R4, B6                        ;Bank0
            
            jbc     Flag2, HavLoad                ;没有负载？ 
            jmp     Det_Lod5
            ;;;;;;;;;;;;;;;;;;;;;;;
 Det_Lod1:                                        ;无负载时检测是否有负载 
            jbc     P6, ExVot                     ;有外部启动电压？ 
            jmp     Det_Lod2
            
            inc     ExV_Cnt
            mov     a, @Ex_Load
            xor     a, ExV_Cnt
            jbc     Status, Z
            jmp     Det_Lod4
            bs      R4, B6
            jbc     Disp_B0, 2                    ;中间箭头L15
            jmp     Main
            bs      Disp_B0, 2
            jmp     Disp_Tim2
            ;;;;;;;;;;;;;;;;;;;;;;;
 Det_Lod4:                                        ;有负载 
            bs      Flag2, HavLoad
            bc      Flag2, Can_Det                ;为AD转换准备 
            bs      P6_Dat,  JD1                  ;bs      R6, JD1
            clr     ExV_Cnt
            clr     LoadCnt
            
            bs      R4, B6
            bs      Disp_B6, 0                    ;电池标志 
            bs      Disp_B6, 1
            bs      Disp_B6, 3
            bs      Disp_B5, 7
            bc      Disp_B0, 2                    ;b0bclr       disp_B0.3
            jmp     Disp_Tim2
            ;;;;;;;;;;;;;;;;;;;;;;;
 Det_Lod2:                                        ;无外部电压 
            clr     ExV_Cnt
            bs      R4, B6
            jbs     Disp_B0, 2                    ;有负载显示标记 
            jmp     Main
            bc      Disp_B0, 2
            jmp     Disp_Tim2
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Det_Lod5:                                        ;有负载时检测负载是否离去 
            bc      R4, B6
            jbc     Flag2, Can_Det
            jmp     Det_Lod6
            djz     ExV_Cnt                       ;启动AD转换 
            jmp     Main
            inc     LoadCnt                       ;等待256*5ms=1280ms
            mov     a, @03
            xor     a, LoadCnt                    ;延时3*256*5ms=3840ms
            jbs     Status, Z
            jmp     Main
            clr     LoadCnt
            bs      Flag2, Can_Det
            clr     ADC_Buf0
            clr     ADC_Buf1
            clr     ADC_Buf2
            clr     ADC_Buf3
            bc      Re, Adif                      ;屏蔽AD中断 
            bs      R9, Adrun                     ;启动AD转换 
            bc      Flag1, Ad_Ok
            jmp     Main
            ;;;;;;;;;;;;;;;;;;;;;;;
 Det_Lod6:                                        ;启动AD转换 
            jbs     Flag1, Ad_Ok                  ;检测有负载后，负载离去 
            jmp     Det_Lod8
            bc      Flag1, Ad_Ok
            
            mov     a, RD
            add     ADC_Buf0, a
            jbc     status, C
            inc     ADC_Buf1
            mov     a, RC
            and     a, @0x0f
            add     ADC_Buf1, a
            
            inc     ExV_Cnt
            mov     a, @DetectNb                  ;0x10
            xor     a, ExV_Cnt
            jbs     Status, Z
            jmp     Det_Lod8
            clr     ExV_Cnt
            
            mov     a, ADC_Buf1
            mov     General1, a
            mov     a, Adc_Bas0                   ;ADC_Buf0-Adc_Bas0
            sub     a, ADC_Buf0                   ;A<-- R-a
            jbc     Status, C
            jmp     $+5
            mov     a, @01
            sub     General1, a                   ;R<-- R-a
            jbs     Status, C
            jmp     Det_Lod3                      ;有借位,Adc_Buf1.0小于100h
            ;;;;;;;;;;;;;;;;;;;;;;;
            mov     a, Adc_Bas1
            sub     General1, a                   ;General1-Adc_Bas1
            jbs     Status, C
            jmp     Det_Lod3                      ;Adc_Buf1.0<Adc_Bas1,0
            ;;;;;;;;;;;;;;;;;;;;;;;               ;添加的，可以屏蔽 
            mov     a, General1                   ; ADC_Buf1-2
            jbs     status, Z
            jmp     Det_Lod7
            ;;;;;;;;;;;;;;;;;;;;;;;               ; 重复,上面屏蔽后作用 
            mov     a, @2                         ; 2008-3-24
            sub     a, General1                   ; ADC_Buf1-2
            jbc     Status, C                     ; 不够减?
            jmp     Det_Lod7                      ; 够 c=1 没有借位 
            
            ;;;;;;;;;;;;;;;;;;;;;;;
 Det_Lod3:                                        ; 电流小 
            inc     LoadCnt                       ; 电流小于20mA
            clr     ADC_Buf0
            clr     ADC_Buf1
            
            mov     a, LoadCnt
            and     a, @0x0f0
            jbc     Status, Z
            jmp     Det_Lod8
            bc      P6_Dat, JD1
            ;bc      P6, JD1
            call    delay_10ms
            call    delay_10ms
            jbs     P6, ExVot
            jmp     Det_Lod9
           
            call    delay_10ms
            jbs     R6, ExVot
            jmp     Det_Lod9
            
            clr     LoadCnt
            bc      Flag2, HavLoad
            
            bs      R4, b6
            mov     a, @0x0f0
            and     disp_B6, a                    ; b0bset       disp_B6.0
            bc      disp_B5, 7
            
           ; bs      Disp_B6, 0
           ; bc      Disp_B6, 1
           ; bc      Disp_B6, 3
            bc      P6_Dat, JD1
            bc      Flag2, FGCmd
            bs      Flag2, GoVer
            bc      Flag1, GoCmd
            jmp     Disp_Tim2
 Det_Lod7:
            clr     LoadCnt
            clr     ADC_Buf0
            clr     ADC_Buf1
 Det_Lod8:
            bc      Re, Adif
            bs      R9, Adrun
            jmp     Main
 Det_Lod9:
            ;bs      R6, JD1
            bs      P6_Dat, JD1
            clr     LoadCnt
            jmp     main
;*************************************************** 
;--------------------------------------------------; 
 Main2:                                           ;非设定时的键处理 
            bc      Flag3, HaveKey
            jbc     Flag2, HavLoad
            jmp     Main24
            jbc     Flag1, Pow_off
            jmp     Main26
            ;;;;;;;;;;;;;;;;;;;;;;;
            mov     a, KeyVol
            xor     a, @01                        ;"+" Key?
            jbs     Status, Z
            jmp     Main21
            jbs     Flag2, HavLoad
            jmp     Det_Lod4                      ;启动一次充电过程 
            jmp     main
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main21:
            mov     a, KeyVol
            xor     a, @03                        ;Clock Key?
            jbs     Status, Z
            jmp     Main22
            
            bc      Flag3, Set_Go                 ;Seting Clock
            bs      R4, B6                        ;Bank1
            bs      Disp_B0, 3                    ;上部箭头 L14
            bs      Disp_B5, 3                    ;L22 CLK
            mov     a, week
            mov     Sweek, a
            mov     a, hour
            mov     Shour, a
            mov     a, minut
            mov     Sminut, a
            call    Disp_Week
            jmp     Main23
 Main22:
            mov     a, KeyVol
            xor     a, @02                        ;Timing Key?
            jbs     Status, Z
            jmp     Main24
            
            bs      R4, B6                        ;Bank1
            bs      Flag3, Set_go
            bs      Disp_B5, 2                    ;L21 (/)
            bs      Disp_B0, 1                    ;L16
            bc      Disp_B5, 3
            mov     a, Ghour
            mov     Shour, a
            mov     a, Gminut
            mov     Sminut, a
            mov     a, Gweek
            mov     FulWeek, a
            call    Set_Week
 Main23:
            bs      Disp_B5, 6                    ;L6 SET
            
            bc      R4, B6                        ;Bank0
            bs      Flag2, Seting
            clr     Setg_T
            clr     Set_Nb
            bs      Flag3, FlashB
            bs      Flag2, Hav_disp
            jmp     Main
 Main24:
            mov     a, KeyVol
            xor     a, @04                        ;FullG键?
            jbs     Status, Z
            jmp     Main26
            jbc     Flag2, FullGo
            jmp     Main25
            bs      Flag2, FullGo
            bs      R4, B6
            bs      Disp_B0, 0
            bs      Flag2, Hav_disp
            jmp     Main
 Main25:
            bc      Flag2, FullGo
            bs      R4, B6
            bc      Disp_B0, 0
            bs      Flag2, Hav_disp
            jmp     Main
 Main26:
            jbc     Flag2, HavLoad
            jmp     Main
            mov     a, KeyVol
            xor     a, @05                        ;Power键?
            jbs     Status, Z
            jmp     Main
            
            jbc     Flag1, Pow_off
            jmp     Main27
            bs      Flag1, Pow_off
            bc      Status, iocs
            
            ior     iocF0
            and     a, @0b11011111                ;stop TccC
            iow     iocF0
            bc      P5, IR2
            bc      P6, IR1
            bc      P6, IR3
            
            disi
            bs      R4, B6
            mov     a, @0b00000010                ;off Lcd
            mov     Comad, a
            call    sent_comd
            eni
            call    delay_10ms
            bc      R4, B6
            jmp     Main
 Main27:
            bc      Flag1, Pow_off
            ior     iocF0
            or      a, @0b00100000
            iow     iocF0
            
            bs      R4, B6                        ;Bank1
            disi
            mov     a, @0b00000011                ;on Lcd
            mov     Comad, a
            call    sent_comd
            call    delay_10ms
            eni
            call    delay_10ms
            bs      R4, B6
            jmp     Disp_Tim2
;**************************************************
 Main3:                                           ;定时设置 Timing 后 
            bc      R4, B6                        ;Bank0
            jbs     Flag3, Set_go
            jmp     Main4
            disi                                  ;Set_go=1
            bs      status, PS0
            call    Key_scan                      ;键扫描 
            eni
            jbs     Flag3, HaveKey
            jmp     Main5
            bc      Flag3, HaveKey
            mov     a, KeyVol
            bs      R4, B6                        ;Bank1
            
            and     a, @07
            add     R2, a
            jmp     Main3B                        ;00 
            jmp     Main31                        ;01 K1 +
            jmp     Main37                        ;02 K2 Timing
            jmp     Main34                        ;03 K3 Clock 放弃 
            
            jmp     Main38                        ;04 K4 FullGO(ok)
            jmp     Main35                        ;05 K5 Power(Esc.)取消 
            jmp     Main3B                        ;06
            jmp     Main3B                        ;07
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main31:                                          ;01 K1 +
            mov     a, Set_Nb
            and     a, @03
            add     R2, a
            jmp     Main32                        ;00 Sminut
            jmp     Main33                        ;01 Shour
            jmp     Main3I
            jmp     Main3J
            ;;;;;;;;;;;;;;;
 Main32:                                          ;K1 00 Sminut +
            mov     a, @01
            add     a, Sminut
            daa
            mov     Sminut, a
            xor     a, @0x60
            jbs     Status, Z
            jmp     Main3B
            clr     Sminut
            jmp     Main3B
 Main33:                                          ;K1 01 Shour +
            mov     a, @01
            add     a, Shour
            daa
            mov     Shour, a
            xor     a, @0x24
            jbs     Status, Z
            jmp     Main3B
            clr     Shour
            jmp     Main3B
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main3I:                                          ;K1 03 Week +    mov     a, FulWeek
            jbs     Flag3, Set_Wb
            jmp     Main3I2
            swapa   Set_Nb                        ;启用并移位 
            and     a, @0x07
            mov     General1, a
            rlca    General1
            and     a, @0x0e
            add     R2, a
            
            bs      FulWeek, 0                    ;0
            jmp     Main3I1
            
            bs      FulWeek, 1                    ;1
            jmp     Main3I1
            
            bs      FulWeek, 2                    ;2
            jmp     Main3I1
            
            bs      FulWeek, 3                    ;3
            jmp     Main3I1
            
            bs      FulWeek, 4                    ;4
            jmp     Main3I1
            
            bs      FulWeek, 5                    ;5
            jmp     Main3I1
            
            bs      FulWeek, 6                    ;6
            jmp     Main3I1
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main3I2:
            disi
            bs      Status, Ps0
            call    Clr_SWeek
            eni
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main3I1:
            bc      Flag3, Set_Wb
            clr     SWeek_Cnt
            ;;;;;;;;;;;;;;;;;;;;;;;
            call    Set_Week
            mov     a, @0x10                      ;7
            add     Set_Nb, a
            mov     a, Set_Nb
            and     a, @0x0f0
            sub     a, @0x60
            jbc     Status, c
            jmp     Main3B
            mov     a, @0x0f
            and     Set_Nb, a
            jmp     Main3B
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main3J:                                          ;K1 04 NoBody
            ;mov     a, @0xf0            and     Set_Nb, a
            clr     Set_Nb
            jmp     Main3B
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main34:                                          ;K3 Clock
            mov     a, Shour                      ;时间确认，不定时 
            mov     Ghour, a
            mov     a, Sminut
            mov     Gminut, a
            mov     a, FulWeek
            mov     Gweek, a 
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main35:                                          ;K5 Power(Esc.)取消 
            bc      Flag3, Timing                 ;bc      Disp_B5, 2
            bc      Disp_B5, 2                    ;FullGo标记 
 Main36:                                          ;K3 Clock 放弃 
            jbc     Flag3, Timing
            bs      Disp_B5, 2
            jmp     Main47               
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main37:                                          ;K2 Timing
            bc      Flag3, Set_Wb
            clr     SWeek_Cnt
            inc     Set_Nb                        ;项目加一 
            mov     a, Set_Nb
            and     a, @0x0f
            xor     a, @03
            jbs     Status, Z
            jmp     Main3B
            clr     Set_Nb
            call    Set_Week
            jmp     Main3B
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main38:                                          ;
            mov     a, Shour                      ;K4 FullGO(ok)
            mov     Ghour, a
            mov     a, Sminut
            mov     Gminut, a
            mov     a, FulWeek
            mov     Gweek, a 
            
            bs      Flag3, Timing
            bs      Disp_B5, 2
            jmp     Main47
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main3B:
            clr     Setg_t
            clr     Time_C1
            bs      Flag3, FlashB
            jmp     Main61
;**************************************************
 Main4:                                           ;时间设置 
            disi                                  ;
            bs      status, PS0
            call    Key_scan                      ;键扫描 
            eni
            jbs     Flag3, HaveKey
            jmp     Main5
            bc      Flag3, HaveKey
            mov     a, KeyVol
            bs      R4, B6                        ;Bank1
           
            and     a, @07
            add     R2, a
            jmp     Main3B                        ;0
            jmp     Main41                        ;K1 +
            jmp     Main47                        ;K2 Timing 放弃 
            jmp     Main48                        ;K3 Clock
            jmp     Main46                        ;K4 FullGO（Ok） 
            jmp     Main47                        ;K5 Power (Esc)
            jmp     Main3B                        ;6
            jmp     Main3B                        ;7
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main41:                                              ;K1 +
            mov     a, Set_Nb
            and     a, @03
            add     R2, a
            jmp     Main32                        ;00 Sminut +
            jmp     Main33                        ;01 Shour +
            jmp     Main42                        ;02 SWeek +
            jmp     Main3B                        ;03 No Operation
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main42:                                          ;02 Sweek +
            inc     Sweek
            mov     a, Sweek
            xor     a, @07
            jbs     Status, Z
            jmp     $+2                           ;Main3B
            clr     Sweek
            
            call    Disp_Week
            jmp     Main3B
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main46:                                          ;K4 FullGO（Ok） 
            mov     a, Sweek
            mov     week, a
            mov     a, Shour
            mov     hour, a
            mov     a, Sminut
            mov     minut, a
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main47:                                          ;K2 Timing 放弃 
            bc      Flag2, Seting                 ;K5 Power (Esc)
            bc      Flag3, Set_go
            bc      Disp_B0, 3                    ;L14
            bc      Disp_B0, 1                    ;L16
            bs      Disp_B5, 3                    ;L22 CLK
            ;bc      Disp_B5, 2                    ;L21 (/)
            bc      Disp_B5, 6                    ;L6 SET
            clr     Setg_T
            bs      Flag2, Hav_Disp
            jmp     Main                          ;Disp_Tim3
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main48:                                          ;改变设置项目 
            inc     Set_Nb                        ;K3 Clock
            mov     a, @03                        ;
            xor     a, Set_Nb
            jbs     Status, Z
            jmp     Main3B
            clr     Set_Nb
            jmp     Main3B
;******************************************************
 Main5:                                                ;无按键操作时退出out
            jbs     Flag1, Flash_F
            jmp     Main
            bc      Flag1, Flash_F
            
            inc     Setg_T
            mov     a, @Set_Time
            xor     a, Setg_T
            jbc     Status, Z
            jmp     Main47                             ;Main38
;******************************************************
 Main6:                                           ;设置时Flash
            bs      R4, B6
            jbs     Flag3, FlashB
            jmp     Main67
            bc      Flag3, FlashB
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main61:                                          ;熄灭 
            mov     a, Set_Nb                     ;Flash Set_Go
            and     a, @03
            add     R2, a
            jmp     Main62                        ;00 Sminut
            jmp     Main63                        ;01 Shour
            jmp     Main65                        ;02 Sweek
            jmp     Main6A                        ;03 Reset Set_Nb
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main62:                                          ;00 Sminut
            mov     a, @03
            mov     TempW, a
            jmp     Main64                        ;5
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main63:                                          ;01 Shour
            mov     a, @01
            mov     TempW, a
 Main64:
            clr     General1                      ;Str_Val
            call    Disp_Str1
            jmp     Main69
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Main65:                                          ;Flash Week
            jbc     Flag3, Set_Go
            jmp     Main66
            ;;;;;;;;;;;;;;;;;;;;;;;               ;设置时间 
            bc      Disp_B5, 5                    ;Monday
            bc      Disp_B4, 3                    ;Tuesday
            bc      Disp_B3, 3                    ;Wednesday
            bc      Disp_B2, 7                    ;Thursday
            bc      Disp_B1, 7                    ;Friday
            bc      Disp_B0, 7                    ;Saturday
            bc      Disp_B5, 4                    ;Sunday
            jmp     Main69
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main66:                                           ;设置定时 
            jbc     Flag3, Set_Wb
            jmp     Main69
            disi
            bs      Status, Ps0
            call    Clr_Week
            eni
            
           ; swapa   Set_Nb
           ; and     a, @0x07
           ; mov     SWeek, a
           ; call    Disp_Week
            jmp     Main69
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main67:                                          ;点亮 
            bs      Flag3, FlashB
            mov     a, Set_Nb                     ;
            and     a, @03
            add     R2, a
            jmp     Disp_Tim3                     ;00 Sminut
            jmp     Disp_Tim3                     ;01 Shour
            jmp     Main68                        ;02 Sweek
            jmp     Main6A                        ;03  Reset Set_Nb
            ;;;;;;;;;;;;;;;;;;;;;;;
 Main68:                                          ;
            jbs     Flag3, Set_Go
            jmp     Disp_Tim3
            
            swapa   Set_Nb
            and     a, @0x07
            mov     SWeek, a
            mov     a, R4
            mov     General4, a                   ;保护R4
            call    Disp_Week2
            
            jbc     Flag3, Set_Wb
            jmp     Main69
            inc     SWeek_Cnt
            mov     a, @010
            xor     a, SWeek_Cnt
            jbs     Status, Z
            jmp     Main69
            bs      Flag3, Set_Wb
 Main69:
            bs      Flag2, Hav_Disp 
            jmp     Main
 Main6A:
            mov     a, @0x0f
            and     Set_Nb, a
            jmp     Main
 ;****************************************************
 ;----------开始子程序-------------------------------;   
;*****************************************************
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 delay1mS:      
            mov       a, @4
            jmp       $+2
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 delay_10ms:
            mov     a, @48
            mov     Rb2, a
 dely_10ms1:           
            jmp     $+1
            jmp     $+1
            jmp     $+1
           ; jmp     $+1
            djz     Rb1
            jmp     dely_10ms1
            djz     Rb2
            jmp     dely_10ms1
            Wdtc
            ret
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 delay:      
            mov       a, @20
            mov       Rb1, a
 delay1:
            djz       Rb1
            jmp       delay1
            ret
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;*****************************************************
 sent_comd:
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6
            
            bs      P5, D_Wr
            bs      P5, D_Cs
            call    delay
            bc      P5, D_Cs
            call    delay
            bs      P6, Data                      ;1
            call    delay
            bc      P5, D_Wr
            call    delay
            bs      P5, D_Wr
            call    delay
            bc      P6, Data                      ;0
            call    delay
            bc      P5, D_Wr
            call    delay
            bs      P5, D_Wr
            call    delay
            bc      P5, D_Wr                      ;0
            call    delay
            bs      P5, D_Wr
            call    delay
            mov     a, @08
            mov     General2, a
 sent_comd1:
            jbs     comad, 7
            jmp     sent_comd2
            bs      P6, Data
            jmp     sent_comd3
 sent_comd2:
            bc      P6, Data
 sent_comd3:
            bc      P5, D_Wr                      ;xxxxxxxxb
            call    delay
            bs      P5, D_Wr
            call    delay
            bc      Status, C
            rlc     comad
            djz     General2
            jmp     sent_comd1
            bc      P6, Data                      ;0
            bc      P5, D_Wr
            call    delay
            bs      P5, D_Wr
            call    delay
            bs      P5, D_Cs
            call    delay
 sent_comd4:
            mov     a, General4
            mov     R4, a
            ret
;*****************************************************
;          input:       string's number, value
;          output:      lcd character
;          date:        2007-8-1
;*****************************************************
 Disp_Str:
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6
            
            mov     a, Str_Val
            and     a, @0fh
            call    Table2                        ;查表 
            mov     General1, a                   ;R
 Disp_Str1:
            mov     a, TempW                      ;str_Nb
            and     a, @03h
            sub     a, @Disp_B4+1000000b          ;@Disp_B4-a -->a
            mov     R4, a  
            
            jbc     TempW, 1                      ;str_Nb
            jmp     Disp_Str2
            mov     a, General1                   ;B4,B3 时 
            mov     General3, a
            mov     a, R0
            and     a, @0b00001000
            add     a, General3
            mov     R0, a
            jmp     sent_comd4
            ;;;;;;;;;;;;;;;;;;;;
 Disp_Str2:                                       ;B2,B1 分 
            swapa   General1
            and     a, @0x0f
            mov     General3, a
            mov     a, R0
            and     a, @0b11110000
            add     a, General3
            mov     R0, a
            dec     R4
            swapa   General1
            and     a, @0x0f0
            mov     General3, a
            mov     a, R0
            and     a, @0b10001111
            add     a, General3
            mov     R0, a
            jmp     sent_comd4
;**************************************************
 Disp_Week:
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6
            
            bc        Disp_B5, 5                  ;Monday 
            bc        Disp_B4, 3                  ;Tuesday
            bc        Disp_B3, 3                  ;Wednesday
            bc        Disp_B2, 7                  ;Thursday
            bc        Disp_B1, 7                  ;Friday
            bc        Disp_B0, 7                  ;Saturday bs        
            bc        Disp_B5, 4                  ;Sunday
 Disp_Week2:
            rlca      SWeek
            and       a, @0x0e
            add       R2, a
           
            bs      Disp_B5, 4                    ;00 Sunday
            jmp     Disp_Week1
            
            bs      Disp_B5, 5                    ;01 Monday
            jmp     Disp_Week1
            
            bs      Disp_B4, 3                    ;02 Tuesday
            jmp     Disp_Week1
            
            bs      Disp_B3, 3                    ;03 Wednesday
            jmp     Disp_Week1
            
            bs      Disp_B2, 7                    ;04 Thursday
            jmp     Disp_Week1
            
            bs      Disp_B1, 7                    ;05 Friday
            jmp     Disp_Week1
            
            bs      Disp_B0, 7                    ;06 Saturday
            jmp     Disp_Week1
            
            nop                                   ;07 No Disp
 Disp_Week1:
            mov     a, General4                   ;恢复R4
            mov     R4, a
            ret
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Set_Week:
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6
            
            bc      Disp_B5, 4                    ;Sunday
            jbc     FulWeek, 0
            bs      Disp_B5, 4
            
            bc      Disp_B5, 5                    ;Monday 
            jbc     FulWeek, 1
            bs      Disp_B5, 5
            
            bc      Disp_B4, 3                    ;Tuesday
            jbc     FulWeek, 2
            bs      Disp_B4, 3
            
            bc      Disp_B3, 3                    ;Wednesday
            jbc     FulWeek, 3
            bs      Disp_B3, 3
            
            bc      Disp_B2, 7                    ;Thursday
            jbc     FulWeek, 4
            bs      Disp_B2, 7
            
            bc      Disp_B1, 7                    ;Friday
            jbc     FulWeek, 5
            bs      Disp_B1, 7
            
            bc      Disp_B0, 7                    ;Saturday
            jbc     FulWeek, 6
            bs      Disp_B0, 7
            
            
            mov     a, General4                   ;恢复R4
            mov     R4, a
            ret
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Table2:
            add       R2, a
            
            retl      0xD7                        ;0
            retl      0x06                        ;1
            retl      0xB5                        ;2
            retl      0xA7                        ;3
            retl      0x66                        ;4
            retl      0xE3                        ;5
            retl      0xF3                        ;6
            retl      0x86                        ;7
            retl      0xF7                        ;8
            retl      0xE6                        ;9
            retl      0xF6                        ;A
            retl      0x73                        ;B
            retl      0xD1                        ;C
            retl      0x37                        ;D
            retl      0xF1                        ;E
            retl      0xF0                        ;F
            retl      0xF4                        ;P
            
            
            
            
    ;=============================================;
    ;        ORG     0X400                         ;Sub Prog
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ;=============================================;
    ;        A                       ;
    ;=============================================;
    
    
    
    ;=============================================;
            ORG     0X400                         ;Sub Prog
             
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;;; Port seting ;;;;;;;;l;;;;;;;;;;
 PortInit:                                        ;端口初始化 
            bs      Flag3, PageF
            bc      Status, Iocs
            mov     a, @0b11110010                ;F2H CS,WR,IR2 OUT
            iow     Ioc50                         ;
            mov     a, @0b11111110                ;FEH IR2=0
            mov     P5_Dat, a
            mov     R5, a             
            mov     a, @0b00000010
            iow     iocD0
            
            mov     a, @0b00011110                ;1EH-- IR3,IR1,JD1,Data Out
            iow     Ioc60
            mov     a, @0b00010001                ;1Fh IR3,IR1,JD1 =0
            mov     P6_Dat, a
            mov     R6, a
            
            mov     a, @0b11111111
            iow     Ioc70
            mov     R7, a
 Port_End: 
            bc      status, Ps0 
            bc      Flag3, PageF
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;;;;;;;;;;;;; System Initial   ;;;;;;;;;;;;;;;
 SysInit:
            disi
            ;;;;;;;Open Wdt;;;;;;;;;;;;;;         ;Ior    Ioce0  ;
            mov     a, @0b10001010                ;WDTE,PSWE,Prescaler=1:8
            Iow     Ioce0                         ;
            
            ;;;;;;; Ext_Int ;;;;;;;;;;;;;;;;
            ;;  CONT, IocE0, IOCF0        ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;    Ior    Ioce0                         ;
        ;    or     a, @0B01000000                ;EIS--P60 is input
        ;    iow    IocE0
            
            ;;;;;;;Open Tcc ;;;;;;;;;;;;;;;;
            ;;  CONT, IOCF0               ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            mov     a, @0b00000000                ;内部指令周期,下降沿,不分频 
            contw 
            clr     Tcc
            
            ;;;;;;;  Open ADC  ;;;;;;;;;;;;;
            ;;   R8, R9, IOCE0            ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            bc     status,iocs
            mov     a, @0b11001001                ;Vref,16Mhz,ADPD,  AD1
            mov     R9, a                         ;池电压 
            clr     RA
            Ior     Ioce0
            or      a, @0B00100000                ;set ADIE (00100000)
            Iow     Ioce0
            mov     a, @0b00000010                ;P51 is AD input
            mov     R8, a                         ;输入选择 P51--AD1
            bs      R9, ADRun                     ;start adc
           
            ;;;;;;;    Seting PWM   ;;;;;;;;;;;
            ;; IOCA0,IOCF0,IOC91,IOCA1,IOCB1 ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            bc      status,iocs                   ;select ioc bank0
            ior     iocA0
            and     a, @0b11110000                ; PWM Stop
            iow     iocA0                         ;
            
            ;bs     status,iocs                    ;select ioc bank1
            ;mov    a, @0x00
            ;iow    iocb1                          ;H/L Time Scale Control
            ;mov    a, @0x58                       ;mov    LTimB, a
            ;iow    ioc91                          ;Low-Time
            ;mov    a, @0x70                       ;mov    HTimB, a
            ;iow    iocA1                          ;High Time
            
            ;;;;;;;    Open TccA   ;;;;;;;;;;;;
            ;;     IOC80, IOCF0, IOC51       ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
            bc      status,iocs                   ;select ioc bank0
            ior     ioc80
            and     a, @0b11111000                ;Disable TccA
            iow     ioc80                         ;
            ior     iocF0
            and     a, @0b11110111                ;Disable TccA interruput
            iow     iocF0
            bs      status,iocs                   ;select ioc bank1
            clra
            iow     ioc51
            
            ;;;;;;;    Open TccB   ;;;;;;;;;;;;
            ;; IOC90, IOCF0, IOC71, IOC71    ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
            bs        status, iocs                ;select ioc bank1
            mov       a, @0x28
            iow       ioc71                       ;2800h-->5ms
            clra
            iow       ioc61
            
            bc      status,iocs                   ;select ioc bank0
            ior     ioc90
            and     a, @0b00001111
            or      a, @0b11010000                ;TccBHE,TccBEn,
            iow     ioc90                         ;
            
            ior     iocF0
            or      a, @0b00010000
            iow     iocF0                         ;Enable TccB interruput
            
            ;;;;;;;    Open TccC   ;;;;;;;;;;;;
            ;; IOC90, IOCA0, IOCF0, IOC81    ;;
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
            bs      status,iocs                   ;select ioc bank1
            mov     a, @0x6E
            iow     ioc81                         ;6E -->38kHz(37.874)
            
            bc      status,iocs                   ;select ioc bank0
            ior     iocA0
            and     a, @0b00001111                ;
            ;or      a, @0b10000000
            iow     iocA0                         ;Tccc_Pr close
            
            ior     ioc90
            and     a, @0b11110000
            or      a, @0b00000101                ;TccCEn,TccCTe,
            iow     ioc90                         ;
            
            ior     iocF0
            or      a, @0b00100000                ;Enable TccC interruput
            iow     iocF0
            
            
            ;mov     a, @0x11                      ;00100101b  Exie   LPWTIF,HPWTIF
            ;Iow     Iocf0                         ;TccBIE TCIE
            
            
 Sys_End: 
            disi
            bc      status, Ps0 
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ParaInit:
            bs     R4, B6
            clr     second
            clr     minut
            clr     Gminut
            mov     a, @08
            mov     hour, a
            mov     a, @0x23
            mov     Ghour, a
            mov     a, @01
            mov     week, a
            mov     a, @0x7f
            mov     GWeek, a
            
            bs      Flag2, IRstop
           ; mov     a, @IR_STP
           ; mov     BCont, a
           ; clr     P5_Bak
           ; clr     P6_Bak
            
            bc     R4, B6                         ;ram	bank 0
            
            mov     a, @05
            mov     Bcont, a
	        bc      P6, Ir1
            bc      P5, Ir2
            bc      P6, Ir3

            
            bc      status, Ps0 
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ClearGRam:                                       ;清常规内存 
            mov     a, @0x10                      ;00010000b 
            mov     R4, a
 ClrGRam:                                         ;clr 20h-3fh
            clr     R0
            inc     R4
            mov     a, @0x40
            xor     a, R4 
            jbs     Status, Z
            jmp     ClrGRam
            clr     R0
            bc      status, Ps0 
            ret 
   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ClearERam:                                       ;清扩展内存 
            bs     R4, B6
            mov    a, @0x60  
            mov    R4, a
 ClrExRam:
            clr    R0
            inc    R4
            mov    a, @0x7f
            xor    a, R4 
            jbs    Status, Z
            jmp    ClrExRam
            clr    R0
            bc     status, Ps0 
            ret
;*****************************************************
 Key_scan:
            bs      Flag3, PageF
            Eni
            mov     a, R4
            mov     General4, a
            bc      R4, B6
            
            mov     a, @0b11110001
            and     P6, a
            bc      Status, iocs
            ior     Ioc60                         ;P6,K0--B1,K1--B2, K2--B3
            and     a, @0b11110001
            or      a, @0b00001100                ;K2,K1--in,K0--out
            iow     Ioc60
            call    Kdelay
            jbc     P6, Key1                      ;K1 is press?
            jmp     Key_Scan1
            mov     a, @0x01
            jmp     Key_Scan5
 Key_Scan1:
            jbc     P6, Key2                      ;K2 is press?
            jmp     Key_Scan2
            mov     a, @0x02
            jmp     Key_Scan5
            ;;;;;;;;;;;;;;;;;
 Key_Scan2:                                       ;  bs      P6, Key0
            ior     Ioc60                         ;P6,K0--B1,K1--B2, K2--B3
            and     a, @0b11110001
            or      a, @0b00001010                ;K2,K0--in,K1--out
            iow     Ioc60
            call    Kdelay
            
            jbc     P6, Key0                      ;K3 is press?
            jmp     Key_Scan3
            mov     a, @0x03
            jmp     Key_Scan5 
 Key_Scan3:
            jbc     P6, Key2                      ;K4 is press?
            jmp     Key_Scan4
            mov     a, @0x04
            jmp     Key_Scan5 
 Key_Scan4:                                       ;bs      P6, Key1
            ior     Ioc60                         ;P6,K0--B1,K1--B2, K2--B3
            bc      P6, Key2                      ;011b
            and     a, @0b11110001
            or      a, @0b00000110                ;K1,K0--in,K2--out
            iow     Ioc60
            call    Kdelay
            clr     TempW
            
            jbc     P6, Key0                      ;K5 is press?
            jmp     Key_Scan7
            mov     a, @05
 Key_Scan5:
            mov     TempW, a
            xor     a, KeyVol
            jbs     Status, Z
            jmp     Key_Scan7
            inc     Key_Cunt
            mov     a, @KeyNb1
            xor     a, Key_Cunt
            jbs     Status, Z
            jmp     Key_Scan6
            bs      Flag3, HaveKey
            jmp     Key_Scan8
 Key_Scan6:
            mov     a, @KeyNb2
            xor     a, Key_Cunt
            jbs     Status, Z
            jmp     Key_Scan8
            mov     a, @step_Key
            sub     a, Key_Cunt                   ;Key_Cunt-a --->a
            mov     Key_Cunt, a
            bs      Flag3, HaveKey
            jmp     Key_Scan8
 Key_Scan7:
            mov     a, TempW
            mov     KeyVol, a
            clr     Key_Cunt
 Key_Scan8:
            ior     Ioc60
            or      a, @0b00001110
            iow     Ioc60
            mov     a, General4
            mov     R4, a
            Disi
            bc      status, Ps0 
            bc      Flag3, PageF
            ret
;*****************************************************
 Time_Add:
            bs      Flag3, PageF
            Eni
            mov     a, R4
            mov     General4, a
            bs      R4, B6
            
            mov     a, @1
            add     a, second
            daa
            mov     second, a
            xor     a, @0x60
            jbs     Status, Z
            jmp     Time_AddEnd
            
            clr     second
            mov     a, @1
            add     a, minut
            daa
            mov     minut, a
            xor     a, @0x60
            jbs     Status, Z
            jmp     Time_AddEnd
            
            clr     minut
            mov     a, @1
            add     a, hour
            daa
            mov     hour, a
            xor     a, @0x24
            jbs     Status, Z
            jmp     Time_AddEnd
            
            clr     hour
            bc      Flag2, GoVer
            inc     week
            mov     a, week
            xor     a, @8
            jbs     Status, Z
            jmp     Time_AddEnd
            
            mov     a, @01
            mov     week, a
 Time_AddEnd:
            mov     a, General4
            mov     R4, a
            disi
            bc      status, Ps0 
            bc      Flag3, PageF
            ret
;*****************************************************
 Disp_all: 
            bs      Flag3, PageF                  ;eni
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6
            
            mov     a, @Disp_B0+1000000b
            mov	 R4, a 
 Disp_all1:
            mov     a, @0x0ff
            mov     R0, a
            inc     R4
            mov     a, @Disp_B6+1000001b
            xor     a, R4
            jbs     Status, Z
            jmp     Disp_all1
            jmp     Writ_DatI
;*****************************************************
 Clear_all: 
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6
            
            mov     a, @Disp_B0+1000000b
            mov	 R4, a 
 Clear_all1:
            clr     R0
            inc     R4
            mov     a, @Disp_B6+1000001b
            xor     a, R4
            jbs     Status, z
            jmp     Clear_all1
            mov     a, General4
            mov     R4, a
            disi
            ;jmp     Writ_Dat
;*****************************************************
 Writ_Dat:
           ; eni
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6
  Writ_DatI:
            clr     Addr
            mov     a, @13
            mov     TempW, a
 Writ_Dat0:
            bs      P5, D_Wr
            bs      P6, Data
            bs      P5, D_Cs
            call    Edelay
           ;;;;;;;;;;;;;;;;;;;;;;;;
            bc      P5, D_Cs
            call    Edelay
            bs      P6, Data                       ;1
            call    Edelay
            bc      P5, D_Wr
            call    Edelay
            bs      P5, D_Wr
            call    Edelay
            mov     a, Addr
            add     a, @0x0a
            and     a, @0b00111111
            add     a, @0b01000000
            mov     comad, a
            mov     a, @8                       ;Addr
            mov     General2, a
 Writ_Dat1:
            bc      P5, D_Wr
            jbs     comad, 7
            jmp     Writ_Dat2
            bs      P6, Data
            jmp     Writ_Dat3
 Writ_Dat2:
            bc      P6, Data
 Writ_Dat3:
            call    Edelay
            bs      P5, D_Wr                    ;xxxxxxxxb
            call    Edelay
            bc      P5, D_Wr
            bc      Status, C
            rlc     comad
            djz     General2
            jmp     Writ_Dat1
            call    Edelay
           ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            rrca     Addr
            add     a, @Disp_B0+1000000b
            mov     R4, a
            jbc     Addr, 0
            jmp     Writ_Dat8
 Writ_Dat4:
            mov     a, R0
            mov     comad, a
            mov     a, @04                      ;B0--B3
            mov     General2, a
 Writ_Dat5:
            bc      P5, D_Wr
            jbs     comad, 3
            jmp     Writ_Dat6
            bs      P6, Data
            jmp     Writ_Dat7
 Writ_Dat6:
            bc      P6, Data 
 Writ_Dat7:
            call    Edelay
            bs      P5, D_Wr                    ;xxxxxxxxb 
            call    Edelay
            bc      P5, D_Wr
            bc      Status, C
            rlc     comad
            djz     General2
            jmp     Writ_Dat5
            djz     TempW
            jmp     Writ_Dat8
            jmp     Writ_DatE
 Writ_Dat8:
            mov     a, R0
            mov     comad, a
            mov     a, @04                      ;B4--B7
            mov     General2, a
 Writ_Dat9:
            bc      P5, D_Wr
            jbs     comad, 7
            jmp     Writ_DatA
            bs      P6, Data
            jmp     Writ_DatB
 Writ_DatA:
            bc      P6, Data
 Writ_DatB:
            call    Edelay
            bs      P5, D_Wr                    ;xxxxxxxxb 
            call    Edelay
            bc      P5, D_Wr              
            call    Edelay
            bc      Status, C
            rlc     comad
            djz     General2
            jmp     Writ_Dat9
            inc     R4
            call    Edelay
            djz     TempW
            jmp     Writ_Dat4
 Writ_DatE:
            bs      P5, D_Wr
            bs      P5, D_Cs
            call    Edelay
            jmp     Time_AddEnd
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Edelay:      
            jmp       $+1
            jmp       $+1
            jmp       $+1
            jmp       $+1
            jmp       $+1
            jmp       $+1
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Clr_SWeek:           
            bs      Flag3, PageF
            eni
            swapa   Set_Nb                        ;禁止并移位 
            and     a, @0x07
            mov     General1, a
            rlca    General1
            and     a, @0x0e
            add     R2, a
            
            bc      FulWeek, 0                    ;0
            jmp     Clr_SWeek1
            
            bc      FulWeek, 1                    ;1
            jmp     Clr_SWeek1
            
            bc      FulWeek, 2                    ;2
            jmp     Clr_SWeek1
            
            bc      FulWeek, 3                    ;3
            jmp     Clr_SWeek1
            
            bc      FulWeek, 4                    ;4
            jmp     Clr_SWeek1
            
            bc      FulWeek, 5                    ;5
            jmp     Clr_SWeek1
            
            bc      FulWeek, 6                    ;6
            nop                       ;7
 Clr_SWeek1:
            disi
            bc      status, Ps0 
            bc      Flag3, PageF
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Clr_Week:           
            bs      Flag3, PageF
            eni
            swapa   Set_Nb                        ;禁止并移位 
            and     a, @0x07
            mov     General1, a
            rlca    General1
            and     a, @0x0e
            add     R2, a
            
            bc      Disp_B5, 4                    ;0 Sunday
            jmp     Clr_Week1
            
            bc      Disp_B5, 5                    ;1 Monday
            jmp     Clr_Week1
            
            bc      Disp_B4, 3                    ;2 Tuesday
            jmp     Clr_Week1
            
            bc      Disp_B3, 3                    ;3 Wednesday
            jmp     Clr_Week1
            
            bc      Disp_B2, 7                    ;4 Thursday
            jmp     Clr_Week1
            
            bc      Disp_B1, 7                    ;5 Friday
            jmp     Clr_Week1
            
            bc      Disp_B0, 7                    ;6 Saturday
            nop                       ;7
 Clr_Week1:
            disi
            bc      status, Ps0 
            bc      Flag3, PageF
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Proc_Week:
            bs      Flag3, PageF
            eni
            mov     a, R4
            mov     General4, a                   ;保护R4
            bs      R4, B6                        ;Bank1
            
            mov     a, GWeek
            mov     General1, a
            inca    Week
            mov     General2, a
            bc      Flag3, Week_G
            
 Proc_Week1:
            rrc     General1
            djz     General2
            jmp     Proc_Week1
            jbc     Status, c
            bs      Flag3, Week_G
            
            jmp     Time_AddEnd
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 Kdelay:      
            mov       a, @20
            mov       Rb1, a
 Kdelay1:
            djz       Rb1
            jmp       Kdelay1
            ret
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            
            END