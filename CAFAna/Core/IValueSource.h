#pragma once

#include "CAFAna/Core/IValueSink.h"

#include <vector>

namespace ana::beta
{
  class IValueSource
  {
  public:
    virtual ~IValueSource() {}

    virtual void Register(IValueSink* sink)
    {
      fSinks.push_back(sink);
    }

  protected:
    IValueSource(){}

    IValueSource(const IValueSource&) = delete;
    IValueSource& operator=(const IValueSource&) = delete;

    std::vector<IValueSink*> fSinks;
  };


  class IValueEnsembleSource
  {
  public:
    IValueEnsembleSource(int nuniv, int multiverseId)
      : fNUnivs(nuniv), fMultiverseId(multiverseId)
    {
    }

    virtual ~IValueEnsembleSource() {}

    virtual void Register(IValueEnsembleSink* sink)
    {
      fSinks.push_back(sink);
    }

    // TODO based on a proper Multiverse class
    virtual int NUniverses() const {return fNUnivs;}
    virtual int MultiverseID() const {return fMultiverseId;}

  protected:
    IValueEnsembleSource(){}

    IValueEnsembleSource(const IValueEnsembleSource&) = delete;
    IValueEnsembleSource& operator=(const IValueEnsembleSource&) = delete;

    int fNUnivs, fMultiverseId;

    std::vector<IValueEnsembleSink*> fSinks;
  };


  // TODO is there a clever template way of doing all this?
  class IValuePairSource
  {
  public:
    virtual ~IValuePairSource() {}

    virtual void Register(IValuePairSink* sink)
    {
      fSinks.push_back(sink);
    }

  protected:
    IValuePairSource(){}

    IValuePairSource(const IValuePairSource&) = delete;
    IValuePairSource& operator=(const IValuePairSource&) = delete;

    std::vector<IValuePairSink*> fSinks;
  };


  class NullValueSource: public IValueSource {};
  static NullValueSource kNullValueSource;

  class NullValueEnsembleSource: public IValueEnsembleSource
  {
  public:
    NullValueEnsembleSource() : IValueEnsembleSource(0, 0) {}
  };
  static NullValueEnsembleSource kNullValueEnsembleSource;
}
