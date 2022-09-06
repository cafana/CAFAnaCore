#pragma once

#include "CAFAna/Core/Passthrough.h"

#include "CAFAna/Core/EnsureFiniteBase.h"

#include "CAFAna/Core/Weight.h"

namespace ana
{
  /// Transform a source of records by weighting them
  template<class RecT> class _Weighter: public Passthrough<RecT>, protected EnsureFiniteBase
  {
  public:
    _Weighter(_ISource<RecordTag<RecT>>& src, const _Weight<RecT>& wei)
      : fWeight(wei)
    {
      src.Register(this);
    }

    virtual ~_Weighter()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      const double w = fWeight(rec);
      // TODO also warn/error about negative weights?
      if(EnsureFinite(w, "Weight") && w != 0) Passthrough<RecT>::HandleRecord(rec, w*weight);
    }

  protected:
    _Weight<RecT> fWeight;
  };
}
