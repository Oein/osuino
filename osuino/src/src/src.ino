#define _IS_ARDUINO_
#include "./arduino.hpp"

void setup()
{
    tft.begin();
    Serial.begin(115200);
}
void loop()
{
    update();
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
}