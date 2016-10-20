#include <iostream>
#include <string>
#include <vector>

std::vector<int> Preprocess(const std::string& pattern)
{
    const size_t patternSize = pattern.size();
    std::vector<int> skipTable(256, patternSize);

    for (size_t iPattern = 0; iPattern < patternSize - 1; ++iPattern)
    {
        skipTable[pattern[iPattern]] = patternSize - 1 - iPattern;
    }

    return skipTable;
}

int Search(const std::string& pattern, const std::string& str)
{
    std::vector<int> skipTable = Preprocess(pattern);

    size_t skip = 0;
    const size_t patternSize = pattern.size();
    const size_t strSize = str.size();
    int i;

    while (strSize - skip >= patternSize)
    {
        i = patternSize - 1;
        while (str[skip + i] == pattern[i])
        {
            if (i == 0)
            {
                return skip;
            }
            --i;
        }
        skip += skipTable[str[skip + patternSize - 1]];
    }

    return -1;
}

int main()
{
    int idx = Search("needle", "It's like trying to find a needle in a haystack");
}
