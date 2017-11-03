class Chip8 {

public:
	Chip8();
	~Chip8();

	unsigned char  delay_timer;		// Delay timer
	unsigned char  sound_timer;		// Sound timer		

	bool drawFlag;
	bool playBeep;
	
	void fetch();
	void execute();
	void emulateCycle();
	void debugRender();
	bool loadApplication(const char * filename);



	// Chip8
	unsigned char  pixels[64 * 32];	// Total amount of pixels: 2048
	unsigned char  key[16];

private:
	unsigned short opcode; // Current opcode
	unsigned short pc; // Program counter
	unsigned short I;				// Index register
	unsigned short sp;				// Stack pointer

	unsigned char  V[16];			// V-registers (V0-VF)
	unsigned short stack[16];		// Stack (16 levels)
	unsigned char  memory[4096];	// Memory (size = 4k)		

	void updateTimers();
	void init();

	void cpuNULL();
	void cpuRetClear();
	void cpuARITHMETIC();
	void cpuKEYS();
	void cpuTimerMem1();
	void cpuTimerMem2();

	void dispClear();
	void retFromSub();
	void jump();
	void callSub();
	void skipVXisNN();
	void skipVXnotNN();
	void skipVXisVY();
	void setVXtoNN();
	void addVXNN();

	void setVXtoVY();
	void VXorVY();
	void VXandVY();
	void VXxorXY();
	void addVXVY();
	void subVXVY();
	void rightShift();
	void subVYVX();
	void leftShift();

	void skipVXisntVY();
	void setAddr();
	void jumpV0();
	void random();
	void disp();
	void checkKeyDown();
	void checkKeyUp();
	void getDelay();
	void awaitKey();
	void setDelay();
	void setSound();
	void addIVX();
	void spriteAddr();
	void setBCD();

	void regDump();
	void regLoad();

	void(*Chip8Table[16])();
	void(*Chip8Arithmetic[16])();


};