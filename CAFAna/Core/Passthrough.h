#pragma once

#include "CAFAna/Core/PassthroughExposure.h"

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

namespace ana::beta
{
  // Implementation detail for various source/sink classes
  //
  // TODO is this the best name?
  template<class RecT> class Passthrough: public PassthroughExposure<_IRecordSource<RecT>, _IRecordSink<RecT>>
  {
  public:
    virtual void HandleRecord(const RecT* rec, double weight) override
    {
      for(_IRecordSink<RecT>* sink: _IRecordSource<RecT>::fSinks) sink->HandleRecord(rec, weight);
    }

  protected:
    Passthrough(){}
  };
}
