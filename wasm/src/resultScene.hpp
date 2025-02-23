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