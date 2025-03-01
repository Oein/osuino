class DownloadScene
{
public:
    CnavasAPI *api;
    LoadingScene *loadingScene;
    KeyHelper *keyHelper;

    int cursor = 0;
    int page = 0;
    int itemsCount = 1;
    RepeatedButton button0;
    RepeatedButton button1;
    bool last2Button = false;
    bool last3Button = false;

    IStringType searchKeyword = "";

    AnimatedData yOffset = AnimatedData(0, 50);
    AnimatedData opacity = AnimatedData(100, 50);

    bool hasMore = false;
    bool hasPrev = false;
    std::vector<OSUBeatResponseItem> items;

    bool searchMode = false;

    DownloadScene(CnavasAPI *api)
    {
        this->api = api;
        keyHelper = new KeyHelper(api);
        loadingScene = new LoadingScene(api);
    }

    void drawBg(int baseY, bool focused)
    {
        if (focused)
        {
            api->drawRect(10 - 3, baseY - 3, CANVAS_WIDTH - 20 + 6, __SELECT_MAP_HEIGHT__ + 6, COLOR_OSU);
            api->drawRect(10, baseY, CANVAS_WIDTH - 20, __SELECT_MAP_HEIGHT__, rgb_darken(255, 255, 255, opacity.current() / 100));
        }
        else
            api->drawRect(10, baseY, CANVAS_WIDTH - 20, __SELECT_MAP_HEIGHT__, rgb_darken(255, 255, 255, 0.3));
    }

    void drawSearch(int baseY, bool focused)
    {
        drawBg(baseY, focused);
        api->drawTextTopLeft(15, baseY + 15, searchKeyword == "" ? "Search..." : searchKeyword, COLOR_BLACK);
    }

    void drawNext(int baseY, bool focused)
    {
        drawBg(baseY, focused);
        api->drawTextTopLeft(15, baseY + 15, "Next", COLOR_BLACK);
    }

    void drawPrev(int baseY, bool focused)
    {
        drawBg(baseY, focused);
        api->drawTextTopLeft(15, baseY + 15, "Prev", COLOR_BLACK);
    }

    void renderItem(int cursor, int baseY, bool focused = false)
    {
        if (cursor == 0)
        {
            drawSearch(baseY, focused);
            return;
        }

        if (cursor == 1)
        {
            if (hasMore)
            {
                drawNext(baseY, focused);
                return;
            }
            else if (hasPrev)
            {
                drawPrev(baseY, focused);
                return;
            }
        }

        if (cursor == 2)
        {
            if (hasMore && hasPrev)
            {
                drawPrev(baseY, focused);
                return;
            }
        }

        drawBg(baseY, focused);
        int realCursor = cursor - 1;
        if (hasMore)
            realCursor--;
        if (hasPrev)
            realCursor--;

        api->drawTextTopLeft(15, baseY + 15, items[realCursor].title + " - " + items[realCursor].artist, COLOR_BLACK);
        float star = items[realCursor].map.star;
        int n1 = (int)star;
        int n2 = (int)((star - n1) * 10);
        api->drawTextTopLeftSmaller(25, baseY + 33, "(" + int2string(n1) + "." + int2string(n2) + ") " + items[realCursor].map.version, COLOR_BLACK);
    }

    void render()
    {
        api->clear();
        int renderY = __SELECT_MAP_START_Y__ + yOffset.current();
        for (int i = __SELECT_PREV_MAPS__; i >= 1; i--)
        {
            int ncc = this->cursor - i;
            if (ncc < 0)
            {
                renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;
                continue;
            }
            renderItem(ncc, renderY);
            renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;
        }
        renderItem(cursor, renderY, true);
        renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;
        for (int i = 1; i <= __SELECT_NEXT_MAPS__; i++)
        {
            int ncc = this->cursor + i;
            if (ncc >= itemsCount)
            {
                renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;
                continue;
            }
            renderItem(ncc, renderY);
            renderY += __SELECT_MAP_GAP__ + __SELECT_MAP_HEIGHT__;
        }

        keyHelper->setBtn(true, true, true, true);
        keyHelper->setBtnText("Prev", "Next", "Select", "Back");
        keyHelper->render();
    }

    void initlize()
    {
        cursor = 0;
        page = 0;
        searchKeyword = "";
        searchMode = false;
        yOffset.set(0);
        opacity.set(100);
    }

    void search()
    {
        loadingScene->setProgress(0);
        loadingScene->update(true);
        IOSUMapResponse response = osuapi.search(searchKeyword, page);
        loadingScene->setProgress(100);
        loadingScene->update(true);

        if (response.success)
        {
            items.clear();
            int itemsCnt = 1;
            if (response.data.size() >= 20)
            {
                hasMore = true;
                itemsCnt++;
            }
            else
                hasMore = false;
            if (page > 0)
            {
                hasPrev = true;
                itemsCnt++;
            }
            else
                hasPrev = false;
            for (int i = 0; i < response.data.size(); i++)
            {
                for (int j = 0; j < response.data[i].maps.size(); j++)
                {
                    
                    // if response.data[i].maps[j].version starts with [(any char)K]
                    // then skip this map

                    IStringType version = response.data[i].maps[j].version;
                    if(version.length() >= 4) {
                        if(version[0] == '[' && version[2] == 'K' && version[3] == ']') {
                            if(version[1] != '4') {
                                continue;
                            }
                        }
                    }
                    if(version.length() >= 5) {
                        // match [10K]
                        if(version[0] == '[' && version[3] == 'K' && version[4] == ']') {
                            if(version[1] == '1' && version[2] == '0') {
                                continue;
                            }
                        }
                    }
                    items.push_back(
                        OSUBeatResponseItem(
                            response.data[i].title,
                            response.data[i].artist,
                            response.data[i].maps[j]));
                }
            }
            itemsCount = itemsCnt + items.size();
        }

        render();
    }

    int lastCursor = 0;
    int lastPage = 0;
    void update(bool forceRender)
    {
        if (forceRender)
        {
            initlize();
            search();
        }

        bool updated = forceRender;
        updated |= cursor != lastCursor;
        updated |= page != lastPage;
        updated |= yOffset.update();
        updated |= opacity.update();

        if(searchMode) {
            if(input.result.handled == HnadledType::CANCELED) {
                searchMode = false;
                input.result.handled = HnadledType::NONE;
            }
            else if(input.result.handled == HnadledType::HANDLED) {
                searchKeyword = input.result.result;
                searchMode = false;
                input.result.handled = HnadledType::NONE;
                search();
                updated = true;
            } else {
                input.update();
            }
            return;
        }

        if (updated)
        {
            lastCursor = cursor;
            lastPage = page;
            render();
        }

        if (button0.get(buttonPressed(0)))
        {
            cursor--;
            if (cursor < 0)
                cursor = 0;
            yOffset.set(10);
            yOffset.setTarget(0);
            opacity.set(60);
            opacity.setTarget(100);
        }
        if (button1.get(buttonPressed(1)))
        {
            cursor++;
            if(cursor >= itemsCount)
                cursor = itemsCount - 1;
            yOffset.set(-10);
            yOffset.setTarget(0);
            opacity.set(60);
            opacity.setTarget(100);
        }

        if(last2Button != buttonPressed(2))
        {
            last2Button = buttonPressed(2);
            if(!last2Button)
            {
                if(cursor == 0)
                {
                    searchMode = true;
                    input.requestInput();
                    printf("Requesting input\n");
                }
                else if(hasMore && cursor == 1)
                {
                    page++;
                    search();
                }
                else if(!hasMore && hasPrev && cursor == 1)
                {
                    page--;
                    search();
                }
                else if(hasMore && hasPrev && cursor == 2)
                {
                    page--;
                    search();
                }
                else
                {
                    // download the map
                    int realCursor = cursor - 1;
                    if(hasMore)
                        realCursor--;
                    if(hasPrev)
                        realCursor--;
                    printf("Downloading map %s - %s\n", items[realCursor].title.c_str(), items[realCursor].artist.c_str());
                }
            }
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
    }
};
