#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

#include "CAFAna/Core/Cut.h"

namespace ana
{
  // TODO is this the best name?
  template<class RecT, class SpillT> class _CutApplier: public _IRecordSink<RecT>, public _IRecordSource<RecT>
  {
  public:
    // TODO the SpillT part of this
    _CutApplier(const _Cut<RecT, SpillT>& cut) : fCut(cut) {}

    virtual ~_CutApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double w = 1)
    {
      if(fCut(rec)){
        for(IValueSink* sink: _IRecordSource<RecT>::fSinks) sink->Fill(rec, w);
      }
    }

  protected:
    _Cut<RecT, SpillT> fCut;
  };
}
