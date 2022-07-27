#pragma once

#include "CAFAna/Core/Passthrough.h"

#include "CAFAna/Core/Weight.h"

namespace ana::beta
{
  /// Transform a source of records by weighting them
  template<class RecT> class _Weighter: public Passthrough<RecT>
  {
  public:
    _Weighter(_ISource<RecT>& src, const _Weight<RecT>& wei)
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
      // TODO warning/error about negative weights?
      if(w != 0) Passthrough<RecT>::HandleRecord(rec, w*weight);
    }

  protected:
    _Weight<RecT> fWeight;
  };
}
