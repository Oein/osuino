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
    
    typename std::vector<T>::iterator begin()
    {
        return data.begin();
    }
    typename std::vector<T>::iterator end()
    {
        return data.end();
    }
    
    // erase with begin and end
    void erase(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end)
    {
        data.erase(begin, end);
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

template <typename T>
class IMapUL
{
public:
    std::map<unsigned long, T> data;

    unsigned long size()
    {
        return data.size();
    }
    void insert(unsigned long key, T value)
    {
        data.insert(std::pair<unsigned long, T>(key, value));
    }
    T at(unsigned long key)
    {
        return data.at(key);
    }
    void clear()
    {
        data.clear();
    }
    bool has(unsigned long key)
    {
        return data.find(key) != data.end();
    }
};
