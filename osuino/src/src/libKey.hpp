class KeyHelper
{
public:
    CnavasAPI *api;
    KeyHelper(CnavasAPI *api)
    {
        this->api = api;
    }

    bool buttons[4] = {false, false, false, false};

    void setBtn(bool button0, bool button1, bool button2, bool button3)
    {
        buttons[0] = button0;
        buttons[1] = button1;
        buttons[2] = button2;
        buttons[3] = button3;
    }

    IStringType names[4] = {"S", "D", "L", ";"};

    void setBtnText(IStringType button0Text, IStringType button1Text, IStringType button2Text, IStringType button3Text)
    {
        names[0] = button0Text;
        names[1] = button1Text;
        names[2] = button2Text;
        names[3] = button3Text;
    }

    IStringType buttonKey[4] = {"S", "D", "L", ";"};

    void render()
    {
        api->drawRect(0, CANVAS_HEIGHT - 40, CANVAS_WIDTH, 40, rgb(10, 50, 50));
        int drawX = 5;
        int btnWidth = (CANVAS_WIDTH - 10 - 15) / 4;

        for (int i = 0; i < 4; i++)
        {
            if (buttons[i])
            {
                api->drawRect(drawX, CANVAS_HEIGHT - 35, 30, 30, COLOR_BLACK);
                api->drawText(drawX + 15, CANVAS_HEIGHT - 20, buttonKey[i], COLOR_WHITE);
                // TODO: implement on arduino
                api->drawTextTopLeft(drawX + 33, CANVAS_HEIGHT - 20, names[i], COLOR_WHITE);
            }
            drawX += btnWidth + 5;
        }
    }
};
