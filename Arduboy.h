#include <SPI.h>
// Choose which board to compile for in the next 2 lines.....

#define ARDUBOY_10   //< compile for the production Arduboy v1.0
//#define AB_DEVKIT    //< compile for the official dev kit

#define CLK   13
#define MOSI  11

//-------------------------------------------------------------
#ifdef AB_DEVKIT
#define CS		6
#define DC		4
#define RST		12

#define PIN_LEFT_BUTTON 9
#define PIN_RIGHT_BUTTON 5
#define PIN_UP_BUTTON 8
#define PIN_DOWN_BUTTON 10
#define PIN_A_BUTTON A0
#define PIN_B_BUTTON A1

//-------------------------------------------------------------
#else
#define CS 12
#define DC 4
#define RST 6

#define PIN_LEFT_BUTTON A2
#define PIN_RIGHT_BUTTON A1
#define PIN_UP_BUTTON A0
#define PIN_DOWN_BUTTON A3
#define PIN_A_BUTTON 7
#define PIN_B_BUTTON 8

#endif
//--------------------------------------------------------------
class Arduboy {
	public:
		Arduboy();
		uint8_t getInput();
		void start();
		void blank();
    void white();
    void grey();
    void drawbitmap(const unsigned char *bitmap, int posn, int size, int invert);
    void draw24x16tile(const unsigned char *bitmap, int x, int y);
    void draw4x4tiles(unsigned char *bitmap, int x, int y);
    void drawchar(unsigned char *bitmap, int x, int y);
		
		void drawTile(int x, int _y, const unsigned char *image);
		void drawTile(int x, int _y, unsigned char image[]);
		void drawSprite(int x, int y, const unsigned char *image, uint8_t frame);

		void scrollLeft();

		private:
		uint8_t readCapacitivePin(int pinToMeasure);
		uint8_t readCapXtal(int pinToMeasure);
		volatile uint8_t *mosiport, *clkport, *csport, *dcport;
		uint8_t mosipinmask, clkpinmask, cspinmask, dcpinmask;
		bool scrolling;
};
