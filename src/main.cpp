#include <stdio.h>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <string.h>

// https://github.com/codewitch-honey-crisis/htcw_zip/blob/master/examples/demo/src/main.cpp

// MARK: - Constants

#define IColorType int
#define IStringType std::string
#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 480

#define __INTRO_SCENE_MS_ 100
#define __INTRO_SCENE_TRIANGLE_NOSPAWN_MS_ 1450 - 50
#define __INTRO_SCENE_END_MS_ 1600

IColorType rgb(int r, int g, int b) {
    return (r << 16) | (g << 8) | b;
}
IColorType rgb_darken(int r, int g, int b, float factor__) {
    float factor = factor__ < 0 ? 0 : factor__ > 1 ? 1 : factor__;
    return rgb(r * factor, g * factor, b * factor);
}

int randint(int min, int max) {
    return min + (rand() % (max - min + 1));
}

int string2int(std::string str) {
    return std::stoi(str);
}

// MARK: - Button API for Emscripten
bool __buttonPressed__state__[4] = {false, false, false, false};
bool buttonPressed(int button) {
    return __buttonPressed__state__[button];
}

EM_BOOL keyCallbackPress(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if(keyEvent->key[0] == 's') __buttonPressed__state__[0] = true;
    if(keyEvent->key[0] == 'd') __buttonPressed__state__[1] = true;
    if(keyEvent->key[0] == 'l') __buttonPressed__state__[2] = true;
    if(keyEvent->key[0] == ';') __buttonPressed__state__[3] = true;
    return 0;
}

EM_BOOL keyCallbackRelease(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    if(keyEvent->key[0] == 's') __buttonPressed__state__[0] = false;
    if(keyEvent->key[0] == 'd') __buttonPressed__state__[1] = false;
    if(keyEvent->key[0] == 'l') __buttonPressed__state__[2] = false;
    if(keyEvent->key[0] == ';') __buttonPressed__state__[3] = false;
    return 0;
}

// MARK: - Canvas API for Emscripten

class CnavasAPI {
public:
    CnavasAPI() {
        EM_ASM(
            Module.canvas = document.getElementById('canvas');
            Module.canvas.width = 320;
            Module.canvas.height = 480;
            Module.canvas.style.backgroundColor = 'black';
            Module.ctx = Module.canvas.getContext('2d');
        );
    }

    void drawRect(int x, int y, int width, int height, IColorType color) {
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($4 >> 16) + ',' + (($4 >> 8) & 0xFF) + ',' + ($4 & 0xFF) + ')';
            Module.ctx.fillRect($0, $1, $2, $3);
        }, x, y, width, height, color);
    }

    void drawCircle(int x, int y, int radius, IColorType color) {
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.arc($0, $1, $2, 0, 2 * Math.PI);
            Module.ctx.fill();
        }, x, y, radius, color);
    }

    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color) {
        EM_ASM_({
            Module.ctx.fillStyle = 'rgb(' + ($6 >> 16) + ',' + (($6 >> 8) & 0xFF) + ',' + ($6 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.moveTo($0, $1);
            Module.ctx.lineTo($2, $3);
            Module.ctx.lineTo($4, $5);
            Module.ctx.fill();
        }, x1, y1, x2, y2, x3, y3, color);
    }

    void drawTriangleOutline(int x1, int y1, int x2, int y2, int x3, int y3, IColorType color) {
        EM_ASM_({
            Module.ctx.strokeStyle = 'rgb(' + ($6 >> 16) + ',' + (($6 >> 8) & 0xFF) + ',' + ($6 & 0xFF) + ')';
            Module.ctx.beginPath();
            Module.ctx.moveTo($0, $1);
            Module.ctx.lineTo($2, $3);
            Module.ctx.lineTo($4, $5);
            Module.ctx.closePath();
            Module.ctx.stroke();
        }, x1, y1, x2, y2, x3, y3, color);
    }

    void clear() {
        EM_ASM(
            Module.ctx.clearRect(0, 0, Module.canvas.width, Module.canvas.height);
        );
    }

    void drawIntroText(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255)) {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2);
        }, text.c_str(), x, y, color);
    }

    void drawOsuLogoText(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255)) {
        EM_ASM_({
            Module.ctx.font = '26pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2);
        }, text.c_str(), x, y, color);
    }

    void drawOsuLogoTextSmaller(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255)) {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2);
        }, text.c_str(), x, y, color);
    }

    void drawButtonName(int x, int y, IStringType text, IColorType color = rgb(255, 255, 255)) {
        EM_ASM_({
            Module.ctx.font = '13pt Arial';
            Module.ctx.fillStyle = 'rgb(' + ($3 >> 16) + ',' + (($3 >> 8) & 0xFF) + ',' + ($3 & 0xFF) + ')';
            var measure = Module.ctx.measureText(UTF8ToString($0));
            var textWidth = measure.width;
            var textHeight = measure.actualBoundingBoxAscent + measure.actualBoundingBoxDescent;
            Module.ctx.fillText(UTF8ToString($0), $1 - textWidth / 2, $2 + textHeight / 2);
        }, text.c_str(), x, y, color);
    }
};

class Keyframes {
public:
    unsigned long long startTime;

    Keyframes() {
        startTime = emscripten_get_now();
    }

    int deltaTime() {
        return emscripten_get_now() - startTime;
    }

    void reset() {
        startTime = emscripten_get_now();
    }
};

class DeltaTime {
public:
    unsigned long long lastTime;

    DeltaTime() {
        lastTime = emscripten_get_now();
    }

    int deltaTime() {
        unsigned long long currentTime = emscripten_get_now();
        int deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        return deltaTime;
    }
};

// MARK: - Lib Linked List

template <class T>
class ListNode {
  public:
    T element;
    ListNode* next;
    ListNode* prev;

    ListNode(T element, ListNode* prev, ListNode* next) : element(element)
    {
      this->next = next;
      this->prev = prev;
    };
};

template <class T>
class LinkedList  {
  private:
    int length;
    ListNode<T>* head;
    ListNode<T>* tail;
    ListNode<T>* curr;
  public:
    LinkedList();
    LinkedList(const LinkedList<T>&);
    ~LinkedList();
    T& getCurrent();
    T& First() const;
    T& Last() const;
    int getLength();
    void Append(T);
    void DeleteLast();
    void DeleteFirst();
    void DeleteCurrent();
    bool next();
    bool moveToStart();
    bool prev();
    void Delete(T&);
    bool Search(T);
    void Clear();
    void PutFirstToLast();
    void Update(T elem);
    LinkedList& operator = (const LinkedList<T>&);
};

template <class T>
LinkedList<T>::LinkedList() {
    length = 0;
    head = nullptr;
    tail = nullptr;
    curr = nullptr;
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T> & list) {
    length = 0;
    head = nullptr;
    tail = nullptr;
    curr = nullptr;

    ListNode<T> * temp = list.head;

    while(temp != nullptr)
    {
        Append(temp->element);
        temp = temp->next;
    }
}

template <class T>
LinkedList<T> & LinkedList<T>::operator=(const LinkedList<T> & list)
{
    Clear();

    ListNode<T> * temp = list.head;

    while(temp != nullptr)
    {
        Append(temp->element);
        temp = temp->next;
    }

    return *this;
}

template <class T>
LinkedList<T>::~LinkedList() {
    Clear();
}

template<class T>
T& LinkedList<T>::getCurrent()
{
  return curr->element;
}

template<class T>
T& LinkedList<T>::First() const
{
  return head->element;
}

template<class T>
T& LinkedList<T>::Last() const
{
  return tail->element;
}

template<class T>
int LinkedList<T>::getLength()
{
  return length;
}

template <class T>
void LinkedList<T>::Append(T element)
{
    ListNode<T> * node = new ListNode<T>(element, tail, nullptr);

    if(length == 0)
        curr = tail = head = node;
    else {
        tail->next = node;
        tail = node;
    }

    length++;

}

template <class T>
void LinkedList<T>::DeleteLast()
{
    if(length == 0)
      return;
    curr = tail;
    DeleteCurrent();
}

template <class T>
void LinkedList<T>::DeleteFirst()
{
    if(length == 0)
      return;
    curr = head;
    DeleteCurrent();
}

template <class T>
bool LinkedList<T>::next()
{
    if(length == 0)
        return false;

    if(curr->next == nullptr)
        return false;

    curr = curr->next;
    return true;
}

template <class T>
bool LinkedList<T>::moveToStart()
{
    curr = head;
    return length != 0;
}

template<class T>
bool LinkedList<T>::prev()
{
    if(length == 0)
        return false;

    if(curr->prev != nullptr)
        return false;

    curr = curr->prev;
    return true;
}

template <class T>
void LinkedList<T>::Delete(T & elem)
{
    if(Search(elem))
        DeleteCurrent();
}

template <class T>
void LinkedList<T>::DeleteCurrent()
{
    if(length == 0)
        return;
    length--;
    ListNode<T> * temp = curr;

    if(temp->prev != nullptr)
        temp->prev->next = temp->next;
    if(temp->next != nullptr)
        temp->next->prev = temp->prev;

    if(length == 0)
        head = curr = tail = nullptr;
    else if(curr == head)
        curr = head = head->next;
    else if(curr == tail)
        curr = tail = tail->prev;
    else
        curr = curr->prev;

     delete temp;
}

template <class T>
bool LinkedList<T>::Search(T elem)
{
    if(length == 0)
        return false;
    if(moveToStart())
        do {
            if(curr->element == elem)
                return true;
        } while (next());
    return false;
}

template <class T>
void LinkedList<T>::PutFirstToLast()
{
  if(length < 2)
    return;
  ListNode<T>* temp = head->next;
  head->next->prev = nullptr;
  head->next = nullptr;
  head->prev = tail;
  tail->next = head;
  tail = head;
  head = temp;
}

template <class T>
void LinkedList<T>::Update(T elem)
{
    if(Search(elem))
        curr->element = elem;
}

template <class T>
void LinkedList<T>::Clear()
{
    if(length == 0)
        return;
    ListNode<T> * temp = head;

    while(temp != nullptr)
    {
        head = head->next;
        delete temp;
        temp = head;
    }

    head = curr = tail = nullptr;

    length = 0;

}

// MARK: - Lib: OSU FILE

class OSUNote {
public:
    int col;
    int time;
    int type;
    int endtime;
};

class OSUNote_wihtoutcol {
public:
    int time;
    int type;
    int endtime;
};

const std::string OSUGAMEFILE = "64,192,1152,5,0,0:0:0:0:\n192,192,1444,1,0,0:0:0:0:\n320,192,1737,1,0,0:0:0:0:\n448,192,2030,128,0,2322:0:0:0:0:\n64,192,2322,128,0,2615:0:0:0:0:\n320,192,2615,1,0,0:0:0:0:\n192,192,2761,128,0,3054:0:0:0:0:\n448,192,3054,1,0,0:0:0:0:\n64,192,3347,1,0,0:0:0:0:\n448,192,3493,128,0,3786:0:0:0:0:\n320,192,3786,128,0,4078:0:0:0:0:\n192,192,4078,128,0,4371:0:0:0:0:\n64,192,4371,128,0,4664:0:0:0:0:\n448,192,4664,128,0,4956:0:0:0:0:\n192,192,4956,1,0,0:0:0:0:\n320,192,5103,128,0,5395:0:0:0:0:\n64,192,5395,1,0,0:0:0:0:\n448,192,5688,1,0,0:0:0:0:\n192,192,5834,128,0,6127:0:0:0:0:\n320,192,6127,128,0,6420:0:0:0:0:\n64,192,6420,128,0,6712:0:0:0:0:\n448,192,6712,128,0,7005:0:0:0:0:\n192,192,7005,128,0,7298:0:0:0:0:\n320,192,7298,1,0,0:0:0:0:\n64,192,7444,128,0,7737:0:0:0:0:\n192,192,7737,1,0,0:0:0:0:\n448,192,7883,1,0,0:0:0:0:\n64,192,8030,1,0,0:0:0:0:\n320,192,8176,128,0,8469:0:0:0:0:\n192,192,8469,128,0,8761:0:0:0:0:\n448,192,8761,128,0,9054:0:0:0:0:\n320,192,9054,128,0,9347:0:0:0:0:\n64,192,9347,128,0,9639:0:0:0:0:\n192,192,9639,128,0,9932:0:0:0:0:\n320,192,9932,128,0,10225:0:0:0:0:\n448,192,10225,128,0,10517:0:0:0:0:\n192,192,10664,1,0,0:0:0:0:\n320,192,10883,1,0,0:0:0:0:\n64,192,11103,128,0,11395:0:0:0:0:\n448,192,11395,128,0,11688:0:0:0:0:\n64,192,11688,1,0,0:0:0:0:\n192,192,11688,1,0,0:0:0:0:\n320,192,11981,1,0,0:0:0:0:\n448,192,12273,1,0,0:0:0:0:\n64,192,12566,1,0,0:0:0:0:\n320,192,12712,1,0,0:0:0:0:\n192,192,12859,1,0,0:0:0:0:\n448,192,13151,1,0,0:0:0:0:\n320,192,13444,1,0,0:0:0:0:\n192,192,13664,1,0,0:0:0:0:\n64,192,13883,1,0,0:0:0:0:\n448,192,14030,1,0,0:0:0:0:\n192,192,14322,1,0,0:0:0:0:\n320,192,14615,1,0,0:0:0:0:\n448,192,14908,1,0,0:0:0:0:\n64,192,15054,1,0,0:0:0:0:\n192,192,15200,1,0,0:0:0:0:\n320,192,15493,1,0,0:0:0:0:\n64,192,15639,1,0,0:0:0:0:\n448,192,15786,1,0,0:0:0:0:\n192,192,16078,1,0,0:0:0:0:\n320,192,16225,1,0,0:0:0:0:\n64,192,16371,128,0,16664:0:0:0:0:\n448,192,16664,1,0,0:0:0:0:\n192,192,16810,1,0,0:0:0:0:\n320,192,16956,1,0,0:0:0:0:\n64,192,17103,1,0,0:0:0:0:\n448,192,17249,128,0,17542:0:0:0:0:\n192,192,17542,128,0,17834:0:0:0:0:\n448,192,17834,1,0,0:0:0:0:\n64,192,17981,1,0,0:0:0:0:\n320,192,18127,128,0,18420:0:0:0:0:\n192,192,18420,128,0,18712:0:0:0:0:\n64,192,18712,128,0,19005:0:0:0:0:\n448,192,19005,128,0,19298:0:0:0:0:\n192,192,19298,128,0,19591:0:0:0:0:\n320,192,19591,128,0,19883:0:0:0:0:\n64,192,19883,5,0,0:0:0:0:\n192,192,19883,1,0,0:0:0:0:\n448,192,20176,1,0,0:0:0:0:\n320,192,20176,1,0,0:0:0:0:\n64,192,20469,1,0,0:0:0:0:\n448,192,20469,1,0,0:0:0:0:\n192,192,20761,1,0,0:0:0:0:\n320,192,20761,1,0,0:0:0:0:\n448,192,21054,128,0,21347:0:0:0:0:\n64,192,21347,1,0,0:0:0:0:\n192,192,21493,1,0,0:0:0:0:\n320,192,21639,128,0,21932:0:0:0:0:\n64,192,21932,1,0,0:0:0:0:\n448,192,22078,1,0,0:0:0:0:\n64,192,22225,1,0,0:0:0:0:\n320,192,22371,1,0,0:0:0:0:\n448,192,22517,1,0,0:0:0:0:\n192,192,22517,128,0,22810:0:0:0:0:\n320,192,22810,1,0,0:0:0:0:\n64,192,22956,1,0,0:0:0:0:\n448,192,23103,128,0,23395:0:0:0:0:\n192,192,23395,1,0,0:0:0:0:\n64,192,23688,1,0,0:0:0:0:\n448,192,23981,1,0,0:0:0:0:\n320,192,24273,128,0,24566:0:0:0:0:\n64,192,24566,128,0,24859:0:0:0:0:\n192,192,24859,128,0,25151:0:0:0:0:\n448,192,25151,128,0,25444:0:0:0:0:\n320,192,25444,128,0,25737:0:0:0:0:\n64,192,25737,1,0,0:0:0:0:\n448,192,25883,1,0,0:0:0:0:\n192,192,26030,1,0,0:0:0:0:\n320,192,26322,1,0,0:0:0:0:\n64,192,26469,1,0,0:0:0:0:\n448,192,26615,128,0,26908:0:0:0:0:\n192,192,26908,1,0,0:0:0:0:\n320,192,27054,1,0,0:0:0:0:\n64,192,27200,1,0,0:0:0:0:\n448,192,27347,1,0,0:0:0:0:\n192,192,27493,128,0,27786:0:0:0:0:\n320,192,27786,128,0,28078:0:0:0:0:\n64,192,28078,128,0,28371:0:0:0:0:\n192,192,28371,128,0,28664:0:0:0:0:\n448,192,28664,128,0,28956:0:0:0:0:\n64,192,28956,128,0,29249:0:0:0:0:\n320,192,29249,5,0,0:0:0:0:\n448,192,29249,1,0,0:0:0:0:\n64,192,29542,1,0,0:0:0:0:\n192,192,29542,1,0,0:0:0:0:\n448,192,29834,1,0,0:0:0:0:\n64,192,29834,1,0,0:0:0:0:\n320,192,30127,1,0,0:0:0:0:\n192,192,30273,1,0,0:0:0:0:\n448,192,30420,128,0,31005:0:0:0:0:\n64,192,31005,128,0,31298:0:0:0:0:\n320,192,31298,1,0,0:0:0:0:\n192,192,31444,1,0,0:0:0:0:\n64,192,31591,1,0,0:0:0:0:\n320,192,31737,1,0,0:0:0:0:\n448,192,31737,1,0,0:0:0:0:\n64,192,31956,1,0,0:0:0:0:\n192,192,31956,1,0,0:0:0:0:\n320,192,32176,1,0,0:0:0:0:\n448,192,32322,1,0,0:0:0:0:\n192,192,32469,1,0,0:0:0:0:\n320,192,32615,1,0,0:0:0:0:\n64,192,32761,128,0,33054:0:0:0:0:\n448,192,33054,1,0,0:0:0:0:\n192,192,33200,1,0,0:0:0:0:\n320,192,33347,128,0,33639:0:0:0:0:\n64,192,33639,1,0,0:0:0:0:\n448,192,33786,1,0,0:0:0:0:\n192,192,33932,128,0,34225:0:0:0:0:\n320,192,34225,128,0,34517:0:0:0:0:\n448,192,34517,128,0,34810:0:0:0:0:\n64,192,34810,128,0,35103:0:0:0:0:\n320,192,35103,1,0,0:0:0:0:\n448,192,35103,1,0,0:0:0:0:\n192,192,35395,1,0,0:0:0:0:\n320,192,35688,1,0,0:0:0:0:\n448,192,35981,1,0,0:0:0:0:\n192,192,36273,1,0,0:0:0:0:\n64,192,36566,1,0,0:0:0:0:\n320,192,36859,1,0,0:0:0:0:\n192,192,37151,1,0,0:0:0:0:\n448,192,37444,128,0,37737:0:0:0:0:\n320,192,37737,128,0,38030:0:0:0:0:\n192,192,38030,128,0,38322:0:0:0:0:\n64,192,38322,128,0,38615:0:0:0:0:\n320,192,38615,128,0,39200:0:0:0:0:\n192,192,39200,128,0,39493:0:0:0:0:\n320,192,39493,1,0,0:0:0:0:\n448,192,39493,1,0,0:0:0:0:\n64,192,39786,1,0,0:0:0:0:\n448,192,39786,1,0,0:0:0:0:\n192,192,40005,1,0,0:0:0:0:\n320,192,40225,1,0,0:0:0:0:\n64,192,40371,1,0,0:0:0:0:\n448,192,40517,1,0,0:0:0:0:\n192,192,40664,128,0,40956:0:0:0:0:\n320,192,40956,1,0,0:0:0:0:\n448,192,41249,1,0,0:0:0:0:\n64,192,41395,1,0,0:0:0:0:\n320,192,41542,1,0,0:0:0:0:\n320,192,41834,128,0,42127:0:0:0:0:\n64,192,42127,128,0,42420:0:0:0:0:\n192,192,42420,128,0,42712:0:0:0:0:\n448,192,42712,1,0,0:0:0:0:\n320,192,42859,1,0,0:0:0:0:\n192,192,43005,1,0,0:0:0:0:\n64,192,43151,1,0,0:0:0:0:\n448,192,43298,128,0,43591:0:0:0:0:\n192,192,43591,1,0,0:0:0:0:\n320,192,43737,1,0,0:0:0:0:\n64,192,43883,1,0,0:0:0:0:\n192,192,44103,1,0,0:0:0:0:\n320,192,44322,1,0,0:0:0:0:\n64,192,44469,128,0,44908:0:0:0:0:\n448,192,44908,1,0,0:0:0:0:\n192,192,45054,128,0,45493:0:0:0:0:\n320,192,45493,1,0,0:0:0:0:\n64,192,45639,1,0,0:0:0:0:\n448,192,45859,1,0,0:0:0:0:\n192,192,46078,1,0,0:0:0:0:\n320,192,46225,128,0,46664:0:0:0:0:\n64,192,46664,1,0,0:0:0:0:\n448,192,46810,1,0,0:0:0:0:\n192,192,47030,1,0,0:0:0:0:\n448,192,47249,1,0,0:0:0:0:\n64,192,47395,1,0,0:0:0:0:\n192,192,47542,1,0,0:0:0:0:\n320,192,47688,1,0,0:0:0:0:\n448,192,47834,1,0,0:0:0:0:\n64,192,47981,128,0,48273:0:0:0:0:\n320,192,48273,1,0,0:0:0:0:\n448,192,48420,128,0,48712:0:0:0:0:\n64,192,48712,1,0,0:0:0:0:\n320,192,48859,1,0,0:0:0:0:\n192,192,49005,1,0,0:0:0:0:\n64,192,49151,1,0,0:0:0:0:\n448,192,49151,1,0,0:0:0:0:\n320,192,49371,1,0,0:0:0:0:\n192,192,49591,1,0,0:0:0:0:\n448,192,49737,1,0,0:0:0:0:\n64,192,49883,1,0,0:0:0:0:\n320,192,50030,128,0,50322:0:0:0:0:\n192,192,50322,1,0,0:0:0:0:\n448,192,50615,1,0,0:0:0:0:\n320,192,50761,1,0,0:0:0:0:\n192,192,50908,128,0,51200:0:0:0:0:\n448,192,51200,128,0,51493:0:0:0:0:\n64,192,51493,128,0,51786:0:0:0:0:\n320,192,51786,1,0,0:0:0:0:\n192,192,51932,1,0,0:0:0:0:\n448,192,52078,1,0,0:0:0:0:\n320,192,52298,1,0,0:0:0:0:\n64,192,52517,1,0,0:0:0:0:\n192,192,52664,1,0,0:0:0:0:\n448,192,52810,1,0,0:0:0:0:\n320,192,52956,1,0,0:0:0:0:\n192,192,53103,1,0,0:0:0:0:\n64,192,53249,128,0,53834:0:0:0:0:\n192,192,53542,1,0,0:0:0:0:\n320,192,53834,1,0,0:0:0:0:\n448,192,53834,1,0,0:0:0:0:\n320,192,54127,128,0,54420:0:0:0:0:\n64,192,54566,1,0,0:0:0:0:\n448,192,54712,1,0,0:0:0:0:\n320,192,54859,1,0,0:0:0:0:\n192,192,55005,1,0,0:0:0:0:\n192,192,55298,128,0,55591:0:0:0:0:\n320,192,55737,1,0,0:0:0:0:\n192,192,55883,1,0,0:0:0:0:\n64,192,56030,1,0,0:0:0:0:\n448,192,56176,128,0,56469:0:0:0:0:\n192,192,56469,1,0,0:0:0:0:\n64,192,56615,128,0,56908:0:0:0:0:\n320,192,56908,1,0,0:0:0:0:\n192,192,57054,1,0,0:0:0:0:\n448,192,57200,1,0,0:0:0:0:\n320,192,57347,128,0,57639:0:0:0:0:\n64,192,57639,1,0,0:0:0:0:\n192,192,57786,1,0,0:0:0:0:\n448,192,57932,128,0,58225:0:0:0:0:\n64,192,58225,128,0,58517:0:0:0:0:\n320,192,58517,128,0,59688:0:0:0:0:\n192,192,59688,128,0,60273:0:0:0:0:\n448,192,60273,128,0,60566:0:0:0:0:\n64,192,60273,1,0,0:0:0:0:\n192,192,60566,1,0,0:0:0:0:\n320,192,60712,1,0,0:0:0:0:\n64,192,60859,5,0,0:0:0:0:\n448,192,60859,1,0,0:0:0:0:\n64,192,61151,1,0,0:0:0:0:\n320,192,61444,1,0,0:0:0:0:\n320,192,61737,1,0,0:0:0:0:\n64,192,62030,1,0,0:0:0:0:\n192,192,62030,1,0,0:0:0:0:\n192,192,62322,1,0,0:0:0:0:\n448,192,62615,1,0,0:0:0:0:\n448,192,62908,1,0,0:0:0:0:\n320,192,63200,1,0,0:0:0:0:\n64,192,63200,1,0,0:0:0:0:\n320,192,63493,1,0,0:0:0:0:\n192,192,63786,1,0,0:0:0:0:\n192,192,64078,1,0,0:0:0:0:\n448,192,64371,1,0,0:0:0:0:\n64,192,64371,1,0,0:0:0:0:\n64,192,64664,1,0,0:0:0:0:\n320,192,64956,1,0,0:0:0:0:\n320,192,65249,1,0,0:0:0:0:\n192,192,65395,1,0,0:0:0:0:\n64,192,65542,1,0,0:0:0:0:\n448,192,65542,1,0,0:0:0:0:\n448,192,65834,1,0,0:0:0:0:\n192,192,66127,1,0,0:0:0:0:\n192,192,66420,1,0,0:0:0:0:\n320,192,66712,1,0,0:0:0:0:\n448,192,66712,1,0,0:0:0:0:\n320,192,67005,1,0,0:0:0:0:\n64,192,67298,1,0,0:0:0:0:\n64,192,67591,1,0,0:0:0:0:\n192,192,67883,1,0,0:0:0:0:\n448,192,67883,1,0,0:0:0:0:\n448,192,68176,1,0,0:0:0:0:\n64,192,68469,1,0,0:0:0:0:\n192,192,68615,1,0,0:0:0:0:\n320,192,68761,1,0,0:0:0:0:\n448,192,68908,1,0,0:0:0:0:\n64,192,69054,1,0,0:0:0:0:\n64,192,69347,1,0,0:0:0:0:\n320,192,69639,1,0,0:0:0:0:\n192,192,69786,1,0,0:0:0:0:\n448,192,69932,128,0,70225:0:0:0:0:\n64,192,70225,128,0,70517:0:0:0:0:\n192,192,70517,1,0,0:0:0:0:\n320,192,70664,1,0,0:0:0:0:\n448,192,70810,1,0,0:0:0:0:\n192,192,71030,1,0,0:0:0:0:\n320,192,71249,1,0,0:0:0:0:\n64,192,71395,1,0,0:0:0:0:\n192,192,71615,1,0,0:0:0:0:\n320,192,71834,1,0,0:0:0:0:\n448,192,71981,128,0,72273:0:0:0:0:\n64,192,72273,128,0,72566:0:0:0:0:\n320,192,72566,128,0,72859:0:0:0:0:\n192,192,72859,1,0,0:0:0:0:\n448,192,73005,1,0,0:0:0:0:\n64,192,73151,1,0,0:0:0:0:\n320,192,73298,1,0,0:0:0:0:\n64,192,73444,128,0,73737:0:0:0:0:\n448,192,73737,1,0,0:0:0:0:\n320,192,73883,1,0,0:0:0:0:\n192,192,74030,1,0,0:0:0:0:\n64,192,74176,1,0,0:0:0:0:\n448,192,74322,128,0,74615:0:0:0:0:\n192,192,74615,128,0,74908:0:0:0:0:\n320,192,74908,1,0,0:0:0:0:\n448,192,74908,1,0,0:0:0:0:\n64,192,75200,1,0,0:0:0:0:\n320,192,75493,1,0,0:0:0:0:\n192,192,75786,1,0,0:0:0:0:\n448,192,76078,1,0,0:0:0:0:\n320,192,76371,1,0,0:0:0:0:\n64,192,76664,1,0,0:0:0:0:\n192,192,76956,1,0,0:0:0:0:\n448,192,77249,1,0,0:0:0:0:\n320,192,77395,1,0,0:0:0:0:\n64,192,77542,128,0,77834:0:0:0:0:\n192,192,77834,1,0,0:0:0:0:\n320,192,77981,1,0,0:0:0:0:\n448,192,78127,128,0,78420:0:0:0:0:\n64,192,78420,128,0,79298:0:0:0:0:\n320,192,79298,1,0,0:0:0:0:\n448,192,79298,128,0,79591:0:0:0:0:\n192,192,79591,128,0,80469:0:0:0:0:\n448,192,80469,128,0,80761:0:0:0:0:\n64,192,80761,128,0,81054:0:0:0:0:\n320,192,81054,1,0,0:0:0:0:\n192,192,81200,1,0,0:0:0:0:\n448,192,81493,128,0,81786:0:0:0:0:\n192,192,81786,1,0,0:0:0:0:\n320,192,81932,128,0,82371:0:0:0:0:\n64,192,82371,1,0,0:0:0:0:\n448,192,82517,1,0,0:0:0:0:\n192,192,82737,1,0,0:0:0:0:\n320,192,82956,1,0,0:0:0:0:\n64,192,83103,128,0,83542:0:0:0:0:\n192,192,83542,1,0,0:0:0:0:\n448,192,83688,128,0,83981:0:0:0:0:\n320,192,83981,128,0,84273:0:0:0:0:\n192,192,84273,128,0,84566:0:0:0:0:\n448,192,84712,1,0,0:0:0:0:\n64,192,84859,128,0,85151:0:0:0:0:\n448,192,85298,1,0,0:0:0:0:\n320,192,85444,1,0,0:0:0:0:\n192,192,85591,1,0,0:0:0:0:\n64,192,85737,1,0,0:0:0:0:\n448,192,85883,128,0,86176:0:0:0:0:\n192,192,86176,1,0,0:0:0:0:\n320,192,86322,1,0,0:0:0:0:\n64,192,86469,1,0,0:0:0:0:\n192,192,86615,128,0,86908:0:0:0:0:\n448,192,87054,1,0,0:0:0:0:\n64,192,87200,1,0,0:0:0:0:\n320,192,87347,1,0,0:0:0:0:\n192,192,87493,1,0,0:0:0:0:\n448,192,87639,1,0,0:0:0:0:\n320,192,87786,128,0,88078:0:0:0:0:\n64,192,88225,128,0,88517:0:0:0:0:\n448,192,88517,1,0,0:0:0:0:\n192,192,88664,1,0,0:0:0:0:\n320,192,88810,1,0,0:0:0:0:\n448,192,88956,128,0,89395:0:0:0:0:\n64,192,89395,1,0,0:0:0:0:\n320,192,89542,128,0,89834:0:0:0:0:\n192,192,89834,128,0,90127:0:0:0:0:\n448,192,90273,1,0,0:0:0:0:\n64,192,90420,128,0,90859:0:0:0:0:\n448,192,90859,1,0,0:0:0:0:\n320,192,91005,1,0,0:0:0:0:\n192,192,91151,1,0,0:0:0:0:\n64,192,91298,1,0,0:0:0:0:\n448,192,91298,128,0,92176:0:0:0:0:\n320,192,91591,1,0,0:0:0:0:\n192,192,91883,1,0,0:0:0:0:\n64,192,92176,128,0,92469:0:0:0:0:\n320,192,92469,128,0,92761:0:0:0:0:\n192,192,92761,1,0,0:0:0:0:\n448,192,92908,1,0,0:0:0:0:\n64,192,93054,1,0,0:0:0:0:\n320,192,93200,128,0,93493:0:0:0:0:\n192,192,93493,1,0,0:0:0:0:\n448,192,93639,128,0,94225:0:0:0:0:\n64,192,93932,1,0,0:0:0:0:\n192,192,94225,128,0,94517:0:0:0:0:\n320,192,94517,1,0,0:0:0:0:\n448,192,94664,1,0,0:0:0:0:\n64,192,94810,128,0,95395:0:0:0:0:\n320,192,95103,1,0,0:0:0:0:\n192,192,95395,1,0,0:0:0:0:\n320,192,95542,1,0,0:0:0:0:\n64,192,95688,1,0,0:0:0:0:\n448,192,95688,128,0,95981:0:0:0:0:\n192,192,95981,1,0,0:0:0:0:\n320,192,95981,1,0,0:0:0:0:\n64,192,96273,1,0,0:0:0:0:\n448,192,96566,1,0,0:0:0:0:\n320,192,96566,1,0,0:0:0:0:\n192,192,96859,1,0,0:0:0:0:\n64,192,97151,1,0,0:0:0:0:\n448,192,97151,1,0,0:0:0:0:\n320,192,97444,1,0,0:0:0:0:\n64,192,97737,1,0,0:0:0:0:\n192,192,97737,1,0,0:0:0:0:\n320,192,98030,1,0,0:0:0:0:\n448,192,98322,1,0,0:0:0:0:\n192,192,98322,1,0,0:0:0:0:\n64,192,98615,1,0,0:0:0:0:\n320,192,98908,1,0,0:0:0:0:\n448,192,98908,1,0,0:0:0:0:\n192,192,99200,1,0,0:0:0:0:\n64,192,99493,1,0,0:0:0:0:\n320,192,99493,1,0,0:0:0:0:\n448,192,99786,1,0,0:0:0:0:\n192,192,100078,1,0,0:0:0:0:\n64,192,100078,1,0,0:0:0:0:\n320,192,100371,1,0,0:0:0:0:\n64,192,100664,128,0,100956:0:0:0:0:\n448,192,100664,1,0,0:0:0:0:\n320,192,100956,128,0,101249:0:0:0:0:\n64,192,101249,1,0,0:0:0:0:\n192,192,101395,1,0,0:0:0:0:\n320,192,101542,1,0,0:0:0:0:\n448,192,101688,1,0,0:0:0:0:\n64,192,101981,1,0,0:0:0:0:\n448,192,102127,1,0,0:0:0:0:\n192,192,102420,128,0,102712:0:0:0:0:\n64,192,102712,128,0,103005:0:0:0:0:\n448,192,103005,1,0,0:0:0:0:\n320,192,103151,1,0,0:0:0:0:\n192,192,103298,128,0,103591:0:0:0:0:\n64,192,103591,1,0,0:0:0:0:\n448,192,103737,1,0,0:0:0:0:\n320,192,103883,128,0,104176:0:0:0:0:\n64,192,104176,1,0,0:0:0:0:\n192,192,104395,1,0,0:0:0:0:\n320,192,104615,1,0,0:0:0:0:\n64,192,104761,1,0,0:0:0:0:\n448,192,104981,1,0,0:0:0:0:\n320,192,105200,1,0,0:0:0:0:\n192,192,105347,1,0,0:0:0:0:\n448,192,105566,1,0,0:0:0:0:\n64,192,105786,1,0,0:0:0:0:\n320,192,105932,1,0,0:0:0:0:\n192,192,106078,1,0,0:0:0:0:\n448,192,106225,128,0,106517:0:0:0:0:\n64,192,106517,1,0,0:0:0:0:\n192,192,106664,1,0,0:0:0:0:\n320,192,106810,1,0,0:0:0:0:\n448,192,106956,1,0,0:0:0:0:\n64,192,107103,128,0,107395:0:0:0:0:\n320,192,107395,128,0,107688:0:0:0:0:\n192,192,107688,1,0,0:0:0:0:\n448,192,107981,1,0,0:0:0:0:\n320,192,107981,1,0,0:0:0:0:\n64,192,108273,1,0,0:0:0:0:\n192,192,108566,1,0,0:0:0:0:\n448,192,108566,1,0,0:0:0:0:\n320,192,108859,1,0,0:0:0:0:\n64,192,109151,1,0,0:0:0:0:\n192,192,109151,1,0,0:0:0:0:\n448,192,109444,1,0,0:0:0:0:\n192,192,109737,1,0,0:0:0:0:\n320,192,109883,1,0,0:0:0:0:\n64,192,110030,1,0,0:0:0:0:\n192,192,110322,1,0,0:0:0:0:\n448,192,110322,1,0,0:0:0:0:\n320,192,110615,1,0,0:0:0:0:\n448,192,110908,1,0,0:0:0:0:\n64,192,110908,1,0,0:0:0:0:\n192,192,111200,1,0,0:0:0:0:\n320,192,111493,1,0,0:0:0:0:\n64,192,111639,1,0,0:0:0:0:\n448,192,111786,1,0,0:0:0:0:\n192,192,111932,1,0,0:0:0:0:\n320,192,112078,128,0,112371:0:0:0:0:\n64,192,112371,1,0,0:0:0:0:\n448,192,112517,128,0,112810:0:0:0:0:\n192,192,112810,1,0,0:0:0:0:\n64,192,112956,1,0,0:0:0:0:\n320,192,113103,1,0,0:0:0:0:\n192,192,113249,128,0,113542:0:0:0:0:\n448,192,113542,1,0,0:0:0:0:\n320,192,113688,1,0,0:0:0:0:\n192,192,113834,1,0,0:0:0:0:\n64,192,113981,1,0,0:0:0:0:\n448,192,114127,128,0,114420:0:0:0:0:\n64,192,114420,1,0,0:0:0:0:\n192,192,114420,128,0,114712:0:0:0:0:\n448,192,114712,1,0,0:0:0:0:\n320,192,114712,128,0,115005:0:0:0:0:\n64,192,115005,1,0,0:0:0:0:\n448,192,115151,1,0,0:0:0:0:\n192,192,115298,128,0,115591:0:0:0:0:\n320,192,115591,1,0,0:0:0:0:\n64,192,115737,1,0,0:0:0:0:\n448,192,115883,128,0,116176:0:0:0:0:\n192,192,116176,1,0,0:0:0:0:\n320,192,116322,1,0,0:0:0:0:\n64,192,116469,128,0,116761:0:0:0:0:\n448,192,116761,1,0,0:0:0:0:\n192,192,116908,1,0,0:0:0:0:\n320,192,117054,128,0,117347:0:0:0:0:\n64,192,117347,1,0,0:0:0:0:\n448,192,117493,1,0,0:0:0:0:\n192,192,117639,128,0,117932:0:0:0:0:\n320,192,117932,1,0,0:0:0:0:\n64,192,118078,1,0,0:0:0:0:\n448,192,118225,128,0,118517:0:0:0:0:\n192,192,118517,128,0,118810:0:0:0:0:\n320,192,118810,128,0,119103:0:0:0:0:\n64,192,119103,128,0,119395:0:0:0:0:\n448,192,119395,1,0,0:0:0:0:\n192,192,119688,1,0,0:0:0:0:\n320,192,119981,1,0,0:0:0:0:\n448,192,120127,1,0,0:0:0:0:\n64,192,120273,1,0,0:0:0:0:\n192,192,120420,1,0,0:0:0:0:\n320,192,120566,1,0,0:0:0:0:\n320,192,120859,1,0,0:0:0:0:\n64,192,121151,1,0,0:0:0:0:\n64,192,121444,1,0,0:0:0:0:\n448,192,121737,1,0,0:0:0:0:\n192,192,122030,1,0,0:0:0:0:\n320,192,122176,1,0,0:0:0:0:\n448,192,122322,1,0,0:0:0:0:\n64,192,122615,1,0,0:0:0:0:\n320,192,122761,1,0,0:0:0:0:\n64,192,122908,128,0,123200:0:0:0:0:\n192,192,123200,128,0,123493:0:0:0:0:\n320,192,123493,128,0,123786:0:0:0:0:\n448,192,123786,128,0,124078:0:0:0:0:\n192,192,124078,1,0,0:0:0:0:\n320,192,124225,1,0,0:0:0:0:\n64,192,124371,1,0,0:0:0:0:\n192,192,124517,1,0,0:0:0:0:\n448,192,124664,1,0,0:0:0:0:\n320,192,124956,1,0,0:0:0:0:\n192,192,125249,1,0,0:0:0:0:\n448,192,125542,1,0,0:0:0:0:\n64,192,125688,1,0,0:0:0:0:\n192,192,125834,128,0,126127:0:0:0:0:\n320,192,126127,128,0,126420:0:0:0:0:\n448,192,126420,1,0,0:0:0:0:\n64,192,126566,1,0,0:0:0:0:\n192,192,126712,1,0,0:0:0:0:\n320,192,126859,1,0,0:0:0:0:\n448,192,127005,1,0,0:0:0:0:\n320,192,127298,1,0,0:0:0:0:\n192,192,127444,1,0,0:0:0:0:\n64,192,127591,128,0,128176:0:0:0:0:\n448,192,128176,128,0,128761:0:0:0:0:\n320,192,128761,1,0,0:0:0:0:\n192,192,128908,1,0,0:0:0:0:\n64,192,129054,128,0,129347:0:0:0:0:\n448,192,129347,1,0,0:0:0:0:\n320,192,129493,1,0,0:0:0:0:\n192,192,129639,128,0,129932:0:0:0:0:\n64,192,129932,1,0,0:0:0:0:\n448,192,130078,1,0,0:0:0:0:\n320,192,130225,128,0,130517:0:0:0:0:\n192,192,130517,1,0,0:0:0:0:\n64,192,130664,1,0,0:0:0:0:\n320,192,130810,1,0,0:0:0:0:\n192,192,130956,1,0,0:0:0:0:\n448,192,131103,128,0,135786:0:0:0:0:";

enum OSUFileReadingState {
    X,
    Y,
    Time,
    Type,
    HitSound,
    EndTime,
    HitSample,
    NEWLINE,
};

class OSUFile {
public:
    LinkedList<OSUNote_wihtoutcol> notes[4];
    void load(std::string notedata) {
        // Load file
        OSUNote note;
        int state = OSUFileReadingState::X;
        std::string buffer = "";
        for(int i = 0; i < notedata.length(); i++) {
            char c = notedata[i];
            if(c == '\n' && state == OSUFileReadingState::NEWLINE) {
                notes[note.col].Append({
                    note.time,
                    note.type,
                    note.endtime,
                });
                printf("Note: %d, %d, %d, %d\n", note.col, note.time, note.type, note.endtime);
                note = OSUNote();
                state = OSUFileReadingState::X;
                continue;
            }
            if(state == OSUFileReadingState::X) {
                if(c == ',') {
                    int colData = string2int(buffer);
                    note.col = (int)(colData * 4 / 512);
                    buffer = "";
                    state = OSUFileReadingState::Y;
                } else {
                    buffer += c;
                }
                continue;
            }
            if(state == OSUFileReadingState::Y) {
                if(c == ',') {
                    state = OSUFileReadingState::Time;
                }
                continue;
            }
            if(state == OSUFileReadingState::Time) {
                if(c == ',') {
                    note.time = string2int(buffer);
                    buffer = "";
                    state = OSUFileReadingState::Type;
                } else {
                    buffer += c;
                }
                continue;
            }
            if(state == OSUFileReadingState::Type) {
                if(c == ',') {
                    note.type = string2int(buffer);
                    buffer = "";
                    state = OSUFileReadingState::HitSound;
                } else {
                    buffer += c;
                }
                continue;
            }
            if(state == OSUFileReadingState::HitSound) {
                if(c == ',') {
                    state = OSUFileReadingState::EndTime;
                }
                continue;
            }
            if(state == OSUFileReadingState::EndTime) {
                if(c == ':') {
                    note.endtime = string2int(buffer);
                    buffer = "";
                    state = OSUFileReadingState::NEWLINE;
                } else {
                    buffer += c;
                }
                continue;
            }
        }
    
        printf("Loaded %d, %d, %d, %d notes\n", notes[0].getLength(), notes[1].getLength(), notes[2].getLength(), notes[3].getLength());
    }
};

// MARK: - Scene: Intro

enum IntroSceneKeyframe {
    INTRONone,
    Wel,
    Come,
    To,
    OSU,
    TrianglesStarted,
    TrianglesEnded,
};

class IntroSceneTriangle {
public:
    Keyframes keyframes;
    CnavasAPI *api;
    
    int centerX;
    int centerY;
    int size;
    bool filled;

    void setup() {
        centerX = randint(50, CANVAS_WIDTH - 50);
        centerY = randint((CANVAS_HEIGHT - 50) / 2, (CANVAS_HEIGHT + 50) / 2);
        size = randint(20, 40);
        filled = randint(0, 1) == 1;
        keyframes.reset();
    }

    void update(bool shouldReset = true) {
        // 40ms
        int deltaTime = keyframes.deltaTime();
        if(deltaTime < __INTRO_SCENE_MS_) {
            IColorType color = rgb_darken(255, 255, 255, (1.0 - (float)deltaTime / __INTRO_SCENE_MS_));
            if(filled) {
                api->drawTriangle(centerX, centerY - size, centerX - size, centerY + size, centerX + size, centerY + size, color);
            } else {
                api->drawTriangleOutline(centerX, centerY - size, centerX - size, centerY + size, centerX + size, centerY + size, color);
            }
        }
        if(deltaTime > __INTRO_SCENE_MS_ && shouldReset) {
            setup();
        }
    }
};

class IntroScene {
public:
    Keyframes keyframes;
    CnavasAPI *api;
    IntroSceneKeyframe currentKeyframe = IntroSceneKeyframe::INTRONone;
    IntroSceneTriangle triangles[4];

    IntroScene(CnavasAPI *api) {
        this->api = api;
        for(int i = 0; i < 4; i++) {
            triangles[i].api = api;
            triangles[i].setup();
        }
    }

    // 344F Wel | 0s
    // 355F Come | 0.18s
    // 374F To // 0.5s
    // 387F OSU // 0.71s
    // 396F Triangles Started 0.86s
    // 419F Triangles Ended 1.25s
    void update() {
        int deltaTime = keyframes.deltaTime();
        if(deltaTime < 180) {
            if(currentKeyframe == IntroSceneKeyframe::INTRONone) {
                currentKeyframe = IntroSceneKeyframe::Wel;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Wel", rgb(255, 255, 255));
            }
            return;
        }
        if(deltaTime < 320) {
            if(currentKeyframe == IntroSceneKeyframe::Wel) {
                currentKeyframe = IntroSceneKeyframe::Come;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome", rgb(255, 255, 255));
            }
            return;
        }
        if(deltaTime < 500) {
            if(currentKeyframe == IntroSceneKeyframe::Come) {
                currentKeyframe = IntroSceneKeyframe::To;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to", rgb(255, 255, 255));
            }
            return;
        }
        if(deltaTime < 710) {
            if(currentKeyframe == IntroSceneKeyframe::To) {
                currentKeyframe = IntroSceneKeyframe::OSU;
                api->clear();
                api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to OSU!", rgb(255, 255, 255));
            }
            return;
        }
        if(deltaTime >= 860 && deltaTime < __INTRO_SCENE_END_MS_) {
            api->clear();
            api->drawIntroText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "Welcome to OSU!", rgb(255, 255, 255));
            int enableStep__ = (deltaTime - 860) / 40;
            int enableStep = enableStep__ > 3 ? 3 : enableStep__;
            for(int i = 0; i < enableStep; i++) {
                triangles[i].update(
                    deltaTime < __INTRO_SCENE_TRIANGLE_NOSPAWN_MS_
                );
            }
        }
    }
};

// MARK: - Scene: Title

enum TitleSceneKeyframe {
    TitleNone,
    SubmenuOpen,
    SubmenuClose,
};

enum TitleSceneCursor {
    Title__Play,
    Title__Download,
    Title__Options,
};

class TitleSceneButtonWidthHandler {
public:
    float width = 0;
    float targetWidth = 0;
    float movementACC = 0.0;

    DeltaTime deltaTime;

    int getWidth() {
        // printf("Width: %f\n", width);
        return width;
    }

    void setTargetWidth(float targetWidth) {
        this->targetWidth = targetWidth;
        movementACC = (targetWidth - width) / 400.0;
    }

    void update() {
        if(width < targetWidth) {
            width += movementACC * (float)deltaTime.deltaTime();
            if(width > targetWidth) {
                width = targetWidth;
            }
        }
        if(width > targetWidth) {
            width += movementACC * (float)deltaTime.deltaTime();
            if(width < targetWidth) {
                width = targetWidth;
            }
        }
    }
};

class TitleScene {
public:
    CnavasAPI *api;
    TitleScene(CnavasAPI *api) {
        this->api = api;
    }

    TitleSceneKeyframe submenuOpenAnimationType = TitleSceneKeyframe::TitleNone;
    float submenuOpenProgress = 0.0;

    DeltaTime deltaTime;

    bool noneTitleDrawn = false;
    bool lastRButton = false;

    bool last0Button = false;
    bool last1Button = false;

    TitleSceneCursor cursorFocused = TitleSceneCursor::Title__Play;

    TitleSceneButtonWidthHandler playButtonWidthHandler;
    TitleSceneButtonWidthHandler downloadButtonWidthHandler;
    TitleSceneButtonWidthHandler optionsButtonWidthHandler;


    int buttonsWidth = (CANVAS_WIDTH - 20) / 4 * 3;
    int focusedWidth = ((CANVAS_WIDTH - 20) / 4 * 3) / 16 * 7;
    int unfocusedWidth = ((CANVAS_WIDTH - 20) / 4 * 3) / 16 * 4.5;

    // PLAY #64C
    // DOWNLOAD #EA0
    // OPTIONS #AC0

    void updateButtonTargetWidth() {
        playButtonWidthHandler.setTargetWidth(
            cursorFocused == TitleSceneCursor::Title__Play ? focusedWidth : unfocusedWidth
        );
        downloadButtonWidthHandler.setTargetWidth(
            cursorFocused == TitleSceneCursor::Title__Download ? focusedWidth : unfocusedWidth
        );
        optionsButtonWidthHandler.setTargetWidth(
            cursorFocused == TitleSceneCursor::Title__Options ? focusedWidth : unfocusedWidth
        );
    }

    void update() {
        playButtonWidthHandler.update();
        downloadButtonWidthHandler.update();
        optionsButtonWidthHandler.update();

        if(!noneTitleDrawn && submenuOpenAnimationType == TitleSceneKeyframe::TitleNone) {
            api->clear();
            api->drawCircle(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, 80, rgb(255, 255, 255)); 
            api->drawCircle(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, 70, rgb(0xF1, 0x60, 0xA1)); 
            api->drawOsuLogoText(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, "OSU!", rgb(255, 255, 255));
            noneTitleDrawn = true;

            printf("Scene:Title - NoneDrawn\n");
            return;
        }
        if(submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen) {
            if(submenuOpenProgress < 1.0) {
                api->clear();
                submenuOpenProgress += deltaTime.deltaTime() / 150.0;
                printf("Scene:Title - SubmenuOpen Progress: %f\n", submenuOpenProgress);
                submenuOpenProgress = submenuOpenProgress > 1.0 ? 1.0 : submenuOpenProgress;

                // OSU LOGO ANIMATION
                int logoMovX = ((10 + CANVAS_WIDTH / 8) - (CANVAS_WIDTH / 2)) * submenuOpenProgress + (CANVAS_WIDTH / 2);
                int logoMovSz = (CANVAS_WIDTH / 8 - 80) * submenuOpenProgress + 80;
                int logoInnerSz = (CANVAS_WIDTH * 7 / 64 - 70) * submenuOpenProgress + 70;
                
                api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, logoMovSz, rgb(255, 255, 255)); 
                api->drawCircle(logoMovX, CANVAS_HEIGHT / 2, logoInnerSz, rgb(0xF1, 0x60, 0xA1)); 
                if(submenuOpenProgress < 0.5) {
                    api->drawOsuLogoText(logoMovX, CANVAS_HEIGHT / 2, "OSU!", rgb(255, 255, 255));
                } else {
                    api->drawOsuLogoTextSmaller(logoMovX, CANVAS_HEIGHT / 2, "OSU!", rgb(255, 255, 255));
                }

                int baseX = ((10 + CANVAS_WIDTH / 4) - -buttonsWidth) * submenuOpenProgress + -buttonsWidth;
                int animatedFocusedWidth = (focusedWidth - 0) * submenuOpenProgress + 0;
                int animatedUnfocusedWidth = (unfocusedWidth - 0) * submenuOpenProgress + 0;
                int buttonsHeight = CANVAS_WIDTH / 4;
                int buttonsY = CANVAS_HEIGHT / 2 - buttonsHeight / 2;

                api->drawRect(      baseX                                , buttonsY, playButtonWidthHandler.getWidth()  , buttonsHeight, rgb(0x66, 0x44, 0xCC));
                api->drawButtonName(baseX + focusedWidth / 2, buttonsY + buttonsHeight / 2, "PLAY", rgb(255, 255, 255));
                api->drawRect(      baseX + focusedWidth                 , buttonsY, downloadButtonWidthHandler.getWidth(), buttonsHeight, rgb(0xEE, 0xAA, 0x00));
                api->drawButtonName(baseX + focusedWidth + unfocusedWidth / 2, buttonsY + buttonsHeight / 2, "DOWN", rgb(255, 255, 255));
                api->drawRect(      baseX + focusedWidth + unfocusedWidth, buttonsY, optionsButtonWidthHandler.getWidth(), buttonsHeight, rgb(0xAA, 0xCC, 0x00));
                api->drawButtonName(baseX + focusedWidth + unfocusedWidth + unfocusedWidth / 2, buttonsY + buttonsHeight / 2, "OPT", rgb(255, 255, 255));
            } else {
                // OSU LOGO
                api->clear();
                api->drawCircle(10 + CANVAS_WIDTH / 8, CANVAS_HEIGHT / 2, CANVAS_WIDTH / 8, rgb(255, 255, 255)); 
                api->drawCircle(10 + CANVAS_WIDTH / 8, CANVAS_HEIGHT / 2, CANVAS_WIDTH * 7 / 64, rgb(0xF1, 0x60, 0xA1)); 
                api->drawOsuLogoTextSmaller(10 + CANVAS_WIDTH / 8, CANVAS_HEIGHT / 2, "OSU!", rgb(255, 255, 255));

                // BUTTONS
                int baseX = (10 + CANVAS_WIDTH / 4);
                int buttonsY = CANVAS_HEIGHT / 2 - CANVAS_WIDTH / 4 / 2;
                int textY = buttonsY + CANVAS_WIDTH / 4 / 2;
                api->drawRect(      baseX                                , buttonsY, playButtonWidthHandler.getWidth()  , CANVAS_WIDTH / 4, rgb(0x66, 0x44, 0xCC));
                api->drawButtonName(baseX + playButtonWidthHandler.getWidth() / 2, textY, "PLAY", rgb(255, 255, 255));
                api->drawRect(      baseX + playButtonWidthHandler.getWidth() , buttonsY, downloadButtonWidthHandler.getWidth(), CANVAS_WIDTH / 4, rgb(0xEE, 0xAA, 0x00));
                api->drawButtonName(baseX + playButtonWidthHandler.getWidth() + downloadButtonWidthHandler.getWidth() / 2, textY, "DOWN", rgb(255, 255, 255));
                api->drawRect(      baseX + playButtonWidthHandler.getWidth() + downloadButtonWidthHandler.getWidth(), buttonsY, optionsButtonWidthHandler.getWidth(), CANVAS_WIDTH / 4, rgb(0xAA, 0xCC, 0x00));
                api->drawButtonName(baseX + playButtonWidthHandler.getWidth() + downloadButtonWidthHandler.getWidth() + optionsButtonWidthHandler.getWidth() / 2, textY, "OPT", rgb(255, 255, 255));
            }
        }


        if(lastRButton != buttonPressed(2)) {
            lastRButton = buttonPressed(2);
            if(lastRButton) {
                if(submenuOpenAnimationType == TitleSceneKeyframe::TitleNone) {
                    submenuOpenAnimationType = TitleSceneKeyframe::SubmenuOpen;
                    cursorFocused = TitleSceneCursor::Title__Play;
                    submenuOpenProgress = 0.0;

                    playButtonWidthHandler.setTargetWidth(focusedWidth);
                    downloadButtonWidthHandler.setTargetWidth(unfocusedWidth);
                    optionsButtonWidthHandler.setTargetWidth(unfocusedWidth);

                    deltaTime.deltaTime();
                    printf("Scene:Title - SubmenuOpen Started\n");
                }
            }
        }

        if(last0Button != buttonPressed(0)) {
            last0Button = buttonPressed(0);
            if(last0Button) {
                if(submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen) {
                    if(cursorFocused == TitleSceneCursor::Title__Play) {
                        cursorFocused = TitleSceneCursor::Title__Options;
                    }
                    else if(cursorFocused == TitleSceneCursor::Title__Download) {
                        cursorFocused = TitleSceneCursor::Title__Play;
                    }
                    else if(cursorFocused == TitleSceneCursor::Title__Options) {
                        cursorFocused = TitleSceneCursor::Title__Download;
                    }

                    updateButtonTargetWidth();
                }
            }
        }
        if(last1Button != buttonPressed(1)) {
            last1Button = buttonPressed(1);
            if(last1Button) {
                if(submenuOpenAnimationType == TitleSceneKeyframe::SubmenuOpen) {
                    if(cursorFocused == TitleSceneCursor::Title__Play) {
                        cursorFocused = TitleSceneCursor::Title__Download;
                    }
                    else if(cursorFocused == TitleSceneCursor::Title__Download) {
                        cursorFocused = TitleSceneCursor::Title__Options;
                    }
                    else if(cursorFocused == TitleSceneCursor::Title__Options) {
                        cursorFocused = TitleSceneCursor::Title__Play;
                    }

                    updateButtonTargetWidth();
                }
            }
        }
    }
};

// MARK: - Main Data Structures

CnavasAPI api;
IntroScene introScene(&api);
TitleScene titleScene(&api);

enum Scene {
    Intro,
    Title,
};

Scene currentScene = Scene::Intro;

void update() {
    if(currentScene == Scene::Intro) {
        introScene.update();
        if(introScene.keyframes.deltaTime() > __INTRO_SCENE_END_MS_) {
            currentScene = Scene::Title;
        }
        return;
    }
    if(currentScene == Scene::Title) {
        titleScene.update();
        return;
    }
}

// MARK: - Emscripten Entry Point

int main() {
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackPress);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackRelease);
    emscripten_set_main_loop(update, 0, 1);
    return 0;
}
