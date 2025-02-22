#include <stdio.h>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <string.h>

// MARK: - Env
#define __DO_NOT_ANIMATE__ true
#define __NO_DIE__ true
#define IColorType int
#define IStringType std::string
#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 480

// MARK: - Constants

#define __INTRO_SCENE_MS_ 100
#define __INTRO_SCENE_TRIANGLE_NOSPAWN_MS_ 1450 - 50
#define __INTRO_SCENE_END_MS_ 1600

// 맵 크기
#define __SELECT_MAP_HEIGHT__ 50
// 맵 선택창 간격
#define __SELECT_MAP_GAP__ 10

// 정확도 Y 좌표
#define _ACCURACY_BASE_Y_ CANVAS_HEIGHT / 4 * 3 - 10
// 콤보 Y 좌표
#define _COMBO_BASE_Y_ CANVAS_HEIGHT / 2 - 100
// 롱노트 콤보 스텍 주기
#define _LONG_NOTE_COMBO_STACK_MS_ 250

const int __SELECT_MAP_COUNT__ = (CANVAS_HEIGHT - __SELECT_MAP_GAP__) / (__SELECT_MAP_HEIGHT__ + __SELECT_MAP_GAP__);
const int __SELECT_PREV_MAPS__ = (__SELECT_MAP_COUNT__ - 1) / 2;
const int __SELECT_NEXT_MAPS__ = (__SELECT_MAP_COUNT__ - __SELECT_PREV_MAPS__ - 1);
const int __SELECT_MAP_START_Y__ = 10;

float _NOTE_SPEED_ = 400.0;

#define _JUDGE_MAX_100_ 41
#define _JUDGE_MAX_90_ 54
#define _JUDGE_MAX_80_ 75
#define _JUDGE_MAX_70_ 96
#define _JUDGE_MAX_60_ 120
#define _JUDGE_MAX_50_ 129
#define _JUDGE_MAX_40_ 138
#define _JUDGE_MAX_30_ 147
#define _JUDGE_MAX_20_ 159
#define _JUDGE_MAX_10_ 177

float _NOTE_START_Y_ = 0.0;
float _NOTE_END_Y_ = CANVAS_HEIGHT - 80.0;
float _NOTE_Y_RANGE_ = _NOTE_END_Y_ - _NOTE_START_Y_;
float _NOTE_PIXEL_PER_MS_ = _NOTE_Y_RANGE_ / _NOTE_SPEED_;

// single note 높이
float _SINGLE_NOTE_HEIGHT_ = 20;

// late 노트 인식 범위
float _NOTE_LATE_ALLOWANCE_ = _JUDGE_MAX_50_;
// 노트를 얼마나 내려오기 전까지 무시할 것인가
float _NOTE_IGNORE_DELTA_ = _NOTE_Y_RANGE_ / 3 * 2 * _NOTE_PIXEL_PER_MS_;

#define _JUDGE_MAX_0_ CANVAS_HEIGHT / 2 * _NOTE_PIXEL_PER_MS_

float _HEALTH_BY_SCORE_[] = {
    -7,
    0.5,  // 10,
    0.71, // 20
    0.8,  // 30
    0.88, // 40
    0.95, // 50
    1,    // 60
    1.05, // 70
    1.15, // 80
    1.3,  // 90
    1.5,  // 100
};

// MARK: - Hardware specific API

IColorType rgb(int r, int g, int b)
{
    return (r << 16) | (g << 8) | b;
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
    return std::stoi(str);
}

IStringType int2string(int num)
{
    return std::to_string(num);
}

IStringType char2string(char chr)
{
    return std::string(1, chr);
}

int largest(int a, int b)
{
    return a > b ? a : b;
}

bool __buttonPressed__state__[4] = {false, false, false, false};
bool buttonPressed(int button)
{
    return __buttonPressed__state__[button];
}

void update();

void buttonUpdate() {
    EM_ASM_({
        // document.getElementById('k' + index).style.background = __buttonPressed__state__[index] ? 'red' : 'black';
        Module.k0 = document.getElementById("k0");
        Module.k1 = document.getElementById("k1");
        Module.k2 = document.getElementById("k2");
        Module.k3 = document.getElementById("k3");
        Module.k0.style.background = $0 ? "red" : "black";
        Module.k1.style.background = $1 ? "red" : "black";
        Module.k2.style.background = $2 ? "red" : "black";
        Module.k3.style.background = $3 ? "red" : "black";
    }, __buttonPressed__state__[0], __buttonPressed__state__[1], __buttonPressed__state__[2], __buttonPressed__state__[3]);
}

EM_BOOL keyCallbackPress(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if (keyEvent->key[0] == 's')
        __buttonPressed__state__[0] = true;
    if (keyEvent->key[0] == 'd')
        __buttonPressed__state__[1] = true;
    if (keyEvent->key[0] == 'l')
        __buttonPressed__state__[2] = true;
    if (keyEvent->key[0] == ';')
        __buttonPressed__state__[3] = true;
        buttonUpdate();
    update();
    return 0;
}

EM_BOOL keyCallbackRelease(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if (keyEvent->key[0] == 's')
        __buttonPressed__state__[0] = false;
    if (keyEvent->key[0] == 'd')
        __buttonPressed__state__[1] = false;
    if (keyEvent->key[0] == 'l')
        __buttonPressed__state__[2] = false;
    if (keyEvent->key[0] == ';')
        __buttonPressed__state__[3] = false;
    buttonUpdate();
    update();
    return 0;
}

class CnavasAPI
{
public:
    CnavasAPI()
    {
        EM_ASM(
            Module.canvas = document.getElementById('canvas');
            Module.canvas.width = 320;
            Module.canvas.height = 480;
            Module.canvas.style.backgroundColor = 'black';
            Module.ctx = Module.canvas.getContext('2d'););
    }

    void drawRect(int x, int y, int width, int height, IColorType color)
    {
        // printf("drawRect(%d, %d, %d, %d, %d)\n", x, y, width, height, color);
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($4 >> 16) + ',' + (($4 >> 8) & 0xFF) + ',' + ($4 & 0xFF) + ')';
            Module.ctx.fillRect($0, $1, $2, $3); }, x, y, width, height, color);
    }

    void drawCircle(int x, int y, int radius, IColorType color)
    {
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.arc($0, $1, $2, 0, 2 * Math.PI);
            Module.ctx.fill(); }, x, y, radius, color);
    }

    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($6 >> 16) + ',' + (($6 >> 8) & 0xFF) + ',' + ($6 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.moveTo($0, $1);
            Module.ctx.lineTo($2, $3);
            Module.ctx.lineTo($4, $5);
            Module.ctx.fill(); }, x1, y1, x2, y2, x3, y3, color);
    }

    void drawTriangleOutline(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        EM_ASM_({
            Module.ctx.strokeStyle = 'rgb(' + ($6 >> 16) + ',' + (($6 >> 8) & 0xFF) + ',' + ($6 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.moveTo($0, $1);
            Module.ctx.lineTo($2, $3);
            Module.ctx.lineTo($4, $5);
            Module.ctx.closePath();
            Module.ctx.stroke(); }, x1, y1, x2, y2, x3, y3, color);
    }

    void clear()
    {
        EM_ASM(
            Module.ctx.clearRect(0, 0, Module.canvas.width, Module.canvas.height););
    }

    void drawIntroText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawOsuLogoText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawOsuLogoTextSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawButtonName(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawTextTopLeft(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawTextTopLeftSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '10pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawComboNumber(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            // draw background with black
            Module.ctx.fillStyle = 'rgb(0, 0, 0)';
            Module.ctx.fillRect($1 - textWidth / 2 - 5, $2 - textHeight / 2 - 5, textWidth + 10, textHeight + 10);

            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawComboText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            // draw background with black
            Module.ctx.fillStyle = 'rgb(0, 0, 0)';
            Module.ctx.fillRect($1 - textWidth / 2 - 5, $2 - textHeight / 2 - 5, textWidth + 10, textHeight + 10);

            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
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
#include "./mem.cpp"
class FS
{
public:
    IVector<IStringType> list()
    {
        IVector<IStringType> result;
        result.push_back("/1.osu");
        result.push_back("/2.osu");
        result.push_back("/3.osu");
        result.push_back("/4.osu");
        result.push_back("/5.osu");
        result.push_back("/6.osu");
        result.push_back("/7.osu");
        result.push_back("/8.osu");
        result.push_back("/9.osu");
        result.push_back("/10.osu");
        return result;
    }

    IStringType get(IStringType filename)
    {
        if (filename == "/1.osu")
            return OSUGAMEFILE1;
        if (filename == "/2.osu")
            return OSUGAMEFILE2;
        if (filename == "/3.osu")
            return OSUGAMEFILE3;
        if (filename == "/4.osu")
            return OSUGAMEFILE4;
        if (filename == "/5.osu")
            return OSUGAMEFILE5;
        if (filename == "/6.osu")
            return OSUGAMEFILE6;
        if (filename == "/7.osu")
            return OSUGAMEFILE7;
        if (filename == "/8.osu")
            return OSUGAMEFILE8;
        if (filename == "/9.osu")
            return OSUGAMEFILE9;
        if (filename == "/10.osu")
            return OSUGAMEFILE10;
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
        if (now != target)
        {
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
    int endtime;
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
                title = line.substr(format1.length());
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
                diffName = line.substr(format2.length());
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
                beatmapSetID = line.substr(format4.length());
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
                OverallDifficulty = line.substr(format5.length());
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
                Artist = line.substr(format6.length());
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
                if(note.endtime && note.time > note.endtime) {
                    int oldTime = note.time;
                    note.time = note.endtime;
                    note.endtime = oldTime;
                }
                notes[note.col].push_back({
                    note.time,
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

    void unload()
    {
        notes[0].clear();
        notes[1].clear();
        notes[2].clear();
        notes[3].clear();
    }

    bool loadFrom(bool loadOnlyMetadata = false)
    {
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
                api->drawTextTopLeft(drawX + 33, CANVAS_HEIGHT - 20, names[i], COLOR_WHITE);
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
#define NOTE(key, index) NMAP.notes[key].data[index]

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
        if (last2Button != buttonPressed(2))
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

enum IngameButtonType
{
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
        if (!buttonState)
        {
            lastButton = false;
            return IngameButtonType::RELEASED;
        }

        if (!lastButton)
        {
            lastButton = true;
            return IngameButtonType::JUST_PRESSED;
        }

        return IngameButtonType::HOLDING;
    }
};

float health = 100;
int combo = 0;
bool shouldDrawPanjung = false;

class AccuracyViewer
{
public:
    CnavasAPI *api;
    int targetAcc = 0;
    bool requested = false;
    Timer timer;

    AccuracyViewer(CnavasAPI *api)
    {
        this->api = api;
    }

    void update()
    {
        if (requested)
        {
            if (timer.deltaTime() > 250)
            {
                requested = false;
                targetAcc = 0;
                return;
            }

            if (targetAcc == 0)
            {
                api->drawComboText(CANVAS_WIDTH / 2, _ACCURACY_BASE_Y_, "BREAK", COLOR_WHITE);
            }
            else
                api->drawComboText(CANVAS_WIDTH / 2, _ACCURACY_BASE_Y_, "MAX " + int2string(targetAcc) + "%", COLOR_WHITE);
        }
    }

    void setTargetAcc(int acc)
    {
        targetAcc = acc;
        requested = true;
        timer.reset();

        if(acc == 0) {
            combo = 0;
            health += _HEALTH_BY_SCORE_[0];
            return;
        }

        combo++;
        health += _HEALTH_BY_SCORE_[acc];
    }
};

AccuracyViewer *accuracyViewer;

class IngameLineHandler
{
public:
    CnavasAPI *api;
    IngameButton igbtn;
    int keyIndex = 0;
    int keyX = 0;

    int renderFrom = 0;

    int processFrom = 0;
    int processingLongNoteIndex = -1;

    Timer longNoteCombo;

    IngameLineHandler(CnavasAPI *api, int keyIndex)
    {
        this->api = api;
        this->keyIndex = keyIndex;
        this->keyX = CANVAS_WIDTH / 4 * keyIndex;
    }

    void initlize()
    {
        for(int i = 0; i < CANVAS_HEIGHT; i++)
            rendered[i] = false;
        renderFrom = 0;
        processFrom = 0;
        processingLongNoteIndex = -1;
    }

    int calculateAcc(int deltaTime)
    {
        int diff = abs(deltaTime);
        if (diff < _JUDGE_MAX_100_)
            return 100;
        if (diff < _JUDGE_MAX_90_)
            return 90;
        if (diff < _JUDGE_MAX_80_)
            return 80;
        if (diff < _JUDGE_MAX_70_)
            return 70;
        if (diff < _JUDGE_MAX_60_)
            return 60;
        if (diff < _JUDGE_MAX_50_)
            return 50;
        if (diff < _JUDGE_MAX_40_)
            return 40;
        if (diff < _JUDGE_MAX_30_)
            return 30;
        if (diff < _JUDGE_MAX_20_)
            return 20;
        if (diff < _JUDGE_MAX_10_)
            return 10;
        return 0;
    }

    bool rendered[CANVAS_HEIGHT] = {
        false,
    };

    void handleDraw(int from, int to, bool fill) {
        if(from < 0) return;
        if(to >= CANVAS_HEIGHT) return;

        if(fill) {
            api->drawRect(keyX, from, CANVAS_WIDTH / 4, to - from + 1, NOTE_COLORS[keyIndex]);
        } else {
            api->drawRect(keyX, from, CANVAS_WIDTH / 4, to - from + 1, COLOR_BLACK);
        }
    }

    void processAccu(int accu) {
        accuracyViewer->setTargetAcc(accu);
    }

    int getAccuFromTime(int delta) {
        int time = abs(delta);
        if(time <= _JUDGE_MAX_100_) return 100;
        if(time <= _JUDGE_MAX_90_) return 90;
        if(time <= _JUDGE_MAX_80_) return 80;
        if(time <= _JUDGE_MAX_70_) return 70;
        if(time <= _JUDGE_MAX_60_) return 60;
        if(time <= _JUDGE_MAX_50_) return 50;
        if(time <= _JUDGE_MAX_40_) return 40;
        if(time <= _JUDGE_MAX_30_) return 30;
        if(time <= _JUDGE_MAX_20_) return 20;
        if(time <= _JUDGE_MAX_10_) return 10;
        return 0;
    }

    void processLongNote(int time, IngameButtonType btn) {
        // 롱노트 처리 중인 상태임

        int noteDeleteTime = NOTE(keyIndex, processingLongNoteIndex).endtime + _NOTE_LATE_ALLOWANCE_;
        if(noteDeleteTime < time) {
            // 롱노트가 이미 지나감
            processAccu(0);
            processFrom = processingLongNoteIndex + 1;
            processingLongNoteIndex = -1;
            printf("Line %d: Long note missed\n", keyIndex);
            return;
        }

        int delta = NOTE(keyIndex, processingLongNoteIndex).endtime - time;
        if(btn == IngameButtonType::HOLDING) {
            // 롱노트를 계속 누르고 있음
            if(longNoteCombo.deltaTime() > _LONG_NOTE_COMBO_STACK_MS_) {
                longNoteCombo.reset();
                combo++;
            }
            return;
        }

        if(btn == IngameButtonType::RELEASED) {
            // 롱노트를 놓음
            processAccu(getAccuFromTime(delta));
            processFrom = processingLongNoteIndex + 1;
            processingLongNoteIndex = -1;
            return;
        }
    }

    void processKeypress(int time) {
        IngameButtonType btn = igbtn.get(buttonPressed(keyIndex));
        if(processingLongNoteIndex != -1) return processLongNote(time, btn);

        // 아직 아무것도 관여되어있지 않음.
        for(int i = processFrom; i < NMAP.notes[keyIndex].size(); i++) {
            bool isLongNote = NOTE(keyIndex, i).endtime > 0;
            if(NOTE(keyIndex, i).time > time + CANVAS_HEIGHT * _NOTE_PIXEL_PER_MS_) {
                // 아직 노트가 안나왔음
                break;
            }

            int noteDeleteTime = (isLongNote ? NOTE(keyIndex, i).endtime : NOTE(keyIndex, i).time) + _NOTE_LATE_ALLOWANCE_;
            if(noteDeleteTime < time) {
                // 노트가 이미 지나감
                processAccu(0);
                processFrom = i + 1;
                printf("Line %d: Note missed\n", keyIndex);
                break;
            }

            // 노트를 눌르지 않을경우 아무 처리도 하지 않음
            if(btn != IngameButtonType::JUST_PRESSED) continue;

            int delta = NOTE(keyIndex, i).time - time;
            if(delta > _NOTE_IGNORE_DELTA_) {
                // 노트가 아직 너무 먼 곳에 있음
                break;
            }
            int accu = getAccuFromTime(delta);
            if(isLongNote) {
                if(accu == 0) {
                    // 롱노트를 놓침
                    processAccu(0);
                    processFrom = i + 1;
                    break;
                }
                // 롱노트를 누름
                processAccu(accu);
                processingLongNoteIndex = i;
                longNoteCombo.reset();
                break;
            }

            // single note
            processAccu(accu);
            // 이번 틱에서 버튼 누름은 처리했기에 다음 노트까지 신경을 쓰지 않음
            processFrom = i + 1;
            break;
        }
    }

    void render(int time)
    {
        processKeypress(time);

        bool newRendered[CANVAS_HEIGHT] = {
            false,
        };

        for(int i = renderFrom;i < NMAP.notes[keyIndex].size(); i++) {
            bool isLongNote = NOTE(keyIndex, i).endtime > 0;

            // single note
            int noteY = _NOTE_END_Y_ - (NOTE(keyIndex, i).time - time) * _NOTE_PIXEL_PER_MS_;
            int noteH = _SINGLE_NOTE_HEIGHT_;

            if(isLongNote) {
                noteY = _NOTE_END_Y_ - (NOTE(keyIndex, i).endtime - time) * _NOTE_PIXEL_PER_MS_;
                noteH = (NOTE(keyIndex, i).endtime - NOTE(keyIndex, i).time) * _NOTE_PIXEL_PER_MS_;
            }

            if(noteY + noteH <= 0) {
                // 노트의 끝이 화면에 들어오지 않음
                break;
            }

            if(noteY < 0) {
                // 노트가 화면 위에 걸침
                noteH += noteY;
                noteY = 0;
            }

            if(noteY >= CANVAS_HEIGHT) {
                // 노트가 화면 밖에 있음
                renderFrom = i + 1;
                continue;
            }

            if(noteY + noteH >= CANVAS_HEIGHT) {
                // 노트가 화면 아래에 걸침
                noteH = CANVAS_HEIGHT - noteY;
            }

            assert(noteY >= 0 && noteY < CANVAS_HEIGHT);
            assert(noteH > 0 && noteY + noteH <= CANVAS_HEIGHT);

            for(int j = noteY; j < noteY + noteH && j < CANVAS_HEIGHT; j++) {
                newRendered[j] = true;
            }
        }

        int seqStart = -1;
        int seqEnd = -1;
        bool seqData = false;
        for(int i = 0; i < CANVAS_HEIGHT; i++) {
            if(newRendered[i] != rendered[i]) {
                rendered[i] = newRendered[i];
                if(seqStart == -1) {
                    seqData = newRendered[i];
                    seqStart = i;
                    seqEnd = i;
                    continue;
                }
                if(seqData != newRendered[i]) {
                    handleDraw(seqStart, seqEnd, seqData);
                    seqStart = i;
                    seqEnd = i;
                    seqData = newRendered[i];
                    continue;
                }
                seqEnd = i;
                
                continue;
            }

            if(seqStart != -1) {
                handleDraw(seqStart, seqEnd, seqData);
                seqStart = -1;
                seqEnd = -1;
                seqData = false;
            }
        }

        handleDraw(seqStart, seqEnd, seqData);
    }
};

class IngameScene
{
public:
    CnavasAPI *api;
    Timer timer;
    IngameLineHandler *lineHandlers[4];
    LoadingScene *loadingScene;
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

    void render()
    {
        int deltaTime = this->timer.deltaTime();
        for (int i = 0; i < 4; i++)
        {
            lineHandlers[i]->render(deltaTime);
        }

        // 판정선 그리기
        api->drawRect(0, _NOTE_END_Y_, CANVAS_WIDTH, 2, COLOR_WHITE);

        if (health > 100)
            health = 100;
        // 체력선 그리기
        // api->drawTextTopLeft(5, 5, "Health: " + int2string((int)health), COLOR_WHITE);

#ifndef __NO_DIE__
        if (health <= 0)
        {
            currentScene = Scene::Result;
        }
#endif

        // combo

        api->drawComboText(CANVAS_WIDTH / 2, _COMBO_BASE_Y_ - 26, "COMBO", COLOR_WHITE);
        api->drawComboNumber(CANVAS_WIDTH / 2, _COMBO_BASE_Y_, int2string(combo), COLOR_WHITE);

        accuracyViewer->update();
    }

    TwoCursor lastLoaded = TwoCursor(1234567890, 0);

    void syncnolus_ingameLoading()
    {
        if (lastLoaded.cursor == requestedPlaymap.cursor && lastLoaded.subcursor == requestedPlaymap.subcursor)
            return;

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
        api->clear();
        printf("Scene:Ingame - Initlized\n");
    }

    void update(bool forceRender)
    {
        syncnolus_ingameLoading();
        if (forceRender)
            initlize();

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
IntroScene *introScene = new IntroScene(&api);
TitleScene titleScene(&api);
SelectScene selectScene(&api);
IngameScene ingameScene(&api);
ResultScene resultScene(&api);
OptionsScene optionsScene(&api);
DownloadScene downloadScene(&api);

Scene lastRendered = Scene::Intro;

void updateSubcall()
{
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
        introScene->update();
        if (introScene->keyframes.deltaTime() > __INTRO_SCENE_END_MS_) {
            currentScene = Scene::Title;
            delete introScene;
        }
        return;
    }
    if (currentScene == Scene::Title)
    {
        titleScene.update(forceRender);
        return;
    }
}

float avgFPS = 0;
unsigned long lastTime = emscripten_get_now();

void update()
{
    avgFPS = 1000.0 / (emscripten_get_now() - lastTime);
    updateSubcall();
    lastTime = emscripten_get_now();

    api.drawComboText(40, 20, "FPS: " + int2string((int)avgFPS), COLOR_WHITE);
}

// MARK: - Emscripten Entry Point

int main()
{
    printf("============================\n");
    printf("OSUino started\n");
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackPress);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackRelease);
    emscripten_set_main_loop(update, 0, 1);
    return 0;
}
