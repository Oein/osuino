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
