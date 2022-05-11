#pragma once

#include "CAFAna/Core/Passthrough.h"
#include "CAFAna/Core/PassthroughEnsemble.h"

#include "CAFAna/Core/Cut.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT, class SpillT> class _CutApplier: public Passthrough<RecT>
  {
  public:
    _CutApplier(_ISource<RecT>& src, const _Cut<RecT, SpillT>& cut)
      : fCut(cut)
    {
      src.Register(this);
    }

    virtual ~_CutApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      if(fCut(rec)) Passthrough<RecT>::HandleRecord(rec, weight);
    }

  protected:
    _Cut<RecT, SpillT> fCut;
  };



  template<class RecT, class SpillT> class _EnsembleCutApplier: public PassthroughEnsemble<RecT>
  {
  public:
    _EnsembleCutApplier(_IEnsembleSource<RecT>& src,
                        const _Cut<RecT, SpillT>& cut)
      : PassthroughEnsemble<RecT>(src), fCut(cut)
    {
      src.Register(this);
    }

    virtual ~_EnsembleCutApplier()
    {
    }

    virtual void HandleSingleRecord(const RecT* rec, double weight, int universeId) override
    {
      if(fCut(rec)) PassthroughEnsemble<RecT>::HandleSingleRecord(rec, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec, const std::vector<double>& weights) override
    {
      if(fCut(rec)) PassthroughEnsemble<RecT>::HandleEnsemble(rec, weights);
    }

  protected:
    _Cut<RecT, SpillT> fCut;
  };
}
