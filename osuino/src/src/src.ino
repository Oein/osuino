#define _IS_ARDUINO_
#include "./defines.hpp"

// MARK: - Libraries
#include <stdio.h>
#include <string.h>
#include "./platform/platform-arduino.hpp"

#define BUTTON0 34
#define BUTTON1 35
#define BUTTON2 21
#define BUTTON3 23

#include "./main.hpp"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

void setup()
{
    tft.begin();
    Serial.begin(115200);
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    pinMode(BUTTON0, INPUT_PULLUP);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(BUTTON3, INPUT_PULLUP);
}

int t = 0;

void loop()
{
    bool lstate[4] = {__buttonPressed__state__[0], __buttonPressed__state__[1], __buttonPressed__state__[2], __buttonPressed__state__[3]};
    __buttonPressed__state__[0] = digitalRead(BUTTON0) == HIGH;
    __buttonPressed__state__[1] = digitalRead(BUTTON1) == HIGH;
    __buttonPressed__state__[2] = digitalRead(BUTTON2) == HIGH;
    __buttonPressed__state__[3] = digitalRead(BUTTON3) == HIGH;
    if (lstate[0] != __buttonPressed__state__[0] || lstate[1] != __buttonPressed__state__[1] || lstate[2] != __buttonPressed__state__[2] || lstate[3] != __buttonPressed__state__[3])
    {
        Serial.printf("Button state %d %d %d %d\n", __buttonPressed__state__[0], __buttonPressed__state__[1], __buttonPressed__state__[2], __buttonPressed__state__[3]);
    }

    // Serial.printf("Button state %d %d %d %d\n", __buttonPressed__state__[0], __buttonPressed__state__[1], __buttonPressed__state__[2], __buttonPressed__state__[3]);

    update();
#ifdef _SERIAL_DRAW_
    if (Serial.available() > 0)
    {
        const char c = Serial.read();
        if (c == 's')
            __buttonPressed__state__[0] = true;
        if (c == 'd')
            __buttonPressed__state__[1] = true;
        if (c == 'l')
            __buttonPressed__state__[2] = true;
        if (c == 'k')
            __buttonPressed__state__[3] = true;
        if (c == 'S')
            __buttonPressed__state__[0] = false;
        if (c == 'D')
            __buttonPressed__state__[1] = false;
        if (c == 'L')
            __buttonPressed__state__[2] = false;
        if (c == 'K')
            __buttonPressed__state__[3] = false;
        // printf("Key state %d %d %d %d\n", __buttonPressed__state__[0], __buttonPressed__state__[1], __buttonPressed__state__[2], __buttonPressed__state__[3]);
    }
#endif
}