#include <atomic>
#include <cassert>
#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

/*
* \class ThreadPool
* \brief Grouping of threads that can be used to execute tasks in a parallel fashion
*/
class ThreadPool
{
private:
    /*
    * \class ITask
    * \brief Interface class to a task. This allows us to hold various templated
    *        tasks in the thread pool
    */
    class ITask
    {
    public:
        virtual ~ITask() { }
        virtual void Execute() = 0;
    };

    /*
    * \class ConcreteTask
    * \brief Actual task to be executed by the thread pool
    */
    template <class R>
    class ConcreteTask : public ITask
    {
    public:
        template <class F>
        explicit ConcreteTask(F&& f) : mTask{ f } { }

        virtual ~ConcreteTask() { }
        void Execute() override { mTask(); }
        std::future<R> GetFuture() { return mTask.get_future(); }

    private:
        std::packaged_task<R()> mTask;  /*!< Wrapper over the callable function representing the task */
    };

public:
    /*
    * \fn       ThreadPool
    * \brief    Default Ctor. Will create as many threads as there are hardware threads available.
    */
    ThreadPool() : mStop{ false }
    {
        mWorkerThreads.reserve(std::thread::hardware_concurrency());
        for (size_t i = 0; i < mWorkerThreads.capacity(); ++i)
        {
            mWorkerThreads.emplace_back([this]()
            {
                while (!mStop)
                {
                    auto task = GetNextTask();
                    if (task != nullptr)
                        task->Execute();
                }
            });
        }
    }

    /*
    * \fn       ~ThreadPool
    * \brief    Dtor
    */
    ~ThreadPool()
    {
        Stop();
        for (auto& t : mWorkerThreads)
            if (t.joinable())
                t.join();
    }

public:
    /*
    * \fn           AddTask
    * \brief        Add a task to be executed by the ThreadPool
    * \param func   Task to be executed
    * \return       Future through which the task return value will be accessible
    */
    template <class F>
    std::future<typename std::result_of<F()>::type> AddTask(F&& func)
    {
        std::lock(mTaskMutex, mNewTaskMutex);
        std::unique_lock<std::mutex>(mNewTaskMutex, std::adopt_lock);
        std::lock_guard<std::mutex>(mTaskMutex, std::adopt_lock);

        auto task = std::make_unique<ConcreteTask<typename std::result_of<F()>::type>>([func]() { return func(); });
        auto fut = task->GetFuture();

        mTasks.emplace_back(std::move(task));

        // Notifying the worker threads that a new task is avalaible
        mNewTasksCV.notify_one();
        return fut;
    }

    /*
    * \fn Stop
    * \brief Stop the ThreadPool. The tasks which were currently executing will be completed.
    */
    void Stop()
    {
        mStop = true;
        std::unique_lock<std::mutex> lock(mNewTaskMutex);
        // Notifying every thread waiting on new task that there won't be any
        mNewTasksCV.notify_all();
    }

private:
    /*
    * \fn Empty
    * \brief Indicate if the task queue is empty
    * \return True if the task queue is empty, else false
    */
    bool Empty() const
    {
        std::lock_guard<std::mutex> lg{ mTaskMutex };
        return mTasks.empty();
    }

    /*
    * \fn       GetNextTask
    * \brief    Get the next task to be executed
    */
    std::unique_ptr<ITask> GetNextTask()
    {
        if (Empty())
        {
            // No task in the queue. Waiting until the situation changes
            std::unique_lock<std::mutex> lock(mNewTaskMutex);
            mNewTasksCV.wait(lock, [this]() { return mStop || !Empty(); });
        }

        std::lock_guard<std::mutex> lg{ mTaskMutex };
        // Check that there actually is a task in the queue and that we're not closing shop
        if (mTasks.empty())
            return nullptr;

        // Get a task from the queue to feed the workers
        auto next = std::move(mTasks.front());
        mTasks.pop_front();
        return next;
    }

private:
    // Disallowing copy constructor and copy assignment because they don't make any sense for a thread pool
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    std::vector<std::thread> mWorkerThreads;    /*!< Threads on which the tasks will be executed */
    std::deque<std::unique_ptr<ITask>> mTasks;  /*!< Tasks to be executed */
    mutable std::mutex mTaskMutex;              /*!< Mutex controlling the access to the task queue */
    std::condition_variable mNewTasksCV;        /*!< Condition variable used to alert thread about incoming tasks */
    std::mutex mNewTaskMutex;                   /*!< Mutex controlling the access to the task queue */
    std::atomic<bool> mStop;                    /*!< Indication of whether or not the ThreadPool is stopped */
};
