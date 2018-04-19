#include <cassert>
#include <deque>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Transaction
{
public:

private:
    unsigned long m_min;
    unsigned long m_max;
};

class Data
{
public:
    std::string GetData() const 
    { 
        assert(!m_versions.empty());
        return m_versions.back().m_data; 
    }

private:
    struct DataVersion { std::string m_data; };

    std::deque<DataVersion> m_versions;
};

int main()
{
    std::map<int, Data> dataStore;
    unsigned int clockTime = 0;

    std::vector<std::thread> workers;

    for(auto& w : workers)
    {
        if(w.joinable())
        {
            w.join();
        }
    }
}