#include <iostream>
#include <string>
#include <vector>

class Trie
{
private:
    struct TrieNode
    {
        bool IsMatch;
        char Char;
        std::vector<size_t> ChildrenIdx;
    };

public: // We can't copy it, but we can move it
    Trie() : mNodes{ { false, 'R', {} } } { }

public:
    void AddString(const std::string& str)
    {
        if (str.empty())
        {
            return;
        }

        mDictionary.push_back(str);
        AddString(0, str);
    }

    std::pair<bool, size_t> Search(const std::string& str) const
    {
        return Search(0, str);
    }

    template <typename StreamT>
    void Print(StreamT& stream) const
    {
        Print(0, stream, 0);
    }

private:
    void AddString(size_t rootIdx, const std::string& str)
    {
        if (str.empty())
        {
            return;
        }

        const char firstChar = str[0];
        const auto& childrenIdx = mNodes[rootIdx].ChildrenIdx;

        for (const auto& childIdx : childrenIdx)
        {
            if (mNodes[childIdx].Char == firstChar)
            {
                AddString(childIdx, str.substr(1));
                return;
            }
        }

        mNodes.emplace_back(TrieNode{ str.size() == 1, firstChar, {} });
        const size_t newNodeIdx = mNodes.size() - 1;
        mNodes[rootIdx].ChildrenIdx.push_back(newNodeIdx);

        AddString(newNodeIdx, str.substr(1));
    }

    std::pair<bool, size_t> Search(size_t rootIdx, const std::string& str) const
    {
        if (str.empty())
        {
            return{ false, 0 };
        }

        const auto& childrenIdx = mNodes[rootIdx].ChildrenIdx;
        for (const auto& childIdx : childrenIdx)
        {
            if (mNodes[childIdx].Char == str.front())
            {
                if (mNodes[childIdx].IsMatch)
                {
                    return{ true,childIdx };
                }
                else
                {
                    return Search(childIdx, str.substr(1));
                }
            }
        }

        return{ false, 0 };
    }

    template <typename StreamT>
    void Print(size_t rootIdx, StreamT& stream, size_t level) const
    {
        const auto& node = mNodes[rootIdx];

        for (size_t iIndent = 0; iIndent < level; ++iIndent)
        {
            stream << " ";
        }

        stream << node.Char << std::endl;

        ++level;

        for (const auto& childIdx : node.ChildrenIdx)
        {
            Print(childIdx, stream, level);
        }
    }

private:
    std::vector<TrieNode> mNodes;
    std::vector<std::string> mDictionary;
};

int main()
{
    Trie t;

    t.AddString("he");
    t.AddString("she");
    t.AddString("his");
    t.AddString("hers");

    auto foundIdx = t.Search("he hit her");

    t.Print(std::cout);
}