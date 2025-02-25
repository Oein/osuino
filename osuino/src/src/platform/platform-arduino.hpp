#define IColorType int16_t
#define IStringType String
#define subString(sub, start, end) sub.substring(start, end)

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

class CnavasAPI
{
public:
    CnavasAPI()
    {
    }

    void drawRect(int x, int y, int width, int height, IColorType color)
    {
        tft.fillRect(x, y, width, height, color);
    }

    void drawCircle(int x, int y, int radius, IColorType color)
    {
        tft.fillCircle(x, y, radius, color);
    }

    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        tft.fillTriangle(x1, y1, x2, y2, x3, y3, color);
    }

    void drawTriangleOutline(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        tft.drawTriangle(x1, y1, x2, y2, x3, y3, color);
    }

    void clear()
    {
        tft.fillScreen(COLOR_BLACK);
    }

    void drawIntroText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 26pt
        // relative to center
        tft.setFreeFont(FSB24);
        tft.setTextColor(color);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
    }

    void drawOsuLogoText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 26pt
        // relative to center
        drawIntroText(x, y, text, color);
    }

    void drawOsuLogoTextSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        tft.setFreeFont(FSB12);
        tft.setTextColor(color);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
    }

    void drawButtonName(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        drawOsuLogoTextSmaller(x, y, text, color);
    }

    void drawText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        drawOsuLogoTextSmaller(x, y, text, color);
    }

    void drawTextTopLeft(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to top left
        tft.setFreeFont(FSB12);
        tft.setTextColor(color);
        tft.setTextDatum(TL_DATUM);
        tft.drawString(text, x, y);
    }

    void drawTextTopLeftSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 10pt
        // relative to top left
        tft.setFreeFont(FSB12);
        tft.setTextColor(color);
        tft.setTextDatum(TL_DATUM);
        tft.drawString(text, x, y);
    }
    
    void drawComboText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        tft.setFreeFont(FSB12);
        tft.setTextColor(color, COLOR_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
    }

    void drawComboNumber(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        // 13pt
        // relative to center
        tft.setFreeFont(FSB24);
        tft.setTextColor(color, COLOR_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(text, x, y);
    }
};