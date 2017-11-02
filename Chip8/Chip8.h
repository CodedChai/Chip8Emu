class Chip8 {
public:
	Chip8();
	~Chip8();

	bool drawFlag;
	bool playBeep;

	void emulateCycle();
	void debugRender();
	bool loadApplication(const char * filename);

	// Chip8
	unsigned char  pixels[64 * 32];	// Total amount of pixels: 2048
	unsigned char  key[16];

private:
	unsigned short pc;				// Program counter
	unsigned short opcode;			// Current opcode
	unsigned short I;				// Index register
	unsigned short sp;				// Stack pointer

	unsigned char  V[16];			// V-registers (V0-VF)
	unsigned short stack[16];		// Stack (16 levels)
	unsigned char  memory[4096];	// Memory (size = 4k)		

	unsigned char  delay_timer;		// Delay timer
	unsigned char  sound_timer;		// Sound timer		

	void init();
};