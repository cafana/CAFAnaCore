#pragma once

#include "CAFAna/Core/ISource.h"
#include "CAFAna/Core/IEnsembleSource.h"

namespace ana::beta
{
  typedef _ISource<double> IValueSource;

  typedef _IEnsembleSource<double> IValueEnsembleSource;

  typedef _ISource<std::pair<double, double>> IValuePairSource;


  class NullValueSource: public IValueSource {};
  static NullValueSource kNullValueSource;

  class NullValueEnsembleSource: public IValueEnsembleSource
  {
  public:
    NullValueEnsembleSource(){}
  };
  static NullValueEnsembleSource kNullValueEnsembleSource;
}
