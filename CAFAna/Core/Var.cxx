#include "CAFAna/Core/Var.h"

#include "CAFAna/Core/DepMan.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>

namespace ana
{
  //----------------------------------------------------------------------
  template<class T> _Var<T>::
  _Var(const std::function<VarFunc_t>& fun)
    : fFunc(fun), fID(fgNextID++)
  {
    DepMan<_Var<T>>::Instance().RegisterConstruction(this);
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T>::
  _Var(const std::function<VarFunc_t>& fun, int id)
    : fFunc(fun), fID(id)
  {
    DepMan<_Var<T>>::Instance().RegisterConstruction(this);
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T>::~_Var()
  {
    DepMan<_Var<T>>::Instance().RegisterDestruction(this);
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T>::_Var(const _Var<T>& v)
    : fFunc(0), fID(-1)
  {
    if(&v == this) return;

    if(v.fFunc){
      fFunc = v.fFunc;
      fID = v.fID;

      DepMan<_Var<T>>::Instance().RegisterConstruction(this);
    }
    else{
      // If we are copying from a Var with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<_Var<T>>::Instance().RegisterDependency(&v, this);
    }
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T>& _Var<T>::
  operator=(const _Var<T>& v)
  {
    if(&v == this) return *this;

    if(v.fFunc){
      fFunc = v.fFunc;
      fID = v.fID;

      DepMan<_Var<T>>::Instance().RegisterConstruction(this);
    }
    else{
      fFunc = 0;
      fID = -1;

      // If we are copying from a Var with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<_Var<T>>::Instance().RegisterDependency(&v, this);
    }

    return *this;
  }


  //----------------------------------------------------------------------
  // Extract the value (trivial) and print a good error message if it's NaN
  template<class T>
  double ValHelper(const _Var<T>& v,
                   const std::string& op,
                   double c, const T* sr)
  {
    const double val = v(sr);
    if(std::isnan(val)){
      std::cout << "Warning: Cut compares NaN " << op << " " << c << std::endl;
    }
    return val;
  }

  template<class T>
  void ValHelper(const _Var<T>& va, const _Var<T>& vb,
                 const std::string& op,
                 const T* sr,
                 double& a, double& b)
  {
    a = va(sr);
    b = vb(sr);
    if(std::isnan(a) || std::isnan(b)){
      std::cout << "Warning: Cut compares " << a << " " << op << " " << b << std::endl;
    }
  }


  // These comparison operators are extremely rote. Use a macro

#define COMPARISON(OP, OPNAME)                                          \
  /* Comparison of a Var with a constant */                             \
  template<class T> _Cut<T> _Var<T>::                                   \
  operator OP(double c) const                                           \
  {                                                                     \
    struct OPNAME                                                       \
    {                                                                   \
      _Var<T> v; double c;                                              \
      bool operator()(const T* sr)                                      \
      {                                                                 \
        return ValHelper(v, #OP, c, sr) OP c;                           \
      }                                                                 \
    };                                                                  \
                                                                        \
    return _Cut<T>(OPNAME{*this, c});                                   \
  }                                                                     \
                                                                        \
  /* Comparison of a Var with another Var */                            \
  template<class T> _Cut<T> _Var<T>::                                   \
  operator OP(const _Var<T>& v) const                                   \
  {                                                                     \
    struct OPNAME                                                       \
    {                                                                   \
      _Var<T> a, b;                                                     \
      bool operator()(const T* sr)                                      \
      {                                                                 \
        double va, vb;                                                  \
        ValHelper(a, b, #OP, sr, va, vb);                               \
        return va OP vb;                                                \
      }                                                                 \
    };                                                                  \
                                                                        \
    return _Cut<T>(OPNAME{*this, v});                                   \
  }                                                                     \
  void dummy() // trick to require a semicolon

  COMPARISON(>,  Greater);
  COMPARISON(>=, GreaterEquals);
  COMPARISON(<,  Less);
  COMPARISON(<=, LessEquals);
  COMPARISON(==, Equals);
  COMPARISON(!=, NotEquals);


  // explicitly instantiate the template for the types we know we have
  template class _Var<caf::SRProxy>;
  template class _Var<caf::SRSpillProxy>;
  template class _Var<caf::SRNeutrinoProxy>;

  template<class T> int _Var<T>::fgNextID = 0;

  //----------------------------------------------------------------------
  /// Helper for \ref Var2D
  template<class T> class Var2DFunc
  {
  public:
    Var2DFunc(const _Var<T>& a, const Binning binsa,
              const _Var<T>& b, const Binning binsb)
      : fA(a), fBinsA(binsa),
	fB(b), fBinsB(binsb)
    {
    }

    double operator()(const T* sr) const
    {
      // Calculate current values of the variables in StandardRecord once
      const double va = fA(sr);
      const double vb = fB(sr);

      // Since there are no overflow/underflow bins, check the range
      if(va < fBinsA.Min() || vb < fBinsB.Min()) return -1;
      if(va > fBinsA.Max() || vb > fBinsB.Max()) return fBinsA.NBins() * fBinsB.NBins();

      // FindBin uses root convention, first bin is bin 1, bin 0 is underflow
      const int ia = fBinsA.FindBin(va) - 1;
      const int ib = fBinsB.FindBin(vb) - 1;

      const int i = ia*fBinsB.NBins()+ib;

      return i+.5;
    }

  protected:
    const _Var<T> fA;
    const Binning fBinsA;
    const _Var<T> fB;
    const Binning fBinsB;
  };

  /// Helper for \ref Var3D
  template<class T> class Var3DFunc
  {
  public:
    Var3DFunc(const _Var<T>& a, const Binning binsa,
              const _Var<T>& b, const Binning binsb,
              const _Var<T>& c, const Binning binsc)
      : fA(a), fBinsA(binsa),
	fB(b), fBinsB(binsb),
	fC(c), fBinsC(binsc)
    {
    }

    double operator()(const T* sr) const
    {
      /// Calculate current values of the variables in StandardRecord once
      const double va = fA(sr);
      const double vb = fB(sr);
      const double vc = fC(sr);

      /// Since there are no overflow/underflow bins, check the range
      if(va < fBinsA.Min() || vb < fBinsB.Min() || vc < fBinsC.Min()){
        return -1.0;
      }

      if(va > fBinsA.Max() || vb > fBinsB.Max() || vc > fBinsC.Max()){
        return fBinsA.NBins() * fBinsB.NBins() * fBinsC.NBins();
      }

      const int ia = fBinsA.FindBin(va) - 1;
      const int ib = fBinsB.FindBin(vb) - 1;
      const int ic = fBinsC.FindBin(vc) - 1;
      const int i = ia*fBinsB.NBins()*fBinsC.NBins() + ib*fBinsC.NBins() + ic;

      return i+.5;
    }

  protected:
    const _Var<T> fA;
    const Binning fBinsA;
    const _Var<T> fB;
    const Binning fBinsB;
    const _Var<T> fC;
    const Binning fBinsC;
  };

  //----------------------------------------------------------------------
  template<class T> _Var<T>
  Var2D(const _Var<T>& a, const Binning& binsa,
        const _Var<T>& b, const Binning& binsb)
  {
    return _Var<T>(Var2DFunc<T>(a, binsa, b, binsb));
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T>
  Var2D(const _Var<T>& a, int na, double a0, double a1,
        const _Var<T>& b, int nb, double b0, double b1)
  {
    return Var2D(a, Binning::Simple(na, a0, a1),
                 b, Binning::Simple(nb, b0, b1));
  }

  // explicitly instantiate the template for the types we know we have
  template Var Var2D(const Var&, const Binning&, const Var&, const Binning&);
  template SpillVar Var2D(const SpillVar&, const Binning&, const SpillVar&, const Binning&);
  template NuTruthVar Var2D(const NuTruthVar&, const Binning&, const NuTruthVar&, const Binning&);

  template Var Var2D(const Var&, int, double, double, const Var&, int, double, double);
  template SpillVar Var2D(const SpillVar&, int, double, double, const SpillVar&, int, double, double);
  template NuTruthVar Var2D(const NuTruthVar&, int, double, double, const NuTruthVar&, int, double, double);

  //----------------------------------------------------------------------
  template<class T> _Var<T>
  Var3D(const _Var<T>& a, const Binning& binsa,
        const _Var<T>& b, const Binning& binsb,
        const _Var<T>& c, const Binning& binsc)
  {
    return _Var<T>(Var3DFunc<T>(a, binsa, b, binsb, c, binsc));
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T>
  Var3D(const _Var<T>& a, int na, double a0, double a1,
        const _Var<T>& b, int nb, double b0, double b1,
        const _Var<T>& c, int nc, double c0, double c1)
  {
    return Var3D(a, Binning::Simple(na, a0, a1),
                 b, Binning::Simple(nb, b0, b1),
                 c, Binning::Simple(nc, c0, c1));
  }

  // explicitly instantiate the template for the types we know we have
  template Var Var3D(const Var&, const Binning&, const Var&, const Binning&, const Var&, const Binning&);
  template SpillVar Var3D(const SpillVar&, const Binning&, const SpillVar&, const Binning&, const SpillVar&, const Binning&);
  template NuTruthVar Var3D(const NuTruthVar&, const Binning&, const NuTruthVar&, const Binning&, const NuTruthVar&, const Binning&);

  template Var Var3D(const Var&, int, double, double, const Var&, int, double, double, const Var&, int, double, double);
  template SpillVar Var3D(const SpillVar&, int, double, double, const SpillVar&, int, double, double, const SpillVar&, int, double, double);
  template NuTruthVar Var3D(const NuTruthVar&, int, double, double, const NuTruthVar&, int, double, double, const NuTruthVar&, int, double, double);

  //----------------------------------------------------------------------
  Var Scaled(const Var& v, double s)
  {
    struct Scale
    {
      const Var v; double s;
      double operator()(const caf::SRProxy* sr) const {return s*v(sr);}
    };

    return Var(Scale{v, s});
  }

  //----------------------------------------------------------------------
  Var Constant(double c)
  {
    struct Const
    {
      double c;
      double operator()(const caf::SRProxy*) const {return c;}
    };

    return Var(Const{c});
  }

  //--------------------------------------------------------------------

  Var Sqrt(const Var& v)
  {
    struct Sqrter
    {
      const Var v;
      double operator()(const caf::SRProxy* sr) const {return sqrt(v(sr));}
    };

    return Var(Sqrter{v});
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T> _Var<T>::
  operator*(const _Var<T>& v) const
  {
    static std::map<std::pair<int, int>, int> ids;
    const std::pair<int, int> key(ID(), v.ID());

    struct Times
    {
      const _Var<T> a, b;
      double operator()(const T* sr) const {return a(sr) * b(sr);}
    };

    if(ids.count(key) == 0) ids[key] = fgNextID++;
    return _Var<T>(Times{*this, v});
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T> _Var<T>::
  operator/(const _Var<T>& v) const
  {
    static std::map<std::pair<int, int>, int> ids;
    const std::pair<int, int> key(ID(), v.ID());

    struct Divide
    {
      const _Var<T> a, b;
      double operator()(const T* sr) const
      {
        const double denom = b(sr);
        if(denom != 0)
          return a(sr) / denom;
        else
          return 0.0;
      }
    };

    if(ids.count(key) == 0) ids[key] = fgNextID++;
    return _Var<T>(Divide{*this, v}, ids[key]);
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T> _Var<T>::
  operator+(const _Var<T>& v) const
  {
    static std::map<std::pair<int, int>, int> ids;
    const std::pair<int, int> key(ID(), v.ID());

    struct Plus
    {
      const _Var<T> a, b;
      double operator()(const T* sr) const {return a(sr) + b(sr);}
    };

    if(ids.count(key) == 0) ids[key] = fgNextID++;
    return _Var<T>(Plus{*this, v}, ids[key]);
  }

  //----------------------------------------------------------------------
  template<class T> _Var<T> _Var<T>::
  operator-(const _Var<T>& v) const
  {
    static std::map<std::pair<int, int>, int> ids;
    const std::pair<int, int> key(ID(), v.ID());

    struct Minus
    {
      const _Var<T> a, b;
      double operator()(const T* sr) const {return a(sr) - b(sr);}
    };

    if(ids.count(key) == 0) ids[key] = fgNextID++;
    return _Var<T>(Minus{*this, v}, ids[key]);
  }
} // namespace
