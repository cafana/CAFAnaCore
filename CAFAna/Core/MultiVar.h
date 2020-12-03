#pragma once

#include "CAFAna/Core/Binning.h"

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

    _MultiVar(const _MultiVar& va, const Binning& binsa,
              const _MultiVar& vb, const Binning& binsb);

    _MultiVar(const _MultiVar& a, int na, double a0, double a1,
              const _MultiVar& b, int nb, double b0, double b1)
      : _MultiVar(a, Binning::Simple(na, a0, a1),
                  b, Binning::Simple(nb, b0, b1))
    {
    } 

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

} // namespace
