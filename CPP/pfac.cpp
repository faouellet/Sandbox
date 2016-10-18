#include <array>
#include <iostream>
#include <string>
#include <vector>

// Regex engine implement Parallel Failureless Aho-Corasick string searching algorithm
class StringSearcher
{
public:
    StringSearcher()
    {
        AddState();
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
            const int charIdx = chr - 'a';

            if (mTransitionMatrix[currentState][charIdx] == -1)
            {
                mTransitionMatrix[currentState][charIdx] = mTransitionMatrix.size();

                currentState = mTransitionMatrix[currentState][charIdx];

                AddState();
            }
        }

        mOutputTable[currentState] |= (1 << mDictionary.size());
        mDictionary.push_back(pattern);
    }

    void Search(const std::string& str)
    {
        int currentState = 0;

        for (size_t i = 0; i < str.size(); i++)
        {
            currentState = mTransitionMatrix[currentState][str[i] - 'a'];

            // Nothing matched, shamefur dispray, commit sudoku
            if (currentState == -1)
            {
                return;
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
    }

private:
    void AddState() 
    {
        mTransitionMatrix.emplace_back();
        mTransitionMatrix.back().fill(-1); 

        mOutputTable.emplace_back(0);
    }

private:
    std::vector<std::array<int, 256>> mTransitionMatrix;
    std::vector<int> mOutputTable;
    std::vector<std::string> mDictionary;
};

int main()
{
    StringSearcher searcher;

    searcher.AddSearchPattern("he");
    searcher.AddSearchPattern("she");
    searcher.AddSearchPattern("his");
    searcher.AddSearchPattern("hers");

    searcher.Search("ahishers");
}