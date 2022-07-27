#pragma once

#include "CAFAna/Core/IExposureSink.h"

#include "CAFAna/Core/Tags.h"

namespace ana
{
  template<class RecT> class SinkInterface<RecordTag<RecT>>: public IExposureSink
  {
  public:
    virtual void HandleRecord(const RecT* rec, double weight) = 0;
  };

  template<class RecT> using _IRecordSink = _ISink<RecordTag<RecT>>;
}
