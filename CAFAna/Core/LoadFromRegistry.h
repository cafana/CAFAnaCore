#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

class TDirectory;

namespace ana
{
  /// Mechanism for derived classes to register their LoadFrom functions
  template <typename BaseClass>
  class LoadFromRegistry
  {
  public:
    // User's function will return the user's type
    template<class T> using FuncAny_t = std::function<std::unique_ptr<T>(TDirectory*, const std::string&)>;

    // This registry will return the base class type
    using Func_t = FuncAny_t<BaseClass>;

    template<class T> static void Register(const std::string& tag,
                                           FuncAny_t<T> func)
    {
      Map()[tag] = [func](TDirectory* d, const std::string& n)
        {
          // Convert the user's type
          return std::unique_ptr<BaseClass>(func(d, n).release());
        };
    }

    static Func_t Get(const std::string& tag)
    {
      auto it = Map().find(tag);
      if(it == Map().end()) return 0;
      return it->second;
    }

  protected:
    static std::map<std::string, Func_t>& Map()
    {
      // https://isocpp.org/wiki/faq/ctors#static-init-order
      static auto m = new std::map<std::string, Func_t>;
      return *m;
    }
  };
}

// Call this in your cxx file to hook up your LoadFrom function
#define REGISTER_LOADFROM(tag, BASE, DERIVED)                            \
  struct Register##DERIVED##LoadFrom{                                    \
    Register##DERIVED##LoadFrom(){                                       \
      LoadFromRegistry<BASE>::FuncAny_t<DERIVED> lf = DERIVED::LoadFrom; \
      LoadFromRegistry<BASE>::Register(tag, lf);                         \
    }                                                                    \
  } gRegister##DERIVED##LoadFrom
