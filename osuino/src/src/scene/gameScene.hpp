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
OsuChunkedMap NMAP;

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

    int getAccuFromTime(int delta, float multiplier = 1.0) {
        int time = abs(delta);
        if(time <= multiplier * _JUDGE_MAX_100_) return 100;
        if(time <= multiplier * _JUDGE_MAX_90_) return 90;
        if(time <= multiplier * _JUDGE_MAX_80_) return 80;
        if(time <= multiplier * _JUDGE_MAX_70_) return 70;
        if(time <= multiplier * _JUDGE_MAX_60_) return 60;
        if(time <= multiplier * _JUDGE_MAX_50_) return 50;
        if(time <= multiplier * _JUDGE_MAX_40_) return 40;
        if(time <= multiplier * _JUDGE_MAX_30_) return 30;
        if(time <= multiplier * _JUDGE_MAX_20_) return 20;
        if(time <= multiplier * _JUDGE_MAX_10_) return 10;
        return 0;
    }

    void processLongNote(int time, IngameButtonType btn) {
        // 롱노트 처리 중인 상태임

        int noteDeleteTime = NMAP.get(keyIndex, processingLongNoteIndex).endtime + _NOTE_LATE_ALLOWANCE_;
        if(noteDeleteTime < time) {
            // 롱노트가 이미 지나감
            processAccu(0);
            processFrom = processingLongNoteIndex + 1;
            processingLongNoteIndex = -1;
#ifdef _PRINT_NOTE_MISSED_
            printf("Line %d: Long note missed\n", keyIndex);
#endif
            return;
        }

        int delta = NMAP.get(keyIndex, processingLongNoteIndex).endtime - time;
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
#ifdef _PRINT_EARLY_LATE_MS_
            printf("Line %d, Long note released %dms\n", keyIndex, delta);
#endif
            processAccu(getAccuFromTime(delta, 2));
            processFrom = processingLongNoteIndex + 1;
            processingLongNoteIndex = -1;
            return;
        }
    }

    void processKeypress(int time) {
        IngameButtonType btn = igbtn.get(buttonPressed(keyIndex));
        if(processingLongNoteIndex != -1) return processLongNote(time, btn);

        // 아직 아무것도 관여되어있지 않음.
        for(int i = processFrom; i < NMAP.colNotesCount[keyIndex]; i++) {
            bool isLongNote = NMAP.get(keyIndex, i).endtime > 0;
            if(NMAP.get(keyIndex, i).time > time + CANVAS_HEIGHT * _NOTE_PIXEL_PER_MS_) {
                // 아직 노트가 안나왔음
                break;
            }

            int noteDeleteTime = (isLongNote ? NMAP.get(keyIndex, i).endtime : NMAP.get(keyIndex, i).time) + _NOTE_LATE_ALLOWANCE_;
            if(noteDeleteTime < time) {
                // 노트가 이미 지나감
                processAccu(0);
                processFrom = i + 1;
#ifdef _PRINT_NOTE_MISSED_
                printf("Line %d: Note missed\n", keyIndex);
#endif
                break;
            }

            // 노트를 눌르지 않을경우 아무 처리도 하지 않음
            if(btn != IngameButtonType::JUST_PRESSED) continue;

            int delta = NMAP.get(keyIndex, i).time - time;
            if(delta > _NOTE_IGNORE_DELTA_) {
                // 노트가 아직 너무 먼 곳에 있음
                break;
            }
#ifdef _PRINT_EARLY_LATE_MS_
            printf("Line %d, Note pressed %dms\n", keyIndex, delta);
#endif
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
        NMAP.unload(keyIndex, renderFrom);
        processKeypress(time);

        if(processingLongNoteIndex != -1) {
            if(renderFrom < processingLongNoteIndex) renderFrom = processingLongNoteIndex;
        } else {
            if(renderFrom < processFrom) renderFrom = processFrom;
        }

#ifndef _RENDER_EVERY_FRAME_
        bool newRendered[CANVAS_HEIGHT] = {
            false,
        };
#endif

        for(int i = renderFrom;i < NMAP.colNotesCount[keyIndex]; i++) {
            bool isLongNote = NMAP.get(keyIndex, i).endtime > 0;

            // single note
            int noteY = _NOTE_END_Y_ - (NMAP.get(keyIndex, i).time - time) * _NOTE_PIXEL_PER_MS_;
            int noteH = _SINGLE_NOTE_HEIGHT_;

            if(isLongNote) {
                noteY = _NOTE_END_Y_ - (NMAP.get(keyIndex, i).endtime - time) * _NOTE_PIXEL_PER_MS_;
                noteH = (NMAP.get(keyIndex, i).endtime - NMAP.get(keyIndex, i).time) * _NOTE_PIXEL_PER_MS_;
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

#ifndef _RENDER_EVERY_FRAME_
            for(int j = noteY; j < noteY + noteH && j < CANVAS_HEIGHT; j++) {
                newRendered[j] = true;
            }
#else
            api->drawRect(keyX, noteY, CANVAS_WIDTH / 4, noteH, NOTE_COLORS[keyIndex]);
    #ifdef _DRAW_NOTE_INDEX_
            api->drawTextTopLeft(keyX + 5, noteY + 5, int2string(i) + "(" + int2string(i / 100) + ")", COLOR_BLACK);
    #endif
#endif
        }

#ifndef _RENDER_EVERY_FRAME_
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
#endif
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
#ifdef _RENDER_EVERY_FRAME_
        api->clear();
#endif
        int deltaTime = this->timer.deltaTime();
        for (int i = 0; i < 4; i++)
        {
            lineHandlers[i]->render(deltaTime);
        }

        // 판정선 그리기
        api->drawRect(0, _NOTE_END_Y_, CANVAS_WIDTH, 2, COLOR_WHITE);

        if (health > 100)
            health = 100;

#ifndef _NO_DIE_
        api->drawTextTopLeft(5, 5, "Health: " + int2string((int)health), COLOR_WHITE);
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
        NMAP.unloadAll();

        loadingScene->setProgress(10);
        loadingScene->update(true);

        lastLoaded = requestedPlaymap;
#ifdef _PRINTF_MAP_LOAD_INFO_
        printf("Loading map with cursor %d, %d\n", lastLoaded.cursor, lastLoaded.subcursor);
#endif
        NMAP.initialize(beatMetafiles.data[lastLoaded.cursor].data[lastLoaded.subcursor].id);

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
#ifdef _PRINTF_INFO_
        printf("Scene:Ingame - Initlized\n");
#endif
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
