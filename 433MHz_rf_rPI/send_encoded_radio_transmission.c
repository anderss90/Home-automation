#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcm2835.h>

#define PIN_TX RPI_GPIO_P1_22
#define PIN_RX RPI_GPIO_P1_07
#define RETURN_SUCCESS 1
#define RETURN_ERROR -1
#define N_ARGS 2
char* recording; 
char* recording_name;
int sample_time; // In seconds 
int sample_rate; // In Hertz
int sample_size; // Number of samples
int sample_interval; 
int i; // iterator
const char* read_path="/home/pi/workspace/433_recordings/";

void debug_args();

int send_recording(char *recording,int time, int rate){
	
	
	sample_interval=(1000*1000/rate); //microseconds
	sample_size = time*rate;
	bcm2835_gpio_fsel(PIN_TX,BCM2835_GPIO_FSEL_OUTP);
	
	printf("Sending for %d seconds\n",time);
	fflush(stdout);
	
        for (i=0;i<sample_size;i++){
        	if(recording[i]==1){
        		bcm2835_gpio_set(PIN_TX);
        	}
        	else if(recording[i]==0){
        		bcm2835_gpio_clr(PIN_TX);
        	}
        	bcm2835_delayMicroseconds(sample_interval);
        }
        bcm2835_gpio_clr(PIN_TX);
        //bcm2835_gpio_fsel(PINT_TX,BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_write(PIN_TX,LOW);
	printf("send_recording returning\n");
	return RETURN_SUCCESS;
}

char* read_recording_from_file(char* name,const char* path){
	printf("Reading from file %s\n",name);
	char *file_name=calloc(100,sizeof(char));
	*file_name='\0';
	strcat(file_name,path);
	strcat(file_name,name);
	strcat(file_name,".txt");

	FILE *recording_file = fopen(file_name, "r");
	if (recording_file == NULL){
		printf("Error opening file!\n");
	        return RETURN_ERROR;
	}
	char junk[100];
	i=0;
	int intjunk[100];
	int time;
	int rate;
	int value;
	//getting meta data
	//fflush(stdout);
	fscanf(recording_file,"%s %i %s %i",junk,&time,junk,&rate);
	
	sample_interval=(1000*1000/rate);
	sample_size=time*rate;
	sample_time=time;
	sample_rate=rate;
	fflush(stdout);
	
	//getting actual data
	char *recording=calloc(sample_size,sizeof(char));
	
	while (fscanf(recording_file,"%s %i %s %i",junk,intjunk,junk,&value)==4){
		recording[i]=(char)value;
		i++;
	}
        fclose(recording_file);       
        printf("Reading from file %s done\n",name);
        fflush(stdout);
        
        //free allocated memory
        free(file_name);
        return recording;
}

void debug_recording(char* recording){
	printf("printing recording");
	for (i=0;i<sample_size;i++){
		printf("%d\n",recording[i]);
		
		fflush(stdout);
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
	

int main(int argc, char **argv){
	
	// init the gpio 
	if (!bcm2835_init()){return RETURN_ERROR;}
	
	//read the command line arguments
	if (read_arguments(argc,argv)==RETURN_ERROR){return RETURN_ERROR;}
	
	//read recording from file
	recording=read_recording_from_file(recording_name,read_path);
	//debug_recording(recording);
	while (send_recording(recording,sample_time,sample_rate)!=RETURN_SUCCESS){}//loop intil success
	
	//close gpio
	bcm2835_close();
	printf("\nMy work here is done\n");
return RETURN_SUCCESS;
}	
