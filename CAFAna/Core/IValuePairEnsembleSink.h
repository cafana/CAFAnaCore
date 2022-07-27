#pragma once

#include "CAFAna/Core/ISink.h"
#include "CAFAna/Core/IExposureSink.h"

namespace ana
{
  template<> class SinkInterface<EnsembleTag<std::pair<double, double>>>: public IExposureSink
  {
  public:
    virtual void FillSingle(double x, double y, double weight, int universeId) = 0;

    virtual void FillEnsemble(double x, double y,
                              const std::vector<double>& weights)
    {
      // Default naive implementation
      for(unsigned int i = 0; i < weights.size(); ++i){
        FillSingle(x, y, weights[i], i);
      }
    }
  };

  typedef _ISink<EnsembleTag<std::pair<double, double>>> IValuePairEnsembleSink;
}
