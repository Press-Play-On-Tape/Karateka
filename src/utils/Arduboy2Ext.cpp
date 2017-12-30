#include "Arduboy2Ext.h"

Arduboy2Ext::Arduboy2Ext() : Arduboy2() { }

typedef struct CSESSION {
  int byte;
  int bit;
  const uint8_t *src;
  int src_pos;
} CSESSION;
static CSESSION cs;

static int getval(int bits)
{
  int val = 0;
  int i;
  for (i = 0; i < bits; i++)
  {
    if (cs.bit == 0x100)
    {
      cs.bit = 0x1;
      cs.byte = pgm_read_byte(&cs.src[cs.src_pos]);
      cs.src_pos ++;
    }
    if (cs.byte & cs.bit)
      val += (1 << i);
    cs.bit <<= 1;
  }
  return val;
}
/*
uint8_t Arduboy2Ext::justPressedButtons() const {

  return (~previousButtonState & currentButtonState);

}

uint8_t Arduboy2Ext::prevButtonState() const {

  return previousButtonState;

}
*/
uint8_t Arduboy2Ext::curButtonState() const {

  return currentButtonState;

}

void Arduboy2Ext::drawCompressedMirror(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color, bool mirror) {

  int bl, len;
  int col;
  int i;
  int a, iCol;
  int byte = 0;
  int bit = 0;
  int w, h;

  // set up decompress state

  cs.src = bitmap;
  cs.bit = 0x100;
  cs.byte = 0;
  cs.src_pos = 0;

  // read header

  w = getval(8) + 1;
  h = getval(8) + 1;

  col = getval(1); // starting colour

  // no need to draw at all if we're offscreen
  if (sx + w < 0 || sx > WIDTH - 1 || sy + h < 0 || sy > HEIGHT - 1)
    return;

  // sy = sy - (frame*h);

  int yOffset = abs(sy) % 8;
  int sRow = sy / 8;
  if (sy < 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int rows = h / 8;
  if (h % 8 != 0) rows++;

  a = 0; // +(frame*rows);
  
  if (mirror) {
    iCol = w - 1;
  }
  else {
    iCol = 0;
  }
	
  byte = 0; bit = 1;
  while (a < rows) {

    bl = 1;
    while (!getval(1))
      bl += 2;

    len = getval(bl) + 1; // span length

    // draw the span


    for (i = 0; i < len; i++) {
    
      if (col) byte |= bit;
      bit <<= 1;

      if (bit == 0x100) { // reached end of byte

        // draw

        int bRow = sRow + a;

        //if (byte) // possible optimisation
        if (bRow <= (HEIGHT / 8) - 1) {
          if (bRow > -2) {
            if (iCol + sx <= (WIDTH - 1)) {
              if (iCol + sx >= 0) {

                if (bRow >= 0) {
                  if (color)
                    sBuffer[(bRow * WIDTH) + sx + iCol] |= byte << yOffset;
                  else
                    sBuffer[(bRow * WIDTH) + sx + iCol] &= ~(byte << yOffset);
                }
                if (yOffset && bRow < (HEIGHT / 8) - 1 && bRow > -2) {
                  if (color)
                    sBuffer[((bRow + 1)*WIDTH) + sx + iCol] |= byte >> (8 - yOffset);
                  else
                    sBuffer[((bRow + 1)*WIDTH) + sx + iCol] &= ~(byte >> (8 - yOffset));
                }
              }
            }
          }
        }

    // iterate
		if (!mirror) {
			iCol ++;
			if (iCol >= w) {
        iCol = 0;
        a++;
			}
		}
		else {
			iCol --;
			if (iCol < 0) {
        iCol = w - 1;
        a++;
			}
		}
		
        // reset byte
        byte = 0; bit = 1;
      }
    }

    col = 1 - col; // toggle colour for next span
  }
}



