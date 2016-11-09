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
    struct Match
    {
        Match() = default;
        Match(unsigned start, unsigned length, const std::string& pattern) 
            : Start{ start }, Length{ length }, Pattern{ pattern } { }

        unsigned Start;
        unsigned Length;
        std::string Pattern;
    };

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

    std::vector<Match> SearchMultithreadCPU(const std::string& str)
    {
        std::vector<std::thread> searchThreads;
        const size_t nbThreads = std::thread::hardware_concurrency();
        const size_t strSize = str.size();
        std::vector<Match> matches;

        // We'll start as many threads as there are hardware threads
        for (size_t iThread = 0; iThread < nbThreads; ++iThread)
        {
            searchThreads.emplace_back([this, iThread, &nbThreads, &strSize, &str, &matches]() 
            {
                size_t currentStrIdx = iThread;
                
                while (currentStrIdx < strSize)
                {
                    int currentState = 0;

                    for (size_t iStr = currentStrIdx; iStr < strSize; ++iStr)
                    {
                        currentState = mTransitionMatrix[currentState][str[iStr]];

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

                        // If we reach this point, that means we have a complete match
                        for (size_t iDict = 0; iDict < mDictionary.size(); ++iDict)
                        {
                            if (mOutputTable[currentState] & (1 << iDict))
                            {
                                const std::string& matchedPattern = mDictionary[iDict];
                                matches.emplace_back(currentStrIdx, iStr - currentStrIdx, matchedPattern);
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

        return matches;
    }

    std::vector<Match> SearchAMP(const std::string& str) const
    {
        const unsigned strSize = str.size();
        const unsigned dictSize = mDictionary.size();
        concurrency::array_view<const int, 1> outputTableView(mOutputTable.size(), mOutputTable.data());
        
        // C++AMP needs dense data. Since the transition table isn't dense, we have to flatten it before moving it to the GPU
        std::vector<int> flatTransitionTable;
        flatTransitionTable.reserve(mTransitionMatrix.size() * NB_ASCII_VALUES);
        for (const auto& row : mTransitionMatrix)
        {
            flatTransitionTable.insert(flatTransitionTable.end(), row.begin(), row.end());
        }
        concurrency::array_view<const int, 2> transitionTableView(mTransitionMatrix.size(), NB_ASCII_VALUES, flatTransitionTable);

        // To not waste any space, we'll pack the characters of the string argument into integers
        std::vector<int> strIntVec;
        strIntVec.reserve((strSize + 3) / 4);
        for (size_t iStr = 0; iStr < strSize; iStr += 4)
        {
            int packedChars = 0;
            packedChars = str[iStr];
            packedChars <<= 8;
            packedChars |= str[iStr+1];
            packedChars <<= 8;
            packedChars |= str[iStr+2];
            packedChars <<= 8;
            packedChars |= str[iStr+3];
            strIntVec.push_back(packedChars);
        }

        concurrency::array_view<const int, 1> strView(strIntVec.size(), strIntVec.data());
        
        std::vector<int> results(strSize * dictSize, -1);
        concurrency::array_view<int, 2> resultsView(strSize, dictSize, results);

        concurrency::parallel_for_each(concurrency::extent<1>(strSize),
                                       [transitionTableView, outputTableView, resultsView, strView, strSize](concurrency::index<1> idx) restrict(amp)
                                       {
                                           const int strStartIdx = idx[0];
                                           int currentState = 0;
                                           int index = 0;
                                           int currentChar = 0;

                                           for (unsigned iStr = strStartIdx; iStr < strSize; ++iStr)
                                           {
                                               index = iStr / 4;
                                               currentChar = strView[index] >> (8 * (3-(iStr % 4))) & 0xFF;

                                               currentState = transitionTableView[currentState][currentChar];

                                               // Nothing matched, shamefur dispray, commit sudoku
                                               if (currentState == -1)
                                               {
                                                   break;
                                               }

                                               // Matched a character but not in a matching state, let's continue
                                               if (outputTableView[currentState] == 0)
                                               {
                                                   continue;
                                               }

                                               // If we reach this point, that means we have a complete match
                                               for (int iDict = 0; iDict < resultsView.extent[1]; ++iDict)
                                               {
                                                   if (outputTableView[currentState] & (1 << iDict))
                                                   {
                                                       resultsView[strStartIdx][iDict] = iStr - strStartIdx;
                                                   }
                                               }
                                           }
                                       });
        resultsView.synchronize();

        std::vector<Match> matches;
        for (size_t iStr = 0; iStr < strSize; ++iStr)
        {
            for (size_t iDict = 0; iDict < dictSize; ++iDict)
            {
                const int matchLength = results[(iStr * dictSize) + iDict];

                if (matchLength > 0)
                {
                    const std::string& matchedPattern = mDictionary[iDict];
                    matches.emplace_back(iStr + 1, matchLength, matchedPattern);
                }
            }
        }

        return matches;
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
        int states = 1;
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
    //std::random_device device;
    //std::default_random_engine engine(device());
    //std::uniform_int_distribution<> dist(0, NB_ASCII_VALUES);
    //std::string dataStr(1000, 0);
    //std::generate_n(dataStr.begin(), 1000, [&dist, &engine]() { return dist(engine); });

    const std::string pattern = "he|she|his|hers";
    //std::string pattern;
    //std::generate_n(std::back_inserter(pattern), 10, [&dist, &engine]() { return dist(engine); });

    StringSearcher searcher{ pattern };
        
    {
        std::smatch match;
        std::regex re{ pattern };
        std::string s = "ahishers";

        auto start = std::chrono::high_resolution_clock::now();
        std::regex_search(s, match, re);
        //std::regex_search(dataStr, match, re);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "std::regex: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        searcher.SearchMultithreadCPU("ahishers");
        //searcher.SearchCPU(dataStr);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "CPU PFAC: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        searcher.SearchAMP("ahishers");
        //searcher.SearchGPU(dataStr);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "GPU PFAC: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    }
}