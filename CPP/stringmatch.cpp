#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Trie
{
private:
    struct TrieNode
    {
        bool IsMatch;
        char Char;
        std::vector<std::unique_ptr<TrieNode>> Children;
    };

public: // We can't copy it, but we can move it
    Trie() : mRoot{ new TrieNode{ false, 'R', {} } } { }
    ~Trie() = default;

    Trie(const Trie& trie) = delete;
    Trie& operator=(const Trie& trie) = delete;

    Trie(Trie&& trie) { mRoot.reset(trie.mRoot.release()); }
    Trie& operator=(Trie&& trie) { mRoot.reset(trie.mRoot.release()); }

public:
    void AddString(const std::string& str)
    {
        if (str.empty())
        {
            return;
        }

        mDictionary.push_back(str);
        AddString(mRoot.get(), str);
    }

    bool Search(const std::string& str) const
    {
        bool found = false;

        for (const auto& child : mRoot->Children)
        {
            found |= Search(child.get(), str);
        }
    }

    template <typename StreamT>
    void Print(StreamT& stream) const
    {
        Print(mRoot.get(), stream, 0);
    }

private:
    void AddString(TrieNode* const root, const std::string& str)
    {
        if (str.empty())
        {
            return;
        }

        const char firstChar = str[0];

        for (const auto& child : root->Children)
        {
            if (child->Char == firstChar)
            {
                AddString(child.get(), str.substr(1));
                return;
            }
        }

        root->Children.emplace_back(new TrieNode{ str.size() == 1, firstChar, {} });
        auto& newNode = root->Children.back();

        AddString(newNode.get(), str.substr(1));
    }

    bool Search(const TrieNode* const root, const std::string& str) const
    {
    }

    template <typename StreamT>
    void Print(const TrieNode* const root, StreamT& stream, size_t level) const
    {
        for (size_t iIndent = 0; iIndent < level; ++iIndent)
        {
            stream << " ";
        }

        stream << root->Char << std::endl;

        ++level;

        for (const auto& child : root->Children)
        {
            Print(child.get(), stream, level);
        }
    }

private:
    std::unique_ptr<TrieNode> mRoot;
    std::vector<std::string> mDictionary;
};

int main()
{
    Trie t;

    t.AddString("he");
    t.AddString("she");
    t.AddString("his");
    t.AddString("hers");

    t.Print(std::cout);
}