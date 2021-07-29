#pragma once

namespace ana::beta
{
  template<class RecT> class _IRecordSink
  {
  public:
    virtual ~_IRecordSink(){}

    virtual void HandleRecord(const RecT* rec, double weight) = 0;
    virtual void HandlePOT(double pot) = 0;
    virtual void HandleLivetime(double livetime) = 0;

    virtual unsigned int NSinks() const = 0;
  };
}
