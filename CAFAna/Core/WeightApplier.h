#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/Weight.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT> class _WeightApplier: public _IRecordSink<RecT>, public _IRecordSource<RecT>
  {
  public:
    _WeightApplier(const _Weight<RecT>& wei) : fWeight(wei) {}

    virtual ~_WeightApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      const double w = fWeight(rec);
      if(w != 0){ // TODO warning/error about negative weights?
        for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleRecord(rec, w*weight);
      }
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
    _Weight<RecT> fWeight;
  };
}
