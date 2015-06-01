#ifndef _A7125REG_h_
#define _A7125REG_h_

#define MODE_REG 			0x00
#define MODECTRL_REG		0x01
#define CALIBRATION_REG		0x02
#define FIFO1_REG 			0x03
#define FIFO2_REG 			0x04
#define FIFO_REG 			0x05
#define IDCODE_REG 			0x06
#define RCOSC1_REG 			0x07
#define RCOSC2_REG 			0x08
#define RCOSC3_REG 			0x09
#define CKO_REG 			0x0A
#define GPIO1_REG 			0x0B
#define GPIO2_REG 			0x0C
#define CLOCK_REG 			0x0D
#define DATARATE_REG		0x0E
#define PLL1_REG 			0x0F
#define PLL2_REG			0x10
#define PLL3_REG 			0x11
#define PLL4_REG			0x12
#define PLL5_REG 			0x13
#define TX1_REG  			0x14
#define TX2_REG  			0x15
#define DELAY1_REG			0x16
#define DELAY2_REG			0x17
#define RX_REG				0x18
#define RXGAIN1_REG			0x19
#define RXGAIN2_REG			0x1A
#define RXGAIN3_REG			0x1B
#define RXGAIN4_REG			0x1C
#define RSSI_REG			0x1D
#define ADC_REG  			0x1E
#define CODE1_REG 			0x1F
#define CODE2_REG 			0x20
#define CODE3_REG 			0x21
#define IFCAL1_REG  		0x22
#define IFCAL2_REG  		0x23
#define VCOCCAL_REG  		0x24
#define VCOCAL1_REG  		0x25
#define VCOCAL2_REG  		0x26
#define BATTERY_REG  		0x27
#define TXTEST_REG  		0x28
#define RXDEM1_REG  		0x29
#define RXDEM2_REG  		0x2A
#define CPC_REG				0x2B
#define CRYSTALTEST_REG		0x2C
#define PLLTEST_REG   		0x2D
#define VCOTEST1_REG 		0x2E
#define VCOTEST2_REG 		0x2F
#define IFAT_REG 			0x30
#define RSCALE_REG 			0x31
#define FILTERTEST_REG 		0x32

//strobe command
#define CMD_SLEEP		0x80	//1000,xxxx	SLEEP mode
#define CMD_IDLE		0x90	//1001,xxxx	IDLE mode
#define CMD_STBY		0xA0	//1010,xxxx Standby mode
#define CMD_PLL			0xB0	//1011,xxxx	PLL mode
#define CMD_RX			0xC0	//1100,xxxx	RX mode
#define CMD_TX			0xD0	//1101,xxxx	TX mode
#define CMD_TFR			0xE0	//1110,xxxx	TX FIFO reset
#define CMD_RFR			0xF0	//1111,xxxx	RX FIFO reset

#include "stm32f10x_lib.h"
#include "soft.h"

uint8 initRF(void);
uint8 dui_Telecontrol_ID(void);
void A7105_rx(void);

extern YAOKONG gsv_yaokong;
extern bool gbv_yaokong_time;
extern bool gbv_a7105_reset;

extern uint32 Telecontrol_ID;//ң��ID��

void init_a7105hardware(void);
void A7105_Reset(void);
void A7105_WriteReg(uint8, uint8);
uint8 A7105_ReadReg(uint8);
void ByteSend(uint8 src);
uint8 ByteRead(void);
void A7105_WriteID(void);
void A7105_Config(void);
void A7105_Cal(void);
void RxPacket(uint8 a[]);
void StrobeCmd(uint8);
void SetCH(uint8);
void A7105_WriteFIFO(uint8 p[], uint8 byte);
void A7105_ReadFIFO(uint8 a[],uint8 number);
void A7105_ReadID(uint8 *id);
void A7105_rx_en(void);
void set_SDIO_in(void);
void set_SDIO_out(void);
void init_a7105(void);

#endif
