#pragma once

#include "CAFAna/Core/PassthroughEnsemble.h"

#include "CAFAna/Core/EnsureFiniteBase.h"

#include "CAFAna/Core/Weight.h"

namespace ana
{
  /// Transform a source of ensemble records by weighting them
  template<class RecT> class _EnsembleWeighter: public PassthroughEnsemble<RecT>, protected EnsureFiniteBase
  {
  public:
    _EnsembleWeighter(_IEnsembleSource<RecT>& src,
                           const _Weight<RecT>& wei)
      : PassthroughEnsemble<RecT>(src), fWeight(wei)
    {
      src.Register(this);
    }

    virtual ~_EnsembleWeighter()
    {
    }

    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      const double w = fWeight(rec);
      // TODO also warn/error about negative weights?
      if(EnsureFinite(w, "Weight") && w != 0) PassthroughEnsemble<RecT>::HandleSingleRecord(rec, w*weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& orig_weights) override
    {
      const double w = fWeight(rec);
      // TODO also warn/error about negative weights?
      if(!EnsureFinite(w, "Weight") || w == 0) return;

      std::vector<double> new_weights = orig_weights;
      for(double& nw: new_weights) nw *= w;

      PassthroughEnsemble<RecT>::HandleEnsemble(rec, new_weights);
    }

  protected:
    _Weight<RecT> fWeight;
  };
}
