#pragma once

#include "CAFAna/Core/ISource.h"
#include "CAFAna/Core/IEnsembleSource.h"

namespace ana
{
  typedef _ISource<double> IValueSource;

  typedef _IEnsembleSource<double> IValueEnsembleSource;

  typedef _ISource<std::pair<double, double>> IValuePairSource;

  typedef _IEnsembleSource<std::pair<double, double>> IValuePairEnsembleSource;


  class NullValueSource: public IValueSource {};
  static NullValueSource kNullValueSource;

  class NullValueEnsembleSource: public IValueEnsembleSource
  {
  public:
    NullValueEnsembleSource(){}

    // TODO do we need to return a dummy here? Or make EnsembleSpectrum lazier?
    virtual const ana::FitMultiverse* GetMultiverse() const override {return 0;}
  };
  static NullValueEnsembleSource kNullValueEnsembleSource;
}
