#pragma once

namespace ana::beta
{
  class IValueSink
  {
  public:
    virtual ~IValueSink() {}

    virtual void Fill(double x, double weight) = 0;
    virtual void FillPOT(double pot) = 0;
    virtual void FillLivetime(double livetime) = 0;
  };

  class IValuePairSink
  {
  public:
    virtual ~IValuePairSink() {}

    virtual void Fill(double x, double y, double weight) = 0;
    virtual void FillPOT(double pot) = 0;
    virtual void FillLivetime(double livetime) = 0;
  };
}
