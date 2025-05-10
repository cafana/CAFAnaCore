#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace ana
{
  /// A variable that has an independent value on each thread
  ///
  /// Intended for use in implementing caches in a threadsafe manner. \ref T
  /// must be default-constructible.
  template<class T> class ThreadLocal
  {
  public:
    T* operator->()
    {
      const std::thread::id id = std::this_thread::get_id();

      {
        // Normally it's safe for there to be multiple concurrent readers
        std::shared_lock lock(fMutex);

        auto it = fVals.find(id);
        // We found the variable for this thread
        if(it != fVals.end()) return it->second.get();
      }

      // If we've never seen this thread before we need to create a new entry,
      // and we have to have exclusive access for that to be safe.
      std::unique_lock lock(fMutex);

      auto it_success_pair = fVals.emplace(id, std::make_unique<T>());
      return it_success_pair.first->second.get();
    }

  protected:
    std::unordered_map<std::thread::id, std::unique_ptr<T>> fVals;
    std::shared_mutex fMutex;
  };
}
