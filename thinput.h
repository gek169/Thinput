#include<stdint.h>
#include<stdlib.h>
#define THINPUT_BYTE uint8_t

#ifndef THINPUT_S
#define THINPUT_S uint8_t
#define THINPUT_L uint16_t
#define THINPUT_S_SHIFT 8
#define THINPUT_S_MAX 255
#define THINPUT_L_MAX 65535
#endif

#ifndef THINPUT_L
#error "THINPUT_L undefined. It must be exactly twice as large as THINPUT_S and an unsigned integer type"
#endif

#ifndef THINPUT_S_SHIFT
#error "THINPUT_S_SHIFT undefined. It should be the number of bits in THINPUT_S"
#endif

#ifndef THINPUT_S_MAX
#error "THINPUT_S_MAX undefined. it should be the all ones state, for THINPUT_S"
#endif

#ifndef THINPUT_L_MAX
#error "THINPUT_L_MAX undefined. it should be the all ones state, for THINPUT_L"
#endif

#ifndef THINPUT_MAX_INPUTS
#define THINPUT_MAX_INPUTS 256
#warning "THINPUT_MAX_INPUTS has been set to 256- the maximum number of inputs for a THINPUT_S = uint8_t"
#endif

#ifndef THINPUT_INS_SIZE
#error "Thinput requires the maximum size of your instruction buffer for its safety checking."
#endif

#define THINPUT_MAX_OP_EXTRA 2
#define THINPUT_OP_TERMINATE 0
/*Terminate execution. */
#define THINPUT_OP_COPY 1
#define THINPUT_OP_COPY_EXTRA 2
/*
Copy from from in to out
Size is 3S, THINPUT_S opcode, THINPUT_S input#, THINPUT_S output#
*/
#define THINPUT_OP_COPY_BACK 2
#define THINPUT_OP_COPY_BACK_EXTRA 2
/*Copy from out to in
Size is 3S, THINPUT_S opcode, THINPUT_S input#, THINPUT_S output#
*/
#define THINPUT_OP_WRITE 3
#define THINPUT_OP_WRITE_EXTRA 2
/*
Write a constant value to out
size is 3S, THINPUT_S opcode, THINPUT_S output#, THINPUT_S constant, 
*/
#define THINPUT_OP_WRITE_BACK 4
#define THINPUT_OP_WRITE_BACK_EXTRA 2

/*Identical to above, but writes to the in*/
#define THINPUT_OP_MULT_CONSTANT 5
#define THINPUT_OP_MULT_CONSTANT_EXTRA 2
/*
size is 3s, THINPUT_S opcode, THINPUT_S output, THINPUT_S constant
The result of the multiply is clamped between 0 and THINPUT_S_MAX.
The multiply is downshifted 
*/
#define THINPUT_OP_ADD_CONSTANT 6
#define THINPUT_OP_ADD_CONSTANT_EXTRA 2
/*
size is 3s, THINPUT_S opcode, THINPUT_S output, THINPUT_S constant
the result is clamped between 0 and THINPUT_S_MAX.
*/

#define THINPUT_OP_MULT 7
#define THINPUT_OP_MULT_EXTRA 2
/*
size is 3s, THINPUT_S opcode, THINPUT_S out, THINPUT_S out2
The result of the multiply is clamped between 0 and THINPUT_S_MAX.
The multiply is downshifted 
the result is written to out
*/
#define THINPUT_OP_ADD 8
#define THINPUT_OP_ADD_EXTRA 2
/*
size is 3s, THINPUT_S opcode, THINPUT_S output, THINPUT_S out2
the result is clamped between 0 and THINPUT_S_MAX.
the result is written to out
*/
#define THINPUT_OP_SUB_CONSTANT 9
#define THINPUT_OP_SUB_EXTRA 2
/*
3s, opcode, out, constant
the result of the subtract is output - constant and is written to out
The subtract is clamped and if the constant is larger than the value from out, zero will be written to out.
*/
#define THINPUT_OP_SUB 10
#define THINPUT_OP_SUB_EXTRA 2
/*
3s, opcode, output, out2
the result of the subtract is output - out2 and is written to out
The subtract is clamped and if the constant is larger than the value from out, zero will be written to out.
*/
#define THINPUT_OP_SKIP_GREATER_CONSTANT 11
#define THINPUT_OP_SKIP_GREATER_CONSTANT_EXTRA 2
/*
3s, opcode, out1, constant
skip the next instruction if out1 is greater than constant
*/
#define THINPUT_OP_SKIP_GREATER 12
#define THINPUT_OP_SKIP_GREATER_EXTRA 2
/*
3s, opcode, out1, out2
skip the next instruction if out1 is greater than out2
*/
#define THINPUT_OP_COPY_ALL 13
/*Copy the entire in to the out.*/


#define THINPUT_OP_NOTHING THINPUT_S_MAX

#ifndef THINPUT_NO_SAFETY_CHECKS
#define THINPUT_BAD(statement) if(statement) return 1;
#else
#define THINPUT_BAD(statement) /* a comment */
#endif

#define THINPUT_GET_BYTE(targ){\
	targ = *code;code++;\
	THINPUT_BAD(code >= code_max)\
}
#define THINPUT_READ_DATA(index){\
	THINPUT_GET_BYTE(insdata[index])\
}
#define THINPUT_SKIPCHECK() if(skipflag){skipflag = 0; DISPATCH();}

static THINPUT_S thinput_handle(THINPUT_S* in, THINPUT_S* out, THINPUT_S* code){
	static void* dispatch_table[] = { //MASH THAT PAGE DOWN BUTTON, IT'S A LONG RIDE!!!
	        &&do_THINPUT_OP_TERMINATE,		//Starting at LINE 140
			&&do_THINPUT_OP_COPY,
			&&do_THINPUT_OP_COPY_BACK,
			&&do_THINPUT_OP_WRITE,
			&&do_THINPUT_OP_WRITE_BACK,
			&&do_THINPUT_OP_MULT_CONSTANT,
			&&do_THINPUT_OP_ADD_CONSTANT,
			&&do_THINPUT_OP_MULT,
			&&do_THINPUT_OP_ADD,
			&&do_THINPUT_OP_SUB_CONSTANT,
			&&do_THINPUT_OP_SUB,
			&&do_THINPUT_OP_SKIP_GREATER_CONSTANT,
			&&do_THINPUT_OP_SKIP_GREATER,
			&&do_THINPUT_OP_COPY_ALL,
			&&do_THINPUT_OP_TERMINATE, // For the rest of all possible bytes...
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
			&&do_THINPUT_OP_TERMINATE, //
	        };
	    #define DISPATCH() THINPUT_GET_BYTE(ins); goto *dispatch_table[ins&255]
#ifdef DEFAULT_NO_RETURN
	int i = 0;
#endif
	THINPUT_S ins = THINPUT_OP_NOTHING;
	THINPUT_S insdata[THINPUT_MAX_OP_EXTRA];
	THINPUT_L treg1; /*Multiply and add register.*/
	THINPUT_L treg2; /*Multiply and add register.*/
	THINPUT_BYTE skipflag = 0;
	THINPUT_S* code_max = code + THINPUT_INS_SIZE-1;
	DISPATCH();
	{
			
		{
			do_THINPUT_OP_COPY_ALL:
				memcpy(out,in,THINPUT_MAX_INPUTS * sizeof(THINPUT_S));
			DISPATCH();
			do_THINPUT_OP_SKIP_GREATER_CONSTANT:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();
				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				if(out[insdata[0]] > insdata[1]) {skipflag = 1;}
			DISPATCH();
			do_THINPUT_OP_SKIP_GREATER:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();
				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				THINPUT_BAD(insdata[1] >= THINPUT_MAX_INPUTS)
				if(out[insdata[0]] > out[insdata[1]]) skipflag = 1;
			DISPATCH();
			do_THINPUT_OP_COPY_BACK:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();
				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				THINPUT_BAD(insdata[1] >= THINPUT_MAX_INPUTS)
				in[insdata[0]] =  out[insdata[1]];
			DISPATCH();
			do_THINPUT_OP_COPY:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				THINPUT_BAD(insdata[1] >= THINPUT_MAX_INPUTS)
				out[insdata[1]] = in[insdata[0]];
			DISPATCH();
			do_THINPUT_OP_WRITE:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				out[insdata[0]] = insdata[1];
			DISPATCH();
			do_THINPUT_OP_WRITE_BACK:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				in[insdata[0]] = insdata[1];
			DISPATCH();
			do_THINPUT_OP_MULT_CONSTANT:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				treg1 = insdata[1];
				treg2 = out[insdata[0]];
				treg1 = treg1 * treg2;
				treg1 = treg1>>THINPUT_S_SHIFT;
				if(treg1 > THINPUT_S_MAX){treg1 = THINPUT_S_MAX;}
				out[insdata[0]] = treg1;
			DISPATCH();
			do_THINPUT_OP_ADD_CONSTANT:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				treg1 = insdata[1];
				treg2 = out[insdata[0]];
				treg1 = treg1 + treg2;
				if(treg1 > THINPUT_S_MAX)treg1 = THINPUT_S_MAX;
				out[insdata[0]] = treg1;
			DISPATCH();
			do_THINPUT_OP_SUB_CONSTANT:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				treg2 = out[insdata[0]];
				treg1 = insdata[1];
				if(treg1 > treg2)
					treg1 = 0;
				else
					treg1 = treg2 - treg1;
				out[insdata[0]] = treg1;
			DISPATCH();

			do_THINPUT_OP_MULT:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				THINPUT_BAD(insdata[1] >= THINPUT_MAX_INPUTS)
				treg1 = out[insdata[1]];
				treg2 = out[insdata[0]];
				treg1 = treg1 * treg2;
				treg1 = treg1>>(THINPUT_S_SHIFT);
				if(treg1 > THINPUT_S_MAX){treg1 = THINPUT_S_MAX;}
				out[insdata[0]] = treg1;
			DISPATCH();
			do_THINPUT_OP_ADD:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				THINPUT_BAD(insdata[1] >= THINPUT_MAX_INPUTS)
				treg1 = out[insdata[1]];
				treg2 = out[insdata[0]];
				treg1 = treg1 + treg2;
				if(treg1 > THINPUT_S_MAX)treg1 = THINPUT_S_MAX;
				out[insdata[0]] = treg1;
			DISPATCH();
			do_THINPUT_OP_SUB:
				THINPUT_READ_DATA(0);
				THINPUT_READ_DATA(1);
				THINPUT_SKIPCHECK();

				THINPUT_BAD(insdata[0] >= THINPUT_MAX_INPUTS)
				THINPUT_BAD(insdata[1] >= THINPUT_MAX_INPUTS)
				treg2 = out[insdata[0]];
				treg1 = out[insdata[1]];
				if(treg1 > treg2)
					treg1 = 0;
				else
					treg1 = treg2 - treg1;
				out[insdata[0]] = treg1;
			DISPATCH();
			do_THINPUT_OP_TERMINATE:return 0;
		}
#ifdef DEFAULT_NO_RETURN
		i++;
#endif
	}
	return 0;
}
