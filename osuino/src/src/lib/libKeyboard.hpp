enum HnadledType {
    NONE,
    HANDLING,
    HANDLED,
    CANCELED
};

class InputResult {
public:
    HnadledType handled = NONE;
    IStringType result = "";
};

// q w e r t
// y u i o p
// a s d f g
// h j k l z
// x c v b n
// m < _ [O K]

IStringType keys[] = {
    "q", "w", "e", "r", "t",
    "y", "u", "i", "o", "p",
    "a", "s", "d", "f", "g",
    "h", "j", "k", "l", "z",
    "x", "c", "v", "b", "n",
    "m", "0", "1", "2", "3",
    "4", "5", "6", "7", "8",
    "9", "_", "<", "OK", "SFT"
};

IStringType shiftedKeys[] = {
    "Q", "W", "E", "R", "T",
    "Y", "U", "I", "O", "P",
    "A", "S", "D", "F", "G",
    "H", "J", "K", "L", "Z",
    "X", "C", "V", "B", "N",
    "M", ")", "!", "@", "#",
    "$", "%", "^", "&", "*",
    "(", "_", "<", "OK", "SFT"
};

bool lastState[] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};


class Input {
public:
    CnavasAPI *api;
    InputResult result;
    void requestInput() {
        result.handled = HANDLING;
        result.result = "";
        api->clear();

        // draw input box
        drawKeyboard(true);
    }

    int gap = 10;
    int height = 30;
    int width = (CANVAS_WIDTH - 20 - gap * 4) / 5;

    bool shift = false;

    void drawKey(int x, int y, int idx, bool forceRender) {
        bool enabled = idx == cursor;
        IStringType key = shift ? shiftedKeys[idx] : keys[idx];
        if(forceRender || lastState[idx] != enabled) {
            lastState[idx] = enabled;
            api->drawRect(x, y, width, height, enabled ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(x + width / 2, y + height / 2, key, enabled ? COLOR_BLACK : COLOR_WHITE);
        }
    }

    bool lastShift = false;
    void drawKeyboard(bool forceRender) {
        forceRender |= lastShift != shift;
        lastShift = shift;
        // keyboard is like slider
        // it shows 5 keys at a time
        // and cursor is on the center

        api->drawRect(10, 10, CANVAS_WIDTH - 20, 50, COLOR_WHITE);
        api->drawText(CANVAS_WIDTH / 2, 35, result.result, COLOR_BLACK);

        for(int i = 0;i < 5;i ++) {
            drawKey(10 + (width + gap) * i, 70, i, forceRender);
            drawKey(10 + (width + gap) * i, 110, i + 5, forceRender);
            drawKey(10 + (width + gap) * i, 150, i + 10, forceRender);
            drawKey(10 + (width + gap) * i, 190, i + 15, forceRender);
            drawKey(10 + (width + gap) * i, 230, i + 20, forceRender);
            drawKey(10 + (width + gap) * i, 270, i + 25, forceRender);
            drawKey(10 + (width + gap) * i, 310, i + 30, forceRender);
        }

        // manually draw last row
        // 9 _ < OK 
        // OK IS Two columns
        drawKey(10 + (width + gap) * 0, 350, 35, forceRender);
        drawKey(10 + (width + gap) * 1, 350, 36, forceRender);
        drawKey(10 + (width + gap) * 2, 350, 37, forceRender);
        drawKey(10 + (width + gap) * 3, 350, 38, forceRender);
        drawKey(10 + (width + gap) * 4, 350, 39, forceRender);
    }

    RepeatedButton button0;
    RepeatedButton button1;

    int cursor = 0;

    bool last2Button = false;
    bool last3Button = false;
    void update(bool forceRender = false) {
        bool pressed[] = {button0.get(buttonPressed(0)), button1.get(buttonPressed(1)), buttonPressed(2), buttonPressed(3)};
        if(pressed[0]) {
            cursor = (cursor + 39) % 40;
            drawKeyboard(forceRender);
        }
        if(pressed[1]) {
            cursor = (cursor + 1) % 40;
            drawKeyboard(forceRender);
        }
        if(last2Button != pressed[2]) {
            last2Button = pressed[2];
            if(!last2Button) {
                IStringType key = keys[cursor];

                if(key == "OK") {
                    result.handled = HANDLED;
                    return;
                }
                if(key == "_") {
                    // space
                    result.result += " ";
                }
                else if(key == "<") {
                    // backspace
                    if(result.result.length() > 0) {
                        result.result = subString(result.result, 0, result.result.length() - 1);
                    }
                }
                else if(key == "SFT") {
                    shift = !shift;
                }
                else if(shift) {
                    result.result += shiftedKeys[cursor];
                }
                else {
                    result.result += key;
                }

                drawKeyboard(forceRender);
            }
        }

        if(pressed[3] != last3Button) {
            last3Button = pressed[3];
            if(!last3Button) {
                result.handled = CANCELED;
                return;
            }
        }
    }
};