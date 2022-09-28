#pragma once

// This file defines the basic Var object. For specific variables, and examples
// of how to implement your own, see Vars.h

#include "CAFAna/Core/VarBase.h"

#include "CAFAna/Core/Cut.h"

namespace ana
{
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

      // In the case the user passes a bare function, we need to store the
      // function pointer.
      typedef typename std::conditional_t<std::is_function_v<FuncT>,
                                          typename std::add_pointer_t<FuncT>,
                                          FuncT> FuncPT;

      AddType(const FuncT& f) : fFunc(f) {}
      double operator()(const void* x){return fFunc((const ArgT*)x);}
      FuncPT fFunc;
    };

    /// std::function can wrap a real function, function object, or lambda
    explicit _Var(const std::function<VarFunc_t>& func)
      : VarBase(AddType<decltype(func), T>(func))
    {
    }

    template<class FuncT> explicit _Var(const FuncT& func)
      : VarBase(AddType<FuncT, T>(func))
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

    _Var(const _Var&) = default;
    _Var(_Var&&) = default;
    _Var& operator=(const _Var&) = default;
    _Var& operator=(_Var&&) = default;

    /// Allows a variable to be called with double value = myVar(rec) syntax
    double operator()(const T* rec) const
    {
      return fFunc(rec);
    }

    /// Vars with the same definition will have the same ID
    using VarBase::ID;
    using VarBase::MaxID;

    _Cut<T> operator<(double c) const  {return _Cut<T>(VarBase::operator<(c));}
    _Cut<T> operator>(double c) const  {return _Cut<T>(VarBase::operator>(c));}
    _Cut<T> operator>=(double c) const {return _Cut<T>(VarBase::operator>=(c));}
    _Cut<T> operator<=(double c) const {return _Cut<T>(VarBase::operator<=(c));}
    _Cut<T> operator==(double c) const {return _Cut<T>(VarBase::operator==(c));}
    _Cut<T> operator!=(double c) const {return _Cut<T>(VarBase::operator!=(c));}

    _Cut<T> operator>(const _Var& v) const  {return _Cut<T>(VarBase::operator>(v));}
    _Cut<T> operator<(const _Var& v) const  {return _Cut<T>(VarBase::operator<(v));}
    _Cut<T> operator>=(const _Var& v) const {return _Cut<T>(VarBase::operator>=(v));}
    _Cut<T> operator<=(const _Var& v) const {return _Cut<T>(VarBase::operator<=(v));}
    _Cut<T> operator==(const _Var& v) const {return _Cut<T>(VarBase::operator==(v));}
    _Cut<T> operator!=(const _Var& v) const {return _Cut<T>(VarBase::operator!=(v));}

    _Var operator*(const _Var& v) const {return _Var(VarBase::operator*(v));}
    _Var operator/(const _Var& v) const {return _Var(VarBase::operator/(v));}
    _Var operator+(const _Var& v) const {return _Var(VarBase::operator+(v));}
    _Var operator-(const _Var& v) const {return _Var(VarBase::operator-(v));}

  protected:
    explicit _Var(const VarBase& v) : VarBase(v) {}
  };

  // Variants of the comparisons with the constant on the LHS
  template<class T> _Cut<T> operator>(double c, const _Var<T>& v){return v < c;}
  template<class T> _Cut<T> operator<(double c, const _Var<T>& v){return v > c;}
  template<class T> _Cut<T> operator>=(double c, const _Var<T>& v){return v <= c;}
  template<class T> _Cut<T> operator<=(double c, const _Var<T>& v){return v >= c;}
  template<class T> _Cut<T> operator!=(double c, const _Var<T>& v){return v != c;}

} // namespace
