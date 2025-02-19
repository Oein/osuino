#include <stdio.h>
#include <string.h>

// https://github.com/codewitch-honey-crisis/htcw_zip/blob/master/examples/demo/src/main.cpp

// MARK: - Constants

#define IColorType int16_t
#define IStringType String
#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 480

#define __INTRO_SCENE_MS_ 100
#define __INTRO_SCENE_TRIANGLE_NOSPAWN_MS_ 1450 - 50
#define __INTRO_SCENE_END_MS_ 1600

#define __SELECT_MAP_HEIGHT__ 50
#define __SELECT_MAP_GAP__ 10

#define _ACCURACY_BASE_Y_ CANVAS_HEIGHT / 4 * 3 - 10
#define _COMBO_BASE_Y_ CANVAS_HEIGHT / 2 - 100

const int __SELECT_MAP_COUNT__ = (CANVAS_HEIGHT - __SELECT_MAP_GAP__) / (__SELECT_MAP_HEIGHT__ + __SELECT_MAP_GAP__);
const int __SELECT_PREV_MAPS__ = (__SELECT_MAP_COUNT__ - 1) / 2;
const int __SELECT_NEXT_MAPS__ = (__SELECT_MAP_COUNT__ - __SELECT_PREV_MAPS__ - 1);
const int __SELECT_MAP_START_Y__ = 10;

// which means a note shown on the screen for 300ms
// after 300ms it should be pressed
float _NOTE_SPEED_ = 500.0;
// which means a note can be pressed 50ms before it shown on the screen
// and 50ms after it shown on the screen
float _NOTE_ALLOWANCE_ = 50.0;
float _NOTE_START_Y_ = 0.0;
float _NOTE_END_Y_ = CANVAS_HEIGHT - 50.0;
float _NOTE_Y_RANGE_ = _NOTE_END_Y_ - _NOTE_START_Y_;
float _NOTE_Y_SPEED_ = _NOTE_Y_RANGE_ / _NOTE_SPEED_;

#define _JUDEGE_MAX_100_ 41
#define _JUDGE_MAX_90_ 54
#define _JUDGE_MAX_80_ 75
#define _JUDGE_MAX_70_ 96
#define _JUDGE_MAX_60_ 120
#define _JUDGE_MAX_50_ 129
#define _JUDGE_MAX_40_ 138
#define _JUDGE_MAX_30_ 147
#define _JUDGE_MAX_20_ 159
#define _JUDGE_MAX_10_ 177

#define __DO_NOT_ANIMATE__ true

float _HEALTH_BY_SCORE_[] = {
    -7,
    0.5, // 10,
    0.71, // 20
    0.8, // 30
    0.88, // 40
    0.95, // 50
    1, // 60
    1.05, // 70
    1.15, // 80
    1.3, // 90
    1.5, // 100
};

// MARK: - Hardware specific API

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

IStringType char2string(char chr) {
    return String(chr);
}

int largest(int a, int b)
{
    return a > b ? a : b;
}

unsigned long emscripten_get_now() {
  return millis();
}

bool __buttonPressed__state__[4] = {false, false, false, false};
bool buttonPressed(int button)
{
    return __buttonPressed__state__[button];
}

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "./Free_fonts.h"

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
};

// MARK: - Lib Multiplatform data structure

#include <vector>

template <typename T>
class IVector
{
public:
    std::vector<T> data;
    unsigned long size()
    {
        return data.size();
    }
    void push_back(T value)
    {
        data.push_back(value);
    }
    T at(unsigned long index)
    {
        return data.at(index);
    }
    void clear()
    {
        data.clear();
    }
};

#include <map>

template <typename T>
class IMap
{
public:
    std::map<IStringType, T> data;

    unsigned long size()
    {
        return data.size();
    }
    void insert(IStringType key, T value)
    {
        data.insert(std::pair<IStringType, T>(key, value));
    }
    T at(IStringType key)
    {
        return data.at(key);
    }
    void clear()
    {
        data.clear();
    }
    bool has(IStringType key)
    {
        return data.find(key) != data.end();
    }
};

// MARK: - EMSCRIPTEN fs
const IStringType OSUGAMEFILE1 = "osu file format v14\n\n[General]\nAudioFilename: audio.mp3\nAudioLeadIn: 0\nPreviewTime: 103005\nCountdown: 0\nSampleSet: Soft\nStackLeniency: 0.7\nMode: 3\nLetterboxInBreaks: 0\nSpecialStyle: 0\nWidescreenStoryboard: 1\n\n[Editor]\nDistanceSpacing: 1.7\nBeatDivisor: 4\nGridSize: 32\nTimelineZoom: 1.6\n\n[Metadata]\nTitle:Dreamin' attraction!!\nTitleUnicode:Dreamin' attraction!!\nArtist:Hino Isuka\nArtistUnicode:翡乃イスカ\nCreator:DenYi\nVersion:Beginning!!\nSource:Arcaea\nTags:4kmt Murumoo Niks AAAAFanboy rhythm video game instrumental electronic hardcore bofu 2015 大血戦 -the bms of fighters ultimate- GROOVE COASTER 4MAX DIAMOND GALAXY Groundbreaking 2015 Dancing Drive fa featured artist mappers' guild mpg bitpop happy\nBeatmapID:4359139\nBeatmapSetID:2081438\n\n[Difficulty]\nHPDrainRate:6.5\nCircleSize:4\nOverallDifficulty:6.5\nApproachRate:5\nSliderMultiplier:1.4\nSliderTickRate:1\n\n[Events]\n//Background and Video events\n0,0,\"bg.jpg\",0,0\n//Break Periods\n//Storyboard Layer 0 (Background)\n//Storyboard Layer 1 (Fail)\n//Storyboard Layer 2 (Pass)\n//Storyboard Layer 3 (Foreground)\n//Storyboard Layer 4 (Overlay)\n//Storyboard Sound Samples\n\n[TimingPoints]\n1152,292.682926829268,4,2,1,50,1,0\n39786,-100,4,2,1,50,0,1\n48566,-100,4,2,1,50,0,0\n49151,-100,4,2,1,50,0,1\n58517,-100,4,2,1,50,0,0\n103005,-100,4,2,1,50,0,1\n112371,-100,4,2,1,50,0,0\n114712,-100,4,2,1,50,0,1\n119395,-100,4,2,1,50,0,0\n\n\n[HitObjects]\n64,192,1152,5,0,0:0:0:0:\n192,192,1444,1,0,0:0:0:0:\n320,192,1737,1,0,0:0:0:0:\n448,192,2030,128,0,2322:0:0:0:0:\n64,192,2322,128,0,2615:0:0:0:0:\n320,192,2615,1,0,0:0:0:0:\n192,192,2761,128,0,3054:0:0:0:0:\n448,192,3054,1,0,0:0:0:0:\n64,192,3347,1,0,0:0:0:0:\n448,192,3493,128,0,3786:0:0:0:0:\n320,192,3786,128,0,4078:0:0:0:0:\n192,192,4078,128,0,4371:0:0:0:0:\n64,192,4371,128,0,4664:0:0:0:0:\n448,192,4664,128,0,4956:0:0:0:0:\n192,192,4956,1,0,0:0:0:0:\n320,192,5103,128,0,5395:0:0:0:0:\n64,192,5395,1,0,0:0:0:0:\n448,192,5688,1,0,0:0:0:0:\n192,192,5834,128,0,6127:0:0:0:0:\n320,192,6127,128,0,6420:0:0:0:0:\n64,192,6420,128,0,6712:0:0:0:0:\n448,192,6712,128,0,7005:0:0:0:0:\n192,192,7005,128,0,7298:0:0:0:0:\n320,192,7298,1,0,0:0:0:0:\n64,192,7444,128,0,7737:0:0:0:0:\n192,192,7737,1,0,0:0:0:0:\n448,192,7883,1,0,0:0:0:0:\n64,192,8030,1,0,0:0:0:0:\n320,192,8176,128,0,8469:0:0:0:0:\n192,192,8469,128,0,8761:0:0:0:0:\n448,192,8761,128,0,9054:0:0:0:0:\n320,192,9054,128,0,9347:0:0:0:0:\n64,192,9347,128,0,9639:0:0:0:0:\n192,192,9639,128,0,9932:0:0:0:0:\n320,192,9932,128,0,10225:0:0:0:0:\n448,192,10225,128,0,10517:0:0:0:0:\n192,192,10664,1,0,0:0:0:0:\n320,192,10883,1,0,0:0:0:0:\n64,192,11103,128,0,11395:0:0:0:0:\n448,192,11395,128,0,11688:0:0:0:0:\n64,192,11688,1,0,0:0:0:0:\n192,192,11688,1,0,0:0:0:0:\n320,192,11981,1,0,0:0:0:0:\n448,192,12273,1,0,0:0:0:0:\n64,192,12566,1,0,0:0:0:0:\n320,192,12712,1,0,0:0:0:0:\n192,192,12859,1,0,0:0:0:0:\n448,192,13151,1,0,0:0:0:0:\n320,192,13444,1,0,0:0:0:0:\n192,192,13664,1,0,0:0:0:0:\n64,192,13883,1,0,0:0:0:0:\n448,192,14030,1,0,0:0:0:0:\n192,192,14322,1,0,0:0:0:0:\n320,192,14615,1,0,0:0:0:0:\n448,192,14908,1,0,0:0:0:0:\n64,192,15054,1,0,0:0:0:0:\n192,192,15200,1,0,0:0:0:0:\n320,192,15493,1,0,0:0:0:0:\n64,192,15639,1,0,0:0:0:0:\n448,192,15786,1,0,0:0:0:0:\n192,192,16078,1,0,0:0:0:0:\n320,192,16225,1,0,0:0:0:0:\n64,192,16371,128,0,16664:0:0:0:0:\n448,192,16664,1,0,0:0:0:0:\n192,192,16810,1,0,0:0:0:0:\n320,192,16956,1,0,0:0:0:0:\n64,192,17103,1,0,0:0:0:0:\n448,192,17249,128,0,17542:0:0:0:0:\n192,192,17542,128,0,17834:0:0:0:0:\n448,192,17834,1,0,0:0:0:0:\n64,192,17981,1,0,0:0:0:0:\n320,192,18127,128,0,18420:0:0:0:0:\n192,192,18420,128,0,18712:0:0:0:0:\n64,192,18712,128,0,19005:0:0:0:0:\n448,192,19005,128,0,19298:0:0:0:0:\n192,192,19298,128,0,19591:0:0:0:0:\n320,192,19591,128,0,19883:0:0:0:0:\n64,192,19883,5,0,0:0:0:0:\n192,192,19883,1,0,0:0:0:0:\n448,192,20176,1,0,0:0:0:0:\n320,192,20176,1,0,0:0:0:0:\n64,192,20469,1,0,0:0:0:0:\n448,192,20469,1,0,0:0:0:0:\n192,192,20761,1,0,0:0:0:0:\n320,192,20761,1,0,0:0:0:0:\n448,192,21054,128,0,21347:0:0:0:0:\n64,192,21347,1,0,0:0:0:0:\n192,192,21493,1,0,0:0:0:0:\n320,192,21639,128,0,21932:0:0:0:0:\n64,192,21932,1,0,0:0:0:0:\n448,192,22078,1,0,0:0:0:0:\n64,192,22225,1,0,0:0:0:0:\n320,192,22371,1,0,0:0:0:0:\n448,192,22517,1,0,0:0:0:0:\n192,192,22517,128,0,22810:0:0:0:0:\n320,192,22810,1,0,0:0:0:0:\n64,192,22956,1,0,0:0:0:0:\n448,192,23103,128,0,23395:0:0:0:0:\n192,192,23395,1,0,0:0:0:0:\n64,192,23688,1,0,0:0:0:0:\n448,192,23981,1,0,0:0:0:0:\n320,192,24273,128,0,24566:0:0:0:0:\n64,192,24566,128,0,24859:0:0:0:0:\n192,192,24859,128,0,25151:0:0:0:0:\n448,192,25151,128,0,25444:0:0:0:0:\n320,192,25444,128,0,25737:0:0:0:0:\n64,192,25737,1,0,0:0:0:0:\n448,192,25883,1,0,0:0:0:0:\n192,192,26030,1,0,0:0:0:0:\n320,192,26322,1,0,0:0:0:0:\n64,192,26469,1,0,0:0:0:0:\n448,192,26615,128,0,26908:0:0:0:0:\n192,192,26908,1,0,0:0:0:0:\n320,192,27054,1,0,0:0:0:0:\n64,192,27200,1,0,0:0:0:0:\n448,192,27347,1,0,0:0:0:0:\n192,192,27493,128,0,27786:0:0:0:0:\n320,192,27786,128,0,28078:0:0:0:0:\n64,192,28078,128,0,28371:0:0:0:0:\n192,192,28371,128,0,28664:0:0:0:0:\n448,192,28664,128,0,28956:0:0:0:0:\n64,192,28956,128,0,29249:0:0:0:0:\n320,192,29249,5,0,0:0:0:0:\n448,192,29249,1,0,0:0:0:0:\n64,192,29542,1,0,0:0:0:0:\n192,192,29542,1,0,0:0:0:0:\n448,192,29834,1,0,0:0:0:0:\n64,192,29834,1,0,0:0:0:0:\n320,192,30127,1,0,0:0:0:0:\n192,192,30273,1,0,0:0:0:0:\n448,192,30420,128,0,31005:0:0:0:0:\n64,192,31005,128,0,31298:0:0:0:0:\n320,192,31298,1,0,0:0:0:0:\n192,192,31444,1,0,0:0:0:0:\n64,192,31591,1,0,0:0:0:0:\n320,192,31737,1,0,0:0:0:0:\n448,192,31737,1,0,0:0:0:0:\n64,192,31956,1,0,0:0:0:0:\n192,192,31956,1,0,0:0:0:0:\n320,192,32176,1,0,0:0:0:0:\n448,192,32322,1,0,0:0:0:0:\n192,192,32469,1,0,0:0:0:0:\n320,192,32615,1,0,0:0:0:0:\n64,192,32761,128,0,33054:0:0:0:0:\n448,192,33054,1,0,0:0:0:0:\n192,192,33200,1,0,0:0:0:0:\n320,192,33347,128,0,33639:0:0:0:0:\n64,192,33639,1,0,0:0:0:0:\n448,192,33786,1,0,0:0:0:0:\n192,192,33932,128,0,34225:0:0:0:0:\n320,192,34225,128,0,34517:0:0:0:0:\n448,192,34517,128,0,34810:0:0:0:0:\n64,192,34810,128,0,35103:0:0:0:0:\n320,192,35103,1,0,0:0:0:0:\n448,192,35103,1,0,0:0:0:0:\n192,192,35395,1,0,0:0:0:0:\n320,192,35688,1,0,0:0:0:0:\n448,192,35981,1,0,0:0:0:0:\n192,192,36273,1,0,0:0:0:0:\n64,192,36566,1,0,0:0:0:0:\n320,192,36859,1,0,0:0:0:0:\n192,192,37151,1,0,0:0:0:0:\n448,192,37444,128,0,37737:0:0:0:0:\n320,192,37737,128,0,38030:0:0:0:0:\n192,192,38030,128,0,38322:0:0:0:0:\n64,192,38322,128,0,38615:0:0:0:0:\n320,192,38615,128,0,39200:0:0:0:0:\n192,192,39200,128,0,39493:0:0:0:0:\n320,192,39493,1,0,0:0:0:0:\n448,192,39493,1,0,0:0:0:0:\n64,192,39786,1,0,0:0:0:0:\n448,192,39786,1,0,0:0:0:0:\n192,192,40005,1,0,0:0:0:0:\n320,192,40225,1,0,0:0:0:0:\n64,192,40371,1,0,0:0:0:0:\n448,192,40517,1,0,0:0:0:0:\n192,192,40664,128,0,40956:0:0:0:0:\n320,192,40956,1,0,0:0:0:0:\n448,192,41249,1,0,0:0:0:0:\n64,192,41395,1,0,0:0:0:0:\n320,192,41542,1,0,0:0:0:0:\n320,192,41834,128,0,42127:0:0:0:0:\n64,192,42127,128,0,42420:0:0:0:0:\n192,192,42420,128,0,42712:0:0:0:0:\n448,192,42712,1,0,0:0:0:0:\n320,192,42859,1,0,0:0:0:0:\n192,192,43005,1,0,0:0:0:0:\n64,192,43151,1,0,0:0:0:0:\n448,192,43298,128,0,43591:0:0:0:0:\n192,192,43591,1,0,0:0:0:0:\n320,192,43737,1,0,0:0:0:0:\n64,192,43883,1,0,0:0:0:0:\n192,192,44103,1,0,0:0:0:0:\n320,192,44322,1,0,0:0:0:0:\n64,192,44469,128,0,44908:0:0:0:0:\n448,192,44908,1,0,0:0:0:0:\n192,192,45054,128,0,45493:0:0:0:0:\n320,192,45493,1,0,0:0:0:0:\n64,192,45639,1,0,0:0:0:0:\n448,192,45859,1,0,0:0:0:0:\n192,192,46078,1,0,0:0:0:0:\n320,192,46225,128,0,46664:0:0:0:0:\n64,192,46664,1,0,0:0:0:0:\n448,192,46810,1,0,0:0:0:0:\n192,192,47030,1,0,0:0:0:0:\n448,192,47249,1,0,0:0:0:0:\n64,192,47395,1,0,0:0:0:0:\n192,192,47542,1,0,0:0:0:0:\n320,192,47688,1,0,0:0:0:0:\n448,192,47834,1,0,0:0:0:0:\n64,192,47981,128,0,48273:0:0:0:0:\n320,192,48273,1,0,0:0:0:0:\n448,192,48420,128,0,48712:0:0:0:0:\n64,192,48712,1,0,0:0:0:0:\n320,192,48859,1,0,0:0:0:0:\n192,192,49005,1,0,0:0:0:0:\n64,192,49151,1,0,0:0:0:0:\n448,192,49151,1,0,0:0:0:0:\n320,192,49371,1,0,0:0:0:0:\n192,192,49591,1,0,0:0:0:0:\n448,192,49737,1,0,0:0:0:0:\n64,192,49883,1,0,0:0:0:0:\n320,192,50030,128,0,50322:0:0:0:0:\n192,192,50322,1,0,0:0:0:0:\n448,192,50615,1,0,0:0:0:0:\n320,192,50761,1,0,0:0:0:0:\n192,192,50908,128,0,51200:0:0:0:0:\n448,192,51200,128,0,51493:0:0:0:0:\n64,192,51493,128,0,51786:0:0:0:0:\n320,192,51786,1,0,0:0:0:0:\n192,192,51932,1,0,0:0:0:0:\n448,192,52078,1,0,0:0:0:0:\n320,192,52298,1,0,0:0:0:0:\n64,192,52517,1,0,0:0:0:0:\n192,192,52664,1,0,0:0:0:0:\n448,192,52810,1,0,0:0:0:0:\n320,192,52956,1,0,0:0:0:0:\n192,192,53103,1,0,0:0:0:0:\n64,192,53249,128,0,53834:0:0:0:0:\n192,192,53542,1,0,0:0:0:0:\n320,192,53834,1,0,0:0:0:0:\n448,192,53834,1,0,0:0:0:0:\n320,192,54127,128,0,54420:0:0:0:0:\n64,192,54566,1,0,0:0:0:0:\n448,192,54712,1,0,0:0:0:0:\n320,192,54859,1,0,0:0:0:0:\n192,192,55005,1,0,0:0:0:0:\n192,192,55298,128,0,55591:0:0:0:0:\n320,192,55737,1,0,0:0:0:0:\n192,192,55883,1,0,0:0:0:0:\n64,192,56030,1,0,0:0:0:0:\n448,192,56176,128,0,56469:0:0:0:0:\n192,192,56469,1,0,0:0:0:0:\n64,192,56615,128,0,56908:0:0:0:0:\n320,192,56908,1,0,0:0:0:0:\n192,192,57054,1,0,0:0:0:0:\n448,192,57200,1,0,0:0:0:0:\n320,192,57347,128,0,57639:0:0:0:0:\n64,192,57639,1,0,0:0:0:0:\n192,192,57786,1,0,0:0:0:0:\n448,192,57932,128,0,58225:0:0:0:0:\n64,192,58225,128,0,58517:0:0:0:0:\n320,192,58517,128,0,59688:0:0:0:0:\n192,192,59688,128,0,60273:0:0:0:0:\n448,192,60273,128,0,60566:0:0:0:0:\n64,192,60273,1,0,0:0:0:0:\n192,192,60566,1,0,0:0:0:0:\n320,192,60712,1,0,0:0:0:0:\n64,192,60859,5,0,0:0:0:0:\n448,192,60859,1,0,0:0:0:0:\n64,192,61151,1,0,0:0:0:0:\n320,192,61444,1,0,0:0:0:0:\n320,192,61737,1,0,0:0:0:0:\n64,192,62030,1,0,0:0:0:0:\n192,192,62030,1,0,0:0:0:0:\n192,192,62322,1,0,0:0:0:0:\n448,192,62615,1,0,0:0:0:0:\n448,192,62908,1,0,0:0:0:0:\n320,192,63200,1,0,0:0:0:0:\n64,192,63200,1,0,0:0:0:0:\n320,192,63493,1,0,0:0:0:0:\n192,192,63786,1,0,0:0:0:0:\n192,192,64078,1,0,0:0:0:0:\n448,192,64371,1,0,0:0:0:0:\n64,192,64371,1,0,0:0:0:0:\n64,192,64664,1,0,0:0:0:0:\n320,192,64956,1,0,0:0:0:0:\n320,192,65249,1,0,0:0:0:0:\n192,192,65395,1,0,0:0:0:0:\n64,192,65542,1,0,0:0:0:0:\n448,192,65542,1,0,0:0:0:0:\n448,192,65834,1,0,0:0:0:0:\n192,192,66127,1,0,0:0:0:0:\n192,192,66420,1,0,0:0:0:0:\n320,192,66712,1,0,0:0:0:0:\n448,192,66712,1,0,0:0:0:0:\n320,192,67005,1,0,0:0:0:0:\n64,192,67298,1,0,0:0:0:0:\n64,192,67591,1,0,0:0:0:0:\n192,192,67883,1,0,0:0:0:0:\n448,192,67883,1,0,0:0:0:0:\n448,192,68176,1,0,0:0:0:0:\n64,192,68469,1,0,0:0:0:0:\n192,192,68615,1,0,0:0:0:0:\n320,192,68761,1,0,0:0:0:0:\n448,192,68908,1,0,0:0:0:0:\n64,192,69054,1,0,0:0:0:0:\n64,192,69347,1,0,0:0:0:0:\n320,192,69639,1,0,0:0:0:0:\n192,192,69786,1,0,0:0:0:0:\n448,192,69932,128,0,70225:0:0:0:0:\n64,192,70225,128,0,70517:0:0:0:0:\n192,192,70517,1,0,0:0:0:0:\n320,192,70664,1,0,0:0:0:0:\n448,192,70810,1,0,0:0:0:0:\n192,192,71030,1,0,0:0:0:0:\n320,192,71249,1,0,0:0:0:0:\n64,192,71395,1,0,0:0:0:0:\n192,192,71615,1,0,0:0:0:0:\n320,192,71834,1,0,0:0:0:0:\n448,192,71981,128,0,72273:0:0:0:0:\n64,192,72273,128,0,72566:0:0:0:0:\n320,192,72566,128,0,72859:0:0:0:0:\n192,192,72859,1,0,0:0:0:0:\n448,192,73005,1,0,0:0:0:0:\n64,192,73151,1,0,0:0:0:0:\n320,192,73298,1,0,0:0:0:0:\n64,192,73444,128,0,73737:0:0:0:0:\n448,192,73737,1,0,0:0:0:0:\n320,192,73883,1,0,0:0:0:0:\n192,192,74030,1,0,0:0:0:0:\n64,192,74176,1,0,0:0:0:0:\n448,192,74322,128,0,74615:0:0:0:0:\n192,192,74615,128,0,74908:0:0:0:0:\n320,192,74908,1,0,0:0:0:0:\n448,192,74908,1,0,0:0:0:0:\n64,192,75200,1,0,0:0:0:0:\n320,192,75493,1,0,0:0:0:0:\n192,192,75786,1,0,0:0:0:0:\n448,192,76078,1,0,0:0:0:0:\n320,192,76371,1,0,0:0:0:0:\n64,192,76664,1,0,0:0:0:0:\n192,192,76956,1,0,0:0:0:0:\n448,192,77249,1,0,0:0:0:0:\n320,192,77395,1,0,0:0:0:0:\n64,192,77542,128,0,77834:0:0:0:0:\n192,192,77834,1,0,0:0:0:0:\n320,192,77981,1,0,0:0:0:0:\n448,192,78127,128,0,78420:0:0:0:0:\n64,192,78420,128,0,79298:0:0:0:0:\n320,192,79298,1,0,0:0:0:0:\n448,192,79298,128,0,79591:0:0:0:0:\n192,192,79591,128,0,80469:0:0:0:0:\n448,192,80469,128,0,80761:0:0:0:0:\n64,192,80761,128,0,81054:0:0:0:0:\n320,192,81054,1,0,0:0:0:0:\n192,192,81200,1,0,0:0:0:0:\n448,192,81493,128,0,81786:0:0:0:0:\n192,192,81786,1,0,0:0:0:0:\n320,192,81932,128,0,82371:0:0:0:0:\n64,192,82371,1,0,0:0:0:0:\n448,192,82517,1,0,0:0:0:0:\n192,192,82737,1,0,0:0:0:0:\n320,192,82956,1,0,0:0:0:0:\n64,192,83103,128,0,83542:0:0:0:0:\n192,192,83542,1,0,0:0:0:0:\n448,192,83688,128,0,83981:0:0:0:0:\n320,192,83981,128,0,84273:0:0:0:0:\n192,192,84273,128,0,84566:0:0:0:0:\n448,192,84712,1,0,0:0:0:0:\n64,192,84859,128,0,85151:0:0:0:0:\n448,192,85298,1,0,0:0:0:0:\n320,192,85444,1,0,0:0:0:0:\n192,192,85591,1,0,0:0:0:0:\n64,192,85737,1,0,0:0:0:0:\n448,192,85883,128,0,86176:0:0:0:0:\n192,192,86176,1,0,0:0:0:0:\n320,192,86322,1,0,0:0:0:0:\n64,192,86469,1,0,0:0:0:0:\n192,192,86615,128,0,86908:0:0:0:0:\n448,192,87054,1,0,0:0:0:0:\n64,192,87200,1,0,0:0:0:0:\n320,192,87347,1,0,0:0:0:0:\n192,192,87493,1,0,0:0:0:0:\n448,192,87639,1,0,0:0:0:0:\n320,192,87786,128,0,88078:0:0:0:0:\n64,192,88225,128,0,88517:0:0:0:0:\n448,192,88517,1,0,0:0:0:0:\n192,192,88664,1,0,0:0:0:0:\n320,192,88810,1,0,0:0:0:0:\n448,192,88956,128,0,89395:0:0:0:0:\n64,192,89395,1,0,0:0:0:0:\n320,192,89542,128,0,89834:0:0:0:0:\n192,192,89834,128,0,90127:0:0:0:0:\n448,192,90273,1,0,0:0:0:0:\n64,192,90420,128,0,90859:0:0:0:0:\n448,192,90859,1,0,0:0:0:0:\n320,192,91005,1,0,0:0:0:0:\n192,192,91151,1,0,0:0:0:0:\n64,192,91298,1,0,0:0:0:0:\n448,192,91298,128,0,92176:0:0:0:0:\n320,192,91591,1,0,0:0:0:0:\n192,192,91883,1,0,0:0:0:0:\n64,192,92176,128,0,92469:0:0:0:0:\n320,192,92469,128,0,92761:0:0:0:0:\n192,192,92761,1,0,0:0:0:0:\n448,192,92908,1,0,0:0:0:0:\n64,192,93054,1,0,0:0:0:0:\n320,192,93200,128,0,93493:0:0:0:0:\n192,192,93493,1,0,0:0:0:0:\n448,192,93639,128,0,94225:0:0:0:0:\n64,192,93932,1,0,0:0:0:0:\n192,192,94225,128,0,94517:0:0:0:0:\n320,192,94517,1,0,0:0:0:0:\n448,192,94664,1,0,0:0:0:0:\n64,192,94810,128,0,95395:0:0:0:0:\n320,192,95103,1,0,0:0:0:0:\n192,192,95395,1,0,0:0:0:0:\n320,192,95542,1,0,0:0:0:0:\n64,192,95688,1,0,0:0:0:0:\n448,192,95688,128,0,95981:0:0:0:0:\n192,192,95981,1,0,0:0:0:0:\n320,192,95981,1,0,0:0:0:0:\n64,192,96273,1,0,0:0:0:0:\n448,192,96566,1,0,0:0:0:0:\n320,192,96566,1,0,0:0:0:0:\n192,192,96859,1,0,0:0:0:0:\n64,192,97151,1,0,0:0:0:0:\n448,192,97151,1,0,0:0:0:0:\n320,192,97444,1,0,0:0:0:0:\n64,192,97737,1,0,0:0:0:0:\n192,192,97737,1,0,0:0:0:0:\n320,192,98030,1,0,0:0:0:0:\n448,192,98322,1,0,0:0:0:0:\n192,192,98322,1,0,0:0:0:0:\n64,192,98615,1,0,0:0:0:0:\n320,192,98908,1,0,0:0:0:0:\n448,192,98908,1,0,0:0:0:0:\n192,192,99200,1,0,0:0:0:0:\n64,192,99493,1,0,0:0:0:0:\n320,192,99493,1,0,0:0:0:0:\n448,192,99786,1,0,0:0:0:0:\n192,192,100078,1,0,0:0:0:0:\n64,192,100078,1,0,0:0:0:0:\n320,192,100371,1,0,0:0:0:0:\n64,192,100664,128,0,100956:0:0:0:0:\n448,192,100664,1,0,0:0:0:0:\n320,192,100956,128,0,101249:0:0:0:0:\n64,192,101249,1,0,0:0:0:0:\n192,192,101395,1,0,0:0:0:0:\n320,192,101542,1,0,0:0:0:0:\n448,192,101688,1,0,0:0:0:0:\n64,192,101981,1,0,0:0:0:0:\n448,192,102127,1,0,0:0:0:0:\n192,192,102420,128,0,102712:0:0:0:0:\n64,192,102712,128,0,103005:0:0:0:0:\n448,192,103005,1,0,0:0:0:0:\n320,192,103151,1,0,0:0:0:0:\n192,192,103298,128,0,103591:0:0:0:0:\n64,192,103591,1,0,0:0:0:0:\n448,192,103737,1,0,0:0:0:0:\n320,192,103883,128,0,104176:0:0:0:0:\n64,192,104176,1,0,0:0:0:0:\n192,192,104395,1,0,0:0:0:0:\n320,192,104615,1,0,0:0:0:0:\n64,192,104761,1,0,0:0:0:0:\n448,192,104981,1,0,0:0:0:0:\n320,192,105200,1,0,0:0:0:0:\n192,192,105347,1,0,0:0:0:0:\n448,192,105566,1,0,0:0:0:0:\n64,192,105786,1,0,0:0:0:0:\n320,192,105932,1,0,0:0:0:0:\n192,192,106078,1,0,0:0:0:0:\n448,192,106225,128,0,106517:0:0:0:0:\n64,192,106517,1,0,0:0:0:0:\n192,192,106664,1,0,0:0:0:0:\n320,192,106810,1,0,0:0:0:0:\n448,192,106956,1,0,0:0:0:0:\n64,192,107103,128,0,107395:0:0:0:0:\n320,192,107395,128,0,107688:0:0:0:0:\n192,192,107688,1,0,0:0:0:0:\n448,192,107981,1,0,0:0:0:0:\n320,192,107981,1,0,0:0:0:0:\n64,192,108273,1,0,0:0:0:0:\n192,192,108566,1,0,0:0:0:0:\n448,192,108566,1,0,0:0:0:0:\n320,192,108859,1,0,0:0:0:0:\n64,192,109151,1,0,0:0:0:0:\n192,192,109151,1,0,0:0:0:0:\n448,192,109444,1,0,0:0:0:0:\n192,192,109737,1,0,0:0:0:0:\n320,192,109883,1,0,0:0:0:0:\n64,192,110030,1,0,0:0:0:0:\n192,192,110322,1,0,0:0:0:0:\n448,192,110322,1,0,0:0:0:0:\n320,192,110615,1,0,0:0:0:0:\n448,192,110908,1,0,0:0:0:0:\n64,192,110908,1,0,0:0:0:0:\n192,192,111200,1,0,0:0:0:0:\n320,192,111493,1,0,0:0:0:0:\n64,192,111639,1,0,0:0:0:0:\n448,192,111786,1,0,0:0:0:0:\n192,192,111932,1,0,0:0:0:0:\n320,192,112078,128,0,112371:0:0:0:0:\n64,192,112371,1,0,0:0:0:0:\n448,192,112517,128,0,112810:0:0:0:0:\n192,192,112810,1,0,0:0:0:0:\n64,192,112956,1,0,0:0:0:0:\n320,192,113103,1,0,0:0:0:0:\n192,192,113249,128,0,113542:0:0:0:0:\n448,192,113542,1,0,0:0:0:0:\n320,192,113688,1,0,0:0:0:0:\n192,192,113834,1,0,0:0:0:0:\n64,192,113981,1,0,0:0:0:0:\n448,192,114127,128,0,114420:0:0:0:0:\n64,192,114420,1,0,0:0:0:0:\n192,192,114420,128,0,114712:0:0:0:0:\n448,192,114712,1,0,0:0:0:0:\n320,192,114712,128,0,115005:0:0:0:0:\n64,192,115005,1,0,0:0:0:0:\n448,192,115151,1,0,0:0:0:0:\n192,192,115298,128,0,115591:0:0:0:0:\n320,192,115591,1,0,0:0:0:0:\n64,192,115737,1,0,0:0:0:0:\n448,192,115883,128,0,116176:0:0:0:0:\n192,192,116176,1,0,0:0:0:0:\n320,192,116322,1,0,0:0:0:0:\n64,192,116469,128,0,116761:0:0:0:0:\n448,192,116761,1,0,0:0:0:0:\n192,192,116908,1,0,0:0:0:0:\n320,192,117054,128,0,117347:0:0:0:0:\n64,192,117347,1,0,0:0:0:0:\n448,192,117493,1,0,0:0:0:0:\n192,192,117639,128,0,117932:0:0:0:0:\n320,192,117932,1,0,0:0:0:0:\n64,192,118078,1,0,0:0:0:0:\n448,192,118225,128,0,118517:0:0:0:0:\n192,192,118517,128,0,118810:0:0:0:0:\n320,192,118810,128,0,119103:0:0:0:0:\n64,192,119103,128,0,119395:0:0:0:0:\n448,192,119395,1,0,0:0:0:0:\n192,192,119688,1,0,0:0:0:0:\n320,192,119981,1,0,0:0:0:0:\n448,192,120127,1,0,0:0:0:0:\n64,192,120273,1,0,0:0:0:0:\n192,192,120420,1,0,0:0:0:0:\n320,192,120566,1,0,0:0:0:0:\n320,192,120859,1,0,0:0:0:0:\n64,192,121151,1,0,0:0:0:0:\n64,192,121444,1,0,0:0:0:0:\n448,192,121737,1,0,0:0:0:0:\n192,192,122030,1,0,0:0:0:0:\n320,192,122176,1,0,0:0:0:0:\n448,192,122322,1,0,0:0:0:0:\n64,192,122615,1,0,0:0:0:0:\n320,192,122761,1,0,0:0:0:0:\n64,192,122908,128,0,123200:0:0:0:0:\n192,192,123200,128,0,123493:0:0:0:0:\n320,192,123493,128,0,123786:0:0:0:0:\n448,192,123786,128,0,124078:0:0:0:0:\n192,192,124078,1,0,0:0:0:0:\n320,192,124225,1,0,0:0:0:0:\n64,192,124371,1,0,0:0:0:0:\n192,192,124517,1,0,0:0:0:0:\n448,192,124664,1,0,0:0:0:0:\n320,192,124956,1,0,0:0:0:0:\n192,192,125249,1,0,0:0:0:0:\n448,192,125542,1,0,0:0:0:0:\n64,192,125688,1,0,0:0:0:0:\n192,192,125834,128,0,126127:0:0:0:0:\n320,192,126127,128,0,126420:0:0:0:0:\n448,192,126420,1,0,0:0:0:0:\n64,192,126566,1,0,0:0:0:0:\n192,192,126712,1,0,0:0:0:0:\n320,192,126859,1,0,0:0:0:0:\n448,192,127005,1,0,0:0:0:0:\n320,192,127298,1,0,0:0:0:0:\n192,192,127444,1,0,0:0:0:0:\n64,192,127591,128,0,128176:0:0:0:0:\n448,192,128176,128,0,128761:0:0:0:0:\n320,192,128761,1,0,0:0:0:0:\n192,192,128908,1,0,0:0:0:0:\n64,192,129054,128,0,129347:0:0:0:0:\n448,192,129347,1,0,0:0:0:0:\n320,192,129493,1,0,0:0:0:0:\n192,192,129639,128,0,129932:0:0:0:0:\n64,192,129932,1,0,0:0:0:0:\n448,192,130078,1,0,0:0:0:0:\n320,192,130225,128,0,130517:0:0:0:0:\n192,192,130517,1,0,0:0:0:0:\n64,192,130664,1,0,0:0:0:0:\n320,192,130810,1,0,0:0:0:0:\n192,192,130956,1,0,0:0:0:0:\n448,192,131103,128,0,135786:0:0:0:0:\n";
class FS {
public:
    IVector<IStringType> list() {
        IVector<IStringType> result;
        result.push_back("/1.osu");
        return result;
    }

    IStringType get(IStringType filename) {
        if(filename == "/1.osu") return OSUGAMEFILE1;
        return "";
    }
};

FS fsapi;

// MARK: - Lib: Animation

class Keyframes
{
public:
    unsigned long long startTime;

    Keyframes()
    {
        startTime = emscripten_get_now();
    }

    int deltaTime()
    {
        return emscripten_get_now() - startTime;
    }

    void reset()
    {
        startTime = emscripten_get_now();
    }
};

class DeltaTime
{
public:
    unsigned long long lastTime;

    DeltaTime()
    {
        lastTime = emscripten_get_now();
    }

    int deltaTime()
    {
        unsigned long long currentTime = emscripten_get_now();
        int deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        return deltaTime;
    }
};

class Timer
{
public:
    unsigned long long startTime;

    Timer()
    {
        startTime = emscripten_get_now();
    }

    void reset()
    {
        startTime = emscripten_get_now();
    }

    int deltaTime()
    {
        return emscripten_get_now() - startTime;
    }
};

class AnimatedData
{
public:
    float now = 0;
    float target = 0;
    float acc = 0.0;

    float divider = 40.0;

    DeltaTime deltaTime;

    float current()
    {
        return now;
    }

    void setTarget(float newTarget)
    {
        this->target = newTarget;
        acc = (newTarget - now) / divider;
    }

    bool update()
    {
#ifdef __DO_NOT_ANIMATE__
        if(now != target) {
            now = target;
            return true;
        }
        return false;
#else
        float deltaTime = (float)this->deltaTime.deltaTime();
        if (now < target)
        {
            now += acc * deltaTime;
            if (now > target)
            {
                now = target;
            }
            return true;
        }
        if (now > target)
        {
            now += acc * deltaTime;
            if (now < target)
            {
                now = target;
            }
            return true;
        }
        return false;
#endif
    }

    void set(int nTarget)
    {
        now = nTarget;
        target = nTarget;
    }

    AnimatedData(float now)
    {
        this->now = now;
        this->target = now;
    }
    AnimatedData()
    {
        this->now = 0;
        this->target = 0;
    }
    AnimatedData(float now, float divider)
    {
        this->now = now;
        this->target = now;
        this->divider = divider;
    }
};

class RepeatedButton
{
public:
    bool lastButton = false;
    bool buttonPressed = false;
    bool repeat = false;

    Timer timer;
    Timer repeatTimer;

    bool get(bool buttonState)
    {
        if (!buttonPressed && buttonState)
        {
            buttonPressed = true;
            timer.reset();
            return true;
        }
        if (buttonPressed && !buttonState)
        {
            buttonPressed = false;
            repeat = false;
            return false;
        }
        if (buttonPressed && buttonState)
        {
            if (timer.deltaTime() < 200 && !repeat)
            {
                return false;
            }
            if (timer.deltaTime() > 200 && !repeat)
            {
                repeat = true;
                repeatTimer.reset();
                return true;
            }
            if (repeat && repeatTimer.deltaTime() > 75)
            {
                repeatTimer.reset();
                return true;
            }
        }
        return false;
    }
};

// MARK: - Lib: OSU FILE

class OSUNote
{
public:
    int col;
    int time;
    int type;
    int endtime;
};

class OSUNote_wihtoutcol
{
public:
    int time;
    int type;
    int endtime;

    bool isHoldNote() {
        return endtime;
    }
};

enum OSUFileReadingState
{
    FormatVersion,
    AnyHitObjectORMETA,
    HitObjects,
    HitObjects_X,
    HitObjects_Y,
    HitObjects_Time,
    HitObjects_Type,
    HitObjects_HitSound,
    HitObjects_EndTime,
    HitObjects_HitSample,
    HitObjects_NEWLINE,
};

class OSUFile
{
private:
    OSUFileReadingState rstate;
    bool load_line(IStringType line)
    {
        // printf("Line: %s\n", line.c_str());
        if (rstate == OSUFileReadingState::FormatVersion)
        {
            // check starts with "osu file format"
            IStringType format = "osu file format v";
            for (int i = 0; i < format.length(); i++)
            {
                if (line[i] != format[i])
                {
                    printf("Format error\n");
                    return false;
                }
            }
            rstate = OSUFileReadingState::AnyHitObjectORMETA;
            // printf("Format version: %s\n", line.substr(format.length()).c_str());

            return true;
        }
        if (rstate == OSUFileReadingState::AnyHitObjectORMETA)
        {
            IStringType format1 = "Title:";
            IStringType format2 = "Version:";
            IStringType format3 = "[HitObjects]";
            IStringType format4 = "BeatmapSetID:";
            IStringType format5 = "OverallDifficulty:";
            IStringType format6 = "Artist:";

            bool match1 = true;
            for (int i = 0; i < format1.length(); i++)
            {
                if (line[i] != format1[i])
                {
                    match1 = false;
                    break;
                }
            }

            if (match1)
            {
                title = line.substring(format1.length());
                return true;
            }

            bool match2 = true;
            for (int i = 0; i < format2.length(); i++)
            {
                if (line[i] != format2[i])
                {
                    match2 = false;
                    break;
                }
            }
            if (match2)
            {
                diffName = line.substring(format2.length());
                // printf("Diff name: %s\n", diffName.c_str());
                return true;
            }

            bool match3 = true;
            for (int i = 0; i < format3.length(); i++)
            {
                if (line[i] != format3[i])
                {
                    match3 = false;
                    break;
                }
            }

            if (match3)
            {
                rstate = OSUFileReadingState::HitObjects;
                // printf("HitObjects parse started\n");
                return true;
            }

            bool match4 = true;
            for (int i = 0; i < format4.length(); i++)
            {
                if (line[i] != format4[i])
                {
                    match4 = false;
                    break;
                }
            }
            if (match4)
            {
                beatmapSetID = line.substring(format4.length());
                // printf("BeatmapSetID: %s\n", beatmapSetID.c_str());
                return true;
            }

            bool match5 = true;
            for (int i = 0; i < format5.length(); i++)
            {
                if (line[i] != format5[i])
                {
                    match5 = false;
                    break;
                }
            }
            if (match5)
            {
                OverallDifficulty = line.substring(format5.length());
                // printf("OverallDifficulty: %s\n", OverallDifficulty.c_str());
                return true;
            }

            bool match6 = true;
            for (int i = 0; i < format6.length(); i++)
            {
                if (line[i] != format6[i])
                {
                    match6 = false;
                    break;
                }
            }
            if (match6)
            {
                Artist = line.substring(format6.length());
                // printf("Artist: %s\n", Artist.c_str());
                return true;
            }

            return true;
        }

        if (rstate == OSUFileReadingState::HitObjects)
        {
            OSUNote note;
            int state = OSUFileReadingState::HitObjects_X;
            IStringType buffer = "";
            for (int i = 0; i < line.length(); i++)
            {
                char c = line[i];
                if (state == OSUFileReadingState::HitObjects_X)
                {
                    if (c == ',')
                    {
                        int colData = string2int(buffer);
                        note.col = (int)(colData * 4 / 512);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_Y;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_Y)
                {
                    if (c == ',')
                    {
                        state = OSUFileReadingState::HitObjects_Time;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_Time)
                {
                    if (c == ',')
                    {
                        note.time = string2int(buffer);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_Type;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_Type)
                {
                    if (c == ',')
                    {
                        note.type = string2int(buffer);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_HitSound;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_HitSound)
                {
                    if (c == ',')
                    {
                        state = OSUFileReadingState::HitObjects_EndTime;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_EndTime)
                {
                    if (c == ':')
                    {
                        note.endtime = string2int(buffer);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_NEWLINE;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
            }

            if (state == OSUFileReadingState::HitObjects_X)
            {
                printf("End of hit objects\n");
                state = OSUFileReadingState::AnyHitObjectORMETA;
            }
            else if (state != OSUFileReadingState::HitObjects_NEWLINE)
            {
                printf("Wrong note data\n");
                return false;
            }
            else
            {
                notes[note.col].push_back({
                    note.time,
                    note.type,
                    note.endtime,
                });
            }

            return true;
        }

        return true;
    }

public:
    IVector<OSUNote_wihtoutcol> notes[4];
    IStringType title;
    IStringType diffName;
    IStringType beatmapSetID;
    IStringType OverallDifficulty;
    IStringType Artist;

    // used for arduino
    IStringType filePath;

    bool load(IStringType fpath, bool loadOnlyMetadata = false)
    {
        filePath = fpath;
        return loadFrom(loadOnlyMetadata);
    }

    void unload() {
        notes[0].clear();
        notes[1].clear();
        notes[2].clear();
        notes[3].clear();
    }

    bool loadFrom(bool loadOnlyMetadata = false) {
        IStringType notedata = fsapi.get(filePath);

        DeltaTime startTime;
        startTime.deltaTime();

        notes[0].clear();
        notes[1].clear();
        notes[2].clear();
        notes[3].clear();

        rstate = OSUFileReadingState::FormatVersion;

        IStringType buffer = "";
        for (int i = 0; i < notedata.length(); i++)
        {
            char c = notedata[i];
            if (c == '\n')
            {
                bool res = load_line(buffer);
                if (!res)
                    return false;
                buffer = "";
            }
            else
            {
                buffer += c;
            }

            if (loadOnlyMetadata && rstate == OSUFileReadingState::HitObjects)
                break;
        }
        if (!(loadOnlyMetadata && rstate == OSUFileReadingState::HitObjects))
        {
            bool res = load_line(buffer);
            if (!res)
                return false;
        }

        // printf("Loaded notes of %lu %lu %lu %lu\n", notes[0].size(), notes[1].size(), notes[2].size(), notes[3].size());
        // printf("Loaded in %d ms\n", startTime.deltaTime());

        return true;
    }
};

// MARK: - Scenes define

enum Scene
{
    Intro,
    Title,
    Select,
    Ingame,
    Result,
    Options,
    Download,
};

Scene currentScene = Scene::Intro;

// MARK: - Scene: Intro

enum IntroSceneKeyframe
{
    INTRONone,
    Wel,
    Come,
    To,
    OSU,
    TrianglesStarted,
    TrianglesEnded,
};

class IntroSceneTriangle
{
public:
    Keyframes keyframes;
    CnavasAPI *api;

    int centerX;
    int centerY;
    int size;
    bool filled;

    void setup()
    {
        centerX = randint(50, CANVAS_WIDTH - 50);
        centerY = randint((CANVAS_HEIGHT - 50) / 2, (CANVAS_HEIGHT + 50) / 2);
        size = randint(20, 40);
        filled = randint(0, 1) == 1;
        keyframes.reset();
    }

    void update(bool shouldReset = true)
    {
        // 40ms
        int deltaTime = keyframes.deltaTime();
        if (deltaTime < __INTRO_SCENE_MS_)
        {
            IColorType color = rgb_darken(255, 255, 255, (1.0 - (float)deltaTime / __INTRO_SCENE_MS_));
            if (filled)
            {
                api->drawTriangle(centerX, centerY - size, centerX - size, centerY + size, centerX + size, centerY + size, color);
            }
            else
            {
                api->drawTriangleOutline(centerX, centerY - size, centerX - size, centerY + size, centerX + size, centerY + size, color);
            }
        }
        if (deltaTime > __INTRO_SCENE_MS_ && shouldReset)
        {
            setup();
        }
    }
};

class IntroScene
{
public:
    Keyframes keyframes;
    CnavasAPI *api;
    IntroSceneKeyframe currentKeyframe = IntroSceneKeyframe::INTRONone;
    IntroSceneTriangle triangles[4];

    IntroScene(CnavasAPI *api)
    {
        this->api = api;
        for (int i = 0; i < 4; i++)
        {
            triangles[i].api = api;
            triangles[i].setup();
        }
    }

    // 344F Wel | 0s
    // 355F Come | 0.18s
    // 374F To // 0.5s
    // 387F OSU // 0.71s
    // 396F Triangles Started 0.86s
    // 419F Triangles Ended 1.25s
    void update()
    {
        int deltaTime = keyframes.deltaTime();
        if (deltaTime < 180)
        {
            if (currentKeyframe == IntroSceneKeyframe::INTRONone)
            {
                currentKeyframe = IntroSceneKeyframe::Wel;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Wel", COLOR_WHITE);
            }
            return;
        }
        if (deltaTime < 320)
        {
            if (currentKeyframe == IntroSceneKeyframe::Wel)
            {
                currentKeyframe = IntroSceneKeyframe::Come;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome", COLOR_WHITE);
            }
            return;
        }
        if (deltaTime < 500)
        {
            if (currentKeyframe == IntroSceneKeyframe::Come)
            {
                currentKeyframe = IntroSceneKeyframe::To;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to", COLOR_WHITE);
            }
            return;
        }
        if (deltaTime < 710)
        {
            if (currentKeyframe == IntroSceneKeyframe::To)
            {
                currentKeyframe = IntroSceneKeyframe::OSU;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to OSU!", COLOR_WHITE);
            }
            return;
        }
        if (deltaTime >= 860 && deltaTime < __INTRO_SCENE_END_MS_)
        {
            api->clear();
            api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to OSU!", COLOR_WHITE);
            int enableStep__ = (deltaTime - 860) / 40;
            int enableStep = enableStep__ > 3 ? 3 : enableStep__;
            for (int i = 0; i < enableStep; i++)
            {
                triangles[i].update(
                    deltaTime < __INTRO_SCENE_TRIANGLE_NOSPAWN_MS_);
            }
        }
    }
};

// MARK: - Component Key helper
class KeyHelper
{
public:
    CnavasAPI *api;
    KeyHelper(CnavasAPI *api)
    {
        this->api = api;
    }

    bool buttons[4] = {false, false, false, false};

    void setBtn(bool button0, bool button1, bool button2, bool button3)
    {
        buttons[0] = button0;
        buttons[1] = button1;
        buttons[2] = button2;
        buttons[3] = button3;
    }

    IStringType names[4] = {"S", "D", "L", ";"};

    void setBtnText(IStringType button0Text, IStringType button1Text, IStringType button2Text, IStringType button3Text)
    {
        names[0] = button0Text;
        names[1] = button1Text;
        names[2] = button2Text;
        names[3] = button3Text;
    }

    IStringType buttonKey[4] = {"S", "D", "L", ";"};

    void render()
    {
        api->drawRect(0, CANVAS_HEIGHT - 40, CANVAS_WIDTH, 40, rgb(10, 50, 50));
        int drawX = 5;
        int btnWidth = (CANVAS_WIDTH - 10 - 15) / 4;

        for (int i = 0; i < 4; i++)
        {
            if (buttons[i])
            {
                api->drawRect(drawX, CANVAS_HEIGHT - 35, 30, 30, COLOR_BLACK);
                api->drawText(drawX + 15, CANVAS_HEIGHT - 20, buttonKey[i], COLOR_WHITE);
                // TODO: implement on arduino
                api->drawTextTopLeft(drawX + 33, CANVAS_HEIGHT - 30, names[i], COLOR_WHITE);
            }
            drawX += btnWidth + 5;
        }
    }
};

// MARK: - Scene: Title

enum TitleSceneKeyframe
{
    SubmenuClose,
    SubmenuOpen,
};

enum TitleSceneCursor
{
    Title__Play,
    Title__Download,
    Title__Options,
};

class TitleScene
{
public:
    CnavasAPI *api;
    KeyHelper *keyHelper;
    TitleScene(CnavasAPI *api)
    {
        this->api = api;
        keyHelper = new KeyHelper(api);
    }

    TitleSceneKeyframe submenuOpenAnimationType = TitleSceneKeyframe::SubmenuClose;

    DeltaTime deltaTime;

    bool last0Button = false;
    bool last1Button = false;
    bool last2Button = false;
    bool last3Button = false;

    TitleSceneCursor cursorFocused = TitleSceneCursor::Title__Play;

    AnimatedData playButtonWidthHandler;
    AnimatedData downloadButtonWidthHandler;
    AnimatedData optionsButtonWidthHandler;
    AnimatedData osuLogoSizeHandler = AnimatedData(80);
    AnimatedData osuLogoXHandler = AnimatedData(CANVAS_WIDTH / 2);
    AnimatedData opacity = AnimatedData(0, 100);

    int buttonsWidth = (CANVAS_WIDTH - 20) / 4 * 3;
    int focusedWidth = ((CANVAS_WIDTH - 20) / 4 * 3) / 16 * 7;
    int unfocusedWidth = ((CANVAS_WIDTH - 20) / 4 * 3) / 16 * 4.5;

    // PLAY #64C
    // DOWNLOAD #EA0
    // OPTIONS #AC0

    void updateButtonTargetWidth()
    {
        playButtonWidthHandler.setTarget(
            cursorFocused == TitleSceneCursor::Title__Play ? focusedWidth : unfocusedWidth);
        downloadButtonWidthHandler.setTarget(
            cursorFocused == TitleSceneCursor::Title__Download ? focusedWidth : unfocusedWidth);
        optionsButtonWidthHandler.setTarget(
            cursorFocused == TitleSceneCursor::Title__Options ? focusedWidth : unfocusedWidth);
    }

    void render()
    {
        api->clear();

        // BUTTONS ANIMATION
        int baseX = (10 + CANVAS_WIDTH / 4);
        int buttonsY = CANVAS_HEIGHT / 2 - CANVAS_WIDTH / 4 / 2;
        int textY = buttonsY + CANVAS_WIDTH / 4 / 2;
        int logoMovX = osuLogoXHandler.current();
        int logoMovSz = osuLogoSizeHandler.current();
        int logoInnerSz = logoMovSz * 7 / 8;

        api->drawRect(
            baseX - logoMovSz,
            buttonsY,
            playButtonWidthHandler.current() + logoMovSz, CANVAS_WIDTH / 4,
            rgb_darken(0x66, 0x44, 0xCC, opacity.current()));
        IColorType darkenText = rgb_darken(255, 255, 255, opacity.current());
        api->drawButtonName(baseX + playButtonWidthHandler.current() / 2, textY, "PLAY", darkenText);
        api->drawRect(baseX + playButtonWidthHandler.current(), buttonsY, downloadButtonWidthHandler.current(), CANVAS_WIDTH / 4, rgb_darken(0xEE, 0xAA, 0x00, opacity.current()));
        api->drawButtonName(baseX + playButtonWidthHandler.current() + downloadButtonWidthHandler.current() / 2, textY, "DOWN", darkenText);
        api->drawRect(baseX + playButtonWidthHandler.current() + downloadButtonWidthHandler.current(), buttonsY, optionsButtonWidthHandler.current(), CANVAS_WIDTH / 4, rgb_darken(0xAA, 0xCC, 0x00, opacity.current()));
        api->drawButtonName(baseX + playButtonWidthHandler.current() + downloadButtonWidthHandler.current() + optionsButtonWidthHandler.current() / 2, textY, "OPT", darkenText);

        // OSU LOGO ANIMATION
        api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, logoMovSz, COLOR_WHITE);
        api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, logoInnerSz, COLOR_OSU);
        if (logoMovSz >= (CANVAS_WIDTH / 8 + 80) / 2)
            api->drawOsuLogoText(logoMovX, CANVAS_HEIGHT / 2, "OSU!", COLOR_WHITE);
        else
            api->drawOsuLogoTextSmaller(logoMovX, CANVAS_HEIGHT / 2, "OSU!", COLOR_WHITE);

        if (submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen)
        {
            keyHelper->setBtn(true, true, true, true);
            keyHelper->setBtnText("Left", "Right", "Sel", "Back");
        }
        else
        {
            keyHelper->setBtn(false, false, true, false);
            keyHelper->setBtnText("Pres", "any", "Play", "play");
        }
        keyHelper->render();
    }

    void update(bool forceRender)
    {
        bool updated = forceRender;
        updated |= playButtonWidthHandler.update();
        updated |= downloadButtonWidthHandler.update();
        updated |= optionsButtonWidthHandler.update();
        updated |= osuLogoSizeHandler.update();
        updated |= osuLogoXHandler.update();
        updated |= opacity.update();

        if (updated)
            render();

        if (last2Button != buttonPressed(2))
        {
            last2Button = buttonPressed(2);
            if (!last2Button)
            {
                if (submenuOpenAnimationType == TitleSceneKeyframe::SubmenuClose)
                {
                    submenuOpenAnimationType = TitleSceneKeyframe::SubmenuOpen;
                    cursorFocused = TitleSceneCursor::Title__Play;

                    playButtonWidthHandler.setTarget(focusedWidth);
                    downloadButtonWidthHandler.setTarget(unfocusedWidth);
                    optionsButtonWidthHandler.setTarget(unfocusedWidth);
                    osuLogoSizeHandler.setTarget(CANVAS_WIDTH / 8);
                    osuLogoXHandler.setTarget((10 + CANVAS_WIDTH / 8));
                    opacity.setTarget(1);

                    deltaTime.deltaTime();
                    printf("Scene:Title - SubmenuOpen Started\n");
                }
                else if (submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen)
                {
                    if (cursorFocused == TitleSceneCursor::Title__Play)
                    {
                        printf("Scene:Title - Play Button Pressed\n");
                        currentScene = Scene::Select;
                    }
                    if (cursorFocused == TitleSceneCursor::Title__Download)
                    {
                        printf("Scene:Title - Download Button Pressed\n");
                        currentScene = Scene::Download;
                    }
                    if (cursorFocused == TitleSceneCursor::Title__Options)
                    {
                        printf("Scene:Title - Options Button Pressed\n");
                        currentScene = Scene::Options;
                    }

                    cursorFocused = TitleSceneCursor::Title__Play;

                    playButtonWidthHandler.setTarget(focusedWidth);
                    downloadButtonWidthHandler.setTarget(unfocusedWidth);
                    optionsButtonWidthHandler.setTarget(unfocusedWidth);
                    osuLogoSizeHandler.setTarget(CANVAS_WIDTH / 8);
                    osuLogoXHandler.setTarget((10 + CANVAS_WIDTH / 8));
                    opacity.setTarget(1);
                }
            }
        }

        if (last3Button != buttonPressed(3))
        {
            last3Button = buttonPressed(3);
            if (!last3Button)
            {
                if (submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen)
                {
                    submenuOpenAnimationType = TitleSceneKeyframe::SubmenuClose;
                    cursorFocused = TitleSceneCursor::Title__Play;

                    playButtonWidthHandler.setTarget(focusedWidth);
                    downloadButtonWidthHandler.setTarget(unfocusedWidth);
                    optionsButtonWidthHandler.setTarget(unfocusedWidth);
                    osuLogoSizeHandler.setTarget(80);
                    osuLogoXHandler.setTarget(CANVAS_WIDTH / 2);
                    opacity.setTarget(0);

                    deltaTime.deltaTime();
                    printf("Scene:Title - SubmenuClose Started\n");
                }
            }
        }

        if (last0Button != buttonPressed(0))
        {
            last0Button = buttonPressed(0);
            if (!last0Button)
            {
                if (submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen)
                {
                    if (cursorFocused == TitleSceneCursor::Title__Play)
                    {
                        cursorFocused = TitleSceneCursor::Title__Options;
                    }
                    else if (cursorFocused == TitleSceneCursor::Title__Download)
                    {
                        cursorFocused = TitleSceneCursor::Title__Play;
                    }
                    else if (cursorFocused == TitleSceneCursor::Title__Options)
                    {
                        cursorFocused = TitleSceneCursor::Title__Download;
                    }

                    updateButtonTargetWidth();
                }
            }
        }
        if (last1Button != buttonPressed(1))
        {
            last1Button = buttonPressed(1);
            if (!last1Button)
            {
                if (submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen)
                {
                    if (cursorFocused == TitleSceneCursor::Title__Play)
                    {
                        cursorFocused = TitleSceneCursor::Title__Download;
                    }
                    else if (cursorFocused == TitleSceneCursor::Title__Download)
                    {
                        cursorFocused = TitleSceneCursor::Title__Options;
                    }
                    else if (cursorFocused == TitleSceneCursor::Title__Options)
                    {
                        cursorFocused = TitleSceneCursor::Title__Play;
                    }

                    updateButtonTargetWidth();
                }
            }
        }
    }
};

// MARK: - Scene+: Loading
class LoadingScene
{
public:
    CnavasAPI *api;
    int progress = 0;
    LoadingScene(CnavasAPI *api)
    {
        this->api = api;
    }

    void render()
    {
        api->clear();
        api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Loading...", COLOR_WHITE);
        api->drawRect(10, CANVAS_HEIGHT * 2 / 3, CANVAS_WIDTH - 20, 20, COLOR_WHITE);
        api->drawRect(10, CANVAS_HEIGHT * 2 / 3, (CANVAS_WIDTH - 20) * progress / 100, 20, rgb(0, 255, 0));

        // TODO: Change when arduino implementation started
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT * 2 / 3 + 12, int2string(progress), COLOR_BLACK);
    }

    int __lastProgress = 0;
    void update(bool forceRender)
    {
        bool updated = forceRender;
        if (progress != __lastProgress)
        {
            __lastProgress = progress;
            updated = true;
        }
        if (updated)
            render();
    }

    void setProgress(int progress)
    {
        this->progress = progress;
    }
};

// MARK: - Scene: Select

bool _GLOBAL_MAP_SD_MODIFIED_ = true;
IMap<int> _GLOBAL_MAPS_;
IVector<IVector<OSUFile>> _GLOBAL_MAPS_SD_;

#define MP(x, y) _GLOBAL_MAPS_SD_.data[x].data[y]
#define NMAP MP(requestedPlaymap.cursor, requestedPlaymap.subcursor)

class TwoCursor
{
public:
    int cursor = 0;
    int subcursor = 0;

    TwoCursor(int cursor, int subcursor)
    {
        this->cursor = cursor;
        this->subcursor = subcursor;
    }
};

TwoCursor requestedPlaymap = TwoCursor(0, 0);

class SelectScene
{
public:
    CnavasAPI *api;
    LoadingScene *loadingScene;
    KeyHelper *keyHelper;

    RepeatedButton button0;
    RepeatedButton button1;
    SelectScene(CnavasAPI *api)
    {
        this->api = api;
        loadingScene = new LoadingScene(api);
        keyHelper = new KeyHelper(api);
    }

    bool last2Button = false;
    bool last3Button = false;

    void loadOSUFile(IStringType filepath)
    {
        OSUFile of = OSUFile();
        of.load(filepath, true);
        if (!_GLOBAL_MAPS_.has(of.beatmapSetID))
        {
            _GLOBAL_MAPS_.insert(of.beatmapSetID, _GLOBAL_MAPS_.size());
            _GLOBAL_MAPS_SD_.push_back(IVector<OSUFile>());
        }

        _GLOBAL_MAPS_SD_.data[_GLOBAL_MAPS_.at(of.beatmapSetID)].push_back(of);
    }

    void syncnolus_sdmapLoading()
    {
        // TODO: implement on arduino
        loadingScene->setProgress(0);
        loadingScene->update(true);
        IVector<IStringType> files = fsapi.list();
        int totalFiles = files.size();
        for (int i = 0; i < totalFiles; i++)
        {
            IStringType file = files.data[i];
            
                loadOSUFile(file);
                loadingScene->setProgress((i + 1) * 100 / totalFiles);
                loadingScene->update(false);
        }

        printf("Loaded %lu map sets\n", _GLOBAL_MAPS_.size());
        for (int i = 0; i < _GLOBAL_MAPS_SD_.size(); i++)
        {
            printf("Mapset %d has %lu maps\n", i, _GLOBAL_MAPS_SD_.at(i).size());
        }
    }

    int cursor = 0;
    int mapsetCursor = 0;
    AnimatedData yOffset = AnimatedData(0, 50);
    AnimatedData opacity = AnimatedData(100, 50);

    void renderMap(int cursor, int subcursor, bool selected, int y)
    {
        if (selected)
        {
            api->drawRect(10 - 3, y - 3, CANVAS_WIDTH - 20 + 6, __SELECT_MAP_HEIGHT__ + 6, COLOR_OSU);
            api->drawRect(10, y, CANVAS_WIDTH - 20, __SELECT_MAP_HEIGHT__, rgb_darken(255, 255, 255, opacity.current() / 100));
        }
        else
            api->drawRect(10, y, CANVAS_WIDTH - 20, __SELECT_MAP_HEIGHT__, rgb_darken(255, 255, 255, 0.3));
        api->drawTextTopLeft(15, y + 15, int2string(cursor + 1) + ". " + _GLOBAL_MAPS_SD_.at(cursor).at(subcursor).title + " - " + _GLOBAL_MAPS_SD_.at(cursor).at(subcursor).Artist, COLOR_BLACK);
        api->drawTextTopLeftSmaller(25, y + 33, char2string(subcursor + 'a') + ". (" + _GLOBAL_MAPS_SD_.at(cursor).at(subcursor).OverallDifficulty + ") " + _GLOBAL_MAPS_SD_.at(cursor).at(subcursor).diffName, COLOR_BLACK);
    }

    void render()
    {
        api->clear();
        if (_GLOBAL_MAPS_.size() == 0)
        {
            api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2 - 20, "Select Menu", COLOR_WHITE);
            api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2 + 20, "No mapset found", COLOR_WHITE);
            return;
        }

        int renderY = __SELECT_MAP_START_Y__ + yOffset.current();
        for (int i = __SELECT_PREV_MAPS__; i >= 1; i--)
        {
            TwoCursor cursor = getCursorBasedMap(-i);
            renderMap(cursor.cursor, cursor.subcursor, false, renderY);
            renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;
        }

        renderMap(cursor, mapsetCursor, true, renderY);
        renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;

        for (int i = 1; i <= __SELECT_NEXT_MAPS__; i++)
        {
            TwoCursor cursor = getCursorBasedMap(i);
            renderMap(cursor.cursor, cursor.subcursor, false, renderY);
            renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;
        }

        keyHelper->setBtn(true, true, true, true);
        keyHelper->setBtnText("Prev", "Next", "Play", "Menu");
        keyHelper->render();
    }

    TwoCursor getCursorBasedMap(int offset)
    {
        int globalCursor = cursor;
        int subCursor = mapsetCursor;
        while (offset > 0)
        {
            subCursor++;
            if (subCursor >= _GLOBAL_MAPS_SD_.at(globalCursor).size())
            {
                globalCursor++;
                subCursor = 0;
            }
            offset--;

            if (globalCursor >= _GLOBAL_MAPS_SD_.size())
            {
                globalCursor = 0;
                subCursor = 0;
            }
        }

        while (offset < 0)
        {
            subCursor--;
            if (subCursor < 0)
            {
                globalCursor--;
                if (globalCursor < 0)
                {
                    globalCursor = _GLOBAL_MAPS_SD_.size() - 1;
                }

                subCursor = _GLOBAL_MAPS_SD_.at(globalCursor).size() - 1;
            }
            offset++;
        }

        return TwoCursor(globalCursor, subCursor);
    }

    TwoCursor lastCursor = TwoCursor(0, 0);

    void update(bool forceRender)
    {
        if (forceRender)
        {
            cursor = 0;
            mapsetCursor = 0;
            lastCursor = TwoCursor(0, 0);
        }

        if (_GLOBAL_MAP_SD_MODIFIED_)
        {
            loadingScene->setProgress(0);
            loadingScene->update(true);
            syncnolus_sdmapLoading();
            _GLOBAL_MAP_SD_MODIFIED_ = false;
        }

        bool updated = forceRender;
        updated |= lastCursor.cursor != cursor;
        updated |= lastCursor.subcursor != mapsetCursor;
        updated |= yOffset.update();
        updated |= opacity.update();
        if (updated)
        {
            lastCursor = TwoCursor(cursor, mapsetCursor);
            render();
        }

        if (button0.get(buttonPressed(0)))
        {
            TwoCursor cursor = getCursorBasedMap(-1);
            this->cursor = cursor.cursor;
            this->mapsetCursor = cursor.subcursor;
            yOffset.set(10);
            yOffset.setTarget(0);
            opacity.set(60);
            opacity.setTarget(100);
        }
        if (button1.get(buttonPressed(1)))
        {
            TwoCursor cursor = getCursorBasedMap(1);
            this->cursor = cursor.cursor;
            this->mapsetCursor = cursor.subcursor;
            yOffset.set(-10);
            yOffset.setTarget(0);
            opacity.set(60);
            opacity.setTarget(100);
        }
        if (last3Button != buttonPressed(3))
        {
            last3Button = buttonPressed(3);
            if (!last3Button)
            {
                currentScene = Scene::Title;
                return;
            }
        }
        if(last2Button != buttonPressed(2))
        {
            last2Button = buttonPressed(2);
            if (!last2Button)
            {
                requestedPlaymap = TwoCursor(cursor, mapsetCursor);
                currentScene = Scene::Ingame;
                return;
            }
        }
    }
};

// MARK: - Scene: Ingame

enum IngameButtonType {
    RELEASED,
    JUST_PRESSED,
    HOLDING,
};

class IngameButton
{
public:
    bool lastButton = false;

    IngameButtonType get(bool buttonState)
    {
        if(!buttonState) {
            lastButton = false;
            return IngameButtonType::RELEASED;
        }

        if(!lastButton) {
            lastButton = true;
            return IngameButtonType::JUST_PRESSED;
        }

        return IngameButtonType::HOLDING;
    }
};


float health = 100;
int combo = 0;

class AccuracyViewer {
public:
    CnavasAPI *api;
    int targetAcc = 0;
    bool requested = false;
    Timer timer;
    AnimatedData accY = AnimatedData(0, 100);
    
    AccuracyViewer(CnavasAPI *api)
    {
        this->api = api;
    }

    void update() {
        accY.update();
        if(requested) {
            if(timer.deltaTime() > 250) {
                requested = false;
                targetAcc = 0;
                return;
            }

            if(targetAcc == 0) {
                api->drawText(CANVAS_WIDTH / 2, accY.current() + _ACCURACY_BASE_Y_, "BREAK", COLOR_WHITE);
            }
            else api->drawText(CANVAS_WIDTH / 2, accY.current() + _ACCURACY_BASE_Y_, "MAX " + int2string(targetAcc) + "%", COLOR_WHITE);
        }
    }

    void setTargetAcc(int acc) {
        targetAcc = acc;
        requested = true;
        timer.reset();
        accY.set(10);
        accY.setTarget(0);
    }
};

AccuracyViewer* accuracyViewer;

class IngameLineHandler {
public:
    CnavasAPI *api;
    IngameButton igbtn;
    int keyIndex = 0;
    int keyX = 0;

    // which menas notes have to be rendered from index of `noteRenderIndex`
    int noteRenderIndex = 0;

    IngameLineHandler(CnavasAPI *api, int keyIndex)
    {
        this->api = api;
        this->keyIndex = keyIndex;
        this->keyX = CANVAS_WIDTH / 4 * keyIndex;
    }

    void initlize()
    {
        noteRenderIndex = 0;
    }

    bool handlingLongNote;
    int longNoteIndex = 0;

    int calculateAcc(int deltaTime) {
        int diff = abs(deltaTime);
        if(diff < _JUDEGE_MAX_100_) return 100;
        if(diff < _JUDGE_MAX_90_) return 90;
        if(diff < _JUDGE_MAX_80_) return 80;
        if(diff < _JUDGE_MAX_70_) return 70;
        if(diff < _JUDGE_MAX_60_) return 60;
        if(diff < _JUDGE_MAX_50_) return 50;
        if(diff < _JUDGE_MAX_40_) return 40;
        if(diff < _JUDGE_MAX_30_) return 30;
        if(diff < _JUDGE_MAX_20_) return 20;
        if(diff < _JUDGE_MAX_10_) return 10;
        return 0;
    }

    void render(int time)
    {
        if(handlingLongNote) {
            api->drawRect(keyX, CANVAS_HEIGHT - 40, CANVAS_WIDTH / 4, 40, COLOR_OSU);
        }
        for(int i = noteRenderIndex; i < NMAP.notes[keyIndex].size(); i++)
        {
            int renderY = _NOTE_END_Y_ - (NMAP.notes[keyIndex].at(i).time - time) * _NOTE_Y_SPEED_;

            float renderH = 10;
            if(NMAP.notes[keyIndex].at(i).isHoldNote()) {
                float holdHieght = (NMAP.notes[keyIndex].at(i).endtime - NMAP.notes[keyIndex].at(i).time)  * _NOTE_Y_SPEED_;
                renderH = holdHieght < 10 ? 10 : holdHieght;

                renderH = holdHieght;
                renderY -= holdHieght;
                if(renderY < 0) {
                    renderH += renderY;
                    renderY = 0;
                }
            }


            if(renderY < 0) {
                break;
            }
            if(renderY > CANVAS_HEIGHT) {
                if(NMAP.notes[keyIndex].at(i).isHoldNote()) {
                    if(!handlingLongNote) {
                        accuracyViewer->setTargetAcc(0);
                        health -= 10;
                        combo = 0;
                        noteRenderIndex = i + 1;
                    }
                } else {
                    health -= 10;
                    combo = 0;
                    printf("Missed note\n");
                    noteRenderIndex = i + 1;
                    accuracyViewer->setTargetAcc(0);
                }
                break;
            }
            if(renderY + renderH > CANVAS_HEIGHT) {
                renderH = CANVAS_HEIGHT - renderY;
            }

            api->drawRect(keyX, renderY, CANVAS_WIDTH / 4, renderH, NOTE_COLORS[keyIndex]);
        }

        IngameButtonType buttonState = igbtn.get(buttonPressed(keyIndex));
        if(handlingLongNote) {
            printf("LNT %d\n",NMAP.notes[keyIndex].at(longNoteIndex).endtime - time);
            printf("EDT %d\n", NMAP.notes[keyIndex].at(longNoteIndex).endtime);
            printf("T %d\n", time);
            if(buttonState == IngameButtonType::RELEASED) {
                int acc = calculateAcc(NMAP.notes[keyIndex].at(longNoteIndex).endtime - time);
                if(acc == 0) {
                    accuracyViewer->setTargetAcc(0);
                    health -= 10;
                    noteRenderIndex = largest(noteRenderIndex, longNoteIndex + 1);
                    combo = 0;
                    printf("Missed long note end\n");
                } else {
                    accuracyViewer->setTargetAcc(100);
                    noteRenderIndex = largest(noteRenderIndex, longNoteIndex + 1);
                }
                handlingLongNote = false;
                printf("Handling long note end === RELEASED\n");
            }
            printf("Handling long note\n");
            return;
        }

        if(buttonState == IngameButtonType::JUST_PRESSED) {
            // get the most closest note
            int closestNoteIndex = noteRenderIndex;
            int closestNoteTime = 1234567890;
            for(int i = noteRenderIndex; i < NMAP.notes[keyIndex].size(); i++) {
                int noteTime = NMAP.notes[keyIndex].at(i).time - time;
                if((noteTime) < (closestNoteTime)) {
                    closestNoteTime = noteTime;
                    closestNoteIndex = i;
                }
            }

            if(closestNoteTime > _JUDGE_MAX_10_ + 100) return;
            
            if(NMAP.notes[keyIndex].at(closestNoteIndex).isHoldNote()) {
                int acc = calculateAcc(closestNoteTime);
                accuracyViewer->setTargetAcc(acc);
                health += _HEALTH_BY_SCORE_[acc / 10];
                if(acc == 0) {
                    // miss
                    noteRenderIndex = largest(noteRenderIndex, closestNoteIndex + 1);
                    combo = 0;
                    printf("Missed long note start\n");
                } else {
                    handlingLongNote = true;
                    longNoteIndex = closestNoteIndex;
                    combo++;
                }
            } else {
                int acc = calculateAcc(closestNoteTime);
                accuracyViewer->setTargetAcc(acc);
                health += _HEALTH_BY_SCORE_[acc / 10];
                if(acc == 0) {
                    // miss
                    noteRenderIndex = largest(noteRenderIndex, closestNoteIndex + 1);
                    combo = 0;
                    printf("Missed note\n");
                } else {
                    // hit
                    noteRenderIndex = largest(noteRenderIndex, closestNoteIndex + 1);
                    combo++;
                }
            }
        }
    }
};

class IngameScene
{
public:
    CnavasAPI *api;
    Timer timer;
    IngameLineHandler *lineHandlers[4];
    LoadingScene *loadingScene;
    AnimatedData comboY = AnimatedData(0, 50);
    IngameScene(CnavasAPI *api)
    {
        this->api = api;
        for (int i = 0; i < 4; i++)
        {
            lineHandlers[i] = new IngameLineHandler(api, i);
        }
        loadingScene = new LoadingScene(api);
        accuracyViewer = new AccuracyViewer(api);
    }

    DeltaTime deltaTime;

    int lastCombo = 0;

    void render()
    {
        comboY.update();

        api->clear();
        accuracyViewer->update();
        for (int i = 0; i < 4; i++)
        {
            lineHandlers[i]->render(timer.deltaTime());
        }

        // 판정선 그리기
        api->drawRect(0, _NOTE_END_Y_, CANVAS_WIDTH, 2, COLOR_WHITE);

        if(health > 100) health = 100;
        // 체력선 그리기
        api->drawTextTopLeft(5, 5, "Health: " + int2string((int)health), COLOR_WHITE);

        if(health <= 0) {
            currentScene = Scene::Result;
        }

        if(combo != lastCombo) {
            lastCombo = combo;
            comboY.set(10);
            comboY.setTarget(0);
        }

        // combo
        if(combo > 0) {
            api->drawOsuLogoTextSmaller(CANVAS_WIDTH / 2, _COMBO_BASE_Y_ - 26, "COMBO", COLOR_WHITE);
            api->drawOsuLogoText(CANVAS_WIDTH / 2, _COMBO_BASE_Y_ + comboY.current(), int2string(combo), COLOR_WHITE);
        }
    }

    TwoCursor lastLoaded = TwoCursor(1234567890, 0);

    void syncnolus_ingameLoading()
    {
        if (lastLoaded.cursor == requestedPlaymap.cursor && lastLoaded.subcursor == requestedPlaymap.subcursor) return;

        loadingScene->setProgress(0);
        loadingScene->update(true);

        // unload last loaded
        if (lastLoaded.cursor < _GLOBAL_MAPS_SD_.size())
        {
            _GLOBAL_MAPS_SD_.data[lastLoaded.cursor].data[lastLoaded.subcursor].unload();
            printf("Unloaded map %s\n", _GLOBAL_MAPS_SD_.at(lastLoaded.cursor).at(lastLoaded.subcursor).title.c_str());
        }

        loadingScene->setProgress(10);
        loadingScene->update(true);

        lastLoaded = requestedPlaymap;
        printf("Loading map %s\n", _GLOBAL_MAPS_SD_.data[lastLoaded.cursor].data[lastLoaded.subcursor].title.c_str());
        _GLOBAL_MAPS_SD_.data[lastLoaded.cursor].data[lastLoaded.subcursor].loadFrom();

        loadingScene->setProgress(100);
        loadingScene->update(true);
    }

    void initlize()
    {
        timer.reset();
        for (int i = 0; i < 4; i++)
        {
            lineHandlers[i]->initlize();
        }
        health = 100;
        combo = 0;
        deltaTime.deltaTime();
    }

    void update(bool forceRender)
    {
        syncnolus_ingameLoading();
        if(forceRender) initlize();

        // This is in game so we need to render it every frame
        render();
    }
};

// MARK: - Scene: Result
class ResultScene
{
public:
    CnavasAPI *api;
    ResultScene(CnavasAPI *api)
    {
        this->api = api;
    }

    void render()
    {
        api->clear();
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Result", COLOR_WHITE);
    }

    void update(bool forceRender)
    {
        bool updated = forceRender;
        if (updated)
            render();
    }
};

// MARK: - Scene: Options
class OptionsScene
{
public:
    CnavasAPI *api;
    OptionsScene(CnavasAPI *api)
    {
        this->api = api;
    }

    bool last3Button = false;

    void render()
    {
        api->clear();
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Options", COLOR_WHITE);
    }

    void update(bool forceRender)
    {
        bool updated = forceRender;
        if (updated)
            render();

        if (last3Button != buttonPressed(3))
        {
            last3Button = buttonPressed(3);
            if (!last3Button)
            {
                currentScene = Scene::Title;
                return;
            }
        }
    }
};

// MARK: - Scene: Download
class DownloadScene
{
public:
    CnavasAPI *api;
    DownloadScene(CnavasAPI *api)
    {
        this->api = api;
    }

    bool last3Button = false;

    void render()
    {
        api->clear();
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Downloading...", COLOR_WHITE);
    }

    void update(bool forceRender)
    {
        bool updated = forceRender;
        if (updated)
            render();

        if (last3Button != buttonPressed(3))
        {
            last3Button = buttonPressed(3);
            if (!last3Button)
            {
                currentScene = Scene::Title;
                return;
            }
        }
    }
};

// MARK: - Main Data Structures

CnavasAPI api;
IntroScene introScene(&api);
TitleScene titleScene(&api);
SelectScene selectScene(&api);
IngameScene ingameScene(&api);
ResultScene resultScene(&api);
OptionsScene optionsScene(&api);
DownloadScene downloadScene(&api);

Scene lastRendered = Scene::Intro;

void updateSubcall() {
    bool forceRender = lastRendered != currentScene;
    lastRendered = currentScene;

    if (currentScene == Scene::Ingame)
    {
        ingameScene.update(forceRender);
        return;
    }
    if (currentScene == Scene::Select)
    {
        selectScene.update(forceRender);
        return;
    }
    if (currentScene == Scene::Result)
    {
        resultScene.update(forceRender);
        return;
    }
    if (currentScene == Scene::Options)
    {
        optionsScene.update(forceRender);
        return;
    }
    if (currentScene == Scene::Download)
    {
        downloadScene.update(forceRender);
        return;
    }

    if (currentScene == Scene::Intro)
    {
        introScene.update();
        if (introScene.keyframes.deltaTime() > __INTRO_SCENE_END_MS_)
            currentScene = Scene::Title;
        return;
    }
    if (currentScene == Scene::Title)
    {
        titleScene.update(forceRender);
        return;
    }
}

void update()
{
    updateSubcall();
}

// MARK: - Arduino Entry Point

void setup() {
  tft.begin();
  Serial.begin(115200);
}
void loop() {
  update();
  if(Serial.available() > 0) {
    const char c = Serial.read();
    if(c == 's') __buttonPressed__state__[0] = true;
    if(c == 'd') __buttonPressed__state__[1] = true;
    if(c == 'l') __buttonPressed__state__[2] = true;
    if(c == 'k') __buttonPressed__state__[3] = true;
    if(c == 'S') __buttonPressed__state__[0] = false;
    if(c == 'D') __buttonPressed__state__[1] = false;
    if(c == 'L') __buttonPressed__state__[2] = false;
    if(c == 'K') __buttonPressed__state__[3] = false;
    printf("Key state %d %d %d %d\n", __buttonPressed__state__[0], __buttonPressed__state__[1], __buttonPressed__state__[2], __buttonPressed__state__[3]);
  }
}