#pragma once

#include "CAFAna/Core/IRecordEnsembleSink.h"
#include "CAFAna/Core/IRecordEnsembleSource.h"

#include "CAFAna/Core/IValueEnsembleSink.h"

#include "CAFAna/Core/EnsureFiniteBase.h"
#include "CAFAna/Core/PassthroughExposure.h"

namespace ana
{
  /// Transform a source of ensemble records into a source of ensemble values
  /// using a Var
  template<class RecT>
  class _EnsembleValuer :
    public PassthroughExposure<_IRecordEnsembleSink<RecT>,
                               IValueEnsembleSource>,
    protected EnsureFiniteBase
  {
  public:
    _EnsembleValuer(_IEnsembleSource<RecT>& src, const _Var<RecT>& var)
      : fSource(&src), fVar(var)
    {
      src.Register(this);
    }

    virtual ~_EnsembleValuer()
    {
    }

    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val, "Var")) return;
      for(IValueEnsembleSink* sink: this->fSinks) sink->FillSingle(val, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec,
                                const std::vector<double>& weights) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val, "Var")) return;
      for(IValueEnsembleSink* sink: this->fSinks) sink->FillEnsemble(val, weights);
    }

    virtual const FitMultiverse* GetMultiverse() const override {return fSource->GetMultiverse();}

  protected:
    const _IEnsembleSource<RecT>* fSource;
    _Var<RecT> fVar;
  };
}
