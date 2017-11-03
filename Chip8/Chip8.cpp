#include "Chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned char chip8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {

}

Chip8::~Chip8() {

}

// When setting up Chip 8 make sure everything is cleared and reset
void Chip8::init() {
	pc = 0x200;		// Program counter starts at 0x200
	opcode = 0;		// Reset current opcode
	I = 0;			// Reset index register
	sp = 0;			// Reset stack pointer

	// Clear pixels
	for (int i = 0; i < 2048; ++i)
		pixels[i] = 0x0;

	// Clear stack
	for (int i = 0; i < 16; ++i)
		stack[i] = 0;

	for (int i = 0; i < 16; ++i)
		key[i] = V[i] = 0;

	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;

	// Load fontset into memory
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	// Clear screen once
	drawFlag = true;

	playBeep = false;

	srand(time(NULL));
}

void Chip8::fetch()
{
	opcode = memory[pc] << 8 | memory[pc + 1];
	printf("opcode %X\n", opcode);

}

////////////////////////////////////////////////////////////////////////////////////////////

void Chip8::cpuNULL()
{
	// Do Nothing
	printf("Unimplemented opcode %X\n", opcode);
}
void Chip8::cpuARITHMETIC() {
	//Chip8Arithmetic[(opcode & 0x000F)];
}

void Chip8::cpuRetClear() {
	//Chip8RetClear[(opcode & 0x000F)];
}
void Chip8::cpuKEYS() {
	//Chip8Keys[(opcode & 0x000F)];

}

void Chip8::cpuTimerMem1() {
	//Chip8TimerMem1[(opcode & 0x00F0) >> 4];
}

void Chip8::cpuTimerMem2() {
	//Chip8TimerMem1[(opcode & 0x000F)];
}

// 0x00E0: Clears the screen
void Chip8::dispClear() {
	for (int i = 0; i < 2048; ++i)
		pixels[i] = 0x0;
	drawFlag = true;
	pc += 2;
}

// 0x00EE: Returns from subroutine
void Chip8::retFromSub() {
	--sp;			// 16 levels of stack, decrease stack pointer to prevent overwrite
	pc = stack[sp]; // Put the stored return address from the stack back into the program counter
	pc += 2;
}

// 0x1NNN: Jumps to address NNN
void Chip8::jump() {
	pc = opcode & 0x0FFF;
}

// 0x2NNN: Calls subroutine at NNN
void Chip8::callSub() {
	stack[sp] = pc;	// Store current address in stack
	++sp;			// Increment stack pointer
	pc = opcode & 0x0FFF;	// Set the program counter to address NNN
}

// 0x3NNN: Skips the next instruction if V[X] equals NN
void Chip8::skipVXisNN() {
	if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		pc += 4; // Skip
	else
		pc += 2; // Normal
}

// 0x4XNN: Skips the next instruction if V[X] doesn't equal NN
void Chip8::skipVXnotNN() {
	if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		pc += 4; // Skip
	else
		pc += 2; // Normal
}

// 0x5XY0: Skips the next instruction if V[X] equals V[Y]
void Chip8::skipVXisVY() {
	if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
		pc += 4; // Skip
	else
		pc += 2; // Normal
}

// 0x6XNN: Sets V[X] to NN
void Chip8::setVXtoNN() {
	V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
	pc += 2;
}

// 0x7XNN: Adds NN to VX
void Chip8::addVXNN() {
	V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
	pc += 2;
}

// 0x8XY0: Sets V[X] to the value of V[Y]
void Chip8::setVXtoVY() {
	V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
	pc += 2;
}

// 0x8XY1: Sets V[X] to V[X] or V[Y]
void Chip8::VXorVY() {
	V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
	pc += 2;
}

// 0x8XY2: Sets V[X] to V[X] and V[Y]
void Chip8::VXandVY() {
	V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
	pc += 2;
}

// 0x8XY3: Sets V[X] to V[X] xor V[Y]
void Chip8::VXxorXY() {
	V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
	pc += 2;
}

// 0x8XY4: Adds V[Y] to V[X]. V[F] is set to 1 when there's a carry and to 0 when there isn't
void Chip8::addVXVY() {
	if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) // Check for carry
		V[0xF] = 1; // Set that there is a carry
	else
		V[0xF] = 0; // Set that there isn't a carry
	V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4]; // V[X] = V[X] + V[Y]
	pc += 2;
}

// 0x8XY5: V[Y] is subtracted from V[X]. V[F] is set to 0 when there's a borrow and 1 when there isn't
void Chip8::subVXVY() {
	if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) // Check for borrow
		V[0xF] = 0; // Set that there is a borrow
	else
		V[0xF] = 1; // Set that there isn't a borrow
	V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4]; // V[X] = V[X] - V[Y]
	pc += 2;
}

// 0x8XY6: Shifts V[X] right by one. V[F] is set to the value of the least significant bit of V[X] before the shift
void Chip8::rightShift() {
	V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1; // Set the LSB
	V[(opcode & 0x0F00) >> 8] >>= 1;
	pc += 2;
}

// 0x8XY7: Sets V[X] to V[Y] minus V[X]. V[F] is set to 0 when there's a borrow and 1 when there isn't
void Chip8::subVYVX() {
	if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// Check for borrow
		V[0xF] = 0; // Set that there is a borrow
	else
		V[0xF] = 1;	// Set that there ins't a borrow
	V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]; // V[X] = V[Y] - V[X]
	pc += 2;
}

// 0x8XYE: Shifts V[X] left by one. V[F] is set to the value of the most significant bit before shift.
void Chip8::leftShift() {
	V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7; // Set V[F] to MSB
	V[(opcode & 0x0F00) >> 7] <<= 1;
	pc += 2;
}

// 0x9XY0: Skips the next instruction if V[X] doesn't equal V[Y]
void Chip8::skipVXisntVY() {
	if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
		pc += 4;
	else
		pc += 2;
}

// ANNN: Sets I to the address of NNN
void Chip8::setAddr() {
	I = opcode & 0x0FFF;
	pc += 2;
}

// BNNN: Jumps to the address NNN plus V[0]
void Chip8::jumpV0() {
	pc = (opcode & 0x0FFF) + V[0];
}

// CXNN: Sets V[X] to a random number and NN
void Chip8::random() {
	V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
	pc += 2;
}

/* DXYN: Draws a sprite at the coordinates(V[X], V[Y]) that has a width of 8 pixels and a height of N pixels.
Each ros of 8 pixels is read as bit-coded starting from memory location I
I value doesn't change after the execution of this instruction
VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if it
doesn't happen
*/
void Chip8::disp() {
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++) {
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++) {
				if ((pixel & (0x80 >> xline)) != 0) {
					if (pixels[(x + xline + ((y + yline) * 64))] == 1) {
						V[0xF] = 1;
					}
					pixels[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		drawFlag = true;
		pc += 2;
	}
}

// EX9E: Skips the next instruction if the key stored in V[X] is pressed
void Chip8::checkKeyDown() {
	if (key[V[(opcode & 0x0F00) >> 8]] != 0)
		pc += 4;
	else
		pc += 2;
}

// EXA1: Skips the next instruction if the key stored in V[X] isn't pressed
void Chip8::checkKeyUp() {
	if (key[V[(opcode & 0x0F00) >> 8]] == 0)
		pc += 4;
	else
		pc += 2;
}

// FX07: Sets V[X] to the value of the delay timer
void Chip8::getDelay() {
	V[(opcode & 0x0F00) >> 8] = delay_timer;
	pc += 2;
}

// FX0A: A key press is awaited, and then stored in V[X]
void Chip8::awaitKey() {
	{
		bool keyPress = false;
		for (int i = 0; i < 16; ++i) {
			if (key[i] != 0) {
				V[(opcode & 0x0F00) >> 8] = i;
				keyPress = true;
			}
		}

		// If we didn't get a keypress skip this cycle and try again
		if (!keyPress)
			return;

		pc += 2;
	}
}

// FX15: Sets the delay timer to V[X]
void Chip8::setDelay() {
	delay_timer = V[(opcode & 0x0F00) >> 8];
	pc += 2;
}

// FX18: Sets the sound timer to V[X]
void Chip8::setSound() {
	sound_timer = V[(opcode & 0x0F00) >> 8];
	pc += 2;
}

// FX1E: Adds V[X] to I
void Chip8::addIVX() {
	if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF) // V[F] is set to 1 when range overflow and 0 when it isn't
		V[0xF] = 1;
	else
		V[0xF] = 0;
	I += V[(opcode & 0x0F00) >> 8];
	pc += 2;

}

// FX29: Sets I to the location of the sprite for the character in V[X]. Characters 0-F (in hexadecimal) are represented by a 4x5 font
void Chip8::spriteAddr() {
	I = V[(opcode & 0x0F00) >> 8] * 0x5;
	pc += 2;
}

// FX33: Stores the binary coded decimal representation of V[X] at the address I, I+1 and I+2
void Chip8::setBCD() {
	memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
	memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
	memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
	pc += 2;
}

// FX55: Stores V[0] to V[X] in memory starting at address I
void Chip8::regDump() {
	for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
		memory[I + 1] = V[i];

	// On the original interpreter, when the operation is done, I = I + X + 1.
	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
}

// FX65: Fills V[0] to V[X] with value from memory starting at address I
void Chip8::regLoad() {
	for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
		V[i] = memory[I + i];

	// On the original interpreter, when the operation is done, I = I + X + 1.
	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
}
/*
void(*Chip8Table[16]) =
{
	cpuRetClear, jump, callSub, skipVXisNN, skipVXnotNN, skipVXisVY, setVXtoNN, addVXNN,
	cpuARITHMETIC, skipVXisntVY, setAddr, jumpV0, random, disp, cpuKEYS, cpuTimerMem1
};

void(*Chip8Arithmetic[16]) =
{
	setVXtoVY, VXorVY, VXandVY, VXxorXY, addVXVY, subVXVY, rightShift, subVYVX,
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, leftShift, cpuNULL
};

void(*Chip8RetClear[16]) =
{
	dispClear, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL,
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, retFromSub, cpuNULL
};

void(*Chip8Keys[16]) =
{
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, checkKeyDown,
	cpuNULL, cpuNULL, checkKeyUp, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL
};

void(*Chip8TimerMem1[16]) =
{
	cpuTimerMem2, cpuTimerMem2, spriteAddr, setBCD, cpuNULL, regDump, regLoad, cpuNULL,
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL
};

void(*Chip8TimerMem2[16]) =
{
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, setDelay, cpuNULL, getDelay,
	setSound, cpuNULL, awaitKey, cpuNULL, cpuNULL, cpuNULL, addIVX, cpuNULL
}; */
////////////////////////////////////////////////////////////////////////////////////////////


void Chip8::emulateCycle() {

	// Fetch opcode (since opcodes are 2 bytes must grab 2 bytes)
	opcode = memory[pc] << 8 | memory[pc + 1];
	// printf("opcode%X\n", opcode);

	// Process opcode
	// Check first hex value then so on
	switch (opcode & 0xF000)
	{
	// Begin case 0x0000
	case 0x0000:
		switch (opcode & 0x000F) {
		case 0x000: // 0x00E0: Clears the screen
			dispClear();
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			retFromSub();
			break;

		default:
			printf("Unkown opcode [0x0000]: 0x%X\n", opcode);
		}
	break;
	// End case 0x000

	//Begin case 0x1000
	case 0x1000: // 0x1NNN: Jumps to address NNN
		jump();
		break;
	// End case 0x1000
	
	// Begin case 0x2000
	case 0x2000: // 0x2NNN: Calls subroutine at NNN
		callSub();
		break;
	// End case 0x2000

	// Begin case 0x3000
	case 0x3000: // 0x3NNN: Skips the next instruction if V[X] equals NN
		skipVXisNN();
		break;
	// End case 0x3000

	// Begin case 0x4000
	case 0x4000: // 0x4XNN: Skips the next instruction if V[X] doesn't equal NN
		skipVXnotNN();
		break;
	// End case 0x4000

	// Begin case 0x5000
	case 0x5000: // 0x5XY0: Skips the next instruction if V[X] equals V[Y]
		skipVXisVY();
		break;
	// End case 0x5000

	// Begin case 0x6000
	case 0x6000: // 0x6XNN: Sets V[X] to NN
		setVXtoNN();
		break;
	// End case 0x6000

	// Begin case 0x7000
	case 0x7000: // 0x7XNN: Adds NN to VX
		addVXNN();
		break;
	// End case 0x7000

	// Begin case 0x8000
	case 0x8000:
		switch (opcode & 0x000F) {

		// Begin case 0x8XY0
		case 0x000: // 0x8XY0: Sets V[X] to the value of V[Y]
			setVXtoVY();
			break;
		// End case 0x8XY0

		// Begin case 0x8XY1
		case 0x0001: // 0x8XY1: Sets V[X] to V[X] or V[Y]
			VXorVY();
			break;
		// End case 0x8XY1

		// Begin case 0x8XY2
		case 0x0002: // 0x8XY2: Sets V[X] to V[X] and V[Y]
			VXandVY();
			break;
		// End case 0x8XY2

		// Begin case 0x8XY3
		case 0x0003: // 0x8XY3: Sets V[X] to V[X] xor V[Y]
			VXxorXY();
			break;
		// End case 0x8XY3

		// Begin case 0x8XY4
		case 0x0004: // 0x8XY4: Adds V[Y] to V[X]. V[F] is set to 1 when there's a carry and to 0 when there isn't					
			addVXVY();
			break;
		// End case 0x8XY4

		// Begin case 0x8XY5
		case 0x0005: // 0x8XY5: V[Y] is subtracted from V[X]. V[F] is set to 0 when there's a borrow and 1 when there isn't
			subVXVY();
			break;
		// End case 0x8XY5

		// Begin case 0x8XY6
		case 0x0006: // 0x8XY6: Shifts V[X] right by one. V[F] is set to the value of the least significant bit of V[X] before the shift
			rightShift();
			break;
		// End case 0x8XY6

		// Begin case 0x8XY7
		case 0x0007: // 0x8XY7: Sets V[X] to V[Y] minus V[X]. V[F] is set to 0 when there's a borrow and 1 when there isn't
			subVYVX();
			break;
		// End case 0x8XY7

		// Begin case 0x8XYE
		case 0x000E: // 0x8XYE: Shifts V[X] left by one. V[F] is set to the value of the most significant bit before shift.
			leftShift();
			break;
		// End case 0x8XYE
			
		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
		}
		break;
	// End case 0x8000

	// Begin case 0x9000
	case 0x9000: // 0x9XY0: Skips the next instruction if V[X] doesn't equal V[Y]
		skipVXisntVY();
		break;
	// End case 0x9000

	// Begin case 0xA000
	case 0xA000: // ANNN: Sets I to the address of NNN
		setAddr();
		break;
	// End case 0xA000

	// Begin case 0xB000
	case 0xB000: // BNNN: Jumps to the address NNN plus V[0]
		jumpV0();
		break;
	// End case 0xB000

	// Begin case 0xC000
	case 0xC000: // CXNN: Sets V[X] to a random number and NN
		random();
		break;
	// End case 0xC000

	// Begin case 0xD000
	case 0xD000: /* DXYN: Draws a sprite at the coordinates(V[X], V[Y]) that has a width of 8 pixels and a height of N pixels.
					Each ros of 8 pixels is read as bit-coded starting from memory location I
					I value doesn't change after the execution of this instruction
					VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if it 
					doesn't happen
				 */
		disp();
	break;
	// End case 0xD000

	// Begin case 0xE000
	case 0xE000:
		switch (opcode & 0x00FF) {
		// Begin case 0xEX9E
		case 0x009E: // EX9E: Skips the next instruction if the key stored in V[X] is pressed
			checkKeyDown();
			break;
		// End case 0xEX9E

		// Begin case 0xEXA1
		case 0x00A1: // EXA1: Skips the next instruction if the key stored in V[X] isn't pressed
			checkKeyUp();
			break;
		// End case 0xEXA1
		default:
			printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
		}
		break;
	// End case 0xE000
	// Begin case 0xF000
	case 0xF000:
		switch (opcode & 0x00FF) {
		// Begin case FX07
		case 0x0007: // FX07: Sets V[X] to the value of the delay timer
			getDelay();
			break;
		// End case FX07

		// Begin case FX0A
		case 0x000A: // FX0A: A key press is awaited, and then stored in V[X]
			awaitKey();
		break;
		// End case FX0A
		
		// Begin case FX15
		case 0x0015: // FX15: Sets the delay timer to V[X]
			setDelay();
			break;
		// End case FX15

		// Begin case FX18
		case 0x0018: // FX18: Sets the sound timer to V[X]
			setSound();
			break;
		// End case FX18

		// Begin case FX1E
		case 0x001E: // FX1E: Adds V[X] to I
			addIVX();
			break;
		// End case FX1E

		// Begin case FX29
		case 0x0029: // FX29: Sets I to the location of the sprite for the character in V[X]. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			spriteAddr();
			break;
		// End case FX29

		// Begin case FX33
		case 0x0033: // FX33: Stores the binary coded decimal representation of V[X] at the address I, I+1 and I+2
			setBCD();
			break;
		// End case FX33

		// Begin case FX55
		case 0x0055: // FX55: Stores V[0] to V[X] in memory starting at address I
			regDump();
			break;
		// End case FX55

		// Begin case FX65
		case 0x0065: // FX65: Fills V[0] to V[X] with value from memory starting at address I
			regLoad();
			break;
		// End case FX65
		default:
			printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
		}
		break;
	// End case 0xF000
	default:
		printf("Unknown opcode 0x%X\n", opcode);
	}

	// Update timers
	updateTimers();
}

void Chip8::updateTimers() {
	// Update timers
	if (delay_timer > 0)
		--delay_timer;
	if (sound_timer > 0) {
		if (sound_timer == 1) {
			playBeep = true;
		}
		--sound_timer;
	}
}

void Chip8::execute()
{
	//===================================
	// Fetch Next Opcode And Execute It
	//===================================
	fetch();
	printf("%X\n", (opcode & 0xF000) >> 12);
	Chip8Table[(opcode & 0xF000) >> 12];
	updateTimers();
}



// Render in the console to help find bugs
void Chip8::debugRender() {
	// Draw
	for (int y = 0; y < 32; ++y) {
		for (int x = 0; x < 64; ++x) {
			if (pixels[(y * 64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

bool Chip8::loadApplication(const char * filename) {
	init();
	printf("Loading: %s\n", filename);

	// Open file
	FILE * pFile = fopen(filename, "rb");
	if (pFile == NULL) {
		fputs("File error", stderr);
		return false;
	}

	// Check file size
	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("Filesize: %d\n", (int)lSize);

	// Allocate memory to contain whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL) {
		fputs("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) {
		fputs("Reading error", stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	if ((4096 - 512) > lSize) {
		for (int i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");

	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	return true;
}