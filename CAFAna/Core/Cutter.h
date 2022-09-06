#pragma once

#include "CAFAna/Core/Passthrough.h"

#include "CAFAna/Core/Cut.h"

namespace ana
{
  /// Transforms a record source by removing records according to a cut
  template<class RecT, class SpillT> class _Cutter: public Passthrough<RecT>
  {
  public:
    _Cutter(_ISource<RecordTag<RecT>>& src, const _Cut<RecT, SpillT>& cut)
      : fCut(cut)
    {
      src.Register(this);
    }

    virtual ~_Cutter()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      if(fCut(rec)) Passthrough<RecT>::HandleRecord(rec, weight);
    }

  protected:
    _Cut<RecT, SpillT> fCut;
  };
}
