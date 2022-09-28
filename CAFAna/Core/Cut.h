#pragma once

// This file defines the basic Cut object. For specific cuts, and examples of
// how to implement your own, see Cuts.h

#include "CAFAna/Core/FwdDeclare.h"

#include "CAFAna/Core/CutBase.h"

#include <cassert>

namespace ana
{
  /// Template for Cuts applied to any type of object
  template<class RecT, class SpillT = void> class _Cut: protected CutBase
  {
  public:
    /// The type of the function part of a cut
    typedef bool (CutFunc_t)(const RecT* sr);
    typedef double (ExposureFunc_t)(const SpillT* spill);

    /// User function wants to be called with a specific type, but to pass the
    /// user function to CutBase it needs to accept void*
    template<class FuncT, class ArgT, class RetT> struct AddType
    {
      static_assert(std::is_invocable_r_v<RetT, FuncT, const ArgT*>);

      // In the case the user passes a bare function, we need to store the
      // function pointer.
      typedef typename std::conditional_t<std::is_function_v<FuncT>,
                                          typename std::add_pointer_t<FuncT>,
                                          FuncT> FuncPT;

      AddType(const FuncT& f) : fFunc(f) {}
      RetT operator()(const void* x){return fFunc((const ArgT*)x);}
      FuncPT fFunc;
    };

    /// std::function can wrap a real function, function object, or lambda
    explicit _Cut(const std::function<CutFunc_t>& func,
                  const std::function<ExposureFunc_t>& liveFunc = 0,
                  const std::function<ExposureFunc_t>& potFunc = 0)
      : CutBase(AddType<decltype(func), RecT, bool>(func),
                AddType<decltype(liveFunc), SpillT, double>(liveFunc),
                AddType<decltype(potFunc), SpillT, double>(potFunc))
    {
      // This would be better enforced at compile time by only making the
      // three-argument constructor available for non-voids using enable_if.
      assert(!(liveFunc && std::is_same_v<SpillT, void>));
      assert(!(potFunc && std::is_same_v<SpillT, void>));
    }

    template<class T, class U, class V>
    explicit _Cut(const T& func,
                  const U& liveFunc,
                  const V& potFunc)
      : CutBase(AddType<T, RecT, bool>(func),
                AddType<U, SpillT, double>(liveFunc),
                AddType<V, SpillT, double>(potFunc))
    {
    }

    template<class T, class U>
    explicit _Cut(const T& func,
                  const U& liveFunc = 0)
      : CutBase(AddType<T, RecT, bool>(func),
                AddType<U, SpillT, double>(liveFunc),
                0)
    {
    }

    template<class T, class U = ExposureFunc_t, class V = ExposureFunc_t>
    explicit _Cut(const T& func)
      : CutBase(AddType<T, RecT, bool>(func), 0, 0)
    {
    }

    _Cut(const _Cut&) = default;
    _Cut(_Cut&&) = default;

    // Convert from cuts without spillT to those with a typed spillT but null
    // function pointer
    _Cut(const _Cut<RecT, void>& c) : CutBase(c)
    {
    }

    _Cut& operator=(const _Cut& c) = default;
    _Cut& operator=(_Cut&& c) = default;

    /// Allows a cut to be called with bool result = myCut(rec) syntax
    bool operator()(const RecT* rec) const
    {
      return fFunc(rec);
    }

    /// Provide a Livetime function if your cut is a timing cut etc
    double Livetime(const SpillT* spill) const
    {
      return fLiveFunc ? fLiveFunc(spill) : -1;
    }

    /// Could be useful for cuts on specific batches?
    double POT(const SpillT* spill) const
    {
      return fPOTFunc ? fPOTFunc(spill) : -1;
    }

    /// Cuts with the same definition will have the same ID
    using CutBase::ID;
    using CutBase::MaxID;


    // Forward to base implementation
    _Cut operator&&(const _Cut& c) const {return _Cut(CutBase::operator&&(c));}
    _Cut operator||(const _Cut& c) const {return _Cut(CutBase::operator||(c));}
    _Cut operator!() const {return _Cut(CutBase::operator!());}

    // Python doesn't allow overloading and/or/not, but &/|/~ are
    // available. Don't expose these publicly to C++ users though.
    // This #define is set in Dict/classes.h
#ifdef DICT_FOR_PYTHON
    _Cut __and__(const _Cut& c) const {return *this && c;}
    _Cut __or__(const _Cut& c) const {return *this || c;}
    _Cut __invert__() const {return !(*this);}
    // Python's default implementation of and/or is very dangerous - it returns
    // one of its operands, which means the user probably won't notice the
    // problem. Make all checks of truthiness abort with a message.
    using CutBase::operator bool;
#endif

  protected:
    friend class _Var<RecT>;
    // friend with all types of cuts
    template<class T, class U> friend class _Cut;

    explicit _Cut(const CutBase& c) : CutBase(c) {}
  };

  // Allow cut with void SpillT to be combined on the LHS of an expression with
  // a full cut on the right.
  template<class RecT, class SpillT> inline _Cut<RecT, SpillT> operator&&(const _Cut<RecT, void>& a, const _Cut<RecT, SpillT>& b){return _Cut<RecT, SpillT>(a) && b;}
  template<class RecT, class SpillT> inline _Cut<RecT, SpillT> operator||(const _Cut<RecT, void>& a, const _Cut<RecT, SpillT>& b){return _Cut<RecT, SpillT>(a) || b;}


  template<class T> struct NoCut{bool operator()(const T*) const {return true;}};

} // namespace
