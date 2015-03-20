// Pour ThreadPool
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <vector>

// Pour test
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

class ThreadPool {
private:
  struct BaseTask {
    virtual ~BaseTask() = default;
    virtual void execute() = 0;
  };

  template <class F> class ConcreteTask : public BaseTask {
  public:
    ConcreteTask(F &&Func) : m_Func(Func) {}

    void execute() { m_Func(); }

  private:
    F m_Func;
  };

public:
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  ThreadPool(unsigned int N = std::thread::hardware_concurrency())
      : m_Stop{false} {
    for (unsigned int i = 0; i < N; ++i) {
      m_Threads.emplace_back([&]() {
        while (!stopped()) {
          auto NextTask = getNextTaskToRun();
          if (NextTask)
            NextTask->execute();
        }
      });
    }
  }

  ~ThreadPool() {
    stop();
    joinAll();
  }

public:
  template <class F> void addTask(F &&Func) {
    std::lock(m_Mutex, m_TaskMutex);
    std::lock_guard<std::mutex> TaskLock(m_TaskMutex, std::adopt_lock);
    std::lock_guard<std::mutex> Lock(m_Mutex, std::adopt_lock);
    m_Tasks.emplace_back(makeTask(std::forward<F>(Func)));
    m_CV.notify_one();
  }

  void joinAll() const {
    for (auto &T : m_Threads)
      if (T.joinable())
        T.join();
  }

  void stop() {
    m_Stop = true;
    std::unique_lock<std::mutex> Lock(m_TaskMutex);
    m_CV.notify_all();
  }

private:
  bool empty() const {
    std::lock_guard<std::mutex> Lock(m_Mutex);
    return m_Tasks.empty();
  }

  std::unique_ptr<BaseTask> getNextTaskToRun() {
    if (empty()) {
      std::unique_lock<std::mutex> Lock(m_TaskMutex);
      m_CV.wait(Lock, [&]() { return m_Stop || !empty(); });
    }

    std::lock_guard<std::mutex> Lock(m_Mutex);
    if (m_Tasks.empty())
      return nullptr;

    auto NextTask = std::move(m_Tasks.front());
    m_Tasks.pop_front();
    return NextTask;
  }

  template <class F> static std::unique_ptr<BaseTask> makeTask(F &&Func) {
    return std::unique_ptr<BaseTask>(
        new ConcreteTask<F>(std::forward<F>(Func)));
  }

  bool stopped() const { return m_Stop && empty(); }

private:
  std::atomic<bool> m_Stop;
  std::condition_variable m_CV;
  mutable std::mutex m_Mutex;
  std::mutex m_TaskMutex;
  std::deque<std::unique_ptr<BaseTask>> m_Tasks;
  mutable std::vector<std::thread> m_Threads;
};

const int NB_ARRAYS = 4;
const int NB_VALS = 100;

int main() {
  std::vector<std::vector<int>> Vals(NB_ARRAYS, std::vector<int>(NB_VALS));
  std::default_random_engine Engine;
  std::uniform_int_distribution<int> Dist{0, 1000};
  for (int i = 0; i < NB_ARRAYS; ++i)
    for (int j = 0; j < NB_VALS; ++j)
      Vals[i][j] = Dist(Engine);

  auto avant = std::chrono::system_clock::now();
  ThreadPool Pool;
  for (int i = 0; i < NB_ARRAYS; ++i)
    //Pool.addTask(
    //    [&]() { std::find(Vals[i].begin(), Vals[i].end(), Dist(Engine)); });
    Pool.addTask([i]() { std::cout << "HELLO " << i << std::endl; });

        Pool.stop();
  Pool.joinAll();
  auto apres = std::chrono::system_clock::now();
  std::cout << "Execution parallele (pool par defaut de "
            << std::thread::hardware_concurrency() << " threads) : "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   apres - avant).count() << " ms." << std::endl;
}

