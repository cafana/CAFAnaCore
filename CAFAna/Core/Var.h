#pragma once

// This file defines the basic Var object. For specific variables, and examples
// of how to implement your own, see Vars.h

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/Cut.h"

#include <functional>

namespace ana
{
  /// None of the implementation of _Var actually depends on the record
  /// type. This base class implements all that basic functionality, and then
  /// _Var just adds a typed wrapper on top.
  class VarBase
  {
  public:
    typedef double (VoidVarFunc_t)(const void* rec);

    friend class DepMan<VarBase>;
    friend class Var2DFunc;
    friend class Var3DFunc;

    friend double ValHelper(const VarBase&, const std::string&, double c, const void*);
    friend void ValHelper(const VarBase&, const VarBase&, const std::string&, const void*, double&, double&);

    ~VarBase();

  protected:
    VarBase(const std::function<VoidVarFunc_t>& func, int id = -1);

    VarBase(const VarBase& v);

    VarBase(const VarBase& a, const Binning& binsa,
            const VarBase& b, const Binning& binsb);

    VarBase(const VarBase& a, const Binning& binsa,
            const VarBase& b, const Binning& binsb,
            const VarBase& c, const Binning& binsc);

    VarBase& operator=(const VarBase& v);

    /// Allows a variable to be called with double value = myVar(rec) syntax
    double operator()(const void* rec) const
    {
      return fFunc(rec);
    }

    /// Vars with the same definition will have the same ID
    int ID() const {return fID;}

    static int MaxID() {return fgNextID-1;}

    CutBase operator<(double c) const;
    CutBase operator>(double c) const;
    CutBase operator>=(double c) const;
    CutBase operator<=(double c) const;
    CutBase operator==(double c) const;
    CutBase operator!=(double c) const;

    CutBase operator>(const VarBase& v) const;
    CutBase operator<(const VarBase& v) const;
    CutBase operator>=(const VarBase& v) const;
    CutBase operator<=(const VarBase& v) const;
    CutBase operator==(const VarBase& v) const;
    CutBase operator!=(const VarBase& v) const;

    // Most useful for combining weights.
    VarBase operator*(const VarBase& v) const;
    VarBase operator/(const VarBase& v) const;
    VarBase operator+(const VarBase& v) const;
    VarBase operator-(const VarBase& v) const;

    std::function<VoidVarFunc_t> fFunc;

    int fID;
    /// The next ID that hasn't yet been assigned
    static int fgNextID;
  };

  /// Template for Vars applied to any type of object
  template<class T> class _Var : protected VarBase
  {
  public:
    /// The type of the function part of a var
    typedef double (VarFunc_t)(const T* sr);

    /// User function wants to be called with a specific type, but to pass the
    /// user function to CutBase it needs to accept void*
    template<class FuncT, class ArgT> struct AddType
    {
      static_assert(std::is_invocable_r_v<double, FuncT, const ArgT*>);

      AddType(const FuncT& f) : fFunc(f) {}
      double operator()(const void* x){return fFunc((const ArgT*)x);}
      FuncT fFunc;
    };

    /// std::function can wrap a real function, function object, or lambda
    _Var(const std::function<VarFunc_t>& func)
      : VarBase(AddType<decltype(func), T>(func))
    {
    }

    /// \brief Variable formed from two input variables
    ///
    /// The binning of each variable has to be given to allow conversion into a
    /// 1D binned representation.
    _Var(const _Var& a, const Binning& binsa,
         const _Var& b, const Binning& binsb)
      : VarBase(a, binsa, b, binsb)
    {
    }

    _Var(const _Var& a, int na, double a0, double a1,
         const _Var& b, int nb, double b0, double b1)
      : VarBase(a, Binning::Simple(na, a0, a1),
                b, Binning::Simple(nb, b0, b1))
    {
    }

    _Var(const _Var& a, const Binning& binsa,
         const _Var& b, const Binning& binsb,
         const _Var& c, const Binning& binsc)
      : VarBase(a, binsa, b, binsb, c, binsc)
    {
    }

    _Var(const _Var& a, int na, double a0, double a1,
         const _Var& b, int nb, double b0, double b1,
         const _Var& c, int nc, double c0, double c1)
      : VarBase(a, Binning::Simple(na, a0, a1),
                b, Binning::Simple(nb, b0, b1),
                c, Binning::Simple(nc, c0, c1))
    {
    }

    /// Allows a variable to be called with double value = myVar(rec) syntax
    double operator()(const T* rec) const
    {
      return fFunc(rec);
    }

    /// Vars with the same definition will have the same ID
    using VarBase::ID;
    using VarBase::MaxID;

    _Cut<T> operator<(double c) const  {return VarBase::operator<(c);}
    _Cut<T> operator>(double c) const  {return VarBase::operator>(c);}
    _Cut<T> operator>=(double c) const {return VarBase::operator>=(c);}
    _Cut<T> operator<=(double c) const {return VarBase::operator<=(c);}
    _Cut<T> operator==(double c) const {return VarBase::operator==(c);}
    _Cut<T> operator!=(double c) const {return VarBase::operator!=(c);}

    _Cut<T> operator>(const _Var& v) const  {return VarBase::operator>(v);}
    _Cut<T> operator<(const _Var& v) const  {return VarBase::operator<(v);}
    _Cut<T> operator>=(const _Var& v) const {return VarBase::operator>=(v);}
    _Cut<T> operator<=(const _Var& v) const {return VarBase::operator<=(v);}
    _Cut<T> operator==(const _Var& v) const {return VarBase::operator==(v);}
    _Cut<T> operator!=(const _Var& v) const {return VarBase::operator!=(v);}

    // Most useful for combining weights.
    _Var operator*(const _Var& v) const {return VarBase::operator*(v);}
    _Var operator/(const _Var& v) const {return VarBase::operator/(v);}
    _Var operator+(const _Var& v) const {return VarBase::operator+(v);}
    _Var operator-(const _Var& v) const {return VarBase::operator-(v);}

  protected:
    _Var(const VarBase& v) : VarBase(v) {}
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
