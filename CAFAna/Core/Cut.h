#pragma once

// This file defines the basic Cut object. For specific cuts, and examples of
// how to implement your own, see Cuts.h

#include <functional>

#include "CAFAna/Core/FwdDeclare.h"

namespace ana
{
  typedef double (ExposureFunc_t)(const caf::SRSpillProxy* spill);

  /// Template for Cut and SpillCut
  template<class T> class _Cut
  {
  public:
    /// The type of the function part of a cut
    typedef bool (CutFunc_t)(const T* sr);

    /// std::function can wrap a real function, function object, or lambda
    _Cut(const std::function<CutFunc_t>& func,
         const std::function<ExposureFunc_t>& liveFunc = 0,
         const std::function<ExposureFunc_t>& potFunc = 0);

    _Cut(const _Cut<T>& c);

    _Cut& operator=(const _Cut<T>& c);

    ~_Cut();

    /// Allows a cut to be called with bool result = myCut(sr) syntax
    bool operator()(const T* sr) const
    {
      return fFunc(sr);
    }

    /// Provide a Livetime function if your cut is a timing cut etc
    double Livetime(const caf::SRSpillProxy* spill) const
    {
      return fLiveFunc ? fLiveFunc(spill) : -1;
    }

    /// Could be useful for cuts on specific batches?
    double POT(const caf::SRSpillProxy* spill) const
    {
      return fPOTFunc ? fPOTFunc(spill) : -1;
    }

    _Cut<T> operator&&(const _Cut<T>& c) const;
    _Cut<T> operator||(const _Cut<T>& b) const;
    _Cut<T> operator!() const;

    /// Cuts with the same definition will have the same ID
    int ID() const {return fID;}

    static int MaxID() {return fgNextID-1;}

    // Python doesn't allow overloading and/or/not, but &/|/~ are
    // available. Don't expose these publicly to C++ users though.
    // This #define is set in Dict/classes.h
#ifdef DICT_FOR_PYTHON
    _Cut<T> __and__(const _Cut<T>& c) const {return *this && c;}
    _Cut<T> __or__(const _Cut<T>& c) const {return *this || c;}
    _Cut<T> __invert__() const {return !(*this);}
#else
  protected:
#endif
    // Python's default implementation of and/or is very dangerous - it returns
    // one of its operands, which means the user probably won't notice the
    // problem. Make all checks of truthiness abort with a message.
    operator bool() const;

  protected:
    _Cut(const std::function<CutFunc_t>& fun,
         const std::function<ExposureFunc_t>& liveFunc,
         const std::function<ExposureFunc_t>& potFunc,
         int id);

    std::function<CutFunc_t> fFunc;
    std::function<ExposureFunc_t> fLiveFunc, fPOTFunc;

    int fID;
    /// The next ID that hasn't yet been assigned
    static int fgNextID;
  };

  /// \brief Representation of a cut (selection) to be applied to a \ref
  /// caf::StandardRecord object
  ///
  /// A Cut consists of a function, taking a StandardRecord and returning a
  /// boolean indicating if that event passes the cut.
  ///
  /// Cut objects may be combined with the standard boolean operations && ||
  /// and !
  typedef _Cut<caf::SRProxy> Cut;

  /// \brief Equivalent of \ref Cut acting on \ref caf::SRSpill. For use in
  /// spill-by-spill data quality cuts
  typedef _Cut<caf::SRSpillProxy> SpillCut;

  /// \brief Cut designed to be used over the nuTree, ie all neutrinos, not
  /// just those that got slices.
  typedef _Cut<caf::SRNeutrinoProxy> NuTruthCut;

  template<class T> struct NoCut{bool operator()(const T*) const {return true;}};

  /// The simplest possible cut: pass everything, used as a default
  const Cut kNoCut{NoCut<caf::SRProxy>()};

  /// The simplest possible cut: pass everything, used as a default
  const SpillCut kNoSpillCut{NoCut<caf::SRSpillProxy>()};

  /// The simplest possible cut: pass everything, used as a default
  const NuTruthCut kNoNuTruthCut{NoCut<caf::SRNeutrinoProxy>()};
} // namespace
