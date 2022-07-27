#pragma once

#include "CAFAna/Core/IRecordEnsembleSink.h"
#include "CAFAna/Core/IRecordEnsembleSource.h"

#include "CAFAna/Core/IValuePairEnsembleSink.h"

#include "CAFAna/Core/EnsureFiniteBase.h"
#include "CAFAna/Core/PassthroughExposure.h"

namespace ana
{
  /// Transform a source of ensemble records into a source of ensemble value
  /// pairs using two Vars
  template<class RecT>
  class _EnsemblePairValuer :
    public PassthroughExposure<_IRecordEnsembleSink<RecT>,
                               IValuePairEnsembleSource>,
    protected EnsureFiniteBase
  {
  public:
    _EnsemblePairValuer(_IEnsembleSource<RecT>& src,
                        const _Var<RecT>& varx, 
                        const _Var<RecT>& vary)
      : fSource(&src), fVarX(varx), fVarY(vary)
    {
      src.Register(this);
    }

    virtual ~_EnsemblePairValuer()
    {
    }

    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      const double valx = fVarX(rec);
      const double valy = fVarY(rec);
      if(!EnsureFinite(valx, "Var") || !EnsureFinite(valy, "Var")) return;
      for(IValuePairEnsembleSink* sink: this->fSinks) sink->FillSingle(valx, valy, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec,
                                const std::vector<double>& weights) override
    {
      const double valx = fVarX(rec);
      const double valy = fVarY(rec);
      if(!EnsureFinite(valx, "Var") || !EnsureFinite(valy, "Var")) return;
      for(IValuePairEnsembleSink* sink: this->fSinks) sink->FillEnsemble(valx, valy, weights);
    }

    virtual const FitMultiverse* GetMultiverse() const override {return fSource->GetMultiverse();}

  protected:
    const _IEnsembleSource<RecT>* fSource;
    _Var<RecT> fVarX, fVarY;
  };
}
