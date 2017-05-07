#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/labeled_graph.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;
using ThreadID = std::thread::id;

class WaitForGraph
{
private:
    using GraphT = boost::labeled_graph<boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>, ThreadID>;

public:
    static WaitForGraph& get()
    {
        static WaitForGraph graph;
        return graph;
    }

public:
    WaitForGraph() = default;
    WaitForGraph(const WaitForGraph&) = delete;
    WaitForGraph& operator=(const WaitForGraph&) = delete;
    WaitForGraph(WaitForGraph&&) = delete;
    WaitForGraph& operator=(WaitForGraph&&) = delete;

public:
    void AddNode(const ThreadID& tid) 
    {
        std::lock_guard<std::mutex>{mMutex};
        boost::add_vertex(tid, mGraph);
    }

    void AddEdge(const ThreadID& fromTID, const ThreadID& toTID)
    {
        std::lock_guard<std::mutex>{mMutex};

        boost::add_edge_by_label(fromTID, toTID, mGraph);

        std::vector<int> sccs(boost::num_vertices(mGraph));
        if (boost::connected_components(mGraph, sccs.data()) != 0)
        {
            std::cerr << "DEADLOCK DETECTED:\n"
                      << "Thread 1 ID: " << fromTID << "\n"
                      << "Thread 2 ID: " << toTID << "\n";
        }
    }
    
    void RemoveNode(const ThreadID& tid)
    {
        std::lock_guard<std::mutex>{mMutex};
        boost::clear_vertex_by_label(tid, mGraph);
    }

private:
    std::mutex mMutex;
    GraphT mGraph;
};

class DDMutex
{
public:
    void Lock()
    {
        ThreadID currTID = std::this_thread::get_id();
        WaitForGraph::get().AddNode(currTID);

        if (mIsOwned)
        {
            WaitForGraph::get().AddEdge(mOwnerID, currTID);
        }

        mMutex.lock();
        mOwnerID = currTID;
        mIsOwned = true;
    }

    void Unlock()
    {
        WaitForGraph::get().RemoveNode(mOwnerID);
        mIsOwned = false;
        mMutex.unlock();
    }

private:
    std::mutex mMutex;
    ThreadID mOwnerID;
    bool mIsOwned = false;
};

int main()
{
    DDMutex m1, m2;

    std::vector<std::thread> threads;

    threads.emplace_back([&m1, &m2]() { m1.Lock(); m2.Lock(); std::this_thread::sleep_for(1s); m1.Unlock(); m2.Unlock(); });
    threads.emplace_back([&m1, &m2]() { m2.Lock(); m1.Lock(); std::this_thread::sleep_for(1s); m2.Unlock(); m1.Unlock(); });

    for (auto& th : threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    return 0;
}