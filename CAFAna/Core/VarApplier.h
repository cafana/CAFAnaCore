#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

namespace ana
{
  // TODO is this the best name?
  template<class RecT> class _VarApplier: public _IRecordSink<RecT>, public IValueSource
  {
  public:
    _VarApplier(const _Var<RecT>& var) : fVar(var) {}

    virtual ~_VarApplier()
    {
      //      for(IValueSink** sink: fSinks) if(*sink) delete (*sink);
    }

    virtual void HandleRecord(const RecT* rec, double w = 1)
    {
      const double val = fVar(rec);
      // TODO check for NaNs etc that is in existing code
      //      for(IValueSink** sink: fSinks) if(*sink) (*sink)->Fill(val, w);
      for(IValueSink* sink: fSinks) sink->Fill(val, w);
    }

  protected:
    _Var<RecT> fVar;
  };
}
