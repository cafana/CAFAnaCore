#pragma once

namespace ana::beta
{
  class IExposureSink
  {
  public:
    virtual ~IExposureSink() {}

    virtual void FillPOT(double pot) = 0;
    virtual void FillLivetime(double livetime) = 0;

    /// Most instances of this interface will be things like Spectrum
    virtual unsigned int NSinks() const {return 1;}
  };

}
