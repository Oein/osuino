#include <stdio.h>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <string.h>

// https://github.com/codewitch-honey-crisis/htcw_zip/blob/master/examples/demo/src/main.cpp

// MARK: - Constants

#define IColorType int
#define IStringType std::string
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

IStringType char2string(char chr) {
    return std::string(1, chr);
}

int largest(int a, int b)
{
    return a > b ? a : b;
}

// MARK: - Button API for Emscripten
bool __buttonPressed__state__[4] = {false, false, false, false};
bool buttonPressed(int button)
{
    return __buttonPressed__state__[button];
}

void update();

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
    update();
    return 0;
}

// MARK: - Canvas API for Emscripten

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

class FS {
public:
    IVector<IStringType> list() {
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

    IStringType get(IStringType filename) {
        if(filename == "/1.osu") return OSUGAMEFILE1;
        if(filename == "/2.osu") return OSUGAMEFILE2;
        if(filename == "/3.osu") return OSUGAMEFILE3;
        if(filename == "/4.osu") return OSUGAMEFILE4;
        if(filename == "/5.osu") return OSUGAMEFILE5;
        if(filename == "/6.osu") return OSUGAMEFILE6;
        if(filename == "/7.osu") return OSUGAMEFILE7;
        if(filename == "/8.osu") return OSUGAMEFILE8;
        if(filename == "/9.osu") return OSUGAMEFILE9;
        if(filename == "/10.osu") return OSUGAMEFILE10;
        return "";
    }
};

FS fs;

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
                title = line.substr(format1.length());
                // printf("Title: %s\n", title.c_str());
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
        IStringType notedata = fs.get(filePath);

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
        IVector<IStringType> files = fs.list();
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

float avgms = 0;

void update()
{
    Timer t;
    updateSubcall();
    if(t.deltaTime() == 0) return;
    avgms = (avgms + t.deltaTime()) / 2;
    printf("Update took %fms\n", avgms);
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
