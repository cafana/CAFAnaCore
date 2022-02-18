#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/Weight.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT> class _EnsembleSource: public PassthroughExposure<_IRecordSink<RecT>, _IRecordEnsembleSource<RecT>>
  {
  public:
    _EnsembleSource(_ISource<_IRecordSink<RecT>>& src,
                    const std::vector<_Weight<RecT>>& weis,
                    int multiverseId)
      : fWeights(weis), fMultiverseId(multiverseId)
    {
      src.Register(this);
    }

    virtual ~_EnsembleSource()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      if(weight == 0) return;

      std::vector<double> ws(fWeights.size(), weight);
      for(unsigned int i = 0; i < fWeights.size(); ++i){
        // TODO warning/error about negative weights?
        ws[i] *= fWeights[i](rec);
      }

      for(_IRecordEnsembleSink<RecT>* sink: _IRecordEnsembleSource<RecT>::fSinks) sink->HandleEnsemble(rec, ws);
    }

    // TODO TODO
    //    virtual int NUniverses() const override {return fWeights.size();}
    //    virtual int MultiverseID() const override {return fMultiverseId;}

  protected:
    std::vector<_Weight<RecT>> fWeights;
    int fMultiverseId;
  };
}
