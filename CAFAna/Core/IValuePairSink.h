#pragma once

#include "CAFAna/Core/ISink.h"
#include "CAFAna/Core/IExposureSink.h"

namespace ana
{
  template<> class SinkInterface<std::pair<double, double>>: public IExposureSink
  {
  public:
    virtual void Fill(double x, double y, double weight) = 0;
  };

  typedef _ISink<std::pair<double, double>> IValuePairSink;
}
