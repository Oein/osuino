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

#include "./lib/libDatastructure.hpp"

#include "./lib/libFS.hpp"
FS fsapi;
ConfigInstance config(&fsapi);

#include "./lib/libAnimate.hpp"
#include "./lib/libKey.hpp"
#include "./lib/OSUFile.hpp"
#include "./lib/libKeyboard.hpp"
#include "./lib/libAPI.hpp"

OSUAPI osuapi;
Input input;

bool _GLOBAL_MAP_SD_MODIFIED_ = true;

#include "./scene/introScene.hpp"
#include "./scene/resultScene.hpp"
#include "./scene/loadingScene.hpp"
#include "./scene/selectScene.hpp"
#include "./scene/downloadScene.hpp"
#include "./scene/optionsScene.hpp"
#include "./scene/gameScene.hpp"
#include "./scene/titleScene.hpp"

// MARK: - Main Data Structures

CnavasAPI canvasAPI;
IntroScene *introScene = new IntroScene(&canvasAPI);
TitleScene titleScene(&canvasAPI);
SelectScene selectScene(&canvasAPI);
IngameScene ingameScene(&canvasAPI);
ResultScene resultScene(&canvasAPI);
OptionsScene optionsScene(&canvasAPI);
DownloadScene downloadScene(&canvasAPI);
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

#ifdef _DRAW_FPS_
float avgFPS = 0;
unsigned long lastTime = emscripten_get_now();
#endif

void update()
{
    updateSubcall();
#ifdef _DRAW_FPS_
    avgFPS = 1000.0 / (emscripten_get_now() - lastTime);
    lastTime = emscripten_get_now();
    api.drawComboText(40, 20, "FPS: " + int2string((int)avgFPS), COLOR_WHITE);
#endif
}