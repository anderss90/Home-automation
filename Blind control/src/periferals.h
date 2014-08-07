/*
 * periferals.h
 *
 * Created: 15.03.2014 16:38:42
 *  Author: Anders
 */ 


#ifndef PERIFERALS_H_
#define PERIFERALS_H_

#define output_port PORTC
#define output_1 PC0
#define output_2 PC1
#define output_3 PC2
#define output_4 PC3
#define output_5 PC4
#define output_6 PC5

#define led_1 PB6
#define led_2 PD5
#define led_3 PB7


#define button_1 PB0
#define button_2 PD7
#define button_3 PD6

enum {AUTO, MANUAL};
enum {DUMMY,UP,DOWN,MODE};

void periferals_init();
void set_output(uint8_t output,uint8_t value);
void led_init();
void buttons_init();
void led_on(int led);
void led_off(int led);
void on_button_pressed(int button);
void on_button_released(int button);
void read_buttons();
void toggle_mode();




#endif /* PERIFERALS_H_ */