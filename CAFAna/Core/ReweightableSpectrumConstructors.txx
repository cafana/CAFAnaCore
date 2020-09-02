#include "CAFAna/Core/Spectrum.h"

namespace ana
{
  //----------------------------------------------------------------------
  template<class T> ReweightableSpectrum::
  ReweightableSpectrum(SpectrumLoaderBase& loader,
                       const _HistAxis<_Var<T>>& recoAxis,
                       const _HistAxis<_Var<T>>& trueAxis,
                       const _Cut<T>& cut,
                       const SystShifts& shift,
                       const _Var<T>& wei)
    : ReweightableSpectrum(recoAxis, trueAxis.GetBins1D())
  {
    assert(trueAxis.NDimensions() == 1);

    fTrueLabel = trueAxis.GetLabels()[0];

    fMat.resize(trueAxis.GetBins1D().NBins()+2, recoAxis.GetBins1D().NBins()+2);
    fMat.setZero();

    if(recoAxis.HasVars()) loader.AddReweightableSpectrum(*this, recoAxis.GetVar1D(), trueAxis.GetVar1D(), cut, shift, wei);
  }

}
