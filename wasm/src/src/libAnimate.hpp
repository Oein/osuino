
class Keyframes
{
public:
    unsigned long long startTime;

    Keyframes()
    {
        startTime = emscripten_get_now();
    }

    int deltaTime()
    {
        return emscripten_get_now() - startTime;
    }

    void reset()
    {
        startTime = emscripten_get_now();
    }
};

class DeltaTime
{
public:
    unsigned long long lastTime;

    DeltaTime()
    {
        lastTime = emscripten_get_now();
    }

    int deltaTime()
    {
        unsigned long long currentTime = emscripten_get_now();
        int deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        return deltaTime;
    }
};

class Timer
{
public:
    unsigned long long startTime;

    Timer()
    {
        startTime = emscripten_get_now();
    }

    void reset()
    {
        startTime = emscripten_get_now();
    }

    int deltaTime()
    {
        return emscripten_get_now() - startTime;
    }
};

class AnimatedData
{
public:
    float now = 0;
    float target = 0;
    float acc = 0.0;

    float divider = 40.0;

    DeltaTime deltaTime;

    float current()
    {
        return now;
    }

    void setTarget(float newTarget)
    {
        this->target = newTarget;
        acc = (newTarget - now) / divider;
    }

    bool update()
    {
#ifdef _DO_NOT_ANIMATE_
        if (now != target)
        {
            now = target;
            return true;
        }
        return false;
#else
        float deltaTime = (float)this->deltaTime.deltaTime();
        if (now < target)
        {
            now += acc * deltaTime;
            if (now > target)
            {
                now = target;
            }
            return true;
        }
        if (now > target)
        {
            now += acc * deltaTime;
            if (now < target)
            {
                now = target;
            }
            return true;
        }
        return false;
#endif
    }

    void set(int nTarget)
    {
        now = nTarget;
        target = nTarget;
    }

    AnimatedData(float now)
    {
        this->now = now;
        this->target = now;
    }
    AnimatedData()
    {
        this->now = 0;
        this->target = 0;
    }
    AnimatedData(float now, float divider)
    {
        this->now = now;
        this->target = now;
        this->divider = divider;
    }
};

class RepeatedButton
{
public:
    bool lastButton = false;
    bool buttonPressed = false;
    bool repeat = false;

    Timer timer;
    Timer repeatTimer;

    bool get(bool buttonState)
    {
        if (!buttonPressed && buttonState)
        {
            buttonPressed = true;
            timer.reset();
            return true;
        }
        if (buttonPressed && !buttonState)
        {
            buttonPressed = false;
            repeat = false;
            return false;
        }
        if (buttonPressed && buttonState)
        {
            if (timer.deltaTime() < _REPEAT_POST_DELAY_ && !repeat)
            {
                return false;
            }
            if (timer.deltaTime() > _REPEAT_POST_DELAY_ && !repeat)
            {
                repeat = true;
                repeatTimer.reset();
                return true;
            }
            if (repeat && repeatTimer.deltaTime() > _REPEAT_CYCLE_)
            {
                repeatTimer.reset();
                return true;
            }
        }
        return false;
    }
};
