
# An uber-portable input processing library that's perfect for you and the end user

Thinput is a tiny C99, header-only, static inlined, single-function solution for processing received user inputs to achieve
runtime keymapping, which embeds quickly and easily into your application.

Never release programs with "debug keyboard layouts" again! Every program you write will feature full keyboard/button/joystick mapping,
with every input able to be mapped to every single action, entirely editable at runtime.

# How do I use the library?

See thinput_tester.c if you want to see the input mapper in action.

```c
gcc thinput_tester.c -o tinput
```

use the `-inp` flag followed by a series of integers to simulate the output of your favorite input-reading library, like
SDL or Dinput.

then use -ins followed by a series of integers to simulate the bytecode of the keymapping

you can read thinput.h to see the list of opcodes and the arguments they take.

```c
./tinput -inp 1 2 3 4 5 255 7 8 -ins 1 1 1    1 2 2  4 7 230    3 7 255    12 2 1  5 7 254  5 7 128
```

```c
#define THINPUT_MAX_INPUTS 127
#define THINPUT_INS_SIZE 0x10000
```
This code defines the size of the input arrays passed to thinput_handle.

These are *NOT* passed to thinput_handle as arguments to optimize the speed of execution.

```c
#define THINPUT_S uint16_t
#define THINPUT_L uint32_t
#define THINPUT_S_SHIFT 16
#define THINPUT_S_MAX 65535
#define THINPUT_L_MAX 4294967295
```
This code optionally changes thinput to use 16 bit unsigned integers. You should be able to deduce how you could
change these statements to make the library use arbitrary sizes of unsigned integers.

If you are going to use thinput for mouse input or the like, it is recommended you use 16 bit unsigned integers.

```c
#include "thinput.h"
```
Include the library, which builds the static inline function.

```c
THINPUT_S raw_inputs[THINPUT_MAX_INPUTS];
THINPUT_S processed_inputs[THINPUT_MAX_INPUTS];
THINPUT_S instructions[THINPUT_INS_SIZE];
```
The arrays operated on by the library. Note that these arrays are declared global for demonstration purposes only,
any pointers can be passed to thinput_handle.



For reasons i'll explain later on, you may also want to include these statements at the beginning of main:
```c
instructions[0] = 13;
instructions[1] = 0;
```



In your code, you will have somewhere where you read in keyboard,controller, etc inputs which may or may not be
platform dependent.

In SDL, you typically have an event loop such as this:
```c
SDL_Event evt;
while (SDL_PollEvent(&evt))
switch (evt.type) {
case SDL_KEYDOWN:
	switch (evt.key.keysym.sym) {
	case SDLK_UP:
		view_rotx += 5.0;
		break;
	case SDLK_DOWN:
		view_rotx -= 5.0;
		break;
	case SDLK_LEFT:
		view_roty += 5.0;
		break;
	case SDLK_RIGHT:
		view_roty -= 5.0;
		break;
	case SDLK_ESCAPE:
	case SDLK_q:
		isRunning = 0;
	default:
		break;
	}
	break;
case SDL_QUIT:
	isRunning = 0;
	break;
}
```

You would change this to write to the raw_inputs array, call thinput_handle, 
and then later in your program respond to the user inputs in processed_inputs.

```c
SDL_Event evt;
while (SDL_PollEvent(&evt))
switch (evt.type) {
case SDL_KEYDOWN:
	switch (evt.key.keysym.sym) {
	case SDLK_UP:
		raw_inputs[0] = 255;
		break;
	case SDLK_DOWN:
		raw_inputs[1] = 255;
		break;
	case SDLK_LEFT:
		raw_inputs[2] = 255;
		break;
	case SDLK_RIGHT:
		raw_inputs[3] = 255;
		break;
	case SDLK_ESCAPE:
	case SDLK_q:
		raw_inputs[4] = 255;
	default:
		break;
	}
	break;
case SDL_KEYUP:
	switch (evt.key.keysym.sym) {
	case SDLK_UP:
		raw_inputs[0] = 0;
		break;
	case SDLK_DOWN:
		raw_inputs[1] = 0;
		break;
	case SDLK_LEFT:
		raw_inputs[2] = 0;
		break;
	case SDLK_RIGHT:
		raw_inputs[3] = 0;
		break;
	case SDLK_ESCAPE:
	case SDLK_q:
		raw_inputs[4] = 0;
	default:
		break;
	}
	break;
case SDL_QUIT:
	isRunning = 0;
	break;
}

//Now we invoke thinput_handle
thinput_handle(raw_inputs, processed_inputs, instructions);

//Now we process the keyboard inputs
if(processed_inputs[0])
		view_rotx += 5.0;
if(processed_inputs[1])
		view_rotx -= 5.0;
if(processed_inputs[2])
		view_roty += 5.0;
if(processed_inputs[3])
		view_roty -= 5.0;
if(processed_inputs[4])
		isRunning = 0;

```

We have now achieved the same exact effect as before you downloaded this header file and included it in your project.
Great.

This may seem like a waste of time to you, but the magic happens when you want to *share* your program.

Firstly, you have to change your event loop to read all forms of input on the target platform into the raw_inputs array. (Not demonstrated)

Then, all you have to do is change where you load those constant instructions in main:
```c
instructions[0] = 13;
instructions[1] = 0;
```

... to a function that loads a file into the instructions array

```c
FILE* f;
f = fopen("keybind.bin", "r");
if(!f){ //Load the default keybinding, a 1-1 mapping.
	instructions[0] = 13;
	instructions[1] = 0;
}else{ //Load keybinding
	int i = 0; 
	for(;!feof(f) && i < THINPUT_INS_SIZE;i++)
		fread(instructions+i, sizeof(THINPUT_S), 1, f);
	fclose(f);
}
```

### How do I present the ability to create key bindings to the user?

Thinput uses a very simple bytecode for remapping user inputs.

It's not trivial, but the bytecode is powerful enough to give you all the basics, and then some.

* Map button to button (Copy)

* Map joystick to button (Copy, possibly with multiply or add)

* Map button to joystick (Copy, possibly with multiply or add)

* Joystick threshholds (Write combined with skip if greater, or possibly multiply, or possibly add)

* Map keyboard input to mouse or joystick deltas (Copy, multiply)

* Modifier keys (using multiply or skip)

* Autoclick/turbo (using constant write)

* Clone keys (using copy)

* Joystick/mouse movement curve ramps (using skip if greater along with subtract and copy)

* Use the same input to perform multiple actions (Copy)

* Arbitrary Key combinations with varying effects (Multiply, skip if greater)

* Map mouse deltas to buttons/keys (Copy)

* Anything!

Even if you do not create a gui, it will be trivial for anyone to write one for your game/program/demo. 

Include documentation of the settings.bin bytecode with your program.

Technical users will be able to define their own maps with a hex editor, if nothing else.

### Big/little endian???

At serialization/load time, if THINPUT_S is not uint8_t, you might have to reorder the bytes in the instructions array
to achieve portability of `settings.bin` between computers.

All instructions are multiples of THINPUT_S so you can simply re-arrange every grouping of 2 or 4 bytes.

### How do I use this in multi-file projects?

It should work fine in multi-file projects, since the main function is static inline.

Some compilers  may complain anyway about multiple definitions, if that happens to you, simply restrict calls to thinput_handle to a single file.

The types used by thinput.h (THINPUT_S, THINPUT_L...) are simply DEFINEs so you don't need to use them outside of that file.

### Usage Tips for maximum portability/configurability

* Store 4 separate inputs per joystick, horizontal positive, horizontal negative, vertical positive, vertical negative

* Change the size of THINPUT_S to 16 bit using the code I showed above for touch/mouse input reading.

* Don't store button inputs as 1 or 0. Store them as THINPUT_S_MAX or zero (THINPUT_S_MAX is 255 for the uint8_t type, or 65535 for uint16_t)

* Add new opcodes based on what you believe users will want. This is public domain, CC0 software!

* When handling input, take into account the intensity of the input (from zero to THINPUT_S_MAX) to control the 'strength' of the action. This makes the input feel more responsive to end users using non-binary forms of input (joysticks, mice, fingers)

* change stdint.h and stdlib.h includes in thinput.h to cstdint and cstdlib for C++ projects.

### Other tips

* Don't add goto or jump to the opcode table, it will cause the execution of thinput_handle to be non-predictable and allow malformed keybindings to cause the program to hang.

* Append THINPUT_MAX_OP_EXTRA + 1 null bytes to the end of your instructions array if you don't want to allocate an array large enough to hold the maximum number of instructions.

```
Explanation:

THINPUT_MAX_OP_EXTRA is the maximum number of data elements that a THINPUT op takes.

The library as shipped has 2 data elements necessary for its largest instructions (Most of them...)

The null byte is the TERMINATE op, which quits thinput_handle.

The opcode executor increments along the array by the number of extra data elements required for each opcode.

By appending nulls (0) to the end you guarantee, even for a buffer smaller than THINPUT_INS_SIZE, that the program won't attempt to read data outside the buffer.

This is not necessary if you're going to allocate a buffer large enough to hold the maximum possible number of instructions.
```
