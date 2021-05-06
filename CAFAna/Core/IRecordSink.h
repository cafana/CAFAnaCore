#pragma once

namespace ana
{
  template<class RecT> class _IRecordSink
  {
  public:
    virtual ~_IRecordSink(){}

    virtual void HandleRecord(const RecT* rec, double w = 1) = 0;
  };
}
