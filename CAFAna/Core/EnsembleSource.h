#pragma once

#include "CAFAna/Core/IRecordEnsembleSink.h"
#include "CAFAna/Core/IRecordEnsembleSource.h"

#include "CAFAna/Core/Weight.h"

namespace ana
{
  // TODO is this the best name?
  template<class RecT> class _EnsembleSource: public PassthroughExposure<_IRecordSink<RecT>, _IRecordEnsembleSource<RecT>>
  {
  public:
    _EnsembleSource(_ISource<RecT>& src,
                    const std::vector<_Weight<RecT>>& weis,
                    const FitMultiverse& multiverse)
      : fWeights(weis), fMultiverse(&multiverse)
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

    virtual const FitMultiverse* GetMultiverse() const override
    {
      return fMultiverse;
    }

  protected:
    std::vector<_Weight<RecT>> fWeights;
    const FitMultiverse* fMultiverse;
  };
}
