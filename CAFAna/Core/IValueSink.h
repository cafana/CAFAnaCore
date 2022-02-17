#pragma once

namespace ana::beta
{
  class IValueSink
  {
  public:
    virtual ~IValueSink() {}

    virtual void Fill(double x, double weight, int universeId) = 0;

    virtual void FillEnsemble(double x,
                              const std::vector<double>& weights,
                              int multiverseId)
    {
      // Default naive implementation
      for(unsigned int i = 0; i < weights.size(); ++i){
        Fill(x, weights[i], multiverseId*10000 + i);
      }
    }

    virtual void FillPOT(double pot) = 0;
    virtual void FillLivetime(double livetime) = 0;

    /// Most instances of this interface will be things like Spectrum
    virtual unsigned int NSinks() const {return 1;}
  };

  class IValuePairSink
  {
  public:
    virtual ~IValuePairSink() {}

    virtual void Fill(double x, double y, double weight) = 0;
    virtual void FillPOT(double pot) = 0;
    virtual void FillLivetime(double livetime) = 0;

    /// Most instances of this interface will be things like ReweightableSpectrum
    virtual unsigned int NSinks() const {return 1;}
  };
}
