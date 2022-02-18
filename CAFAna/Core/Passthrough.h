#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

namespace ana::beta
{
  // Implementation detail for various source/sink classes
  //
  // TODO is this the best name?
  template<class FromT, class ToT> class PassthroughUnlike: public _IRecordSink<FromT>, public _IRecordSource<ToT>
  {
  public:
    virtual void HandlePOT(double pot) override
    {
      // TODO make use of the SpillT part of fCut to modify the exposure where
      // necessary.
      for(_IRecordSink<ToT>* sink: _IRecordSource<ToT>::fSinks) sink->HandlePOT(pot);
    }

    virtual void HandleLivetime(double livetime) override
    {
      for(_IRecordSink<ToT>* sink: _IRecordSource<ToT>::fSinks) sink->HandleLivetime(livetime);
    }

    virtual unsigned int NSinks() const override
    {
      unsigned int tot = 0;
      for(_IRecordSink<ToT>* sink: _IRecordSource<ToT>::fSinks) tot += sink->NSinks();
      return tot;
    }

  protected:
    PassthroughUnlike(){}
  };


  // Implementation detail for various source/sink classes
  //
  // TODO is this the best name?
  template<class RecT> class Passthrough: public PassthroughUnlike<RecT, RecT>
  {
  public:
    virtual void HandleRecord(const RecT* rec, double weight, int universeId) override
    {
      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleRecord(rec, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& weights, int multiverseId) override
    {
      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleEnsemble(rec, weights, multiverseId);
    }

  protected:
    Passthrough(){}
  };
}
