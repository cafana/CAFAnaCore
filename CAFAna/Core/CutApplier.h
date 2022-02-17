#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/Cut.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT, class SpillT> class _CutApplier: public _IRecordSink<RecT>, public _IRecordSource<RecT>
  {
  public:
    _CutApplier(const _Cut<RecT, SpillT>& cut) : fCut(cut) {}

    virtual ~_CutApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight, int universeId) override
    {
      if(fCut(rec)){
        for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleRecord(rec, weight, universeId);
      }
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& weights, int multiverseId) override
    {
      if(fCut(rec)){
        for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleEnsemble(rec, weights, multiverseId);
      }
    }

    virtual void HandlePOT(double pot) override
    {
      // TODO make use of the SpillT part of fCut to modify the exposure where
      // necessary.
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
    _Cut<RecT, SpillT> fCut;
  };
}
