#include "WProgram.h"
#include "logging.h"
#include "bixi.h"


extern "C" int main(void) {

    CLogging::Init();
    CBixi::Instance();
    CLogging::log("CBixi::CBixi: Bixi is initialized...");

    while (!CBixi::Instance().ShuttingDown()) {
        CBixi::Instance().Continue();
        CLogging::log("Heartbeat");
        digitalWriteFast(13, HIGH);
        delay(100);
        digitalWriteFast(13, LOW);
        delay(100);
        digitalWriteFast(13, HIGH);
        delay(100);
        digitalWriteFast(13, LOW);
        delay(900);
    }
}
