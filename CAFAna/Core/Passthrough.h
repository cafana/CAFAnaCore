#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

namespace ana::beta
{
  // Implementation detail for various source/sink classes
  //
  // TODO is this the best name?
  //
  // This is a Mixin. You should inherit from it, and it should inherit from
  // all your desired base classes.
  template<class... BaseTs> class PassthroughExposure: public BaseTs...
  {
  public:
    virtual void FillPOT(double pot) override
    {
      // TODO make use of the SpillT part of fCut to modify the exposure where
      // necessary.
      for(IExposureSink* sink: this->fSinks) sink->FillPOT(pot);
    }

    virtual void FillLivetime(double livetime) override
    {
      for(IExposureSink* sink: this->fSinks) sink->FillLivetime(livetime);
    }

    virtual unsigned int NSinks() const override
    {
      unsigned int tot = 0;
      for(IExposureSink* sink: this->fSinks) tot += sink->NSinks();
      return tot;
    }

  protected:
    PassthroughExposure(){}
  };


  // Implementation detail for various source/sink classes
  //
  // TODO is this the best name?
  template<class RecT> class Passthrough: public PassthroughExposure<_IRecordSource<RecT>, _IRecordSink<RecT>>
  {
  public:
    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleRecord(rec, weight);
    }

  protected:
    Passthrough(){}
  };


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

  protected:
    PassthroughEnsemble(){}
  };
}
