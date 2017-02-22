#include <atomic>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <vector>

template <class ValueT>
class List
{
    struct Node;

    using NodePtr = std::shared_ptr<Node>;

public:
    List()
    {
        mTail = std::make_shared<Node>();
        mHead = std::make_shared<Node>( ValueT{}, mTail );
    }

    void Delete(const ValueT& val)
    {
        NodePtr beforeNodeToDelete = SearchBeforeNode(val);
        NodePtr nodeToDelete = beforeNodeToDelete->Next;
        while ((nodeToDelete != nullptr) && (nodeToDelete != mTail) &&
            !std::atomic_compare_exchange_strong(&(beforeNodeToDelete->Next),
                                                 &(nodeToDelete), 
                                                 nodeToDelete->Next));
    }

    void Insert(const ValueT& val)
    {
        auto newNode = std::make_shared<Node>(val);

        NodePtr beforeNode = SearchBeforeNode(val);
        NodePtr afterNode = beforeNode->Next;

        newNode->Next = afterNode;

        while (!std::atomic_compare_exchange_strong(&(beforeNode->Next), &(newNode->Next), newNode))
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
    template <class T>
    friend std::ostream& operator<<(std::ostream& stream, const List<T>& list);

private:
    struct Node
    {
        ValueT Value;
        NodePtr Next;

        Node() = default;
        Node(const ValueT& val) : Value{ val }, Next{ nullptr } { }
        Node(const ValueT& val, const NodePtr& next) : Value{ val }, Next{ next } { }
    };

private:
    NodePtr mHead;
    NodePtr mTail;
};

template <class ValueT>
std::ostream& operator<<(std::ostream& stream, const List<ValueT>& list)
{
    std::cout << "Head, ";

    List<ValueT>::NodePtr currNode = list.mHead;
    while ((currNode != nullptr) && (currNode != list.mTail))
    {
        currNode = currNode->Next;
        if (currNode != list.mTail)
        {
            std::cout << currNode->Value << ", ";
        }
    }

    std::cout << "Tail\n";

    return stream;
}

int main()
{
    // Sanity tests with a single thread
    List<int> l;
    std::cout << l;
    l.Insert(9);
    std::cout << l;
    l.Search(9);
    std::cout << l;
    l.Delete(9);
    std::cout << l;

    std::mt19937 generator({});
    std::bernoulli_distribution dist(0.5);

    // Multithreaded tests
    std::vector<std::thread> workers;
    for (int iWorker = 0; iWorker < 2; ++iWorker)
    {
        workers.emplace_back([&]() 
        {
            for (int iTask = 0; iTask < 100; ++iTask)
            {
                if (dist(generator))
                {
                    l.Insert(iTask);
                }
                else
                {
                    l.Delete(iTask);
                }
            }
        });
    }

    for (auto& w : workers)
    {
        if (w.joinable())
        {
            w.join();
        }
    }

    std::cout << l;
}