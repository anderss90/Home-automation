
#include <asf.h>
#define SetBit(x,y) (x|=(1<<y))
#define ClrBit(x,y) (x&=~(1<<y))
#define ToggleBit(x,y) (x^=(1<<y))
#define CheckBit(adr,bit) (adr & (1<<bit))



 
 //#define  F_CPU 16000000
 #include <avr/io.h>  
 #include <avr/interrupt.h>  
 #include <string.h>
 #include <stdio.h>
  #define F_CPU 8000000
 #include <avr/delay.h> 
 #include "radio.h"
 #include "periferals.h"
 
 
 void one_on(){
	 set_output(output_1,1);
	 
 }
 void one_off(){
	 set_output(output_1,0);
 }
void two_on(){
	toggle_mode();
	
	
}
void two_off(){
	set_output(output_2,0);
}
void three_on(){
	on_button_pressed(UP);
}
void three_off(){
	on_button_pressed(DOWN);
}
void all_off(){
	one_off();
	two_off();
	//three_off();
}

 void process_data(){
	 {
		 cli();
		 char buff[8];
		 uint8_t tmp = data &0x000000FF;
		 sprintf(buff,"%04X", (data>>(16+6))&0x0000FFFF);
		 sprintf(buff,"%04X", (data>>6)&0xFFFF);
		 
		 // ON or OFF
		 if (tmp & 0x10)  //ON
		 {
			 if ((tmp & 0x0F) == 0x00) one_on();
			 if ((tmp & 0x0F) == 0x01) two_on();
			 if ((tmp & 0x0F) == 0x02) three_on();
		 }
		 else
		 {
			 if ((tmp & 0x0F) == 0x00) one_off();
			 if ((tmp & 0x0F) == 0x01) two_off();
			 if ((tmp & 0x0F) == 0x02) three_off();
			 
		 }
		 sei();
		 dataready = 0;
		 cli();
		 sei();
	 }
 }
 
 void debug_leds(){
	 int i;
	 for (i=1;i<4;i++){
		 led_on(i);
		 _delay_ms(500);
		 led_off(i);
		 _delay_ms(1000);
	 }
	 
 }
 
 void debug_led_1(){
	 led_on(3);
	 _delay_ms(500);
	 led_off(3);
	 _delay_ms(1000);
 }
 
 int main(void)  
 {
	board_init();
	rf_init();
	periferals_init();
	while(1)
	{
		if (dataready==1)  process_data();
		read_buttons();
		//debug_leds();
		
		
	}
 }  


