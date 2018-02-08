#pragma once

#define USE_OCTOWS2811
#include "OctoWS2811.h"
#include "FastLED.h"

class CBixi
{
    public: // singleton
        static CBixi& Instance();
        CBixi();
        ~CBixi();
        void Continue();
        void ShutDown() { m_shutting_down = true; }
        bool ShuttingDown() { return m_shutting_down; }
        size_t Iteration() { return s_iteration; }
        void logFrameRate();
        uint16_t XYsafe(uint8_t, uint8_t);

    private:
        size_t m_lastIndicator = 0;
        bool   m_shutting_down = false;
        bool   m_indicatorOn = false;
        size_t s_iteration    = 0;
        uint16_t XY(uint8_t x, uint8_t y);
        void DrawOneFrame(byte, int8_t, int8_t);
};
