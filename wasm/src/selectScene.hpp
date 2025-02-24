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
IVector<IVector<OsuMetaFile>> beatMetafiles;
IMapUL<unsigned long> beatset2metaidx;

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
        OsuMetaFile meta;
        StringParser parser(fsapi.get(filepath));
        meta.parse(parser);
        if(!beatset2metaidx.has(meta.beatsetid)) {
            beatset2metaidx.insert(meta.beatsetid, beatMetafiles.size());
            beatMetafiles.push_back(IVector<OsuMetaFile>());
        }

        beatMetafiles.data[beatset2metaidx.at(meta.beatsetid)].push_back(meta);
    }

    void syncnolus_sdmapLoading()
    {
        // TODO: implement on arduino
        loadingScene->setProgress(0);
        loadingScene->update(true);
        IVector<IStringType> files = fsapi.listMetas();
        int totalFiles = files.size();
        for (int i = 0; i < totalFiles; i++)
        {
            IStringType file = files.data[i];

            loadOSUFile(file);
            loadingScene->setProgress((i + 1) * 100 / totalFiles);
            loadingScene->update(false);
        }

#ifdef _PRINTF_INFO_
        printf("Loaded %lu map sets\n", beatMetafiles.size());
        for (int i = 0; i < beatMetafiles.size(); i++)
        {
            printf("Mapset %d has %lu maps\n", i, beatMetafiles.at(i).size());
        }
#endif
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
        api->drawTextTopLeft(15, y + 15, int2string(cursor + 1) + ". " + beatMetafiles.at(cursor).at(subcursor).title + " - " + beatMetafiles.at(cursor).at(subcursor).artist, COLOR_BLACK);
        api->drawTextTopLeftSmaller(25, y + 33, char2string(subcursor + 'a') + ". " + beatMetafiles.at(cursor).at(subcursor).version, COLOR_BLACK);
    }

    void render()
    {
        api->clear();
        if (beatMetafiles.size() == 0)
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
            if (subCursor >= beatMetafiles.at(globalCursor).size())
            {
                globalCursor++;
                subCursor = 0;
            }
            offset--;

            if (globalCursor >= beatMetafiles.size())
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
                    globalCursor = beatMetafiles.size() - 1;
                }

                subCursor = beatMetafiles.at(globalCursor).size() - 1;
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