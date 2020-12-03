/// \file FwdDeclare.h -- consolidate forward declarations of template types
///                       and associated typdefs into one place so that they
///                       can be easily updated if needed.

#pragma once

namespace osc
{
  template<class T> class _IOscCalc;
  template<class T> class _IOscCalcAdjustable;

  typedef _IOscCalc<double> IOscCalc;
  typedef _IOscCalcAdjustable<double> IOscCalcAdjustable;
}

namespace caf
{
  // We assert that we know the names of the downstream experiment's standard
  // record types so that we can instantiate our templates, but we don't need
  // to know anything else about them.
  class StandardRecord;
  class SRSpill;
  class SRNeutrino;

  // Likewise SRProxy
  template<class T> class Proxy;

  typedef Proxy<StandardRecord> SRProxy;
  typedef Proxy<SRSpill> SRSpillProxy;
  typedef Proxy<SRNeutrino> SRNeutrinoProxy;
}

namespace ana
{
  class Spectrum;
  template<class T> class DepMan;

  template<class RecT, class SpillT> class _Cut;
}
