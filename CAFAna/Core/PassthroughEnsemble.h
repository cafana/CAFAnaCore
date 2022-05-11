#pragma once

#include "CAFAna/Core/PassthroughExposure.h"

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

namespace ana::beta
{
  // Implementation detail for various source/sink classes
  //
  // TODO is this the best name?
  template<class RecT> class PassthroughEnsemble: public PassthroughExposure<_IRecordEnsembleSource<RecT>, _IRecordEnsembleSink<RecT>>
  {
  public:
    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      for(_IRecordEnsembleSink<RecT>* sink: _IRecordEnsembleSource<RecT>::fSinks) sink->HandleSingleRecord(rec, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& weights) override
    {
      for(_IRecordEnsembleSink<RecT>* sink: _IRecordEnsembleSource<RecT>::fSinks) sink->HandleEnsemble(rec, weights);
    }

    virtual const FitMultiverse* GetMultiverse() const override
    {
      return fMultiverse;
    }

  protected:
    PassthroughEnsemble(const _IEnsembleSource<RecT>& src)
      : fMultiverse(src.GetMultiverse())
    {
    }

    const FitMultiverse* fMultiverse;
  };
}
