#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/Weight.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT> class _EnsembleSource: public _IRecordSink<RecT>, public _IRecordSource<RecT>
  {
  public:
    template<class SrcT> _EnsembleSource(SrcT& src,
                                         const std::vector<_Weight<RecT>>& weis,
                                         int multiverseId)
      : fWeights(weis), fMultiverseId(multiverseId)
    {
      src.Register(this);
    }

    virtual ~_EnsembleSource()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight,
                              int universeId) override
    {
      assert(universeId == 0); // nominal

      if(weight == 0) return;

      std::vector<double> ws(fWeights.size(), weight);
      for(unsigned int i = 0; i < fWeights.size(); ++i){
        // TODO warning/error about negative weights?
        ws[i] *= fWeights[i](rec);
      }

      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleEnsemble(rec, ws, fMultiverseId);
    }

    virtual void HandleEnsemble(const RecT* rec,
                                const std::vector<double>& weights,
                                int multiverseId) override
    {
      assert(false && "EnsembleSource::HandleEnsemble() doesn't make sense");
      abort();
    }

    // TODO can we implement these three boilerplate functions somewhere shared?
    virtual void HandlePOT(double pot) override
    {
      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandlePOT(pot);
    }

    virtual void HandleLivetime(double livetime) override
    {
      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleLivetime(livetime);
    }

    virtual unsigned int NSinks() const override
    {
      unsigned int tot = 0;
      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) tot += sink->NSinks();
      return tot;
    }

  protected:
    std::vector<_Weight<RecT>> fWeights;
    int fMultiverseId;
  };
}
