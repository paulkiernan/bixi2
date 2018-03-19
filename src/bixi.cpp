// Includes
#include "bixi.h"
#include "freeram.h"
#include "logging.h"

// Defines
#define BRIGHTNESS     140
#define CHIPSET        WS2811
#define COLOR_ORDER    GRB
#define LED_PIN        3
#define kMatrixWidth   144
#define kMatrixHeight  1
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

// Global variables
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);
uint8_t gHue = 0;
unsigned int microseconds = 50;


uint16_t CBixi::XY( uint8_t x, uint8_t y)
{
    uint16_t i;
    if( y & 0x01) {  // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
    } else {  // Even rows run forwards
        i = (y * kMatrixWidth) + x;
    }
  return i;
}


uint16_t CBixi::XYsafe( uint8_t x, uint8_t y)
{
    if( x >= kMatrixWidth) return -1;
    if( y >= kMatrixHeight) return -1;
    return this->XY(x,y);
}


CBixi& CBixi::Instance()
{
    static CBixi bixi;
    return bixi;
}


CBixi::CBixi()
{
    CLogging::log("CBixi::CBixi: Initializing Bixi");

    // Set up LEDs and frame rate reporting
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(
        leds,
        NUM_LEDS
    ).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(BRIGHTNESS);
    //GammaCorrection::Init(1.50);

    char logstr[256];
    sprintf(
        logstr,
        "CBixi::CBixi: Initializations complete, %u byte remaining",
        FreeRam()
    );
    CLogging::log(logstr);
}


CBixi::~CBixi()
{
    CLogging::log("CBixi::~CBixi: Destructing");
    ShutDown();
}


void CBixi::DrawOneFrame(byte color)
{
    for( byte y = 0; y < NUM_LEDS; y++) {
        leds[y] = CHSV( color, 255, 255);
    }
}


void CBixi::logFrameRate()
{
    char logstr[256];
    sprintf(
        logstr,
        "CBixi::logFrameRate: (Iteration %u) Frame rate @ %u fps",
        this->Instance().Iteration(),
        FastLED.getFPS()
    );
    CLogging::log(logstr);
}


void CBixi::Continue()
{
    s_iteration++;

    uint8_t gHueDelta = 3;
    this->DrawOneFrame(gHue);
    gHue += gHueDelta;

    uint32_t ms = millis();
    uint32_t waitTime = 50;

    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();

    while (millis() - ms < waitTime);

    this->logFrameRate();
}
