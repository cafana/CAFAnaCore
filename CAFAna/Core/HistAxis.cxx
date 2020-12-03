#include "CAFAna/Core/HistAxis.h"

#include "CAFAna/Core/MultiVar.h"
#include "CAFAna/Core/Var.h"

#include <cassert>
#include <iostream>

namespace ana
{
  // explicitly instantiate the template for the types we know we have
  template class _HistAxis<Var>;
  template class _HistAxis<MultiVar>;
  template class _HistAxis<SpillVar>;
  template class _HistAxis<NuTruthVar>;

  //----------------------------------------------------------------------
  template<class T> _HistAxis<T>::
  _HistAxis(const std::vector<std::string>& labels,
            const std::vector<Binning>& bins,
            const std::vector<T>& vars)
    : LabelsAndBins(labels, bins), fVars(vars)
  {
    assert(fBins.size() == fVars.size() || fVars.empty());
  }

  //----------------------------------------------------------------------
  template<class T>_HistAxis<T>::
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

  //----------------------------------------------------------------------
  template<class T> T _HistAxis<T>::GetVar1D() const
  {
    switch(fVars.size()){
    case 1:
      return fVars[0];
    case 2:
      if constexpr(std::is_same_v<T, MultiVar>)
        return MultiVar2D(fVars[0], fBins[0], fVars[1], fBins[1]);
      else
        return T(fVars[0], fBins[0], fVars[1], fBins[1]);
    case 3:
      if constexpr(!std::is_same_v<T, MultiVar>) // for MultiVar fall-through to the error
        return T(fVars[0], fBins[0],
                 fVars[1], fBins[1],
                 fVars[2], fBins[2]);
    default:
      std::cout << "Error: HistAxis::GetVar1D() doesn't support "
                << fVars.size() << "-dimensional axes" << std::endl;
      abort();
    }
  }
}
