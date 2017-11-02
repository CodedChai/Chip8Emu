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

	srand(time(NULL));
}

void Chip8::emulateCycle() {

	// Fetch opcode (since opcodes are 2 bytes must grab 2 bytes)
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Process opcode
	// Check first hex value then so on
	switch (opcode & 0xF000)
	{
	// Begin case 0x0000
	case 0x0000:
		switch (opcode & 0x000F) {
		case 0x000: // 0x00E0: Clears the screen
			for (int i = 0; i < 2048; ++i)
				pixels[i] = 0x0;
			drawFlag = true;
			pc += 2;
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			--sp;			// 16 levels of stack, decrease stack pointer to prevent overwrite
			pc = stack[sp]; // Put the stored return address from the stack back into the program counter
			pc += 2;
			break;

		default:
			printf("Unkown opcode [0x0000]: 0x%X\n", opcode);
		}
	break;
	// End case 0x000

	//Begin case 0x1000
	case 0x1000: // 0x1NNN: Jumps to address NNN
		pc = opcode & 0x0FFF;
		break;
	// End case 0x1000
	
	// Begin case 0x2000
	case 0x2000: // 0x2NNN: Calls subroutine at NNN
		stack[sp] = pc;	// Store current address in stack
		++sp;			// Increment stack pointer
		pc = opcode & 0x0FFF;	// Set the program counter to address NNN
		break;
	// End case 0x2000

	// Begin case 0x3000
	case 0x3000: // 0x3NNN: Skips the next instruction if V[X] equals NN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			pc += 4; // Skip
		else
			pc += 2; // Normal
		break;
	// End case 0x3000

	// Begin case 0x4000
	case 0x4000: // 0x4XNN: Skips the next instruction if V[X] doesn't equal NN
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			pc += 4; // Skip
		else
			pc += 2; // Normal
		break;
	// End case 0x4000

	// Begin case 0x5000
	case 0x5000: // 0x5XY0: Skips the next instruction if V[X] equals V[Y]
		if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			pc += 4; // Skip
		else
			pc += 2; // Normal
		break;
	// End case 0x5000

	// Begin case 0x6000
	case 0x6000: // 0x6XNN: Sets V[X] to NN
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;
	// End case 0x6000

	// Begin case 0x7000
	case 0x7000: // 0x7XNN: Adds NN to VX
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;
	// End case 0x7000

	// Begin case 0x8000
	case 0x8000:

		break;
	}

}