#include <amp.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

const int NB_ASCII_VALUES = 128;

// Regex engine implement Parallel Failureless Aho-Corasick string searching algorithm
class StringSearcher
{
public:
    StringSearcher(const std::string& pattern)
    {
        AddState();

        // Since we implement alternative ("|"), we split the pattern using "|" as a delimiter
        std::stringstream stream{ pattern };
        std::string temp;
        std::vector<std::string> patternVec;
        
        while (getline(stream, temp, '|')) 
        {
            patternVec.push_back(temp);
        }

        // Each alternative pattern is then added to the regex engine
        for (const auto& pattern : patternVec)
        {
            AddSearchPattern(pattern);
        }
    }

    void SearchCPU(const std::string& str)
    {
        std::vector<std::thread> searchThreads;
        const size_t nbThreads = std::thread::hardware_concurrency();
        const size_t strSize = str.size();

        // We'll start as many threads as there are hardware threads
        for (size_t iThread = 0; iThread < nbThreads; ++iThread)
        {
            searchThreads.emplace_back([this, iThread, &nbThreads, &strSize, &str]() 
            {
                size_t currentStrIdx = iThread;
                
                while (currentStrIdx < strSize)
                {
                    int currentState = 0;

                    for (size_t i = currentStrIdx; i < strSize; ++i)
                    {
                        currentState = mTransitionMatrix[currentState][str[i] - 'a'];

                        // Nothing matched, shamefur dispray, commit sudoku
                        if (currentState == -1)
                        {
                            break;
                        }

                        // Matched a character but not in a matching state, let's continue
                        if (mOutputTable[currentState] == 0)
                        {
                            continue;
                        }

                        for (size_t j = 0; j < mDictionary.size(); ++j)
                        {
                            if (mOutputTable[currentState] & (1 << j))
                            {
                                std::cout << "Word " << mDictionary[j] << " appears from " << i - mDictionary[j].size() + 1 << " to " << i << std::endl;
                            }
                        }
                    }

                    currentStrIdx += nbThreads;
                }
            });
        }

        // Cleanup
        for (auto& th : searchThreads)
        {
            if (th.joinable())
            {
                th.join();
            }
        }
    }

    void SearchGPU(const std::string& str)
    {
        //concurrency::array_view<const int, 2> transitionTable(mTransitionMatrix.size(), NB_ASCII_VALUES, mTransitionMatrix.data());
        //concurrency::array_view<const int, 1> outputTable(NB_ASCII_VALUES, mOutputTable.data());
        //concurrency::array_view<const char, 1> strArr(str.size(), str.data());
        //
        //
        //concurrency::parallel_for_each(strArr.extent, 
        //                               [transitionTable, outputTable, strArr](concurrency::index<1> idx) restrict(amp) 
        //                               {
        //                                   return;
        //                               });
    }

private:
    void AddState() 
    {
        mTransitionMatrix.emplace_back();
        mTransitionMatrix.back().fill(-1); 

        mOutputTable.emplace_back(0);
    }

    void AddSearchPattern(const std::string& pattern)
    {
        if (pattern.empty())
        {
            return;
        }

        int currentState = 0;
        for (const auto& chr : pattern)
        {
            if (mTransitionMatrix[currentState][chr] == -1)
            {
                mTransitionMatrix[currentState][chr] = mTransitionMatrix.size();

                currentState = mTransitionMatrix[currentState][chr];

                AddState();
            }
        }

        mOutputTable[currentState] |= (1 << mDictionary.size());
        mDictionary.push_back(pattern);
    }

private:
    std::vector<std::array<int, NB_ASCII_VALUES>> mTransitionMatrix;
    std::vector<int> mOutputTable;
    std::vector<std::string> mDictionary;
};

int main()
{
    // Generate random string
    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_int_distribution<> dist(0, NB_ASCII_VALUES);
    std::string dataStr(1000, 0);
    std::generate_n(dataStr.begin(), 1000, [&dist, &engine]() { return dist(engine); });

    //const std::string pattern = "he|she|his|hers";
    std::string pattern;
    std::generate_n(std::back_inserter(pattern), 10, [&dist, &engine]() { return dist(engine); });

    StringSearcher searcher{ pattern };
    
    {
        auto start = std::chrono::high_resolution_clock::now();
        //searcher.SearchGPU("ahishers");
        searcher.SearchGPU(dataStr);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "GPU PFAC: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    }

    {
        std::smatch match;
        std::regex re{ pattern };

        auto start = std::chrono::high_resolution_clock::now();
        std::regex_search(dataStr, match, re);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "syd::regex: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        //searcher.SearchCPU("ahishers");
        searcher.SearchCPU(dataStr);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "CPU PFAC: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    }
}