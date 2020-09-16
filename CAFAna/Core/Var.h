#pragma once

// This file defines the basic Var object. For specific variables, and examples
// of how to implement your own, see Vars.h

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/Cut.h"

#include <functional>

namespace ana
{
  /// Template for Var and SpillVar
  template<class T> class _Var
  {
  public:
    /// The type of the function part of a var
    typedef double (VarFunc_t)(const T* sr);

    /// std::function can wrap a real function, function object, or lambda
    _Var(const std::function<VarFunc_t>& fun);

    _Var(const _Var<T>& v);

    _Var& operator=(const _Var<T>& v);

    ~_Var();

    /// Allows a variable to be called with double value = myVar(sr) syntax
    double operator()(const T* sr) const
    {
      return fFunc(sr);
    }

    /// Vars with the same definition will have the same ID
    int ID() const {return fID;}

    static int MaxID() {return fgNextID-1;}

    _Cut<T> operator<(double c) const;
    _Cut<T> operator>(double c) const;
    _Cut<T> operator>=(double c) const;
    _Cut<T> operator<=(double c) const;
    _Cut<T> operator==(double c) const;
    _Cut<T> operator!=(double c) const;

    _Cut<T> operator>(const _Var<T>& v) const;
    _Cut<T> operator<(const _Var<T>& v) const;
    _Cut<T> operator>=(const _Var<T>& v) const;
    _Cut<T> operator<=(const _Var<T>& v) const;
    _Cut<T> operator==(const _Var<T>& v) const;
    _Cut<T> operator!=(const _Var<T>& v) const;

    // Most useful for combining weights.
    _Var<T> operator*(const _Var<T>& v) const;
    _Var<T> operator/(const _Var<T>& v) const;
    _Var<T> operator+(const _Var<T>& v) const;
    _Var<T> operator-(const _Var<T>& v) const;

  protected:
    _Var(const std::function<VarFunc_t>& fun, int id);
    std::function<VarFunc_t> fFunc;

    int fID;
    /// The next ID that hasn't yet been assigned
    static int fgNextID;
  };

  /// \brief Representation of a variable to be retrieved from a \ref
  /// caf::StandardRecord object
  ///
  /// A Var consists of a function, taking a StandardRecord and returning the
  /// value of the variable (which may be some complicated function).
  typedef _Var<caf::SRProxy> Var;

  /// \brief Equivalent of \ref Var acting on \ref caf::SRSpill. For use in
  /// making plots of POT per run etc
  typedef _Var<caf::SRSpillProxy> SpillVar;

  /// \brief Var designed to be used over the nuTree, ie all neutrinos, not
  /// just those that got slices.
  typedef _Var<caf::SRNeutrinoProxy> NuTruthVar;

  /// \brief For Vars where literally all you need is a single CAF variable
  ///
  /// eg Var myVar = SIMPLEVAR(my.var.str);
  /// NB lack of quotes quotes around my.var.str
#define SIMPLEVAR(CAFNAME) ana::Var([](const caf::SRProxy* sr){return sr->CAFNAME;})
#define SIMPLENUTRUTHVAR(CAFNAME) ana::NuTruthVar([](const caf::SRNeutrinoProxy* nu){return nu->CAFNAME;})

  template<class T> struct One{double operator()(const T*) const {return 1;}};

  template<class T> _Var<T> Unweighted(){return _Var<T>(One<T>());}

  /// The simplest possible Var, always 1. Used as a default weight.
  const Var kUnweighted{One<caf::SRProxy>()};

  const SpillVar kSpillUnweighted{One<caf::SRSpillProxy>()};

  const NuTruthVar kNuTruthUnweighted{One<caf::SRNeutrinoProxy>()};

  /// \brief Variable formed from two input variables
  ///
  /// The binning of each variable has to be given to allow conversion into a
  /// 1D binned representation.
  template<class T> _Var<T>
  Var2D(const _Var<T>& a, const Binning& binsa,
        const _Var<T>& b, const Binning& binsb);

  /// \brief Variable formed from two input variables
  ///
  /// The binning of each variable has to be given to allow conversion into a
  /// 1D binned representation.
  template<class T> _Var<T>
  Var2D(const _Var<T>& a, int na, double a0, double a1,
        const _Var<T>& b, int nb, double b0, double b1);

  /// This is just like a Var2D, but useful for 3D Spectra
  template<class T> _Var<T>
  Var3D(const _Var<T>& a, const Binning& binsa,
        const _Var<T>& b, const Binning& binsb,
        const _Var<T>& c, const Binning& binsc);

  /// This is just like a Var2D, but useful for 3D Spectra
  template<class T> _Var<T>
  Var3D(const _Var<T>& a, int na, double a0, double a1,
        const _Var<T>& b, int nb, double b0, double b1,
        const _Var<T>& c, int nc, double c0, double c1);

  /// Use to rescale another variable.
  Var Scaled(const Var& v, double s);

  /// Use to weight events up and down by some factor
  Var Constant(double c);

  /// Use to take sqrt of a var
  Var Sqrt(const Var& v);

  // Variants of the comparisons with the constant on the LHS
  template<class T> _Cut<T> operator>(double c, const _Var<T>& v){return v < c;}
  template<class T> _Cut<T> operator<(double c, const _Var<T>& v){return v > c;}
  template<class T> _Cut<T> operator>=(double c, const _Var<T>& v){return v <= c;}
  template<class T> _Cut<T> operator<=(double c, const _Var<T>& v){return v >= c;}
  template<class T> _Cut<T> operator!=(double c, const _Var<T>& v){return v != c;}

} // namespace
