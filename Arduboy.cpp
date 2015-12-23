#include "Arduboy.h"


Arduboy::Arduboy() {
	scrolling = false;
}
void Arduboy::start() {
	pinMode(MOSI, OUTPUT);
	pinMode(CLK, OUTPUT);
	pinMode(DC, OUTPUT);
	pinMode(CS, OUTPUT);
	pinMode(8, INPUT_PULLUP);
	pinMode(9, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);
	pinMode(10, INPUT_PULLUP);
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	
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
  if (invert) for (int a = 0; a<size; a++) SPI.transfer(~*bitmap++);
         else for (int a = 0; a<size; a++) SPI.transfer(*bitmap++);
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
  for (int a = 0; a<24*2; a++) SPI.transfer(*bitmap++);                                
}

//----------------------------------------------------------------------
void Arduboy::draw4x4tiles(const unsigned char *bitmap, int x, int y) {
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
  SPI.transfer(50);
  SPI.transfer(55);
  SPI.transfer(0x22); // set page address
  start = y;
  end = y;
  SPI.transfer(1);
  SPI.transfer(1); 
  *dcport |= dcpinmask;
  *csport &= ~cspinmask;
  for (int a = 0; a<6; a++) SPI.transfer(170);                          
}

//---------------------------------------------------------------------
void Arduboy::drawTile(int x, int _y, const unsigned char *image) {
	uint8_t offset = 0;
	uint8_t tail = 0;
	if (x+8 < 0) { return; } // offscreen
	if (x > 127) { return; } 	// offscreen
	if (x < 0 && x+8 >= 0) { offset =- x; x = 0; }
	if (x < 127 && x+8 > 127) { tail = x+8 - 127; }
	
	*csport |= cspinmask;
	*dcport &= ~dcpinmask;
	*csport &= ~cspinmask;
	SPI.transfer(0x20); 		// set display mode
	SPI.transfer(0x00);			// horizontal addressing mode
	SPI.transfer(0x21); 		// set col address
	unsigned char start = x;
	unsigned char end = x + 8 - tail;
	SPI.transfer(start & 0x7F);
	SPI.transfer(end & 0x7F);
	SPI.transfer(0x22); // set page address
	start = _y;
	end = _y;
	SPI.transfer(start & 0x07);
	SPI.transfer(end & 0x07);	
//	*csport |= cspinmask;

//	*csport |= cspinmask;
	*dcport |= dcpinmask;
	*csport &= ~cspinmask;
//	for (uint8_t a = offset; a < 8 - tail; a++) { SPI.transfer(image[a]); }
	for (uint8_t a = offset; a < 8 - tail; a++) { SPI.transfer(pgm_read_byte(image + a)); }
//	*csport |= cspinmask;
}
//---------------------------------------------------------------------------------------
void Arduboy::drawTile(int x, int _y, unsigned char image[]) {
	uint8_t offset = 0;
	uint8_t tail = 0;
	if (x+8 < 0) { return; } // offscreen
	if (x > 127) { return; } 	// offscreen
	if (x < 0 && x+8 >= 0) { offset =- x; x = 0; }
	if (x < 127 && x+8 > 127) { tail = x+8 - 127; }
	
	*csport |= cspinmask;
	*dcport &= ~dcpinmask;
	*csport &= ~cspinmask;
	SPI.transfer(0x20); 		// set display mode
	SPI.transfer(0x00);			// horizontal addressing mode
	SPI.transfer(0x21); 		// set col address
	unsigned char start = x;
	unsigned char end = x + 8 - tail;
	SPI.transfer(start & 0x7F);
	SPI.transfer(end & 0x7F);
	SPI.transfer(0x22); // set page address
	start = _y;
	end = _y;
	SPI.transfer(start & 0x07);
	SPI.transfer(end & 0x07);	
//	*csport |= cspinmask;

//	*csport |= cspinmask;
	*dcport |= dcpinmask;
	*csport &= ~cspinmask;
	for (uint8_t a = offset; a < 8 - tail; a++) { SPI.transfer(image[a]); }
//	for (uint8_t a = offset; a < 8 - tail; a++) { SPI.transfer(pgm_read_byte(image + a)); }
//	*csport |= cspinmask;
}

//---------------------------------------------------------------------------------------
void Arduboy::drawSprite(int x, int y, const unsigned char *image, uint8_t frame) {
	uint8_t xoffset = 0;
	uint8_t yoffset = (y % 8);
	uint8_t tail = 0;
	if (x+8 < 0) { return; } 	// offscreen
	if (x > 127) { return; } 	// offscreen
	if (x < 0 && x+8 >= 0) { xoffset =- x; x = 0; }
	if (x < 127 && x+8 > 127) { tail = x+8 - 127; }
	*csport |= cspinmask;
	*dcport &= ~dcpinmask;
	*csport &= ~cspinmask;
	SPI.transfer(0x20); 		// set display mode
	SPI.transfer(0x00);			// horizontal addressing mode
	SPI.transfer(0x21); 		// set col address
	unsigned char start = x;
	unsigned char end = x + 7 - tail;
	SPI.transfer(start & 0x7F);
	SPI.transfer(end & 0x7F);
	SPI.transfer(0x22);			// set page address
	start = y / 8;
	if (yoffset == 0) { end = start; }
	else { end = start + 1; }
	SPI.transfer(start & 0x07);
	SPI.transfer(end & 0x07);	
//	*csport |= cspinmask;

//	*csport |= cspinmask;
	*dcport |= dcpinmask;
	*csport &= ~cspinmask;
	
	if (start == end) for (uint8_t a = xoffset; a < 8-tail; a++) SPI.transfer(pgm_read_byte( image + a + (frame*8) ));
	else {
		for (uint8_t a = xoffset; a < 8-tail; a++) SPI.transfer(pgm_read_byte( image + a + (frame*8) ) << yoffset);
		for (uint8_t a = xoffset; a < 8-tail; a++) SPI.transfer(pgm_read_byte( image + a + (frame*8) ) >> (8 - yoffset));
	}

//	if (start == end) for (uint8_t a = xoffset; a < 8-tail; a++) SPI.transfer(image[a + (frame*8)]);
//	else {
//		for (uint8_t a = xoffset; a < 8-tail; a++) SPI.transfer(image[a + (frame*8)] << yoffset);
//		for (uint8_t a = xoffset; a < 8-tail; a++) SPI.transfer(image[a + (frame*8)] >> (8 - yoffset));
//	}

//	*csport |= cspinmask;
}

//--------------------------------------------------------------------------------------------------------------
void Arduboy::scrollLeft() {
	if (!scrolling){
		scrolling = true;
		*csport |= cspinmask;
		*dcport &= ~dcpinmask;
		*csport &= ~cspinmask;
		SPI.transfer(0x27);	// Start Left Scroll
		SPI.transfer(0x00);	// Blank
		SPI.transfer(0x00);	// Start Page Address
		SPI.transfer(0x07);	// Time interval (2 frames)
		SPI.transfer(0x0F);	// End Page Address
		SPI.transfer(0x00);	//
		SPI.transfer(0xFF);	//
		SPI.transfer(0x2F);	// Activate Scroll
		delay(12);			// Wait 2 frames
		SPI.transfer(0x2E);	// End scroll
		*csport |= cspinmask;
		scrolling = false;
	}
}
uint8_t Arduboy::getInput() {

	// b00lurdab
	uint8_t value = B00000000;
	
	if (digitalRead(9)==0) { value = value | B00100000; }	// left
	if (digitalRead(8)==0) { value = value | B00010000; }	// up
	if (digitalRead(5)==0) { value = value | B00001000; }	// right
	if (digitalRead(10)==0) { value = value | B00000100; }	// down
	if (digitalRead(A1)==0) 	   { value = value | B00000010; }	// a?
	if (digitalRead(A0)==0)		   { value = value | B00000001; }	// b?
	return value;
}
uint8_t Arduboy::readCapacitivePin(int pinToMeasure) {
	/*	readCapacitivePin
		Input: Arduino pin number
		Output: A number, from 0 to 17 expressing
		how much capacitance is on the pin
		When you touch the pin, or whatever you have
		attached to it, the number will get higher 
	*/

	// Variables used to translate from Arduino to AVR pin naming
	volatile uint8_t* port;
	volatile uint8_t* ddr;
	volatile uint8_t* pin;
	// Here we translate the input pin number from
	// Arduino pin number to the AVR PORT, PIN, DDR,
	// and which bit of those registers we care about.
	byte bitmask;
	port = portOutputRegister(digitalPinToPort(pinToMeasure));
	ddr = portModeRegister(digitalPinToPort(pinToMeasure));
	bitmask = digitalPinToBitMask(pinToMeasure);
	pin = portInputRegister(digitalPinToPort(pinToMeasure));
	// Discharge the pin first by setting it low and output
	*port &= ~(bitmask);
	*ddr  |= bitmask;
	delay(1);
	// Prevent the timer IRQ from disturbing our measurement
	noInterrupts();
	// Make the pin an input with the internal pull-up on
	*ddr &= ~(bitmask);
	*port |= bitmask;

	// Now see how long the pin to get pulled up. This manual unrolling of the loop
	// decreases the number of hardware cycles between each read of the pin,
	// thus increasing sensitivity.
	uint8_t cycles = 17;
	if (*pin & bitmask) { cycles =  0;}
	else if (*pin & bitmask) { cycles =  1;}
	else if (*pin & bitmask) { cycles =  2;}
	else if (*pin & bitmask) { cycles =  3;}
	else if (*pin & bitmask) { cycles =  4;}
	else if (*pin & bitmask) { cycles =  5;}
	else if (*pin & bitmask) { cycles =  6;}
	else if (*pin & bitmask) { cycles =  7;}
	else if (*pin & bitmask) { cycles =  8;}
	else if (*pin & bitmask) { cycles =  9;}
	else if (*pin & bitmask) { cycles = 10;}
	else if (*pin & bitmask) { cycles = 11;}
	else if (*pin & bitmask) { cycles = 12;}
	else if (*pin & bitmask) { cycles = 13;}
	else if (*pin & bitmask) { cycles = 14;}
	else if (*pin & bitmask) { cycles = 15;}
	else if (*pin & bitmask) { cycles = 16;}

	// End of timing-critical section
	interrupts();

	// Discharge the pin again by setting it low and output
	// It's important to leave the pins low if you want to 
	// be able to touch more than 1 sensor at a time - if
	// the sensor is left pulled high, when you touch
	// two sensors, your body will transfer the charge between
	// sensors.
	*port &= ~(bitmask);
	*ddr  |= bitmask;

	return cycles;
}
uint8_t Arduboy::readCapXtal(int pinToMeasure) {
	// Variables used to translate from Arduino to AVR pin naming
	// volatile uint8_t* port;
	// volatile uint8_t* ddr;
	// volatile uint8_t* pin;

	// Here we translate the input pin number from
	// Arduino pin number to the AVR PORT, PIN, DDR,
	// and which bit of those registers we care about.
	byte bitmask;
	if(pinToMeasure == 2){ bitmask = B10000000; }
	else { bitmask = B01000000; }
  
	// port = portOutputRegister(PORTB);
	// ddr = portModeRegister(DDRB);
	// bitmask = PINB;
	// pin = portInputRegister(PINB);
	// Discharge the pin first by setting it low and output
  
	// bitmask =  B10000000
	// ~(bitmask) = B01111111
  
	PORTB &= ~(bitmask);  // SET LOW
	DDRB  |= bitmask;  // OUTPUT
	delay(1);
	// Prevent the timer IRQ from disturbing our measurement
	noInterrupts();
	// Make the pin an input with the internal pull-up on
	DDRB &= ~(bitmask);  // INPUT
	PORTB |= bitmask;

	// Now see how long the pin to get pulled up. This manual unrolling of the loop
	// decreases the number of hardware cycles between each read of the pin,
	// thus increasing sensitivity.
	uint8_t cycles = 17;
		 if (PINB & bitmask) { cycles =  0;}
	else if (PINB & bitmask) { cycles =  1;}
	else if (PINB & bitmask) { cycles =  2;}
	else if (PINB & bitmask) { cycles =  3;}
	else if (PINB & bitmask) { cycles =  4;}
	else if (PINB & bitmask) { cycles =  5;}
	else if (PINB & bitmask) { cycles =  6;}
	else if (PINB & bitmask) { cycles =  7;}
	else if (PINB & bitmask) { cycles =  8;}
	else if (PINB & bitmask) { cycles =  9;}
	else if (PINB & bitmask) { cycles = 10;}
	else if (PINB & bitmask) { cycles = 11;}
	else if (PINB & bitmask) { cycles = 12;}
	else if (PINB & bitmask) { cycles = 13;}
	else if (PINB & bitmask) { cycles = 14;}
	else if (PINB & bitmask) { cycles = 15;}
	else if (PINB & bitmask) { cycles = 16;}

	// End of timing-critical section
	interrupts();

	// Discharge the pin again by setting it low and output
	// It's important to leave the pins low if you want to 
	// be able to touch more than 1 sensor at a time - if
	// the sensor is left pulled high, when you touch
	// two sensors, your body will transfer the charge between
	// sensors.
	PORTB &= ~(bitmask);
	DDRB  |= bitmask;

	return cycles;
}

