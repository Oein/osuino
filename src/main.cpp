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

// MARK: - Button API for Emscripten
bool __buttonPressed__state__[4] = {false, false, false, false};
bool buttonPressed(int button)
{
    return __buttonPressed__state__[button];
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

    void drawIntroText(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255))
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawOsuLogoText(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255))
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawOsuLogoTextSmaller(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255))
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawButtonName(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255))
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawText(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255))
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
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

// MARK: - Lib: OSU FILE

#include "./mem.cpp"

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
            printf("Format version: %s\n", line.substr(format.length()).c_str());

            return true;
        }
        if (rstate == OSUFileReadingState::AnyHitObjectORMETA)
        {
            IStringType format1 = "Title:";
            IStringType format2 = "Version:";
            IStringType format3 = "[HitObjects]";
            IStringType format4 = "BeatmapSetID:";

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
                printf("Title: %s\n", title.c_str());
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
                printf("Diff name: %s\n", diffName.c_str());
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
                printf("HitObjects parse started\n");
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
                printf("BeatmapSetID: %s\n", beatmapSetID.c_str());
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

    // used for arduino
    IStringType filePath;

    bool load(IStringType notedata, bool loadOnlyMetadata = false)
    {
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

        printf("Loaded notes of %lu %lu %lu %lu\n", notes[0].size(), notes[1].size(), notes[2].size(), notes[3].size());
        printf("Loaded in %d ms\n", startTime.deltaTime());

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
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Wel", rgb(255, 255, 255));
            }
            return;
        }
        if (deltaTime < 320)
        {
            if (currentKeyframe == IntroSceneKeyframe::Wel)
            {
                currentKeyframe = IntroSceneKeyframe::Come;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome", rgb(255, 255, 255));
            }
            return;
        }
        if (deltaTime < 500)
        {
            if (currentKeyframe == IntroSceneKeyframe::Come)
            {
                currentKeyframe = IntroSceneKeyframe::To;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to", rgb(255, 255, 255));
            }
            return;
        }
        if (deltaTime < 710)
        {
            if (currentKeyframe == IntroSceneKeyframe::To)
            {
                currentKeyframe = IntroSceneKeyframe::OSU;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to OSU!", rgb(255, 255, 255));
            }
            return;
        }
        if (deltaTime >= 860 && deltaTime < __INTRO_SCENE_END_MS_)
        {
            api->clear();
            api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to OSU!", rgb(255, 255, 255));
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
    TitleScene(CnavasAPI *api)
    {
        this->api = api;
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
        api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, logoMovSz, rgb(255, 255, 255));
        api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, logoInnerSz, rgb(0xF1, 0x60, 0xA1));
        if (logoMovSz >= (CANVAS_WIDTH / 8 + 80) / 2)
            api->drawOsuLogoText(logoMovX, CANVAS_HEIGHT / 2, "OSU!", rgb(255, 255, 255));
        else
            api->drawOsuLogoTextSmaller(logoMovX, CANVAS_HEIGHT / 2, "OSU!", rgb(255, 255, 255));
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
        api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Loading...", rgb(255, 255, 255));
        api->drawRect(10, CANVAS_HEIGHT * 2 / 3, CANVAS_WIDTH - 20, 20, rgb(255, 255, 255));
        api->drawRect(10, CANVAS_HEIGHT * 2 / 3, (CANVAS_WIDTH - 20) * progress / 100, 20, rgb(0, 255, 0));

        // TODO: Change when arduino implementation started
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT * 2 / 3 + 12, int2string(progress), rgb(0, 0, 0));
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

class SelectScene
{
public:
    CnavasAPI *api;
    LoadingScene *loadingScene;
    SelectScene(CnavasAPI *api)
    {
        this->api = api;
        loadingScene = new LoadingScene(api);
    }

    bool last3Button = false;

    void loadOSUFile(IStringType data)
    {
        OSUFile of = OSUFile();
        of.load(data, true);
        if (!_GLOBAL_MAPS_.has(of.beatmapSetID))
        {
            _GLOBAL_MAPS_.insert(of.beatmapSetID, _GLOBAL_MAPS_.size());
            _GLOBAL_MAPS_SD_.push_back(IVector<OSUFile>());
        }

        _GLOBAL_MAPS_SD_.at(_GLOBAL_MAPS_.at(of.beatmapSetID)).push_back(of);
    }

    void syncnolus_sdmapLoading()
    {
        // TODO: implement on arduino
        loadingScene->setProgress(0);
        loadingScene->update(true);
        loadOSUFile(OSUGAMEFILE1);
        loadingScene->setProgress(33);
        loadingScene->update(false);
        loadOSUFile(OSUGAMEFILE2);
        loadingScene->setProgress(66);
        loadingScene->update(false);
        loadOSUFile(OSUGAMEFILE3);
        loadingScene->setProgress(100);
        loadingScene->update(false);

        printf("Loaded %lu map sets\n", _GLOBAL_MAPS_.size());
    }

    int cursor = 0;
    int mapsetCursor = 0;

    void render()
    {
        api->clear();
        if (_GLOBAL_MAPS_.size() == 0)
        {
            api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2 - 20, "Select Menu", rgb(255, 255, 255));
            api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2 + 20, "No mapset found", rgb(255, 255, 255));
        }
    }

    void update(bool forceRender)
    {
        if (forceRender)
        {
            cursor = 0;
            mapsetCursor = 0;
        }

        if (_GLOBAL_MAP_SD_MODIFIED_)
        {
            loadingScene->setProgress(0);
            loadingScene->update(true);
            syncnolus_sdmapLoading();
            _GLOBAL_MAP_SD_MODIFIED_ = false;
        }

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

// MARK: - Scene: Ingame
class IngameScene
{
public:
    CnavasAPI *api;
    IngameScene(CnavasAPI *api)
    {
        this->api = api;
    }

    void render()
    {
        api->clear();
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Ingame...", rgb(255, 255, 255));
    }

    void update(bool forceRender)
    {
        bool updated = forceRender;
        if (updated)
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
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Result", rgb(255, 255, 255));
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
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Options", rgb(255, 255, 255));
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
        api->drawText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Downloading...", rgb(255, 255, 255));
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

void update()
{
    bool forceRender = lastRendered != currentScene;

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

// MARK: - Emscripten Entry Point

int main()
{
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackPress);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackRelease);
    emscripten_set_main_loop(update, 0, 1);
    return 0;
}
