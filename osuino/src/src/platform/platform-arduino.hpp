#include "WiFi.h"

#define IColorType int16_t
#define IStringType String
#define subString(sub, start, length) sub.substring(start, start + length)

IColorType rgb(unsigned char r, unsigned char g, unsigned char b)
{
    if (r < 0 || 255 < r || g < 0 || 255 < g || b < 0 || b > 255)
        return -1;

    unsigned char red = r >> 3;
    unsigned char green = g >> 2;
    unsigned char blue = b >> 3;

    int result = (red << (5 + 6)) | (green << 5) | blue;
    return result;
}
IColorType rgb_darken(int r, int g, int b, float factor__)
{
    float factor = factor__ < 0 ? 0 : factor__ > 1 ? 1
                                                   : factor__;
    return rgb(r * factor, g * factor, b * factor);
}

const IColorType COLOR_BLACK = rgb(0, 0, 0);
const IColorType COLOR_WHITE = rgb(255, 255, 255);
const IColorType COLOR_OSU = rgb(0xF1, 0x60, 0xA1);
const IColorType NOTE_COLORS[] = {
    // #C5C5C8
    // #FFA503
    rgb(0xC5, 0xC5, 0xC8),
    rgb(0xFF, 0xA5, 0x03),
    rgb(0xFF, 0xA5, 0x03),
    rgb(0xC5, 0xC5, 0xC8),
};

int randint(int min, int max)
{
    return min + (rand() % (max - min + 1));
}

int string2int(IStringType str)
{
    return str.toInt();
}

float string2float(IStringType str)
{
    return str.toFloat();
}

IStringType int2string(int num)
{
    return String(num);
}

IStringType char2string(char chr)
{
    return String(chr);
}

IStringType ul2string(unsigned long num)
{
    return String(num);
}

IStringType float2string(float num)
{
    return String(num);
}

int largest(int a, int b)
{
    return a > b ? a : b;
}

unsigned long emscripten_get_now()
{
    return millis();
}

bool __buttonPressed__state__[4] = {false, false, false, false};
bool buttonPressed(int button)
{
    return __buttonPressed__state__[button];
}

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "./../lib/Free_fonts.h"

TFT_eSPI tft = TFT_eSPI();

#ifdef _SERIAL_DRAW_
IStringType number2base64(int num, int length)
{
    const IStringType base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    IStringType result = "";
    while (num > 0)
    {
        result = base[num % 64] + result;
        num /= 64;
    }
    // fill with _ 
    while (result.length() < length)
    {
        result = "_" + result;
    }
    return result;
}
IStringType color2string(IColorType color)
{
    // restore 565 color to 888 color
    unsigned char r = ((color >> 11) & 0x1F) << 3;
    unsigned char g = ((color >> 5) & 0x3F) << 2;
    unsigned char b = (color & 0x1F) << 3;
    return number2base64(r, 2) + number2base64(g, 2) + number2base64(b, 2);
}
#endif

class CnavasAPI
{
public:
    CnavasAPI()
    {
    }

    void drawRect(int x, int y, int width, int height, IColorType color)
    {
        tft.fillRect(x, y, width, height, color);
#ifdef _SERIAL_DRAW_
        Serial.print("@R" + number2base64(x, 2) + number2base64(y, 2) + number2base64(width, 2) + number2base64(height, 2) + color2string(color));
#endif
    }

    void drawCircle(int x, int y, int radius, IColorType color)
    {
        tft.fillCircle(x, y, radius, color);
#ifdef _SERIAL_DRAW_
        Serial.print("@C" + number2base64(x, 2) + number2base64(y, 2) + number2base64(radius, 2) + color2string(color));
#endif
    }

    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        tft.fillTriangle(x1, y1, x2, y2, x3, y3, color);
#ifdef _SERIAL_DRAW_
        Serial.print("@T" + number2base64(x1, 2) + number2base64(y1, 2) + number2base64(x2, 2) + number2base64(y2, 2) + number2base64(x3, 2) + number2base64(y3, 2) + color2string(color));
#endif
    }

    void drawTriangleOutline(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        tft.drawTriangle(x1, y1, x2, y2, x3, y3, color);
#ifdef _SERIAL_DRAW_
        Serial.print("@t" + number2base64(x1, 2) + number2base64(y1, 2) + number2base64(x2, 2) + number2base64(y2, 2) + number2base64(x3, 2) + number2base64(y3, 2) + color2string(color));
#endif
    }

    void clear()
    {
        tft.fillScreen(COLOR_BLACK);
#ifdef _SERIAL_DRAW_
        Serial.print("@c");
#endif
    }

    void drawIntroText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 26pt
        // relative to center
        tft.setFreeFont(FSB24);
        tft.setTextColor(color);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
#ifdef _SERIAL_DRAW_
        Serial.print("@q" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawOsuLogoText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 26pt
        // relative to center
        drawIntroText(x, y, text, color);
#ifdef _SERIAL_DRAW_
        Serial.print("@Q" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawOsuLogoTextTopLeft(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 26pt
        // relative to top left
        tft.setFreeFont(FSB24);
        tft.setTextColor(color);
        tft.setTextDatum(TL_DATUM);
        tft.drawString(text, x, y);
#ifdef _SERIAL_DRAW_
        Serial.print("@x" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawOsuLogoTextSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        tft.setFreeFont(FSB12);
        tft.setTextColor(color);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
#ifdef _SERIAL_DRAW_
        Serial.print("@w" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawButtonName(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        drawOsuLogoTextSmaller(x, y, text, color);
#ifdef _SERIAL_DRAW_
        Serial.print("@W" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        drawOsuLogoTextSmaller(x, y, text, color);
#ifdef _SERIAL_DRAW_
        Serial.print("@e" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawTextTopLeft(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to top left
        tft.setFreeFont(FSB12);
        tft.setTextColor(color);
        tft.setTextDatum(TL_DATUM);
        tft.drawString(text, x, y);
#ifdef _SERIAL_DRAW_
        Serial.print("@E" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawTextTopLeftSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 10pt
        // relative to top left
        tft.setFreeFont(FSB12);
        tft.setTextColor(color);
        tft.setTextDatum(TL_DATUM);
        tft.drawString(text, x, y);
#ifdef _SERIAL_DRAW_
        Serial.print("@r" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }
    
    void drawComboText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        tft.setFreeFont(FSB12);
        tft.setTextColor(color, COLOR_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
#ifdef _SERIAL_DRAW_
        Serial.print("@v" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }

    void drawComboNumber(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        tft.setFreeFont(FSB24);
        tft.setTextColor(color, COLOR_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
#ifdef _SERIAL_DRAW_
        Serial.print("@a" + number2base64(x, 2) + number2base64(y, 2) + number2base64(text.length(), 2) + text + color2string(color));
#endif
    }
};