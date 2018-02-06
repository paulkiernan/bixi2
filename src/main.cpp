#include "bixi.h"
#include "logging.h"
#include "WProgram.h"
#include <avr/io.h>
#include <avr/interrupt.h>


#define HEARTBEAT_PIN 13


IntervalTimer heartbeatTimer;
int ledState = LOW;
volatile unsigned long blinkCount = 0; // use volatile for shared variables


void heartbeatLED() {

    char logString[128];
    sprintf(logString, "main::heartbeatLED: LED blink count: %u", blinkCount);
    CLogging::log(logString);

    if (ledState == LOW) {
        ledState = HIGH;
        blinkCount = blinkCount + 1;  // increase when LED turns on
    } else {
        ledState = LOW;
    }
    digitalWriteFast(HEARTBEAT_PIN, ledState);
}


extern "C" int main(void) {

    delay(1000);
    CLogging::Init();
    pinMode(HEARTBEAT_PIN, OUTPUT);
    heartbeatTimer.begin(heartbeatLED, 150000);  // blinkLED to run every 0.15 seconds
    CBixi::Instance();
    CLogging::log("CBixi::CBixi: Bixi is initialized...");


    while (!CBixi::Instance().ShuttingDown()) {
        CBixi::Instance().Continue();
    }
}
