#include "Arduino.h"
#include <chess.h>
#include <Adafruit_NeoPixel.h>
#define rows_per_board 8
#define columns_per_board 8
#define pins_per_square 3

Adafruit_NeoPixel A = Adafruit_NeoPixel(rows_per_board * pins_per_square, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel B = Adafruit_NeoPixel(rows_per_board * pins_per_square, 3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel C = Adafruit_NeoPixel(rows_per_board * pins_per_square, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel D = Adafruit_NeoPixel(rows_per_board * pins_per_square, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel E = Adafruit_NeoPixel(rows_per_board * pins_per_square, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel F = Adafruit_NeoPixel(rows_per_board * pins_per_square, 7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel G = Adafruit_NeoPixel(rows_per_board * pins_per_square, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel H = Adafruit_NeoPixel(rows_per_board * pins_per_square, 9, NEO_GRB + NEO_KHZ800);

uint32_t white = A.Color(180, 180, 180);
uint32_t red = A.Color(180, 0, 0);
uint32_t orange = A.Color(200, 110, 10);
uint32_t yellow = A.Color(170, 170, 0);
uint32_t green = A.Color(0, 180, 0);
uint32_t blue = A.Color(0, 0, 180);
uint32_t purple = A.Color(0, 180, 180);

void setup() {
	A.begin();
	B.begin();
	C.begin();
	D.begin();
	E.begin();
	F.begin();
	G.begin();
	H.begin();
}

void loop() {
        
	color('A', 1, 'o');
	color('B', 5, 'b');
	delay(1000);
	clear('B', 5);
        delay(1000);
}

/**
	Some functions that may be useful.
*/
void color(char col, int row, char c) {
	uint32_t color;
	switch (c) {
		case 'w':
			color = white;
			break;
		case 'r':
			color = red;
			break;
		case 'o':
			color = orange;
			break;
		case 'y':
			color = yellow;
			break;
		case 'g':
			color = green;
			break;
		case 'b':
			color = blue;
			break;
		case 'p':
			color = purple;
			break;
	}
	charToColumn(col)->setPixelColor(rowToPixel(row), color);
	charToColumn(col)->show();
}

Adafruit_NeoPixel *charToColumn(char col) {
	if (col == 'A' || col == 'a') {
		return &A;
	} else if (col == 'B' || col == 'b') {
		return &B;
	} else if (col == 'C' || col == 'c') {
		return &C;
	} else if (col == 'D' || col == 'd') {
		return &D;
	} else if (col == 'E' || col == 'e') {
		return &E;
	} else if (col == 'F' || col == 'f') {
		return &F;
	} else if (col == 'G' || col == 'g') {
		return &G;
	} else if (col == 'H' || col == 'h') {
		return &H;
	}
}

void clear(char col, int row) {
	charToColumn(col)->setPixelColor(rowToPixel(row), A.Color(0, 0, 0));
	charToColumn(col)->show();
}

void clearAll() {
	A.clear();
	B.clear();
	C.clear();
	D.clear();
	E.clear();
	F.clear();
	G.clear();
	H.clear();
}

int rowToPixel(int row) {
	return (row - 1) * pins_per_square + 1;
}	
