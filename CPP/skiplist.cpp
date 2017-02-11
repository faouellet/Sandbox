#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

template <class KeyT, class ValueT>
class SkipList
{
    struct Node;

    using NodePtr = std::shared_ptr<Node>;
    using NodePtrList = std::vector<NodePtr>;

public:
    SkipList(long maxHeight, double probability) : mMaxHeight{ maxHeight }, mLevel{ 1 }, mProbability{ probability }
    {
        mHead = std::make_shared<Node>();
        mTail = std::make_shared<Node>();

        mHead->SuccNodes.resize(mMaxHeight, mTail);
    }

    void Delete(const KeyT& key)
    {
        NodePtr foundNode;
        NodePtrList update(mMaxHeight);

        if (Search(key, foundNode, &update))
        {
            for (long iLevel = 0; iLevel < mLevel; ++iLevel)
            {
                if (update[iLevel]->SuccNodes[iLevel] != foundNode)
                {
                    break;
                }
                update[iLevel]->SuccNodes[iLevel] = foundNode->SuccNodes[iLevel];
            }

            while ((mLevel > 1) && (mHead->SuccNodes[mLevel] == mTail))
            {
                --mLevel;
            }
        }
    }

    void Insert(const KeyT& key, ValueT val)
    {
        NodePtr foundNode;
        NodePtrList update(mMaxHeight);

        if (Search(key, foundNode, &update))
        {
            foundNode->Value = val;
            return;
        }
        else
        {
            long level = GetRandomLevel();
            if (level > mLevel)
            {
                std::fill(update.begin() + mLevel, update.begin() + level, mHead);
                mLevel = level;
            }

            NodePtr newNode = std::make_shared<Node>(key, val);

            for (long iLevel = 0; iLevel < mLevel; ++iLevel)
            {
                newNode->SuccNodes.emplace_back(update[iLevel]->SuccNodes[iLevel]);
                update[iLevel]->SuccNodes[iLevel] = newNode;
            }
        }
    }

    bool Search(const KeyT& searchKey, ValueT& val)
    {
        NodePtr foundNode;
        if (Search(searchKey, foundNode, nullptr))
        {
            val = foundNode->Value;
            return true;
        }
        return false;
    }

private:
    long GetRandomLevel() const
    {
        static std::mt19937 generator({});
        static std::bernoulli_distribution dist(mProbability);

        long level = 0;
        while (dist(generator) && level < mMaxHeight)
        {
            ++level;
        }

        return level;
    }

    bool Search(const KeyT& searchKey, NodePtr& foundNode, NodePtrList* nodesToUpdate)
    {
        NodePtr currNode = mHead;

        for (long iLevel = mLevel; iLevel >= 0; --iLevel)
        {
            while (currNode->SuccNodes[iLevel] != mTail && currNode->SuccNodes[iLevel]->Key < searchKey)
            {
                currNode = currNode->SuccNodes[iLevel];
            }
            if (nodesToUpdate != nullptr)
            {
                (*nodesToUpdate)[iLevel] = currNode;
            }
        }

        if (currNode->SuccNodes[0] == mTail)
        {
            return false;
        }

        foundNode = currNode->SuccNodes[0];

        return foundNode->Key == searchKey;
    }

private:
    struct Node
    {
        KeyT Key;
        ValueT Value;
        NodePtrList SuccNodes;
     
        Node() = default;
        Node(KeyT key, ValueT val) : Key{ key }, Value{ val }, SuccNodes{} {}
    };

    long mMaxHeight;
    long mLevel;
    double mProbability;
    NodePtr mHead;
    NodePtr mTail;
};

int main()
{
    SkipList<int, std::string> sl{10, 0.5};
    std::string s;
    sl.Search(1, s);
    
    sl.Insert(1, "a");
    sl.Insert(2, "b");
    sl.Insert(3, "c");
    sl.Search(1, s);
    
    sl.Insert(2, "e");
    sl.Search(2, s);
    
    sl.Delete(2);
    sl.Search(3, s);
    sl.Search(2, s);

    return 0;
}