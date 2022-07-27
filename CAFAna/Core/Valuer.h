#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/IValueSink.h"

#include "CAFAna/Core/EnsureFiniteBase.h"
#include "CAFAna/Core/PassthroughExposure.h"

namespace ana
{
  /// Transform a source of records into a source of values using a Var
  template<class RecT>
  class _Valuer :
    public PassthroughExposure<_IRecordSink<RecT>,
                               IValueSource>,
    protected EnsureFiniteBase
  {
  public:
    _Valuer(_ISource<RecordTag<RecT>>& src, const _Var<RecT>& var)
      : fVar(var)
    {
      src.Register(this);
    }

    virtual ~_Valuer()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val, "Var")) return;
      for(IValueSink* sink: this->fSinks) sink->Fill(val, weight);
    }

  protected:
    _Var<RecT> fVar;
  };
}
