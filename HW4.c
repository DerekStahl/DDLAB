/*
===============================================================================
 Name        : HW4.c
 Author      : Derek Stahl and Amilton Pensamento
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>


#define PINSEL1 (*(volatile unsigned int *) 0x4002C004) //Pin select for SDA and SCL
#define PCLKSEL0 (*(volatile unsigned int *) 0x400FC1A8) //Peripheral clock select 0
#define I2C0SCLH (*(volatile unsigned int *) 0x4001C010) //High duty cycle for clock
#define I2C0SCLL (*(volatile unsigned int *) 0x4001C014) //Low duty cycle time for clock
#define I2C0CONSET (*(volatile unsigned int *) 0x4001C000)  //This is the control set register
#define I2C0CONCLR (*(volatile unsigned int *) 0x4001C018) //This is the control clear register
#define I2C0DAT (*(volatile unsigned int *) 0x4001C008) //This is the I2C Data register
#define I2C0STAT (*(volatile unsigned int *) 0x4001C004) //This is the I2C Status register
#define IODIRA 0x00 //This IODIRA register
#define IODIRB 0x01 //This IODIRB register
#define GPIOA 0x12 //input output register for port a
#define GPIOB 0x13 //input output register for port b
#define RTR 0x00 //temperature input reading

int data_read;
// example;  #define MCP_OP 0x40;

//lefts for seven segment display
int zero = 0b0111111;
int one = 0b0000110;
int two = 0b1011011;
int three = 0b1001111;
int four = 0b1100110;
int five = 0b1101101;
int six = 0b1111101;
int seven = 0b0000111;
int eight = 0b1111111;
int nine = 0b1100111;

//io expander opcode
int ioexpOP = 0b0100000;
//temperature sensor opcode
int tempOP = 0b1001001;


void wait_ticks(int ticks)
{
volatile int count;
for (count=0; count<ticks; count++){
//do nothing
}
}


//this is the smart function utilized by the larger read and write functions
void start(){
//START CONDITION
I2C0CONSET = (1<<3);// This sets the SI bit in the control register to 1
I2C0CONSET = (1<<5);//This sets the start bit to 1 in the control register to 1

I2C0CONCLR = (1<<3);//This clears the SI bit

while(((I2C0CONSET>>3) & 1) != 1){   //do nothing while SI bit is 0

 }

I2C0CONCLR = (1<<5);//This clears the start bit
//START CONDITION FINISHED
}




//this is the small write function
void write_data(int data){

I2C0DAT = data;
I2C0CONCLR = (1<<3);//This clears the SI bit

while(((I2C0CONSET>>3) & 1) != 1){   //do nothing while SI bit is 0

}

}
//This is the small read function
int read_data(){

I2C0CONCLR = (1<<2);//This clears the assert acknowledge bit in the Control register
I2C0CONCLR = (1<<3);//This clears the SI bit in the control register

while(((I2C0CONSET>>3) & 1) != 1){   //do nothing while SI bit is 0

}

int data_read = I2C0DAT;//assign the values in the data register to data_read for later
return data_read;
}

//This is the stop function
void stop(){

I2C0CONSET = (1<<4);//This stops the control register
I2C0CONCLR = (1<<3);//This clears the SI bit

while(((I2C0CONSET>>1) & 1) == 1){   //wait for stop bit to go to zero
  I2C0DAT = data;
I2C0CONCLR = (1<<3);//This clears the SI bit

while(((I2C0CONSET>>3) & 1) != 1){   //do nothing while SI bit is 0

}

}
//This is the small read function
int read_data(){

I2C0CONCLR = (1<<2);//This clears the assert acknowledge bit in the Control register
I2C0CONCLR = (1<<3);//This clears the SI bit in the control register

while(((I2C0CONSET>>3) & 1) != 1){   //do nothing while SI bit is 0

}

int data_read = I2C0DAT;//assign the values in the data register to data_read for later
return data_read;
}

//This is the stop function
void stop(){

I2C0CONSET = (1<<4);//This stops the control register
I2C0CONCLR = (1<<3);//This clears the SI bit

while(((I2C0CONSET>>1) & 1) == 1){   //wait for stop bit to go to zero

}

}

// write_seq - start | write([OP+W]) | write(ADDR) | write(Din) | stop
// read_seq  -  start | write([OP+W]) | write(ADDR) | restart | write([OP+R]) | read(Dout) | stop
// OP is 7 bits - W/R is 1 bit
void write_seq(int OP, int ADDR, int Din) {
start(); //start condition from initiator
int W = 0;
int OPW = (OP<<1)|W; // 8 bits, OPW is step two in the write sequence. In start sequence of write sequence

write_data(OPW);
write_data(ADDR);
write_data(Din);
stop();
}

//reads data from whatever device selected
int read_seq(int OP, int ADDR){
start(); //start condition
int OPR;
int R = 1;
int W = 0;
int OPW = (OP<<1)|W;
write_data(OPW);
write_data(ADDR);
start();
OPR = (OP<<1)|R;
write_data(OPR); }

}

// write_seq - start | write([OP+W]) | write(ADDR) | write(Din) | stop
// read_seq  -  start | write([OP+W]) | write(ADDR) | restart | write([OP+R]) | read(Dout) | stop
// OP is 7 bits - W/R is 1 bit
void write_seq(int OP, int ADDR, int Din) {
start(); //start condition from initiator
int W = 0;
int OPW = (OP<<1)|W; // 8 bits, OPW is step two in the write sequence. In start sequence of write sequence

write_data(OPW);
write_data(ADDR);
write_data(Din);
stop();
}

//reads data from whatever device selected
int read_seq(int OP, int ADDR){
start(); //start condition
int OPR;
int R = 1;
int W = 0;
int OPW = (OP<<1)|W;
write_data(OPW);
write_data(ADDR);
start();
OPR = (OP<<1)|R;
write_data(OPR);
int data = read_data();
stop();
return data;

}

//converts the decimal number to display on the left seven segment display
void leftconversion(int left){
if(left == 0){
write_seq(ioexpOP, GPIOA, zero);
}
else if(left == 1){
write_seq(ioexpOP, GPIOA, one);
}
else if(left == 2){
write_seq(ioexpOP, GPIOA, two);
}
else if(left == 3){
write_seq(ioexpOP, GPIOA, three);
}
else if(left == 4){
write_seq(ioexpOP, GPIOA, four);
}
else if(left == 5){
write_seq(ioexpOP, GPIOA, five);
}
else if(left == 6){
write_seq(ioexpOP, GPIOA, six);
}
else if(left == 7){
write_seq(ioexpOP, GPIOA, seven);
}
else if(left == 8){
write_seq(ioexpOP, GPIOA, eight);
}
else if(left == 9){
write_seq(ioexpOP, GPIOA, nine);
}
}

//converts the decimal number to display on the left seven segment display
void rightconversion(int right){
if(right == 0){
write_seq(ioexpOP, GPIOB, zero);
}
else if(right == 1){
write_seq(ioexpOP, GPIOB, one);
}
else if(right == 2){
write_seq(ioexpOP, GPIOB, two);
}
else if(right == 3){
write_seq(ioexpOP, GPIOB, three);
}
else if(right == 4){
write_seq(ioexpOP, GPIOB, four);
}
else if(right == 5){
write_seq(ioexpOP, GPIOB, five);
}
else if(right == 6){
write_seq(ioexpOP, GPIOB, six); }
else if(left == 8){
write_seq(ioexpOP, GPIOA, eight);
}
else if(left == 9){
write_seq(ioexpOP, GPIOA, nine);
}
}

//converts the decimal number to display on the left seven segment display
void rightconversion(int right){
if(right == 0){
write_seq(ioexpOP, GPIOB, zero);
}
else if(right == 1){
write_seq(ioexpOP, GPIOB, one);
}
else if(right == 2){
write_seq(ioexpOP, GPIOB, two);
}
else if(right == 3){
write_seq(ioexpOP, GPIOB, three);
}
else if(right == 4){
write_seq(ioexpOP, GPIOB, four);
}
else if(right == 5){
write_seq(ioexpOP, GPIOB, five);
}
else if(right == 6){
write_seq(ioexpOP, GPIOB, six);
}
else if(right == 7){
write_seq(ioexpOP, GPIOB, seven);
}
else if(right == 8){
write_seq(ioexpOP, GPIOB, eight);
}
else if(right == 9){
write_seq(ioexpOP, GPIOB, nine);
}
}





// MCP op code - 0100...000 - 7 bits


int main(void) {

PINSEL1 &= ~(1<<23); //This sets bit 22 and 23 in the pin select register 1
//to 1 and 0 respectively (SDA0)
PINSEL1 |= (1<<22);


PINSEL1 &= ~(1<<25);
PINSEL1 |= (1<<24);  //This sets bit 24 and 25 in the pin select register 1
//to 1 and 0 respectively (SCL0)



// PCLKSEL0 = 0; //This makes the clock divided by 4

I2C0SCLL = 5; //this sets the duty cycle to be 50% and the frequency 100KHz in conjunction with
I2C0SCLH = 5; //each other

I2C0CONCLR = (1<<6); //This clears enable the I2C0 register
I2C0CONSET = (1<<6); //This set enable the I2C0 register

//END OF INITIALIZATION


// setup direction of pins
write_seq(ioexpOP, IODIRA, 0b10000000); // 1-ip, 0-op, pin 28 input, everything else on port a output
write_seq(ioexpOP, IODIRB, 0b00000000);//everything on port b is an output

int isC = 1;

while(1){
// Temperature reading from sensors

//if button is pressed, switch isc
if (((read_seq(ioexpOP, GPIOA))>>7) == 1) {
isC = !isC;
}

//if isc true, write in fahrenheit
if (isC) {

int dataread = read_seq(tempOP, RTR);
dataread = (int)((dataread*1.8)+32);
int left = dataread/10;
int right = dataread%10;
leftconversion(left);
rightconversion(right);

}

//if isc false, write in celsius
else {

int dataread = read_seq(tempOP, RTR);
int left = dataread/10;
int right = dataread%10;
leftconversion(left);
rightconversion(right);

}
wait_ticks (50000);
}
    return 0;

}
