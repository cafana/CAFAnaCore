#pragma once

#include "CAFAna/Core/Passthrough.h"

#include "CAFAna/Core/Cut.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT, class SpillT> class _CutApplier: public Passthrough<RecT>
  {
  public:
    template<class SrcT> _CutApplier(SrcT& src, const _Cut<RecT, SpillT>& cut)
      : fCut(cut)
    {
      src.Register(this);
    }

    virtual ~_CutApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight, int universeId) override
    {
      if(fCut(rec)) Passthrough<RecT>::HandleRecord(rec, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& weights, int multiverseId) override
    {
      if(fCut(rec)) Passthrough<RecT>::HandleEnsemble(rec, weights, multiverseId);
    }

  protected:
    _Cut<RecT, SpillT> fCut;
  };
}
