#pragma once

//#include "CAFAna/Core/Binning.h"

#include "CAFAna/Core/Var.h" // for Var2DMapper

#include <functional>
#include <vector>

//#include "StandardRecord/Proxy/FwdDeclare.h"

// TODO unify into one header
// namespace caf{
//   class StandardRecord;
//   class SRSpill;
//   class SRNeutrino;
//   template<class T> class Proxy;
//   typedef Proxy<StandardRecord> SRProxy;
//   typedef Proxy<SRSpill> SRSpillProxy;
//   typedef Proxy<SRNeutrino> SRNeutrinoProxy;
// }

namespace ana
{
  class MultiVarID
  {
  public:
    MultiVarID();

    /// Vars with the same definition will have the same ID
    int ID() const {return fID;}

    static int MaxID() {return fgNextID-1;}

  protected:
    int fID;
    /// The next ID that hasn't yet been assigned
    static int fgNextID;
  };


  /// A Var that returns multiple results for each slice. eg the properties of
  /// multiple prongs. All results will be filled into the Spectrum.
  template<class T> class _MultiVar: public MultiVarID
  {
  public:
    /// The type of the function part of a var
    typedef std::vector<double> (VarFunc_t)(const T* sr);

    /// std::function can wrap a real function, function object, or lambda
    _MultiVar(const std::function<VarFunc_t>& fun)
      : fFunc(fun)
    {
    }

    _MultiVar(const _MultiVar& va, const Binning& binsa,
              const _MultiVar& vb, const Binning& binsb)
      : _MultiVar(Func2D(va, binsa, vb, binsb))
    {
    }

    _MultiVar(const _MultiVar& a, int na, double a0, double a1,
              const _MultiVar& b, int nb, double b0, double b1)
      : _MultiVar(a, Binning::Simple(na, a0, a1),
                  b, Binning::Simple(nb, b0, b1))
    {
    }

    _MultiVar(const _MultiVar& va, const Binning& binsa,
              const _MultiVar& vb, const Binning& binsb,
              const _MultiVar& vc, const Binning& binsc)
      : _MultiVar(Func3D(va, binsa, vb, binsb, vc, binsc))
    {
    } 

    /// Allows a variable to be called with values = myVar(sr) syntax
    std::vector<double> operator()(const T* sr) const
    {
      return fFunc(sr);
    }

  protected:
    struct Func2D: protected Var2DMapper
    {
      Func2D(const _MultiVar& a, const Binning binsa,
             const _MultiVar& b, const Binning binsb)
        : Var2DMapper(binsa, binsb), fA(a), fB(b) {}
      auto operator()(const T* x) const{return Map(fA(x), fB(x));}
      const _MultiVar fA, fB;
    };

    struct Func3D: protected Var3DMapper
    {
      Func3D(const _MultiVar& a, const Binning binsa,
             const _MultiVar& b, const Binning binsb,
             const _MultiVar& c, const Binning binsc)
        : Var3DMapper(binsa, binsb, binsc), fA(a), fB(b), fC(c) {}
      auto operator()(const T* x) const{return Map(fA(x), fB(x), fC(x));}
      const _MultiVar fA, fB, fC;
    };

    std::function<VarFunc_t> fFunc;
  };

  //  typedef _MultiVar<caf::SRProxy> MultiVar;

} // namespace
