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

IStringType keys0[] = {"q", "w", "e", "r", "t"};
IStringType keys1[] = {"y", "u", "i", "o", "p"};
IStringType keys2[] = {"a", "s", "d", "f", "g"};
IStringType keys3[] = {"h", "j", "k", "l", "z"};
IStringType keys4[] = {"x", "c", "v", "b", "n"};
IStringType keys5[] = {"m", "0", "1", "2", "3"};
IStringType keys6[] = {"4", "5", "6", "7", "8"};

IStringType keys[] = {
    "q", "w", "e", "r", "t",
    "y", "u", "i", "o", "p",
    "a", "s", "d", "f", "g",
    "h", "j", "k", "l", "z",
    "x", "c", "v", "b", "n",
    "m", "0", "1", "2", "3",
    "4", "5", "6", "7", "8",
    "9", "_", "<", "OK"
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
        drawKeyboard();
    }

    void drawKeyboard() {
        // keyboard is like slider
        // it shows 5 keys at a time
        // and cursor is on the center

        api->drawRect(10, 10, CANVAS_WIDTH - 20, 50, COLOR_WHITE);
        api->drawText(CANVAS_WIDTH / 2, 35, result.result, COLOR_BLACK);

        int gap = 10;
        int width = (CANVAS_WIDTH - 20 - gap * 4) / 5;
        int height = 30;
        for(int i = 0;i < 5;i ++) {
            api->drawRect(10 + (width + gap) * i, 70, width, height, cursor == i ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(10 + (width + gap) * i + width / 2, 70 + height / 2, keys0[i], cursor == i ? COLOR_BLACK : COLOR_WHITE);

            api->drawRect(10 + (width + gap) * i, 110, width, height, cursor == i + 5 ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(10 + (width + gap) * i + width / 2, 110 + height / 2, keys1[i], cursor == i + 5 ? COLOR_BLACK : COLOR_WHITE);

            api->drawRect(10 + (width + gap) * i, 150, width, height, cursor == i + 10 ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(10 + (width + gap) * i + width / 2, 150 + height / 2, keys2[i], cursor == i + 10 ? COLOR_BLACK : COLOR_WHITE);

            api->drawRect(10 + (width + gap) * i, 190, width, height, cursor == i + 15 ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(10 + (width + gap) * i + width / 2, 190 + height / 2, keys3[i], cursor == i + 15 ? COLOR_BLACK : COLOR_WHITE);

            api->drawRect(10 + (width + gap) * i, 230, width, height, cursor == i + 20 ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(10 + (width + gap) * i + width / 2, 230 + height / 2, keys4[i], cursor == i + 20 ? COLOR_BLACK : COLOR_WHITE);

            api->drawRect(10 + (width + gap) * i, 270, width, height, cursor == i + 25 ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(10 + (width + gap) * i + width / 2, 270 + height / 2, keys5[i], cursor == i + 25 ? COLOR_BLACK : COLOR_WHITE);

            api->drawRect(10 + (width + gap) * i, 310, width, height, cursor == i + 30 ? COLOR_WHITE : COLOR_BLACK);
            api->drawButtonName(10 + (width + gap) * i + width / 2, 310 + height / 2, keys6[i], cursor == i + 30 ? COLOR_BLACK : COLOR_WHITE);
        }

        // manually draw last row
        // 9 _ < OK 
        // OK IS Two columns
        api->drawRect(10 + (width + gap) * 0, 350, width, height, cursor == 35 ? COLOR_WHITE : COLOR_BLACK);
        api->drawButtonName(10 + (width + gap) * 0 + width / 2, 350 + height / 2, "9", cursor == 35 ? COLOR_BLACK : COLOR_WHITE);

        api->drawRect(10 + (width + gap) * 1, 350, width, height, cursor == 36 ? COLOR_WHITE : COLOR_BLACK);
        api->drawButtonName(10 + (width + gap) * 1 + width / 2, 350 + height / 2, "BLK", cursor == 36 ? COLOR_BLACK : COLOR_WHITE);

        api->drawRect(10 + (width + gap) * 2, 350, width, height, cursor == 37 ? COLOR_WHITE : COLOR_BLACK);
        api->drawButtonName(10 + (width + gap) * 2 + width / 2, 350 + height / 2, "DEL", cursor == 37 ? COLOR_BLACK : COLOR_WHITE);

        api->drawRect(10 + (width + gap) * 3, 350, width * 2, height, cursor == 38 ? COLOR_WHITE : COLOR_BLACK);
        api->drawButtonName(10 + (width + gap) * 3 + width, 350 + height / 2, "OK", cursor == 38 ? COLOR_BLACK : COLOR_WHITE);
    }

    RepeatedButton button0;
    RepeatedButton button1;

    int cursor = 0;

    bool last2Button = false;
    bool last3Button = false;
    void update() {
        bool pressed[] = {button0.get(buttonPressed(0)), button1.get(buttonPressed(1)), buttonPressed(2), buttonPressed(3)};
        if(pressed[0]) {
            cursor = (cursor + 38) % 39;
            drawKeyboard();
        }
        if(pressed[1]) {
            cursor = (cursor + 1) % 39;
            drawKeyboard();
        }
        if(last2Button != pressed[2]) {
            last2Button = pressed[2];
            if(last2Button) {
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
                else {
                    result.result += key;
                }

                drawKeyboard();
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