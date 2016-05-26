#include "Arduboy.h"

//----------------------------------------------------------------------
Arduboy::Arduboy() {
	scrolling = false;
}

//----------------------------------------------------------------------
void Arduboy::start() {
	pinMode(MOSI, OUTPUT);
	pinMode(CLK, OUTPUT);
	pinMode(DC, OUTPUT);
	pinMode(CS, OUTPUT);
	pinMode(PIN_UP_BUTTON, INPUT_PULLUP);
	pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
	pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
	pinMode(PIN_DOWN_BUTTON, INPUT_PULLUP);
	pinMode(PIN_A_BUTTON, INPUT_PULLUP);
	pinMode(PIN_B_BUTTON, INPUT_PULLUP);
	
	/*
	clkport     = portOutputRegister(digitalPinToPort(CLK));
	clkpinmask  = digitalPinToBitMask(CLK);
	mosiport    = portOutputRegister(digitalPinToPort(MOSI));
	mosipinmask = digitalPinToBitMask(MOSI);
	*/
	
	csport      = portOutputRegister(digitalPinToPort(CS));
	cspinmask   = digitalPinToBitMask(CS);
	dcport      = portOutputRegister(digitalPinToPort(DC));
	dcpinmask   = digitalPinToBitMask(DC);
	// Setup reset pin direction (used by both SPI and I2C)  
	pinMode(RST, OUTPUT);
	digitalWrite(RST, HIGH);
	delay(1);				// VDD (3.3V) goes high at start, lets just chill for a ms
	digitalWrite(RST, LOW);	// bring reset low
	delay(10);				// wait 10ms
	digitalWrite(RST, HIGH);	// bring out of reset
    *csport |= cspinmask;
    *dcport &= ~dcpinmask;
    *csport &= ~cspinmask;
	SPI.transfer(0xAE);	// Display Off
	SPI.transfer(0XD5);	// Set Display Clock Divisor v
	SPI.transfer(0xF0); //   0x80 is default 
	SPI.transfer(0xA8);	// Set Multiplex Ratio v
	SPI.transfer(0x3F); //
	SPI.transfer(0xD3); // Set Display Offset v
	SPI.transfer(0x0);	//
	SPI.transfer(0x40);	// Set Start Line (0)
	SPI.transfer(0x8D);	// Charge Pump Setting v
	SPI.transfer(0x14); //   Enable
	SPI.transfer(0x20);	// Set Memory Mode v
	SPI.transfer(0x00); //   Horizontal Addressing
	SPI.transfer(0xA1); // Set Segment Re-map (A0) | (b0001)
	SPI.transfer(0xC8); // Set COM Output Scan Direction
	SPI.transfer(0xDA); // Set COM Pins v
	SPI.transfer(0x12);	//
	SPI.transfer(0x81); // Set Contrast v
	SPI.transfer(0xCF); //
	SPI.transfer(0xD9); // Set Precharge
	SPI.transfer(0xF1); //
	SPI.transfer(0xDB); // Set VCom Detect
	SPI.transfer(0x40); //
	SPI.transfer(0xA4); // Entire Display ON
	SPI.transfer(0xA6); // Set normal/inverse display
	SPI.transfer(0xAF);	// Display On
	*csport |= cspinmask;

	// turn all the LEDs off...
	digitalWrite(TX_LED, 255);
	digitalWrite(RX_LED, 255);
 
	digitalWrite(RED_LED, 255);
	digitalWrite(GREEN_LED, 255);
	digitalWrite(BLUE_LED, 255);
}

//----------------------------------------------------------------------
void Arduboy::blank() {
	*csport |= cspinmask;
	*dcport &= ~dcpinmask;
	*csport &= ~cspinmask;
	SPI.transfer(0x20); 		// set display mode
	SPI.transfer(0x00);			// horizontal addressing mode
	SPI.transfer(0x21); 		// set col address
	unsigned char start = 0;
	unsigned char end = 127;
	SPI.transfer(start & 0x7F);
	SPI.transfer(end & 0x7F);
	SPI.transfer(0x22); // set page address
	start = 0;
	end = 7;
	SPI.transfer(start & 0x07);
	SPI.transfer(end & 0x07);	
//	*csport |= cspinmask;
//	*csport |= cspinmask;
	*dcport |= dcpinmask;
	*csport &= ~cspinmask;
	for (int a = 0; a<1024; a++) SPI.transfer(0x00);
//	*csport |= cspinmask;	
}

//----------------------------------------------------------------------
void Arduboy::white() {
  *csport |= cspinmask;
  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask;
  SPI.transfer(0x20);     // set display mode
  SPI.transfer(0x00);     // horizontal addressing mode
  SPI.transfer(0x21);     // set col address
  unsigned char start = 0;
  unsigned char end = 127;
  SPI.transfer(start & 0x7F);
  SPI.transfer(end & 0x7F);
  SPI.transfer(0x22); // set page address
  start = 0;
  end = 7;
  SPI.transfer(start & 0x07);
  SPI.transfer(end & 0x07); 
//  *csport |= cspinmask;
//  *csport |= cspinmask;
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;
  for (int a = 0; a<1024; a++) SPI.transfer(0xFF);
//  *csport |= cspinmask; 
}

//----------------------------------------------------------------------
void Arduboy::grey() {
  *csport |= cspinmask;
  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask;
  SPI.transfer(0x20);     // set display mode
  SPI.transfer(0x00);     // horizontal addressing mode
  SPI.transfer(0x21);     // set col address
  unsigned char start = 0;
  unsigned char end = 127;
  SPI.transfer(start & 0x7F);
  SPI.transfer(end & 0x7F);
  SPI.transfer(0x22); // set page address
  start = 0;
  end = 7;
  SPI.transfer(start & 0x07);
  SPI.transfer(end & 0x07); 
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;
  for (int a = 0; a<1024; a++)if (a%2) SPI.transfer(170);
                                  else SPI.transfer(85);
}

//----------------------------------------------------------------------
void Arduboy::drawbitmap(const unsigned char *bitmap, int posn, int size, int invert) {
  *csport |= cspinmask;
  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask;
  SPI.transfer(0x20);     // set display mode
  SPI.transfer(0x00);     // horizontal addressing mode
  SPI.transfer(0x21);     // set col address
  unsigned char start = posn%128;
  unsigned char end = posn%128+size-1; // inclusive
  SPI.transfer(start & 0x7F);
  SPI.transfer(end & 0x7F);
  SPI.transfer(0x22); // set page address
  start = posn/128;
  end = posn/128;
  SPI.transfer(start & 0x07);
  SPI.transfer(end & 0x07); 
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;
  if (invert) for (int a = 0; a<size; a++) SPI.transfer(~pgm_read_byte(bitmap++));
         else for (int a = 0; a<size; a++) SPI.transfer(pgm_read_byte(bitmap++));
}

//----------------------------------------------------------------------
void Arduboy::draw24x16tile(const unsigned char *bitmap, int x, int y) {
  *csport |= cspinmask;
  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask;
  SPI.transfer(0x20);     // set display mode
  SPI.transfer(0x00);     // horizontal addressing mode
  SPI.transfer(0x21);     // set col address
  unsigned char start = x;
  unsigned char end = x+23;
  SPI.transfer(start & 0x7F);
  SPI.transfer(end & 0x7F);
  SPI.transfer(0x22); // set page address
  start = y;
  end = y+1;
  SPI.transfer(start & 0x07);
  SPI.transfer(end & 0x07); 
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;
  for (int a = 0; a<24*2; a++) SPI.transfer(pgm_read_byte(bitmap++));                                
}

//----------------------------------------------------------------------
void Arduboy::draw4x4tiles(unsigned char *bitmap, int x, int y) {
  *csport |= cspinmask;
  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask;
  SPI.transfer(0x20);     // set display mode
  SPI.transfer(0x00);     // horizontal addressing mode
  SPI.transfer(0x21);     // set col address
  unsigned char start = x;
  unsigned char end = x+3;
  SPI.transfer(start & 0x7F);
  SPI.transfer(end & 0x7F);
  SPI.transfer(0x22); // set page address
  start = y;
  end = y;
  SPI.transfer(start & 0x07);
  SPI.transfer(end & 0x07); 
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;
  for (int a = 0; a<4; a++) SPI.transfer(*bitmap++);                                
}

//----------------------------------------------------------------------
void Arduboy::drawchar(unsigned char *bitmap, int x, int y) {
  *csport |= cspinmask;
  *dcport &= ~dcpinmask;
  *csport &= ~cspinmask;
  SPI.transfer(0x20);     // set display mode
  SPI.transfer(0x00);     // horizontal addressing mode
  SPI.transfer(0x21);     // set col address
  unsigned char start = x;
  unsigned char end = x+5; // inclusive?
  SPI.transfer(start);
  SPI.transfer(end);
  SPI.transfer(0x22); // set page address
  start = y;
  end = y;
  SPI.transfer(start);
  SPI.transfer(end); 
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;
  for (int a = 0; a<6; a++) SPI.transfer(*bitmap++);                          
}

//--------------------------------------------------------------------------------------------------------------

uint8_t Arduboy::getInput() {

	// b00lurdab
	uint8_t value = B00000000;
	
	if (digitalRead(PIN_LEFT_BUTTON)==0) { value = value | B00100000; }
	if (digitalRead(PIN_UP_BUTTON)==0) { value = value | B00010000; }	
	if (digitalRead(PIN_RIGHT_BUTTON)==0) { value = value | B00001000; }	
	if (digitalRead(PIN_DOWN_BUTTON)==0) { value = value | B00000100; }	
	if (digitalRead(PIN_A_BUTTON)==0) 	   { value = value | B00000001; }	
	if (digitalRead(PIN_B_BUTTON)==0)		   { value = value | B00000010; }
	return value;
}

