#pragma once

#include "CAFAna/Core/Var.h"

namespace ana
{
  /// Template for Weights -- a weight is a value extracted from a record, the
  /// same as a Var, but intended for weighting events, rather than directly
  /// plotting
  template<class T> class _Weight: protected _Var<T>
  {
  public:
    template<class FuncT> _Weight(const FuncT& func) : _Var<T>(func) {}

    _Weight operator*(const _Weight& w) const {return _Weight((_Var<T>&)(*this) * (_Var<T>&)w);}
    _Weight operator/(const _Weight& w) const {return _Weight((_Var<T>&)(*this) / (_Var<T>&)w);}

    using _Var<T>::operator();
    using _Var<T>::ID;
    using _Var<T>::MaxID;

  protected:
    _Weight(const _Var<T>& v) : _Var<T>(v) {}
  };

  template<class T> struct One{double operator()(const T*) const {return 1;}};

  template<class T> _Weight<T> Unweighted(){return _Weight<T>(One<T>());}
} // namespace
