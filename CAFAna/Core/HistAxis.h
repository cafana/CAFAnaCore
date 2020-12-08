#pragma once

#include "CAFAna/Core/LabelsAndBins.h"

#include <cassert>
#include <string>

namespace ana
{
  [[noreturn]] void HistAxisDimensionError(int dim);

  /// \brief Collect information describing the x-axis of an analysis histogram
  ///
  /// That is, what it should be labelled, what the binning should be, and what
  /// variable will be being filled into it.
  template<class T> class _HistAxis: public LabelsAndBins
  {
  public:
    // The one constructor to rule them all
    _HistAxis(const std::vector<std::string>& labels,
              const std::vector<Binning>& bins,
              const std::vector<T>& vars = {})
      : LabelsAndBins(labels, bins), fVars(vars)
    {
      assert(fBins.size() == fVars.size() || fVars.empty());
    }

    // Forwards
    _HistAxis(const std::string& label,
              const Binning& bins,
              const T& var)
      : _HistAxis(std::vector<std::string>(1, label),
                  std::vector<Binning>(1, bins),
                  std::vector<T>(1, var))
    {
    }

    _HistAxis(const std::string& label,
              const Binning& bins)
      : _HistAxis(std::vector<std::string>(1, label),
                  std::vector<Binning>(1, bins))
    {
    }

    _HistAxis(const std::string& labelX,
              const Binning& binsX,
              const T& varX,
              const std::string& labelY,
              const Binning& binsY,
              const T& varY)
      : _HistAxis({labelX, labelY}, {binsX, binsY}, {varX, varY})
    {
    }

    /// Shortcut for simple binnings
    _HistAxis(const std::string& label,
              int nx, double x0, double x1,
              const T& var)
      : _HistAxis(label, Binning::Simple(nx, x0, x1), var)
    {
    }

    /// Shortcut for simple binnings
    _HistAxis(const std::string& labelX,
              int nx, double x0, double x1,
              const T& varX,
              const std::string& labelY,
              int ny, double y0, double y1,
              const T& varY)
      : _HistAxis(labelX, Binning::Simple(nx, x0, x1), varX,
                  labelY, Binning::Simple(ny, y0, y1), varY)
    {
    }

    _HistAxis(const _HistAxis<T>& ax) = default;

    // MultiD binning expressed as a composition of existing axes
    _HistAxis(const std::vector<_HistAxis<T>>& axes)
      : LabelsAndBins(std::vector<std::string>(), std::vector<Binning>())
    {
      for(const auto& a: axes){
        fLabels.insert(fLabels.end(), a.fLabels.begin(), a.fLabels.end());
        fBins.insert(fBins.end(), a.fBins.begin(), a.fBins.end());
        fVars.insert(fVars.end(), a.fVars.begin(), a.fVars.end());
      }

      assert(fLabels.size() == fBins.size());
      assert(fBins.size() == fVars.size() || fVars.empty());
    }

    _HistAxis(const _HistAxis<T>& xax,
              const _HistAxis<T>& yax)
      : _HistAxis(std::vector<_HistAxis<T>>{xax, yax})
    {
    }

    _HistAxis(const _HistAxis<T>& xax,
              const _HistAxis<T>& yax,
              const _HistAxis<T>& zax)
      : _HistAxis(std::vector<_HistAxis<T>>{xax, yax, zax})
    {
    }

    bool HasVars() const {return !fVars.empty();}

    const std::vector<T>& GetVars() const {return fVars;}

    /// A variable "flattening" all the dimensions into one 1D value. Use
    /// sparingly.
    T GetVar1D() const
    {
      switch(fVars.size()){
      case 1: return fVars[0];
      case 2: return T(fVars[0], fBins[0], fVars[1], fBins[1]);
      case 3: return T(fVars[0], fBins[0], fVars[1], fBins[1], fVars[2], fBins[2]);
      default: HistAxisDimensionError(fVars.size());
      }
    }

  protected:
    std::vector<T> fVars;
  };

  // Typedefs
  //  typedef _HistAxis<Var> HistAxis;
  //  typedef _HistAxis<MultiVar> MultiVarHistAxis;
  //  typedef _HistAxis<SpillVar> SpillHistAxis;
  //  typedef _HistAxis<NuTruthVar> NuTruthHistAxis;
}
