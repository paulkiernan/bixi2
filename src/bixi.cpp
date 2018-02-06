#include "bixi.h"
#include "freeram.h"
#include "logging.h"

#define NUM_LEDS 60
#define DATA_PIN 3

CRGB leds[NUM_LEDS];

CBixi& CBixi::Instance()
{
    static CBixi bixi;
    return bixi;
}


CBixi::CBixi()
{
    CLogging::log("CBixi::CBixi: Initializing Bixi");

    // Turn on the uiltin LED indicator
    pinMode(c_indicatorPin, OUTPUT);

    FastLED.addLeds<WS2811, DATA_PIN, RGB>(
        leds,
        NUM_LEDS
    );
    //LEDS.setBrightness(255);
    //LEDS.setCorrection(CRGB(255, 0, 0));
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


void CBixi::Continue()
{
    s_iteration++;

    size_t now = millis();

    for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
        // Turn our current led on to white, then show the leds
        leds[whiteLed] = CRGB::White;
        // Show the leds (only one of which is set to white, from above)
        FastLED.show();
        // Wait a little bit
        delay(100);
        // Turn our current led back to black for the next loop around
        leds[whiteLed] = CRGB::Black;
    }
    FastLED.countFPS();

    static size_t last_log = 0;
    if(now - last_log >= 10000)
    {
        last_log = now;
        char logString[128];
        sprintf(logString, "CBixi::Continue: Frame rate for last 10 seconds is %u", FastLED.getFPS());
        CLogging::log(logString);
    }

    if(now - m_lastIndicator >= c_indicatorDelayMs)
    {
        m_indicatorOn   = !m_indicatorOn;
        m_lastIndicator = now;
        digitalWrite(c_indicatorPin, m_indicatorOn ? HIGH : LOW);
    }
}
