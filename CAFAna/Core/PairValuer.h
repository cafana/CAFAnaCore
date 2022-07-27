#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/IValuePairSink.h"

#include "CAFAna/Core/EnsureFiniteBase.h"
#include "CAFAna/Core/PassthroughExposure.h"

namespace ana
{
  /// Transform a source of records into a source of value pairs using two Vars
  template<class RecT>
  class _PairValuer :
    public PassthroughExposure<_IRecordSink<RecT>,
                               IValuePairSource>,
    protected EnsureFiniteBase
  {
  public:
    _PairValuer(_ISource<RecordTag<RecT>>& src,
                const _Var<RecT>& varx, 
                const _Var<RecT>& vary)
      : fVarX(varx), fVarY(vary)
    {
      src.Register(this);
    }

    virtual ~_PairValuer()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      const double valx = fVarX(rec);
      const double valy = fVarY(rec);
      if(!EnsureFinite(valx, "Var") || !EnsureFinite(valy, "Var")) return;
      for(IValuePairSink* sink: this->fSinks) sink->Fill(valx, valy, weight);
    }

  protected:
    _Var<RecT> fVarX, fVarY;
  };
}
