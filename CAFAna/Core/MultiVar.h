#pragma once

#include <functional>
#include <vector>

//#include "StandardRecord/Proxy/FwdDeclare.h"

// TODO unify into one header
namespace caf{
  class StandardRecord;
  class SRSpill;
  class SRNeutrino;
  template<class T> class Proxy;
  typedef Proxy<StandardRecord> SRProxy;
  typedef Proxy<SRSpill> SRSpillProxy;
  typedef Proxy<SRNeutrino> SRNeutrinoProxy;
}

namespace ana
{
  class Binning;

  /// A Var that returns multiple results for each slice. eg the properties of
  /// multiple prongs. All results will be filled into the Spectrum.
  template<class T> class _MultiVar
  {
  public:
    /// The type of the function part of a var
    typedef std::vector<double> (VarFunc_t)(const T* sr);

    /// std::function can wrap a real function, function object, or lambda
    _MultiVar(const std::function<VarFunc_t>& fun);

    /// Allows a variable to be called with double value = myVar(sr) syntax
    std::vector<double> operator()(const T* sr) const
    {
      return fFunc(sr);
    }

    /// Vars with the same definition will have the same ID
    int ID() const {return fID;}

    static int MaxID() {return fgNextID-1;}
  protected:
    std::function<VarFunc_t> fFunc;

    int fID;
    /// The next ID that hasn't yet been assigned
    static int fgNextID;
  };

  typedef _MultiVar<caf::SRProxy> MultiVar;

  MultiVar MultiVar2D(const MultiVar& a, const Binning& binsa,
                      const MultiVar& b, const Binning& binsb);

  MultiVar MultiVar2D(const MultiVar& a, int na, double a0, double a1,
                      const MultiVar& b, int nb, double b0, double b1);
} // namespace
