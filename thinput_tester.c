#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define THINPUT_MAX_INPUTS 127
#define THINPUT_INS_SIZE 0x10000
//Optional defines, but if you define one, you must define them all.
/*
#define THINPUT_S uint16_t
#define THINPUT_L uint32_t
#define THINPUT_S_SHIFT 16
#define THINPUT_S_MAX 65535
#define THINPUT_L_MAX 4294967295
*/
//used for fuzzing
#define DEFAULT_NO_RETURN
#include "thinput.h"

THINPUT_S raw_inputs[THINPUT_MAX_INPUTS];
THINPUT_S processed_inputs[THINPUT_MAX_INPUTS];
THINPUT_S instructions[0x10000];

int main(int argc, char** argv){
	srand(time(NULL));
	THINPUT_S mode = 0;
	THINPUT_S dorandom = 0;
	THINPUT_S last_off = 1;
	THINPUT_S nvalid = 8;
	for(int i = 1; i < argc; i++){
		if(!strcmp("-inp", argv[i])){
			mode = 1;
			last_off = i+1;
			continue;
		}
		if(!strcmp("-rand",argv[i])){
			dorandom = 1;
			break;
		}
		if(!strcmp("-ins", argv[i])){
			mode = 0;
			last_off = i+1;
			continue;
		}
		if(mode)
			raw_inputs[i-last_off] = (THINPUT_S)atoi(argv[i]);
		else
			instructions[i-last_off] = (THINPUT_S)atoi(argv[i]);
	}
	for(int i = 0; i < THINPUT_MAX_INPUTS; i++)
		processed_inputs[i] = 0;
	if(dorandom){
		for(int i = 0; i < THINPUT_MAX_INPUTS; i++)
			raw_inputs[i] = rand();
		for(int i = 0; i < THINPUT_INS_SIZE; i++)
				instructions[i] = rand();
	}
	thinput_handle(raw_inputs, processed_inputs, instructions);
	for(int i = 0; i < nvalid; i++)
		printf("raw input %d is %d\n",i,(int)raw_inputs[i]);
	for(int i = 0; i < nvalid; i++)
		printf("Processed input %d is %d\n",i,(int)processed_inputs[i]);
	printf("THINPUT_S_MAX is %d", THINPUT_S_MAX);
	return 0;
}
