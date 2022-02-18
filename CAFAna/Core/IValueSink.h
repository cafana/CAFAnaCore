#pragma once

#include "CAFAna/Core/IExposureSink.h"

namespace ana::beta
{
  class IValueSink : public IExposureSink
  {
  public:
    virtual ~IValueSink() {}

    virtual void Fill(double x, double weight) = 0;
  };


  class IValueEnsembleSink : public IExposureSink
  {
  public:
    virtual ~IValueEnsembleSink()
    {
    }

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


  class IValuePairSink : public IExposureSink
  {
  public:
    virtual ~IValuePairSink() {}

    virtual void Fill(double x, double y, double weight) = 0;
  };
}
