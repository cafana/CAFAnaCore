#pragma once

#include "CAFAna/Core/PassthroughEnsemble.h"

#include "CAFAna/Core/Cut.h"

namespace ana
{
  /// Transforms an ensemble record source by removing records according to a
  /// cut
  template<class RecT, class SpillT> class _EnsembleCutter: public PassthroughEnsemble<RecT>
  {
  public:
    _EnsembleCutter(_IEnsembleSource<RecT>& src,
                    const _Cut<RecT, SpillT>& cut)
      : PassthroughEnsemble<RecT>(src), fCut(cut)
    {
      src.Register(this);
    }

    virtual ~_EnsembleCutter()
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
