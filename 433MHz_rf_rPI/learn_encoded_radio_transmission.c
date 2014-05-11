#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcm2835.h>

#define PIN_TX RPI_GPIO_P1_22
#define PIN_RX RPI_GPIO_P1_07
#define RETURN_SUCCESS 1
#define RETURN_ERROR -1
#define N_ARGS 4
char* recording; 
char* recording_name;
int sample_time; // In seconds 
int sample_rate; // In Hertz
int sample_size; // Number of samples
int sample_interval; 
int i; // iterator
char* save_path="433_recordings/";

void debug_args();

char* record_burst(int time,int rate){
	
	
	sample_interval=(1000*1000/rate); //microseconds
	sample_size = time*rate;
	bcm2835_gpio_fsel(PIN_RX,BCM2835_GPIO_FSEL_INPT);
	
	char* placeholder=calloc(sample_size,sizeof(char));
	
	printf("Recording in 3\n");
	fflush(stdout);
	bcm2835_delay(1000);
	printf("Recording in 2\n");
	bcm2835_delay(1000);
	printf("Recording in 1\n");
	bcm2835_delay(1000);
	
	printf("Recording for %d seconds\n",time);
	printf("sample_size: %d\n",sample_size);
	fflush(stdout);
	
        for (i=0;i<sample_size;i++){
        	placeholder[i]=(char)bcm2835_gpio_lev(PIN_RX);
        	bcm2835_delayMicroseconds(sample_interval);
        }
	printf("record_burst returning\n");
	return placeholder;
}

int save_recording_to_file(char *recording,char* name,char*save_path){
	if(recording==NULL){
		printf("input ptr is NULL");
		return RETURN_ERROR;
	}
	printf("Recording to file %s\n",name);
	char *file_name=calloc(100,sizeof(char));
	*file_name='\0';
	strcat(file_name,save_path);
	strcat(file_name,name);
	strcat(file_name,".txt");

	FILE *recording_file = fopen(file_name, "w");
	if (recording_file == NULL){
		printf("Error opening file!\n");
	        return RETURN_ERROR;
	}
	
	//writing meta data
	fprintf(recording_file,"sample_time: %i sample_rate: %i\n",sample_time,sample_rate);
	
	for (i=0;i<sample_size;i++){
		fprintf(recording_file,"TIME: %d VALUE: %d\n",(int)(i*sample_interval),recording[i]);
	}

	//closing file
        fclose(recording_file);
        printf("Recording to file %s done",name);
        
        //Deallocating allocating memory
        free(file_name); 
        return RETURN_SUCCESS;
}

int read_arguments(int argc,char **argv){
	if (argc<N_ARGS){
		printf("Provide arguments: Sample time in seconds, Sample Rate in Hertz, Recording name\n");
		return RETURN_ERROR;
	}
	sample_time = atoi(argv[N_ARGS-3]); 
	sample_rate = atoi(argv[N_ARGS-2]);
	sample_size = sample_time*sample_rate;
	recording_name = argv[N_ARGS-1];
	sample_interval = (1000*1000/sample_rate);
	debug_args();
	
	//Checking if input makes sense
	if (sample_time<=0 || sample_rate<=0 || strlen(recording_name)<=0 || strlen(recording_name)>=90){
		printf("Illegal argument error\n");
		return RETURN_ERROR;	
	}
}
	
void debug_args(){
	printf("sample_time: %d sample_rate: %d Name: %s sample_size: %d sample_interval: %d\n",sample_time,sample_rate,recording_name,sample_size,sample_interval);
	fflush(stdout);
}

//just for debugging
char* dummy_recorder(){
	char * placeholder = (char*)malloc(sample_size);
	printf("Starting dummy recorder\n");
	printf("placeholder ptr:%d\n",placeholder);
	fflush(stdout);
	
	if (placeholder==NULL){
		printf("pointer is NULL\n");
		return NULL;
	}
	
	for (i=0;i<10;i++){
		placeholder[i]=(i%2);
	}
	placeholder[i]='\0';
	printf("Dummy recorder returning\n");
	fflush(stdout);
	return placeholder;
}


//function for other programs, using this as a library
int record_and_save_to_file(int recording_time,int recording_rate,char* recording_name,char* path){
	if(path!=NULL) save_path=path; 
	// init the gpio 
	if (!bcm2835_init()){return RETURN_ERROR;}
	
	//record stuff        
	int recording_size=recording_time*recording_rate;
	int recording_interval=(1000*1000/recording_rate);
	char recording[recording_size];
	record_burst(recording_time,recording_rate);
	if (recording==NULL) return RETURN_ERROR;
	if(save_recording_to_file(recording,recording_name,save_path)==RETURN_ERROR){return RETURN_ERROR;}
	
	//close gpio
        bcm2835_close();

        //free allocated memory
        free(recording);
	return RETURN_SUCCESS;
}
int main(int argc, char **argv){
	
	// init the gpio 
	if (!bcm2835_init()){return RETURN_ERROR;}
	
	//read the command line arguments
	if (read_arguments(argc,argv)==RETURN_ERROR){return RETURN_ERROR;}
	
	//record radio signals
	recording=record_burst(sample_time,sample_rate);
	if (recording==NULL) return RETURN_ERROR;
	
	//save recording to file
	if(save_recording_to_file(recording,recording_name,save_path)==RETURN_ERROR){return RETURN_ERROR;}
	
	//close gpio
	bcm2835_close();
	
	//free allocated memory
	free(recording);
	printf("\nMy work here is done\n");
return RETURN_SUCCESS;
}	
