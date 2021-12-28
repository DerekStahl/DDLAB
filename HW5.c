#define ISER0 (*(volatile unsigned int *) 0xE000E100) //set enable register 0
#define ICER0 (*(volatile unsigned int *) 0xE000E180) //interrupt clear enable register 0
#define IO0IntEnR (*(volatile unsigned int *) 0x40028090) //GPIO Port 0 Rising edge interrupt enable
#define IO0IntEnF (*(volatile unsigned int *) 0x40028094) //GPIO Port 0 Falling edge interrupt enable
#define IO0IntClr (*(volatile unsigned int *) 0x4002808C) //GPIO Port 0 Interrupt Clears
#define PCONP (*(volatile unsigned int *) 0x400FC0C4) //Power control for peripherals register
#define PCLKSEL0 (*(volatile unsigned int *) 0x400FC1A8) //Peripheral Clock selection

#define T0MR0 (*(volatile unsigned int *) 0x40004018) //
#define T0IR (*(volatile unsigned int *) 0x40004000) //
#define T0MCR (*(volatile unsigned int *) 0x40004014) //
#define T0TCR (*(volatile unsigned int *) 0x40004004) //
#define T0TC (*(volatile unsigned int *) 0x40004008) //



#define PWM1LER (*(volatile unsigned int *) 0x40018050)
#define PWM1MR0 (*(volatile unsigned int *) 0x40018018) //PWM Match Register 0 for PWM1. Can be used to control the frequency
#define PWM1MR1 (*(volatile unsigned int *) 0x4001801C) //PWM Match Register 1 for PWM1. Can be used to control the duty cycle
#define PWM1MCR (*(volatile unsigned int *) 0x40018014) //PWM Match Control Register
#define PWM1PCR (*(volatile unsigned int *) 0x4001804C) //PWM Control Register
#define PWM1TCR (*(volatile unsigned int *) 0x40018004) //PWM timer control register
#define PINSEL4 (*(volatile unsigned int *) 0x4002C010) //Pin select register to set one of the pins as an output for pulse width modulation
#define PINSEL1 (*(volatile unsigned int *) 0x4002C004) //Pin select register that we are using to select AOUT on P0.26 (set 10 to bits 21 and 20) and AIN(set 01 to bit 14 and 15)
#define PWM1IR (*(volatile unsigned int *) 0x40018000) //PWM Match Register 0 for PWM1. Can be used to control the frequency

#define DACR (*(volatile unsigned int *) 0x4008C000) //D to A converter register
#define AD0CR (*(volatile unsigned int *) 0x40034000) //Analog to Digital Control Register
#define AD0GDR (*(volatile unsigned int *) 0x40034004) //Analog to Digital Global Data Register

#define FIO0DIR (*(volatile unsigned int *) 0x2009C000)  //Direction register for port 0
#define FIO0PIN (*(volatile unsigned int *) 0x2009C014)  //Pin Register for port 0
#define PINMODE0 (*(volatile unsigned int *) 0x4002C040) //Pinmode register for switch
#define PINMODE1 (*(volatile unsigned int *) 0x4002C044) //Pinmode for ADC


volatile int amplitude;
int sigFin = 0;



void wait_ms(unsigned int ms)
{
	int us = ms*1000;
	int start = T0TC;  // note starting time
	T0TCR |= (1<<0);   // start timer
	while ((T0TC-start)<us) {} // wait for time to pass
}

//void wait_ms(float sec)
//{
//  wait_us(sec*1000); // convert seconds to microseconds
//}





void PWM1_IRQHandler(void) {
	// automatically calls and sets DACR value

	//PWM1IR = (1<<0); // MRO - rising edge
	//PWM1IR = (1<<1); // MR1 - falling edge

	if((PWM1IR>>0) & 1){
		DACR = (amplitude<<6);
		PWM1IR = (1<<0);
	} else if((PWM1IR>>1) & 1){
		DACR = (0<<6);
		PWM1IR = (1<<1);
	}


}


void decay(void) {
	amplitude = 1023;
	while(amplitude>0) {
		amplitude--;
		// wait between decrements
		wait_ms(1);
	}
}

void pwm_init(int period){
	PCONP |= (1<<6); //enable power for pwm1


	// Initialize PWM to 4 MHz / 6069.80 = 659Hz
	PCLKSEL0 |= (1<<12); // PWM PCLK = CCLK/1


	PWM1MR0 = period; // 6069.80 PCLK cycle period (8-bit equivalent)
	PWM1MR1 = period/2; // 6070/2-1 - --3035 PCLK rising edge = falling edge (50% duty cycle)
	// PWM1LER = (1<<1); // set bit 1 to use new MR1 value
	PWM1LER = (0b11<<0); // set bit 0 to use new MR0 value



	PWM1MCR = (1<<1); // Reset counter on MR0 match
	PWM1PCR = (1<<9); // Single edge mode and enable PWM1.1 only
	PWM1TCR = (1<<0) | (1<<3); // Enable counter and PWM mode


	PWM1MCR |= (1<<0); // MCR - enable MRO
	PWM1MCR |= (1<<3); // MCR - enable MR1

	ISER0 |= (1<<9); // enable PWM IR


}



void ain_funct(){



	PCONP |= (1<<12); //enable power for Analog to Digital

	AD0CR = (1<<21); //sets A/D converter to operational

	PCLKSEL0 |= (1<<24);  //set pclk for Analog to digital converter to 4MHz

	AD0CR |= (1<<0); //selects AD0.0 as the data to be converted

	AD0CR &= ~(1<<16); //makes sure the burst bit is 0 in the AD0CR register

	AD0CR |= (1<<24); //start A/D conversion

	int ad_readreg = 0b000000000000;

	while(((AD0GDR>>31) & 1) != 1){
		;//do nothing
	}

	ad_readreg = ((AD0GDR>>4)&(0xFFF)); //read Result from register

	AD0CR &= ~(1<<24); //clear the start bit

	if(ad_readreg > 3000){
		FIO0PIN &= ~(1<<21);  //turn off light
	}


	else if(ad_readreg < 1000){
		FIO0PIN |= (1<<21);
	}

	else{
		;
	}

}




int main(void) {
	PINSEL1 |= (1<<21); //Configure P0.26 as AOUT
	PINSEL4 |= (1<<0); // Configure P2.0 as PWM1.1
	PINSEL1 |= (1<<14); //Configure P0.23 as AIN

	FIO0DIR &= ~(1<<9); //set P0.9 as an input, the button
	PINMODE0 &= ~(1<<19) & ~(1<<18); //Pull UP on P0.9, the button


	FIO0DIR |= (1<<21); //set P0.0 as a output
	PINMODE1 |= (1<<11) | (1<<10); //set pulldown on P0.0

	PINMODE1 |= (1<<15) | (1<<14); //Pulldown for ADC




	while (1) {

		ain_funct(); //turn on or off the light

		//if button is pressed turn on speaker for 659Hz
		if ((((FIO0PIN>>9) & 1) != 1 )&& sigFin != 1){

			pwm_init(6070);
			decay();

			sigFin = 1;
		}


		//if button has been pressed and then let go, set frequency to 523Hz
		else if((((FIO0PIN>>9) & 1) == 1) && sigFin == 1){

			pwm_init(7648);
			decay();
			sigFin = 0;
		}



		// wait_ms(10); // avoid changing the D too fast
		// pwm_init(7648); // 6748
		// wait_ms(10); // avoid changing the D too fast
	}

	return 0 ;
}
