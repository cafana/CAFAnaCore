#pragma once

#include "CAFAna/Core/ISink.h"
#include "CAFAna/Core/Tags.h"

namespace ana
{
  template<> class SinkInterface<ExposureTag>
  {
  public:
    virtual void FillPOT(double pot) = 0;
    virtual void FillLivetime(double livetime) = 0;

    /// Most instances of this interface will be things like Spectrum
    virtual unsigned int NSinks() const {return 1;}
  };

  typedef _ISink<ExposureTag> IExposureSink;
}
