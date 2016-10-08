#include <algorithm>
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>

using TID = std::thread::id;
using ThreadClock = std::unordered_map<TID, size_t>;

std::atomic<int> ConcurrentAccessCount{ 0 };

struct ShadowState
{
    TID AccessorID;
    size_t AccessorClock;
    bool IsWrite;
};


std::unordered_map<void*, ShadowState> MemoryState;
std::unordered_map<TID, ThreadClock> ThreadStates;

// Assumes correct locking discipline. This is a race condition detector NOT a deadlock detector.
class Mutex
{
public:
    void Lock()
    {
        mMutex.lock();
        const TID thisTID = std::this_thread::get_id();
        ThreadClock& accessorClock = ThreadStates[thisTID];

        ++accessorClock[thisTID];
        
        // During a receive event, we synchronize the accessor clock with the mutex clock.
        // Afterwards, both will have the most up tot date vector clock possible.
        mClock.insert(accessorClock.begin(), accessorClock.end());
        for (auto& thTime : mClock)
        {
            const size_t maxClockTime = std::max(accessorClock[thTime.first], thTime.second);
            thTime.second = maxClockTime;
            accessorClock[thTime.first] = maxClockTime;
        }
    }

    void Unlock()
    {
        const TID thisTID = std::this_thread::get_id();
        ThreadClock& accessorClock = ThreadStates[thisTID];
        
        // Update the accessor clock and mutex clock
        ++accessorClock[thisTID];
        mClock[thisTID] = accessorClock[thisTID];

        mMutex.unlock();
    }

private:
    std::mutex mMutex;
    ThreadClock mClock;
};

template <typename T>
T Read(T&& val)
{
    const TID currentTID = std::this_thread::get_id();

    ++ThreadStates[currentTID].Clock[currentTID];

    ShadowState newState{ currentTID, ThreadStates[thisTID].Clock[thisTID], false };

    CheckForConcurrentAccess(MemoryState[&val], newState);
    
    MemoryState[&val] = newState;

    return val;
}

template <typename T>
void Write(T& oldVal, T&& newVal)
{
    const TID currentTID = std::this_thread::get_id();
    
    ++ThreadStates[currentTID][currentTID];

    ShadowState newState{ currentTID, ThreadStates[currentTID][currentTID], true };

    CheckForConcurrentAccess(MemoryState[&oldVal], newState);

    MemoryState[&oldVal] = newState;

    oldVal = newVal;
}

void CheckForConcurrentAccess(const ShadowState& oldState, const ShadowState& newState)
{
    // You can't race with yourself
    if (oldState.AccessorID == newState.AccessorID)
    {
        return;
    }

    // For a data race to occurs we need at least one write access
    if (!oldState.IsWrite && !newState.IsWrite)
    {
        return;
    }

    // A data race happens when not every element of the current accessor's vector clock is less
    // than the corresponding element in the previous accessor's vector clock.
    auto happensBefore = [](ThreadClock& firstEventClock, ThreadClock& secondEventClock) 
    {
        return std::all_of(firstEventClock.begin(), firstEventClock.end(), 
                           [&secondEventClock](const std::pair<TID, size_t>& firstThreadTime) 
                           { 
                               return firstThreadTime.second <= secondEventClock[firstThreadTime.first];
                           });
    };

    ThreadClock& currentAccessorClock = ThreadStates[newState.AccessorID];
    ThreadClock& previousAccessorClock = ThreadStates[oldState.AccessorID];

    // Can we order the two accesses with a happens-before relation?
    if (!happensBefore(currentAccessorClock, previousAccessorClock) 
        && !happensBefore(previousAccessorClock, currentAccessorClock))
    {
        ConcurrentAccessCount.fetch_add(1);
    }
}

int main()
{
    int data = 0;
    std::vector<std::thread> threadVec;

    Mutex mut;

    for (int i = 0; i < 3; ++i)
    {
        threadVec.emplace_back([&data, &mut]() 
                               { 
                                   //mut.Lock();
                                   Write(data, data + 1); 
                                   //mut.Unlock();
                               });
    }

    for (auto& th : threadVec)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    std::cout << "Concurrent accesses found: " << ConcurrentAccessCount.load() << std::endl;
}
