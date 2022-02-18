#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/Passthrough.h"

namespace ana::beta
{
  class ApplierBase
  {
  protected:
    ApplierBase(){}
    bool EnsureFinite(double val) const;
  };


  // TODO is this the best name?
  template<class RecT> class _VarApplier: public PassthroughExposure<_IRecordSink<RecT>, IValueSource>, protected ApplierBase
  {
  public:
    _VarApplier(_ISource<_IRecordSink<RecT>>& src, const _Var<RecT>& var)
      : fVar(var)
    {
      src.Register(this);
    }

    virtual ~_VarApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val)) return;
      for(IValueSink* sink: this->fSinks) sink->Fill(val, weight);
    }

  protected:
    _Var<RecT> fVar;
  };


  // TODO is this the best name?
  template<class RecT> class _EnsembleVarApplier: public PassthroughExposure<_IRecordEnsembleSink<RecT>, IValueEnsembleSource>, protected ApplierBase
  {
  public:
    _EnsembleVarApplier(_ISource<_IRecordEnsembleSink<RecT>>& src, const _Var<RecT>& var)
      : fVar(var)
    {
      src.Register(this);
    }

    virtual ~_EnsembleVarApplier()
    {
    }

    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val)) return;
      for(IValueEnsembleSink* sink: this->fSinks) sink->FillSingle(val, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec,
                                const std::vector<double>& weights) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val)) return;
      for(IValueEnsembleSink* sink: this->fSinks) sink->FillEnsemble(val, weights);
    }

  protected:
    _Var<RecT> fVar;
  };


  // TODO OK we definitely need a template for this
  template<class RecT> class _VarPairApplier: public PassthroughExposure<_IRecordSink<RecT>, IValuePairSource>, protected ApplierBase
  {
  public:
    _VarPairApplier(_ISource<_IRecordSink<RecT>>& src,
                    const _Var<RecT>& varx, 
                    const _Var<RecT>& vary)
      : fVarX(varx), fVarY(vary)
    {
      src.Register(this);
    }

    virtual ~_VarPairApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      const double valx = fVarX(rec);
      const double valy = fVarY(rec);
      if(!EnsureFinite(valx) || !EnsureFinite(valy)) return;
      for(IValuePairSink* sink: this->fSinks) sink->Fill(valx, valy, weight);
    }

  protected:
    _Var<RecT> fVarX, fVarY;
  };
}
