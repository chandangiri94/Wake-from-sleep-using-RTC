#define F_CPU       4000000UL
#include <avr/io.h>
#include <string.h>
#include "util/delay.h"
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/sleep.h>

#include "UART_1_AVR128DA64.h"




void RTC_init(void);
void LED0_init(void);
inline void LED0_toggle(void);
void SLPCTRL_init(void);


void RTC_init(void)
{
	uint8_t temp;
	temp =CLKCTRL.XOSC32KCTRLA;
	temp &= ~CLKCTRL_ENABLE_bm;
	ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);


	while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
	{
		;
	}
	//SEL=0 use external crystal

	temp =CLKCTRL.XOSC32KCTRLA;
	temp &=~CLKCTRL_SEL_bm;
	ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);

	//Enable oscillator

	temp =CLKCTRL.XOSC32KCTRLA;
	temp |=CLKCTRL_ENABLE_bm;

	ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);

	while(RTC.STATUS >0)
	{
		;
	} //RTC period is set

	//set period


	RTC.CLKSEL=RTC_CLKSEL_OSC32K_gc;

	RTC.DBGCTRL |=RTC_DBGRUN_bm;

// 	RTC.CTRLA =RTC_PRESCALER_DIV32_gc | RTC_RTCEN_bm | RTC_RUNSTDBY_bm;
// 
// 	RTC.INTCTRL |=RTC_OVF_bm;
	
	RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */
	
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
	| RTC_PITEN_bm; /* Enable: enabled */
}

void LED0_init(void)
{
	USART1_init(9600);
	PORTC.OUT |=PIN6_bm;
USART1_sendString("on ");
//	PORTB.DIR |=PIN5_bm;
}

 void LED0_toggle(void)
{
	USART1_sendString("sleep \n");
	PORTC.OUTTGL |= PIN6_bm;
USART1_init(9600);
	USART1_sendString("\n wakeup");
	
}

ISR(RTC_PIT_vect)
{
	LED0_toggle();

 	RTC.PITINTFLAGS = RTC_PI_bm;
 
}
void SLPCTRL_init(void)
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_PDOWN_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
		
}

int main(void)
{
	PORTC.DIRSET = PIN6_bm;
    sei();
	LED0_init();
	RTC_init();
	SLPCTRL_init();
	
	
	while (1)
	{
		sleep_cpu();
		
	}
}
