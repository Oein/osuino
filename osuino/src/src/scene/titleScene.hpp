
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
        api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, (int)logoMovSz, COLOR_WHITE);
        api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, (int)logoInnerSz, COLOR_OSU);
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
#ifdef _PRINTF_INFO_
                    printf("Scene:Title - SubmenuOpen Started\n");
#endif
                }
                else if (submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen)
                {
                    if (cursorFocused == TitleSceneCursor::Title__Play)
                    {
#ifdef _PRINTF_INFO_
                        printf("Scene:Title - Play Button Pressed\n");
#endif
                        currentScene = Scene::Select;
                    }
                    if (cursorFocused == TitleSceneCursor::Title__Download)
                    {
#ifdef _PRINTF_INFO_
                        printf("Scene:Title - Download Button Pressed\n");
#endif
                        currentScene = Scene::Download;
                    }
                    if (cursorFocused == TitleSceneCursor::Title__Options)
                    {
#ifdef _PRINTF_INFO_
                        printf("Scene:Title - Options Button Pressed\n");
#endif
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
#ifdef _PRINTF_INFO_
                    printf("Scene:Title - SubmenuClose Started\n");
#endif
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
