#pragma once

// This file defines the basic Cut object. For specific cuts, and examples of
// how to implement your own, see Cuts.h

#include "CAFAna/Core/FwdDeclare.h"

#include "CAFAna/Core/CutBase.h"

namespace ana
{
  struct DummySpillT{DummySpillT() = delete;};

  /// Template for Cuts applied to any type of object
  template<class RecT, class SpillT = DummySpillT> class _Cut: protected CutBase
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

      AddType(const FuncT& f) : fFunc(f) {}
      RetT operator()(const void* x){return fFunc((const ArgT*)x);}
      FuncT fFunc;
    };

    /// std::function can wrap a real function, function object, or lambda
    _Cut(const std::function<CutFunc_t>& func,
         const std::function<ExposureFunc_t>& liveFunc = 0,
         const std::function<ExposureFunc_t>& potFunc = 0)
      : CutBase(AddType<decltype(func), RecT, bool>(func),
                AddType<decltype(liveFunc), SpillT, double>(liveFunc),
                AddType<decltype(potFunc), SpillT, double>(potFunc))
    {
    }

    template<class T, class U, class V>
    _Cut(const T& func,
         const U& liveFunc,
         const V& potFunc)
      : CutBase(AddType<T, RecT, bool>(func),
                AddType<U, SpillT, double>(liveFunc),
                AddType<V, SpillT, double>(potFunc))
    {
    }

    template<class T, class U>
    _Cut(const T& func,
         const U& liveFunc = 0)
      : CutBase(AddType<T, RecT, bool>(func),
                AddType<U, SpillT, double>(liveFunc),
                0)
    {
    }

    template<class T, class U = ExposureFunc_t, class V = ExposureFunc_t>
    _Cut(const T& func)
      : CutBase(AddType<T, RecT, bool>(func), 0, 0)
    {
    }

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
    _Cut operator&&(const _Cut& c) const {return CutBase::operator&&(c);}
    _Cut operator||(const _Cut& c) const {return CutBase::operator||(c);}
    _Cut operator!() const {return CutBase::operator!();}

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
    _Cut(const CutBase& c) : CutBase(c) {}
  };

  /*
  /// \brief Representation of a cut (selection) to be applied to a \ref
  /// caf::StandardRecord object
  ///
  /// A Cut consists of a function, taking a StandardRecord and returning a
  /// boolean indicating if that event passes the cut.
  ///
  /// Cut objects may be combined with the standard boolean operations && ||
  /// and !
  typedef _Cut<caf::SRProxy, caf::SRSpillProxy> Cut;

  /// \brief Equivalent of \ref Cut acting on \ref caf::SRSpill. For use in
  /// spill-by-spill data quality cuts
  typedef _Cut<caf::SRSpillProxy, caf::SRSpillProxy> SpillCut;

  /// \brief Cut designed to be used over the nuTree, ie all neutrinos, not
  /// just those that got slices.
  typedef _Cut<caf::SRNeutrinoProxy, caf::SRSpillProxy> NuTruthCut;
  */
  template<class T> struct NoCut{bool operator()(const T*) const {return true;}};
  /*
  /// The simplest possible cut: pass everything, used as a default
  const Cut kNoCut{NoCut<caf::SRProxy>()};

  /// The simplest possible cut: pass everything, used as a default
  const SpillCut kNoSpillCut{NoCut<caf::SRSpillProxy>()};

  /// The simplest possible cut: pass everything, used as a default
  const NuTruthCut kNoNuTruthCut{NoCut<caf::SRNeutrinoProxy>()};
  */
} // namespace
