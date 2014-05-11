#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
#include <string.h>

#define pulse_time 300
#define zero_time 300
#define one_time 1200
#define starting_pulse 2650
#define finishing_pulse 10000
#define N_BURSTS 5
#define PIN_TX RPI_GPIO_P1_22
#define msg_size 32
#define NEXA_MSG_SIZE_32   32
#define N_ARGS 2
#define RETURN_ERROR -1
#define RETURN_SUCCESS 1

char* recording_name;

char NEXA_1_ON[NEXA_MSG_SIZE_32] = 
{0x00,0x00,0x01,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00};
char NEXA_1_OFF[NEXA_MSG_SIZE_32]  = {0x00,0x00,0x01,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

char NEXA_2_ON[NEXA_MSG_SIZE_32] = 
{0x00,0x00,0x01,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x01};
char NEXA_2_OFF[NEXA_MSG_SIZE_32]  = {0x00,0x00,0x01,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

char NEXA_3_ON[NEXA_MSG_SIZE_32] = 
{0x00,0x00,0x01,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00};
char NEXA_3_OFF[NEXA_MSG_SIZE_32]  = {0x00,0x00,0x01,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00};




void send_short_pulse(){
	 bcm2835_gpio_set(PIN_TX);
	 bcm2835_delayMicroseconds(pulse_time);
	bcm2835_gpio_clr(PIN_TX);
}

void send_starting_pulse(){
	send_short_pulse();
	bcm2835_delayMicroseconds(starting_pulse);
}

void send_one(){
	send_short_pulse();
	bcm2835_delayMicroseconds(one_time);
	send_short_pulse();
	bcm2835_delayMicroseconds(zero_time);
}

void send_zero(){
	send_short_pulse();
        bcm2835_delayMicroseconds(zero_time);
        send_short_pulse();
        bcm2835_delayMicroseconds(one_time);
}

void send_cmd(char* cmd, int length){
	int i;
	int j;
	for (i=0;i<N_BURSTS;i++){
		send_starting_pulse();
		for (j=0;j<length;j++){
			if(cmd[j]==1)send_one();
			else if(cmd[j]==0)send_zero();
		}
		send_short_pulse();
		bcm2835_delayMicroseconds(finishing_pulse);

	}
}

int read_arguments(int argc,char **argv){
        if (argc<N_ARGS){
                printf("Provide arguments: Recording name\n");
                return RETURN_ERROR;
        }
        recording_name = argv[N_ARGS-1];

        //Checking if input makes sense
        if (strlen(recording_name)<=0 || strlen(recording_name)>=90){
                printf("Illegal argument error\n");
                return RETURN_ERROR;
        }
}


int main(int argc,char **argv){

	// init the gpio
        if (!bcm2835_init()){
		printf("Failed to init bcm2835");
		return RETURN_ERROR;}

        //set TX output
	 bcm2835_gpio_fsel(PIN_TX,BCM2835_GPIO_FSEL_OUTP);


	//read the command line arguments
        if (read_arguments(argc,argv)==RETURN_ERROR){return RETURN_ERROR;}
	if (!strcmp(recording_name,"1on"))send_cmd(NEXA_1_ON,NEXA_MSG_SIZE_32);
	else if (!strcmp(recording_name,"1off"))send_cmd(NEXA_1_OFF,NEXA_MSG_SIZE_32);
	else if (!strcmp(recording_name,"2on"))send_cmd(NEXA_2_ON,NEXA_MSG_SIZE_32);
	else if (!strcmp(recording_name,"2off"))send_cmd(NEXA_2_OFF,NEXA_MSG_SIZE_32);
	else if (!strcmp(recording_name,"3on"))send_cmd(NEXA_3_ON,NEXA_MSG_SIZE_32);
	else if (!strcmp(recording_name,"3off"))send_cmd(NEXA_3_OFF,NEXA_MSG_SIZE_32);

	bcm2835_close();
	
	return 0;
}
