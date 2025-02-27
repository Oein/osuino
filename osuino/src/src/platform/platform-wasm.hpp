#define IColorType int
#define IStringType std::string
#define subString(sub, start, end) sub.substr(start, end)

IColorType rgb(int r, int g, int b)
{
    return (r << 16) | (g << 8) | b;
}
IColorType rgb_darken(int r, int g, int b, float factor__)
{
    float factor = factor__ < 0 ? 0 : factor__ > 1 ? 1
                                                   : factor__;
    return rgb(r * factor, g * factor, b * factor);
}

const IColorType COLOR_BLACK = rgb(0, 0, 0);
const IColorType COLOR_WHITE = rgb(255, 255, 255);
const IColorType COLOR_OSU = rgb(0xF1, 0x60, 0xA1);
const IColorType NOTE_COLORS[] = {
    // #C5C5C8
    // #FFA503
    rgb(0xC5, 0xC5, 0xC8),
    rgb(0xFF, 0xA5, 0x03),
    rgb(0xFF, 0xA5, 0x03),
    rgb(0xC5, 0xC5, 0xC8),
};

int randint(int min, int max)
{
    return min + (rand() % (max - min + 1));
}

int string2int(IStringType str)
{
    return std::stoi(str);
}

float string2float(IStringType str)
{
    return std::stof(str);
}

IStringType int2string(int num)
{
    return std::to_string(num);
}

IStringType ul2string(unsigned long num)
{
    return std::to_string(num);
}

IStringType char2string(char chr)
{
    return std::string(1, chr);
}

IStringType float2string(float num)
{
    return std::to_string(num);
}

int largest(int a, int b)
{
    return a > b ? a : b;
}

bool __buttonPressed__state__[4] = {false, false, false, false};
bool buttonPressed(int button)
{
    return __buttonPressed__state__[button];
}

void update();

void buttonUpdate() {
    EM_ASM_({
        // document.getElementById('k' + index).style.background = __buttonPressed__state__[index] ? 'red' : 'black';
        Module.k0 = document.getElementById("k0");
        Module.k1 = document.getElementById("k1");
        Module.k2 = document.getElementById("k2");
        Module.k3 = document.getElementById("k3");
        Module.k0.style.background = $0 ? "red" : "black";
        Module.k1.style.background = $1 ? "red" : "black";
        Module.k2.style.background = $2 ? "red" : "black";
        Module.k3.style.background = $3 ? "red" : "black";
    }, __buttonPressed__state__[0], __buttonPressed__state__[1], __buttonPressed__state__[2], __buttonPressed__state__[3]);
}

EM_BOOL keyCallbackPress(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if (keyEvent->key[0] == 's')
        __buttonPressed__state__[0] = true;
    if (keyEvent->key[0] == 'd')
        __buttonPressed__state__[1] = true;
    if (keyEvent->key[0] == 'l')
        __buttonPressed__state__[2] = true;
    if (keyEvent->key[0] == ';')
        __buttonPressed__state__[3] = true;
        buttonUpdate();
    update();
    return 0;
}

EM_BOOL keyCallbackRelease(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if (keyEvent->key[0] == 's')
        __buttonPressed__state__[0] = false;
    if (keyEvent->key[0] == 'd')
        __buttonPressed__state__[1] = false;
    if (keyEvent->key[0] == 'l')
        __buttonPressed__state__[2] = false;
    if (keyEvent->key[0] == ';')
        __buttonPressed__state__[3] = false;
    buttonUpdate();
    update();
    return 0;
}

class CnavasAPI
{
public:
    CnavasAPI()
    {
        EM_ASM(
            Module.canvas = document.getElementById('canvas');
            Module.canvas.width = 320;
            Module.canvas.height = 480;
            Module.canvas.style.backgroundColor = 'black';
            Module.ctx = Module.canvas.getContext('2d'););
    }

    void drawRect(int x, int y, int width, int height, IColorType color)
    {
        // printf("drawRect(%d, %d, %d, %d, %d)\n", x, y, width, height, color);
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($4 >> 16) + ',' + (($4 >> 8) & 0xFF) + ',' + ($4 & 0xFF) + ')';
            Module.ctx.fillRect($0, $1, $2, $3); }, x, y, width, height, color);
    }

    void drawCircle(int x, int y, int radius, IColorType color)
    {
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.arc($0, $1, $2, 0, 2 * Math.PI);
            Module.ctx.fill(); }, x, y, radius, color);
    }

    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($6 >> 16) + ',' + (($6 >> 8) & 0xFF) + ',' + ($6 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.moveTo($0, $1);
            Module.ctx.lineTo($2, $3);
            Module.ctx.lineTo($4, $5);
            Module.ctx.fill(); }, x1, y1, x2, y2, x3, y3, color);
    }

    void drawTriangleOutline(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color)
    {
        EM_ASM_({
            Module.ctx.strokeStyle = 'rgb(' + ($6 >> 16) + ',' + (($6 >> 8) & 0xFF) + ',' + ($6 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.moveTo($0, $1);
            Module.ctx.lineTo($2, $3);
            Module.ctx.lineTo($4, $5);
            Module.ctx.closePath();
            Module.ctx.stroke(); }, x1, y1, x2, y2, x3, y3, color);
    }

    void clear()
    {
        EM_ASM(
            Module.ctx.clearRect(0, 0, Module.canvas.width, Module.canvas.height););
    }

    void drawIntroText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawOsuLogoText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawOsuLogoTextTopLeft(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawOsuLogoTextSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawButtonName(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawTextTopLeft(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawTextTopLeftSmaller(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '10pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawComboNumber(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            // draw background with black
            Module.ctx.fillStyle = 'rgb(0, 0, 0)';
            Module.ctx.fillRect($1 - textWidth / 2 - 5, $2 - textHeight / 2 - 5, textWidth + 10, textHeight + 10);

            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }

    void drawComboText(int x, int y, IStringType text, IColorType color = COLOR_WHITE)
    {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            // draw background with black
            Module.ctx.fillStyle = 'rgb(0, 0, 0)';
            Module.ctx.fillRect($1 - textWidth / 2 - 5, $2 - textHeight / 2 - 5, textWidth + 10, textHeight + 10);

            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2); }, text.c_str(), x, y, color);
    }
};
