#pragma once

#include "CAFAna/Core/ISource.h"
#include "CAFAna/Core/Tags.h"

namespace ana::beta
{
  typedef _ISource<double> IValueSource;

  typedef _ISource<EnsembleTag<double>> IValueEnsembleSource;

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
