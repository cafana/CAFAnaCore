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

    // TODO does HandleEnsemble() make sense here? Or combine with EnsembleSource?

  protected:
    _Weight<RecT> fWeight;
  };
}
