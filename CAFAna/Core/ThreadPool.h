#pragma once

#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace ana
{
  class Progress;

  /// \brief A very simple thread pool for use by \ref Surface
  ///
  /// With great power comes great responsibility. Use caution on the grid or
  /// on shared interactive machines before you get yelled at or banned.
  class ThreadPool
  {
  public:
    /// \param maxThreads Maximum number of threads to use at one time.
    ///                   If unspecified, uses number of cores in machine.
    ///                   Use great caution on the grid or on shared
    ///                   interactive machines.
    explicit ThreadPool(unsigned int maxThreads = 0);
    virtual ~ThreadPool();

    void ShowProgress(const std::string& title);

    /// \brief The type of the user's worker functions
    ///
    /// Use a lambda function or std::bind etc to pass arguments
    typedef std::function<void(void)> func_t;

    void AddTask(const func_t& func);

    /// Wait for all threads to complete before returning
    void Finish();

    int NThreads() const {return fMaxThreads;}

  protected:
    void WorkerFunc();

    unsigned int fMaxThreads;

    class TaskQueue: protected std::deque<func_t>
    {
    public:
      void add(const func_t& x)
      {
        std::scoped_lock lock(fLock);
        std::deque<func_t>::push_back(x);
      }

      /// Null if the queue is empty
      func_t consume()
      {
        std::scoped_lock lock(fLock);
        if(empty()) return 0;
        func_t ret = front();
        pop_front();
        return ret;
      }

    protected:
      std::mutex fLock;
    };

    TaskQueue fTasks;

    std::vector<std::thread> fThreads; ///< All threads we ever created
    std::atomic<unsigned int> fNumLiveThreads; ///< Number of threads that are running

    std::atomic<int> fTasksCompleted;
    std::atomic<int> fTotalTasks; ///< How many tasks have we ever seen?

    class ThreadsafeProgress;
    ThreadsafeProgress* fProgress;
  };
}
