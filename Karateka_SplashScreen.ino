#include "src/utils/Arduboy2Ext.h"

static constexpr uint8_t ppotFrames = 36; // ~1.6s animation loop @23fps


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void splashScreen_Init() {

    gameStateDetails.setCurrState(GAME_SPLASH_SCREEN);

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void splashScreen() { 

    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
        
        gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

    }

    arduboy.clear();
    arduboy.drawCompressedMirror(32, 16, PPOT, WHITE, false);

    uint8_t p = 28; // Default pixel position 0 is hidden in the image

    switch (arduboy.frameCount % ppotFrames) {

        // 2nd quarter of frames
        case (ppotFrames*1/4) ... (ppotFrames*2/4)-1:
            p = 29; // Move pixel down to position 1
            /*-fallthrough*/

        // 1st quarter of frames
        case 0 ... (ppotFrames*1/4)-1:
            // Flash 'Play' arrow by clearing the image
            // Overwrite directly to the screen buffer 
            arduboy.sBuffer[91 +(3*WIDTH)] = 0x00;
            arduboy.sBuffer[92 +(3*WIDTH)] = 0x00;
            arduboy.sBuffer[93 +(3*WIDTH)] = 0x00;
            arduboy.sBuffer[94 +(3*WIDTH)] = 0x00;
            arduboy.sBuffer[95 +(3*WIDTH)] = 0x00;
            break;

        // 3rd quarter of frames
        case (ppotFrames*2/4) ... (ppotFrames*3/4)-1:
            p = 30; // Move pixel down to position 2
            break;

        // 4th quarter of frames
        default:
            p = 31; // Move pixel down to position 3
            break;
        
    }

    // Draw pixel to represent the tape spooling
    // Render using pixel function 
    arduboy.drawPixel(52, p, WHITE); // Positions 28, 29, 30, 31

    // On even steps of pixel movement, update the spindle image
    if (p % 2 == 0) { 
        // Render directly to the screen buffer 
        arduboy.sBuffer[45 +(3*WIDTH)] = 0xA5;
        arduboy.sBuffer[46 +(3*WIDTH)] = 0x95;
    }

}