#include <atomic>
#include <iostream>
#include <memory>

template <class ValueT>
class List
{
    struct Node;

    using NodePtr = std::shared_ptr<Node>;

public:
    void Delete(const ValueT& val)
    {
        NodePtr beforeNodeToDelete = SearchBeforeNode(val);
        NodePtr nodeToDelete = beforeNodeToDelete->Next;
        while ((nodeToDelete != nullptr) && 
            std::atomic_compare_exchange_strong(&(beforeNodeToDelete->Next),
                                                &(nodeToDelete), 
                                                nodeToDelete->Next));
    }

    void Insert(const ValueT& val)
    {
        auto newNode = std::make_shared<Node>(val);

        NodePtr beforeNode = SearchBeforeNode(val);
        NodePtr afterNode = beforeNode->Next;

        newNode->Next = afterNode;

        while (std::atomic_compare_exchange_strong(&(beforeNode->Next), &(newNode->Next), newNode))
        {
            afterNode = beforeNode->Next;
            ValueT currNextNodeVal = afterNode->Value;
            if (currNextNodeVal > val)
            {
                beforeNode = SearchBeforeNode(val);
                newNode->Next = beforeNode->Next;
            }
        }
    }

    bool Search(const ValueT& val)
    { 
        NodePtr beforeNode = SearchBeforeNode(val);
        NodePtr foundNode = beforeNode->Next;

        if (foundNode != nullptr && foundNode != mTail)
        {
            return foundNode->Value == val;
        }

        return false;
    }

private:
    NodePtr SearchBeforeNode(const ValueT& val)
    {
        NodePtr currNode = mHead;
        NodePtr nextNode = currNode->Next;

        while (nextNode != mTail && nextNode->Value < val)
        {
            currNode = nextNode;
            nextNode = currNode->Next;
        }

        return currNode;
    }

private:
    struct Node
    {
        ValueT Value;
        NodePtr Next;

        Node() = default;
        Node(const ValueT& val) : ValueT{ val }, Next{ nullptr } { }
    };

private:
    NodePtr mHead;
    NodePtr mTail;
};

int main()
{
    // Sanity tests with a single thread
    List<int> l;
    l.Insert(9);
    l.Search(9);
    l.Delete(9);

    // Multithreaded tests

    std::cout << "Hello\n";
}