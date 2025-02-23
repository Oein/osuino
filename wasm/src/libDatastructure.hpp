#include <vector>

template <typename T>
class IVector
{
public:
    std::vector<T> data;
    unsigned long size()
    {
        return data.size();
    }
    void push_back(T value)
    {
        data.push_back(value);
    }
    T at(unsigned long index)
    {
        return data.at(index);
    }
    void clear()
    {
        data.clear();
    }
};

#include <map>

template <typename T>
class IMap
{
public:
    std::map<IStringType, T> data;

    unsigned long size()
    {
        return data.size();
    }
    void insert(IStringType key, T value)
    {
        data.insert(std::pair<IStringType, T>(key, value));
    }
    T at(IStringType key)
    {
        return data.at(key);
    }
    void clear()
    {
        data.clear();
    }
    bool has(IStringType key)
    {
        return data.find(key) != data.end();
    }
};
