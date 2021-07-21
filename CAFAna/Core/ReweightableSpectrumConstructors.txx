#include "CAFAna/Core/Spectrum.h"

namespace ana
{
#ifdef CAFANACORE_SPECTRUMLOADERBASE
  //----------------------------------------------------------------------
  template<class T, class U> ReweightableSpectrum::
  ReweightableSpectrum(SpectrumLoaderBase& loader,
                       const _HistAxis<_Var<T>>& recoAxis,
                       const _HistAxis<_Var<T>>& trueAxis,
                       const _Cut<T, U>& cut,
                       const SystShifts& shift,
                       const _Weight<T>& wei)
    : ReweightableSpectrum(recoAxis, trueAxis)
  {
    fMat.resize(trueAxis.GetBins1D().NBins()+2, recoAxis.GetBins1D().NBins()+2);
    fMat.setZero();

    if(recoAxis.HasVars()) loader.AddReweightableSpectrum(*this, recoAxis.GetVar1D(), trueAxis.GetVar1D(), cut, shift, wei);
  }
#endif
}
