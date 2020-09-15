#include "CAFAna/Core/ThreadPool.h"

#include "CAFAna/Core/Progress.h"

#include <cassert>
#include <unistd.h>

namespace ana
{
  class ThreadPool::ThreadsafeProgress: protected Progress
  {
  public:
    ThreadsafeProgress(const std::string& title) : Progress(title) {}

    void SetProgress(double frac)
    {
      std::scoped_lock lock(fLock);
      Progress::SetProgress(frac);
    }

  protected:
    std::mutex fLock;
  };

  //----------------------------------------------------------------------
  ThreadPool::ThreadPool(unsigned int maxThreads)
    : fMaxThreads(maxThreads), fNumLiveThreads(0),
      fTasksCompleted(0), fTotalTasks(0), fProgress(0)
  {
    if(maxThreads == 0){
      fMaxThreads = sysconf(_SC_NPROCESSORS_ONLN);
    }
  }

  //----------------------------------------------------------------------
  ThreadPool::~ThreadPool()
  {
    // This is safe, even if it's already been explicitly called
    Finish();
  }

  //----------------------------------------------------------------------
  void ThreadPool::ShowProgress(const std::string& title)
  {
    if(fProgress) delete fProgress;
    fProgress = new ThreadsafeProgress(title);
  }

  //----------------------------------------------------------------------
  void ThreadPool::Finish()
  {
    // TODO switch to std::jthread once we are on C++20
    for(std::thread& th: fThreads) th.join();

    assert(fNumLiveThreads == 0);

    fThreads.clear(); // Make it safe to call a second time

    if(fProgress){
      delete fProgress;
      fProgress = 0;
    }
  }

  //----------------------------------------------------------------------
  void ThreadPool::WorkerFunc()
  {
    while(true){
      func_t task = fTasks.consume();
        
      if(!task){
        // Nothing left to do, commit suicide
        --fNumLiveThreads;
        return;
      }

      // Actually do the user's work
      task();

      ++fTasksCompleted;

      if(fProgress){
        fProgress->SetProgress(fTasksCompleted/double(fTotalTasks));
      }
    }
  }

  //----------------------------------------------------------------------
  void ThreadPool::AddTask(const func_t& func)
  {
    fTasks.add(func);

    ++fTotalTasks;

    if(fNumLiveThreads < fMaxThreads){
      fThreads.emplace_back([this](){WorkerFunc();});
      ++fNumLiveThreads;
    }
  }
}
