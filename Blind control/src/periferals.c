/*
 * periferals.c
 *
 * Created: 15.03.2014 16:38:30
 *  Author: Anders
 */ 
#include <avr/io.h>
#include "periferals.h"
#define F_CPU 8000000
#include <avr/delay.h>

#define SetBit(x,y) (x|=(1<<y))
#define ClrBit(x,y) (x&=~(1<<y))
#define ToggleBit(x,y) (x^=(1<<y))
#define CheckBit(adr,bit) (adr & (1<<bit))

#define upward_time 16000
#define downward_time 14000

int has_pressed_mode=1;
int has_pressed_up=1;


int mode=AUTO;



void led_on(int led);
void led_off(int led);



void led_init(){
	DDRD |= (1 << led_2);
	DDRB |= (1 << led_1) | (1 << led_3);
	for (int i=1;i<4;i++){
		led_off(i);
	}
	
	 
}
void buttons_init(){
	ClrBit(DDRB,button_1);
	ClrBit(DDRD,button_2);
	ClrBit(DDRD,button_3);
}

void periferals_init(){
	// PortC as output
	DDRC |= (1 << output_1) | (1 << output_2) | (1 << output_3) | (1 << output_4) | (1 << output_5) | (1 << output_6);
	PORTC=0;
	led_init();
	buttons_init();
}

void motor_auto_up(){
	set_output(output_5,1);
	set_output(output_6,0);
	led_on(UP);
	_delay_ms(upward_time);
}
void motor_auto_down(){
	set_output(output_5,0);
	set_output(output_6,1);
	led_on(DOWN);
	_delay_ms(downward_time);
	
}

void motor_up(){
	set_output(output_5,1);
	set_output(output_6,0);
	led_on(UP);
	led_off(DOWN);
	_delay_ms(1000);
}
void motor_down(){
	set_output(output_5,0);
	set_output(output_6,1);	
	led_on(DOWN);
	led_off(UP);
	_delay_ms(1000);
}

void motor_stop(){
	set_output(output_5,0);
	set_output(output_6,0);
	led_off(UP);
	led_off(DOWN);
}

void led_off(int led){
	if(led==UP){SetBit(PORTB,led_1);}
	if(led==DOWN){SetBit(PORTD,led_2);}
	if(led==MODE){SetBit(PORTB,led_3);}
}
void led_on(int led){
	if(led==UP){ClrBit(PORTB,led_1);}
	if(led==DOWN){ClrBit(PORTD,led_2);}
	if(led==MODE){ClrBit(PORTB,led_3);}
}

void on_button_pressed(int button){
	
	if(button==MODE){
		toggle_mode();
	}
	
	if(button==UP && has_pressed_up==0){
		has_pressed_up=1;
		if (mode==AUTO){
			motor_auto_up();
		}
		else if(mode==MANUAL){
			motor_up();
		}
	}
	if(button==DOWN){
		if (mode==AUTO){
			motor_auto_down();
		}
		else if(mode==MANUAL){
			motor_down();
		}
	}
}
void on_button_released(int button){
	if(button==MODE){
		//mode=AUTO;
		//led_off(MODE);
		has_pressed_mode=0;
		
	}
	if(button==UP){
		set_output(output_5,0);
		led_off(UP);
		has_pressed_up=0;
	}
	if(button==DOWN){
		set_output(output_6,0);
		led_off(DOWN);
	}
}

void toggle_mode(){
	if (mode==AUTO){
		mode=MANUAL;
		led_on(MODE);
		_delay_ms(500);
	}
	else if (mode==MANUAL){
		mode=AUTO;
		led_off(MODE);
		_delay_ms(500);
	}
}

void read_buttons(){
	if (CheckBit(PIND,button_3)){
		on_button_pressed(MODE);
	}
	else if (!CheckBit(PIND,button_3)){
		on_button_released(MODE);
	}
	
	if (CheckBit(PINB,button_1)){
		 on_button_pressed(UP);
	}
	else if(!CheckBit(PINB,button_1)){
		on_button_released(UP);
	}
	
	if (CheckBit(PIND,button_2)){
		 on_button_pressed(DOWN);
	}
	else if (!CheckBit(PIND,button_2)){
		on_button_released(DOWN);
	}
	
}

void set_output(uint8_t output,uint8_t value){
	if (value==1) SetBit(output_port,output);
	else if (value==0) ClrBit(output_port,output);
}
