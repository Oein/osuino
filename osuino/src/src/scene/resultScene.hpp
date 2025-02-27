class GameResult {
private:
    int allNotes;
    int longNoteAcc[4] = {0, 0, 0, 0};
public:
    bool isWin;
    int score = 933819;
    int maxCombo;
    int accu = 0;
    int accuCount = 0;

    void clear() {
        isWin = false;
        score = 0;
        maxCombo = 0;
    }

    void setAllnotes(int allNotes) {
        this->allNotes = allNotes;
    }

    void processSingleNoteScore(int acc) {
        score += FULLSCORE / allNotes * acc / 100;
        processAccuracy(acc);
    }

    void processMaxCombo(int combo) {
        if(combo > maxCombo) maxCombo = combo;
    }

    void processLongNoteStart(int keyIndex, int acc) {
        longNoteAcc[keyIndex] = acc;
    }

    void processLongNoteEnd(int keyIndex, int acc) {
        int accu = (longNoteAcc[keyIndex] + acc) / 2;
        processSingleNoteScore(accu);
    }

    void processAccuracy(int acc) {
        accu += acc / 10;
        accuCount++;
    }

    float getAccuracy() {
        return (float)accu / accuCount * 10;
    }
};

GameResult gameResult;

class ResultScene
{
public:
    CnavasAPI *api;
    KeyHelper *keyHelper;
    ResultScene(CnavasAPI *api)
    {
        this->api = api;
        this->keyHelper = new KeyHelper(api);
    }

    IStringType getRankString(int score) {
        if(score >= FULLSCORE * 0.97) return "S";
        if(score >= FULLSCORE * 0.9) return "A";
        if(score >= FULLSCORE * 0.8) return "B";
        if(score >= FULLSCORE * 0.7) return "C";
        if(score >= FULLSCORE * 0.6) return "D";
        return "E";
    }

    void render()
    {
        api->clear();
        api->drawRect(0, 0, CANVAS_WIDTH, 40, rgb(10, 50, 50));
        api->drawTextTopLeft(10, 20, "Result", COLOR_WHITE);
        // draw rank string in the triangle(large)
        api->drawCircle(CANVAS_WIDTH / 5, 110, CANVAS_WIDTH / 7, COLOR_WHITE);
        api->drawCircle(CANVAS_WIDTH / 5, 110, CANVAS_WIDTH / 7 * 0.9, COLOR_OSU);
        api->drawOsuLogoText(CANVAS_WIDTH / 5, 110, getRankString(gameResult.score), COLOR_WHITE);

        // draw score
        api->drawTextTopLeft(CANVAS_WIDTH / 2, 80, "Score", COLOR_WHITE);
        api->drawOsuLogoTextTopLeft(CANVAS_WIDTH / 2, 110, ul2string(gameResult.score), COLOR_WHITE);

        // draw max combo
        api->drawTextTopLeft(CANVAS_WIDTH / 2, 150, "Max Combo", COLOR_WHITE);
        api->drawOsuLogoTextTopLeft(CANVAS_WIDTH / 2, 180, int2string(gameResult.maxCombo), COLOR_WHITE);

        // draw accuracy
        api->drawTextTopLeft(CANVAS_WIDTH / 2, 220, "Accuracy", COLOR_WHITE);
        int ac = (int)(gameResult.getAccuracy() * 100);
        int ac1 = ac / 100;
        int ac2 = ac % 100;
        api->drawOsuLogoTextTopLeft(CANVAS_WIDTH / 2, 250, int2string(ac1) + "." + int2string(ac2) + "%", COLOR_WHITE);

        keyHelper->setBtnText("", "", "Retry", "Back");
        keyHelper->setBtn(0, 0, 1, 1);
        keyHelper->render();
    }

    bool last2Button = false;
    bool last3Button = false;

    void update(bool forceRender)
    {
        bool updated = forceRender;
        if (updated)
            render();

        if(buttonPressed(2) != last2Button) {
            last2Button = buttonPressed(2);
            if(!last2Button) {
                currentScene = Scene::Ingame;
            }
        }

        if(buttonPressed(3) != last3Button) {
            last3Button = buttonPressed(3);
            if(!last3Button) {
                currentScene = Scene::Select;
            }
        }
    }
};