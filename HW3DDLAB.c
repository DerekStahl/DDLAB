/*
===============================================================================
 Name        : HW3.c
 Author      : Derek Stahl
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


//THIS IS CORRECT FILE
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#define CLKSRCSEL (*(volatile unsigned int *)0x400FC10C)	//This register selects the clock

//REGISTER DEFINITIONS OF PLL0
#define PLL0CON (*(volatile unsigned int *)0x400FC080)	//This register is the control register
#define PLL0CFG (*(volatile unsigned int *)0x400FC084)	//This register is the configuration register
#define PLL0STAT (*(volatile unsigned int *)0x400FC088)	//This register is the status register
#define PLL0FEED (*(volatile unsigned int *)0x400FC08C)	//This register is the Feed register

#define  CCLKSEL (*(volatile unsigned int *)0x400FC104) //This is the division register

#define CLKOUTCFG (*(volatile unsigned int *)0x400FC1C8) //clock configuration of the clock output pin

//PIN SELECT 3 for the CLKOUT PIN
#define PINSEL3 (*(volatile unsigned int *) 0x4002C00C)

int main(void) {





    // 1. Disconnect PLL0 with one feed sequence if PLL0 is already connected.

	PLL0CON &= ~(1<<1);
	PLL0FEED = 0xAA; PLL0FEED = 0x55;

	//2. Disable PLL0 with one feed sequence.

	PLL0CON &= ~(1<<0);
	PLL0FEED = 0xAA; PLL0FEED = 0x55;

	//4. select 4MHz register
	CLKSRCSEL = 0;
	PLL0FEED = 0xAA; PLL0FEED = 0x55;

	//5. Write to the PLL0CFG and make it effective with one feed sequence. The PLL0CFG can only be updated when PLL0 is disabled.
	PLL0CFG |= (0x2C<<0);
	PLL0CFG |= (1<<16);
	PLL0FEED = 0xAA; PLL0FEED = 0x55;

	//6. Enable PLL0 with one feed sequence.
	PLL0CON |= (1<<0);
	PLL0FEED = 0xAA; PLL0FEED = 0x55;




	//8. Wait for PLL0 to achieve lock
	while(((PLL0STAT<<26) & 1) != 1){
		//do nothing
	}

	//7.  Change the CPU Clock Divider setting for the operation with PLL0. It is critical to do this before connecting PLL0.
	CCLKSEL = 31; //this divides 2m/n by 32
	PLL0FEED = 0xAA; PLL0FEED = 0x55;

    // 9. Connect the PLL0
	PLL0CON |= (1<<1);
	PLL0FEED = 0xAA; PLL0FEED = 0x55;


	//Enables Port 1 bit 27 as the CLKOUT Pin
	PINSEL3 |= (1<<22);
	PINSEL3 &= ~(1<<23);

	CLKOUTCFG &= ~(1<<0) & ~(1<<1) & ~(1<<2) & ~(1<<3);	//selects the CPU clock as the output pin



	while(1){

	}

    return 0 ;
}
