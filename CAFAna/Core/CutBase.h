#pragma once

#include <functional>

namespace ana
{
  template<class T> class DepMan;

  /// None of the implementation of _Cut actually depends on the record
  /// type. This base class implements all that basic functionality, and then
  /// _Cut just adds a typed wrapper on top.
  class CutBase
  {
  public:
    typedef bool (VoidCutFunc_t)(const void* sr);
    typedef double (VoidExpoFunc_t)(const void* sr);

    friend class DepMan<CutBase>;
    friend class VarBase;

    ~CutBase();

  protected:
    CutBase(const std::function<VoidCutFunc_t>& func,
            const std::function<VoidExpoFunc_t>& livefunc,
            const std::function<VoidExpoFunc_t>& potfunc,
            int id = -1);

    CutBase(const CutBase& c);

    CutBase& operator=(const CutBase& c);


    CutBase operator&&(const CutBase& c) const;
    CutBase operator||(const CutBase& c) const;
    CutBase operator!() const;

    // Python's default implementation of and/or is very dangerous - it returns
    // one of its operands, which means the user probably won't notice the
    // problem. Make all checks of truthiness abort with a message.
    operator bool() const;

    bool operator()(const void* rec) const
    {
      return fFunc(rec);
    }

    std::function<VoidExpoFunc_t>
    CombineExposures(const std::function<VoidExpoFunc_t>& a,
                     const std::function<VoidExpoFunc_t>& b) const;

    /// Cuts with the same definition will have the same ID
    int ID() const {return fID;}

    static int MaxID() {return fgNextID-1;}

    std::function<VoidCutFunc_t> fFunc;
    std::function<VoidExpoFunc_t> fLiveFunc, fPOTFunc;

    int fID;
    /// The next ID that hasn't yet been assigned
    static int fgNextID;
  };
} // namespace
