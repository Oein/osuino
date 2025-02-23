#include "./mem.cpp"
class FS
{
public:
    IVector<IStringType> list()
    {
        IVector<IStringType> result;
        result.push_back("/1.osu");
        return result;
    }

    IStringType get(IStringType filename)
    {
        if (filename == "/1.osu")
            return OSUGAMEFILE1;
        return "";
    }
};

