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