#pragma once

#include "CAFAna/Core/ISource.h"
#include "CAFAna/Core/IValueSink.h"

#include <vector>

namespace ana::beta
{
  class IValueSource: public _ISource<IValueSink> {};

  class IValueEnsembleSource: public _ISource<IValueEnsembleSink>
  {
  public:
    IValueEnsembleSource()//int nuniv, int multiverseId)
    //      : fNUnivs(nuniv), fMultiverseId(multiverseId)
    {
    }

    virtual ~IValueEnsembleSource() {}

    // TODO based on a proper Multiverse class
    //    virtual int NUniverses() const {return fNUnivs;}
    //    virtual int MultiverseID() const {return fMultiverseId;}

  protected:
    //    int fNUnivs, fMultiverseId;
  };

  using IValuePairSource = _ISource<IValuePairSink>;


  class NullValueSource: public IValueSource {};
  static NullValueSource kNullValueSource;

  class NullValueEnsembleSource: public IValueEnsembleSource
  {
  public:
    NullValueEnsembleSource(){}// : IValueEnsembleSource(0, 0) {}
  };
  static NullValueEnsembleSource kNullValueEnsembleSource;
}
