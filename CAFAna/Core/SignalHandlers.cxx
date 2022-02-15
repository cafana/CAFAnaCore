// This file sets things up (when libCAFAnaCoreExt is loaded) so that we invoke
// gdb to get a stack trace after any unusual program termination. You can
// suppress this behaviour by setting the CAFE_NO_BACKTRACE environment
// variable.

#include <atomic>
#include <csignal>
#include <cstring>
#include <fstream>

#include <unistd.h>

#include <iostream>

namespace ana
{
  std::atomic_flag cafanaQuitRequested = ATOMIC_FLAG_INIT;   // this is initialization to 'false', atomically
  std::atomic_flag cafanaQuitNow = ATOMIC_FLAG_INIT;

  bool CAFAnaQuitRequested()
  {
    // note that std::atomic_flag::test() doesn't exist until C++20,
    // so we have to always set the flag, then clear it if it wasn't set.
    if (cafanaQuitRequested.test_and_set())
      return true;
    else
    {
      cafanaQuitRequested.clear();
      return false;
    }
  }

  std::string get_argv0()
  {
    char s[1024];
    sprintf(s, "/proc/%d/cmdline", getpid());
    std::ifstream f(s);
    if(!f) return "unknown_argv0";
    std::string ss;
    f >> ss;
    return ss;
  }

  void gdb_backtrace()
  {
    // Have to end with a 'kill' command, otherwise GDB winds up sending us
    // SIGSTOP and never resuming us afterwards.
    char s[1024];
    sprintf(s, "gdb --batch -ex 'set confirm off' -ex sharedlibrary -ex bt -ex kill %s %d", get_argv0().c_str(), getpid());
    system(s);
  }

  bool gIsException = false;

  void handle_terminate()
  {
    std::cout << "\n\nUncaught exception\n" << std::endl;
    gIsException = true;
    // Next thing that happens is abort(), which goes to handle_signal()
  }

  void handle_signal(int sig, siginfo_t*, void*)
  {
    // SIGTERM && SIGINT request nice shutdown
    if (sig == SIGTERM || sig == SIGINT)
    {
      if (!cafanaQuitNow.test_and_set())
      {
        std::cout << "\nReceived SIGINT or SIGTERM -- trying to shut down nicely." << std::endl;
        std::cout << "Interrupt again with Ctrl-C or 'kill <process id>' to exit immediately." << std::endl;
        cafanaQuitRequested.test_and_set();
        return;
      }
      _exit(sig+128);
    }

    if(sig == SIGABRT && !gIsException)
      std::cout << "\n\nAborted\n" << std::endl;
    if(sig == SIGSEGV)
      std::cout << "\n\nSegmentation fault\n" << std::endl;
    if(sig == SIGFPE)
      std::cout << "\n\nFloating point exception\n" << std::endl;
    if(sig == SIGBUS)
      std::cout << "\n\nBus error\n" << std::endl;

    std::cout << "Starting backtrace using gdb..." << std::endl;
    std::cout << "You can use 'export CAFE_NO_BACKTRACE=1' to skip backtraces in future if desired" << std::endl;
    
    gdb_backtrace();

    // gdb_backtrace() never returns. But if it did, this would be the right
    // way to exit with the correct code.
    _exit(sig+128);
  }

  class InstallHandlers
  {
  public:
    InstallHandlers()
    {
      // We're already being debugged somehow. Don't complicate things
      if(getenv("CAFE_NO_BACKTRACE")) return;

      // Check that this is really a CAFAna job, either by affirmative opt-in,
      // or at least a root.exe command line. We don't necessarily want this
      // triggering for unrelated art jobs.
      if(getenv("CAFE_YES_BACKTRACE") == 0 &&
         get_argv0().find("root.exe") == std::string::npos) return;

      // Handle uncaught exceptions
      std::set_terminate(handle_terminate);

      // Handle abort(), segfaults, bus errors, and FPEs
      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sigemptyset(&sa.sa_mask);

      sa.sa_sigaction = handle_signal;
      sa.sa_flags = SA_SIGINFO;

      sigaction(SIGABRT, &sa, NULL);
      sigaction(SIGSEGV, &sa, NULL);
      sigaction(SIGBUS, &sa, NULL);
      sigaction(SIGFPE, &sa, NULL);
      sigaction(SIGINT, &sa, NULL);
      sigaction(SIGTERM, &sa, NULL);
    }
  };

  static InstallHandlers gHandlerInstaller;
}
