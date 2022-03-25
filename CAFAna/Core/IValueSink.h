#pragma once

#include "CAFAna/Core/IExposureSink.h"

#include "CAFAna/Core/IValueSource.h"

namespace ana::beta
{
  template<> class SinkInterface<double>: public IExposureSink
  {
  public:
    virtual void Fill(double x, double weight) = 0;
  };

  typedef _ISink<double> IValueSink;


  template<> class SinkInterface<EnsembleTag<double>>: public IExposureSink
  {
  public:
    virtual void FillSingle(double x, double weight, int universeId) = 0;

    virtual void FillEnsemble(double x,
                              const std::vector<double>& weights)
    {
      // Default naive implementation
      for(unsigned int i = 0; i < weights.size(); ++i){
        FillSingle(x, weights[i], i);
      }
    }
  };

  typedef _ISink<EnsembleTag<double>> IValueEnsembleSink;


  template<> class SinkInterface<std::pair<double, double>>: public IExposureSink
  {
  public:
    virtual void Fill(double x, double y, double weight) = 0;
  };

  typedef _ISink<std::pair<double, double>> IValuePairSink;
}
