#include "CAFAna/Core/Spectrum.h"

namespace ana
{
#ifdef CAFANACORE_SPECTRUMLOADERBASE
  //----------------------------------------------------------------------
  template<class T, class U>
  Spectrum::Spectrum(SpectrumLoaderBase& loader,
                     const _HistAxis<_Var<T>>& axis,
                     const _Cut<T, U>& cut,
                     const SystShifts& shift,
                     const _Weight<T>& wei,
                     Spectrum::ESparse sparse)
    : Spectrum(LabelsAndBins(axis.GetLabels(), axis.GetBinnings()), sparse)
  {
    if(axis.HasVars()) loader.AddSpectrum(*this, axis.GetVar1D(), cut, shift, wei);
  }

  //----------------------------------------------------------------------
  template<class T, class U>
  Spectrum::Spectrum(const std::string& label,
                     const Binning& bins,
                     SpectrumLoaderBase& loader,       
                     const _Var<T>& var,
                     const _Cut<T, U>& cut,
                     const SystShifts& shift,
                     const _Weight<T>& wei,
                     Spectrum::ESparse sparse)
    : Spectrum(loader, _HistAxis<_Var<T>>(label, bins, var), cut, shift, wei, sparse)
  {
  }

  //----------------------------------------------------------------------
  template<class T, class U>
  Spectrum::Spectrum(SpectrumLoaderBase& loader,
                     const _HistAxis<_MultiVar<T>>& axis,
                     const _Cut<T, U>& cut,
                     const SystShifts& shift,
                     const _Weight<T>& wei)
    : Spectrum(LabelsAndBins(axis.GetLabels(), axis.GetBinnings()))
  {
    if(axis.HasVars()) loader.AddSpectrum(*this, axis.GetVar1D(), cut, shift, wei);
  }

  //----------------------------------------------------------------------
  template<class T, class U>
  Spectrum::Spectrum(SpectrumLoaderBase& loader,
                     const _HistAxis<_Var<T>>& xAxis,
                     const _HistAxis<_Var<T>>& yAxis,
                     const _Cut<T, U>& cut,
                     const SystShifts& shift,
                     const _Weight<T>& wei,
                     ESparse sparse)
    : Spectrum(loader, _HistAxis<_Var<T>>(xAxis, yAxis), cut, shift, wei, sparse)
  {
  }

  //----------------------------------------------------------------------
  template<class T, class U>
  Spectrum::Spectrum(const std::string& xLabel,
	             const std::string& yLabel,
                     SpectrumLoaderBase& loader,
                     const Binning& binsx, const _Var<T>& varx,
                     const Binning& binsy, const _Var<T>& vary,
                     const _Cut<T, U>& cut,
                     const SystShifts& shift,
                     const _Weight<T>& wei,
                     ESparse sparse)
    : Spectrum(loader, _HistAxis<_Var<T>>(xLabel, binsx, varx,
                                          yLabel, binsy, vary),
               cut, shift, wei, sparse)
  {
  }

  //----------------------------------------------------------------------
  template<class T, class U>
  Spectrum::Spectrum(SpectrumLoaderBase& loader,
                     const _HistAxis<_Var<T>>& xAxis,
                     const _HistAxis<_Var<T>>& yAxis,
                     const _HistAxis<_Var<T>>& zAxis,
                     const _Cut<T, U>& cut,
                     const SystShifts& shift,
                     const _Weight<T>& wei,
                     ESparse sparse)
    : Spectrum(loader, _HistAxis<_Var<T>>(xAxis, yAxis, zAxis), cut, shift, wei, sparse)
  {
  }

  //----------------------------------------------------------------------
  template<class T, class U>
  Spectrum::Spectrum(const std::string& xLabel,
                     const std::string& yLabel,
                     const std::string& zLabel,
                     SpectrumLoaderBase& loader,
                     const Binning& binsx, const _Var<T>& varx,
                     const Binning& binsy, const _Var<T>& vary,
                     const Binning& binsz, const _Var<T>& varz,
                     const _Cut<T, U>& cut,
                     const SystShifts& shift,
                     const _Weight<T>& wei,
                     ESparse sparse)
    : Spectrum(loader, _HistAxis<_Var<T>>(_HistAxis<_Var<T>>(xLabel, binsx, varx),
                                          _HistAxis<_Var<T>>(yLabel, binsy, vary),
                                          _HistAxis<_Var<T>>(zLabel, binsz, varz)),
               cut, shift, wei, sparse)
  {
  }
#endif
}
