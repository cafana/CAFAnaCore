#include "CAFAna/Core/VarBase.h"

#include "CAFAna/Core/DepMan.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <map>

namespace ana
{
  int VarBase::fgNextID = 1;

  //----------------------------------------------------------------------
  VarBase::VarBase(const std::function<VoidVarFunc_t>& func, int id)
    : fFunc(func), fID((id > 0) ? id : fgNextID++)
  {
    DepMan<VarBase>::Instance().RegisterConstruction(this);
  }

  //----------------------------------------------------------------------
  VarBase::~VarBase()
  {
    DepMan<VarBase>::Instance().RegisterDestruction(this);
  }

  //----------------------------------------------------------------------
  VarBase::VarBase(const VarBase& v)
    : fFunc(0), fID(-1)
  {
    if(&v == this) return;

    if(v.fFunc){
      fFunc = v.fFunc;
      fID = v.fID;

      DepMan<VarBase>::Instance().RegisterConstruction(this);
    }
    else{
      // If we are copying from a Var with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<VarBase>::Instance().RegisterDependency(&v, this);
    }
  }

  //----------------------------------------------------------------------
  VarBase& VarBase::operator=(const VarBase& v)
  {
    if(&v == this) return *this;

    if(v.fFunc){
      fFunc = v.fFunc;
      fID = v.fID;

      DepMan<VarBase>::Instance().RegisterConstruction(this);
    }
    else{
      fFunc = 0;
      fID = -1;

      // If we are copying from a Var with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<VarBase>::Instance().RegisterDependency(&v, this);
    }

    return *this;
  }


  //----------------------------------------------------------------------
  // Extract the value (trivial) and print a good error message if it's NaN
  double ValHelper(const VarBase& v,
                   const std::string& op,
                   double c, const void* rec)
  {
    const double val = v(rec);
    if(std::isnan(val)){
      std::cout << "Warning: Cut compares NaN " << op << " " << c << std::endl;
    }
    return val;
  }

  void ValHelper(const VarBase& va, const VarBase& vb,
                 const std::string& op,
                 const void* rec,
                 double& a, double& b)
  {
    a = va(rec);
    b = vb(rec);
    if(std::isnan(a) || std::isnan(b)){
      std::cout << "Warning: Cut compares " << a << " " << op << " " << b << std::endl;
    }
  }

  namespace{
    // If two Vars are combined with each other in the same way in two places,
    // we want to give the combination the same ID in both cases. This helps
    // caching in SpectrumLoader
    class IDCache
    {
    public:
      int GetID(int idA, int idB, int& nextID)
      {
        // If either is not fully constructed yet it's not safe to proceed
        if(idA <= 0 || idB <= 0) return nextID++;
        const std::pair<int, int> key(idA, idB);
        auto it = fIDs.find(key);
        if(it != fIDs.end()) return it->second;
        const int newid = nextID++;
        fIDs.emplace(key, newid);
        return newid;
      }

    protected:
      std::map<std::pair<int, int>, int> fIDs;
    };
  }


  // These comparison operators are extremely rote. Use a macro

#define COMPARISON(OP, OPNAME)                                          \
  /* Comparison of a Var with a constant */                             \
  CutBase VarBase::                                                     \
  operator OP(double c) const                                           \
  {                                                                     \
    struct OPNAME                                                       \
    {                                                                   \
      VarBase v; double c;                                              \
      bool operator()(const void* rec) const                            \
      {                                                                 \
        return ValHelper(v, #OP, c, rec) OP c;                          \
      }                                                                 \
    };                                                                  \
                                                                        \
    return CutBase(OPNAME{*this, c}, 0, 0, -1);                         \
  }                                                                     \
                                                                        \
  /* Comparison of a Var with another Var */                            \
  CutBase VarBase::                                                     \
  operator OP(const VarBase& v) const                                   \
  {                                                                     \
    struct OPNAME                                                       \
    {                                                                   \
      VarBase a, b;                                                     \
      bool operator()(const void* rec) const                            \
      {                                                                 \
        double va, vb;                                                  \
        ValHelper(a, b, #OP, rec, va, vb);                              \
        return va OP vb;                                                \
      }                                                                 \
    };                                                                  \
                                                                        \
    static IDCache cache;                                               \
    return CutBase(OPNAME{*this, v}, 0, 0,                              \
                   cache.GetID(ID(), v.ID(), fgNextID));                \
  }                                                                     \
  void dummy() // trick to require a semicolon

  COMPARISON(>,  Greater);
  COMPARISON(>=, GreaterEquals);
  COMPARISON(<,  Less);
  COMPARISON(<=, LessEquals);
  COMPARISON(==, Equals);
  COMPARISON(!=, NotEquals);

  // Likewise for combination operators

#define COMBINATION(OP, OPNAME)                                         \
  /* Combination of a Var with another Var */                           \
  VarBase VarBase::                                                     \
  operator OP(const VarBase& v) const                                   \
  {                                                                     \
    struct OPNAME                                                       \
    {                                                                   \
      VarBase a, b;                                                     \
      double operator()(const void* rec) const                          \
      {                                                                 \
        return a(rec) OP b(rec);                                        \
      }                                                                 \
    };                                                                  \
                                                                        \
    static IDCache cache;                                               \
    return VarBase(OPNAME{*this, v},                                    \
                   cache.GetID(ID(), v.ID(), fgNextID));                \
  }                                                                     \
  void dummy2() // trick to require a semicolon

  COMBINATION(*, Times);
  COMBINATION(+, Plus);
  COMBINATION(-, Minus);

  //----------------------------------------------------------------------
  // Have to write division out manually because we want a special case for 0/0
  VarBase VarBase::operator/(const VarBase& v) const
  {
    struct Divide
    {
      const VarBase a, b;
      double operator()(const void* rec) const
      {
        const double denom = b(rec);
        if(denom != 0)
          return a(rec) / denom;
        else
          return 0.0;
      }
    };

    static IDCache cache;
    return VarBase(Divide{*this, v}, cache.GetID(ID(), v.ID(), fgNextID));
  }

  //----------------------------------------------------------------------
  Var2DMapper::Var2DMapper(const Binning& binsa, const Binning& binsb)
    : fBinsA(binsa), fBinsB(binsb)
  {
  }

  double Var2DMapper::Map(double va, double vb) const
  {
    // Since there are no overflow/underflow bins, check the range
    if(va < fBinsA.Min() || vb < fBinsB.Min()) return -1;
    if(va > fBinsA.Max() || vb > fBinsB.Max()) return fBinsA.NBins() * fBinsB.NBins();

    // FindBin uses root convention, first bin is bin 1, bin 0 is underflow
    const int ia = fBinsA.FindBin(va) - 1;
    const int ib = fBinsB.FindBin(vb) - 1;

    const int i = ia*fBinsB.NBins()+ib;

    return i+.5;
  }

  std::vector<double> Var2DMapper::Map(const std::vector<double>& vas,
                                       const std::vector<double>& vbs) const
  {
    assert(vas.size() == vbs.size());
    std::vector<double> ret;
    ret.reserve(vas.size());
    for(unsigned int i = 0; i < vas.size(); ++i)
      ret.push_back(Map(vas[i], vbs[i]));
    return ret;
  }

  /// Helper for 2D VarBase constructor
  class Var2DFunc : protected Var2DMapper
  {
  public:
    Var2DFunc(const VarBase& a, const Binning& binsa,
              const VarBase& b, const Binning& binsb)
      : Var2DMapper(binsa, binsb), fA(a), fB(b)
    {
    }

    double operator()(const void* rec) const
    {
      return Map(fA(rec), fB(rec));
    }

  protected:
    const VarBase fA, fB;
  };


  Var3DMapper::Var3DMapper(const Binning& binsa, const Binning& binsb, const Binning& binsc)
    : fBinsA(binsa), fBinsB(binsb), fBinsC(binsc)
  {
  }

  double Var3DMapper::Map(double va, double vb, double vc) const
  {
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

  std::vector<double> Var3DMapper::Map(const std::vector<double>& vas,
                                       const std::vector<double>& vbs,
                                       const std::vector<double>& vcs) const
  {
    assert(vas.size() == vbs.size() && vbs.size() == vcs.size());
    std::vector<double> ret;
    ret.reserve(vas.size());
    for(unsigned int i = 0; i < vas.size(); ++i)
      ret.push_back(Map(vas[i], vbs[i], vcs[i]));
    return ret;
  }

  /// Helper for 3D VarBase constructor
  class Var3DFunc : protected Var3DMapper
  {
  public:
    Var3DFunc(const VarBase& a, const Binning& binsa,
              const VarBase& b, const Binning& binsb,
              const VarBase& c, const Binning& binsc)
      : Var3DMapper(binsa, binsb, binsc), fA(a), fB(b),	fC(c)
    {
    }

    double operator()(const void* rec) const
    {
      return Map(fA(rec), fB(rec), fC(rec));
    }

  protected:
    const VarBase fA, fB, fC;
  };

  //----------------------------------------------------------------------
  VarBase::VarBase(const VarBase& a, const Binning& binsa,
                   const VarBase& b, const Binning& binsb)
    : VarBase(Var2DFunc(a, binsa, b, binsb))
  {
  }

  //----------------------------------------------------------------------
  VarBase::VarBase(const VarBase& a, const Binning& binsa,
                   const VarBase& b, const Binning& binsb,
                   const VarBase& c, const Binning& binsc)
    : VarBase(Var3DFunc(a, binsa, b, binsb, c, binsc))
  {
  }
} // namespace
