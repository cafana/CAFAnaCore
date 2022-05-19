#pragma once

#include "CAFAna/Core/Passthrough.h"

#include "CAFAna/Core/Weight.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT> class _WeightApplier: public Passthrough<RecT>
  {
  public:
    _WeightApplier(_ISource<RecT>& src, const _Weight<RecT>& wei)
      : fWeight(wei)
    {
      src.Register(this);
    }

    virtual ~_WeightApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      const double w = fWeight(rec);
      // TODO warning/error about negative weights?
      if(w != 0) Passthrough<RecT>::HandleRecord(rec, w*weight);
    }

  protected:
    _Weight<RecT> fWeight;
  };


  template<class RecT> class _EnsembleWeightApplier: public PassthroughEnsemble<RecT>
  {
  public:
    _EnsembleWeightApplier(_IEnsembleSource<RecT>& src,
                           const _Weight<RecT>& wei)
      : fWeight(wei)
    {
      src.Register(this);
    }

    virtual ~_EnsembleWeightApplier()
    {
    }

    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      const double w = fWeight(rec);
      // TODO warning/error about negative weights?
      if(w != 0) PassthroughEnsemble<RecT>::HandleSingleRecord(rec, w*weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& orig_weights) override
    {
      const double w = fWeight(rec);
      // TODO warning/error about negative weights?
      if(w == 0) return;

      std::vector<double> new_weights = orig_weights;
      for(double& nw: new_weights) nw *= w;

      PassthroughEnsemble<RecT>::HandleEnsemble(rec, new_weights);
    }

  protected:
    _Weight<RecT> fWeight;
  };
}
