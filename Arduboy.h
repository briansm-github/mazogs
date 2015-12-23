#include <SPI.h>

#define CLK		13
#define MOSI	11
#define CS		6
#define DC		4
#define RST		12

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
    void draw4x4tiles(const unsigned char *bitmap, int x, int y);
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
