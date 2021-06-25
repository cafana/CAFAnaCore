#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

namespace ana::beta
{
  // TODO is this the best name?
  template<class RecT> class _VarApplier: public _IRecordSink<RecT>, public IValueSource
  {
  public:
    _VarApplier(const _Var<RecT>& var) : fVar(var) {}

    virtual ~_VarApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight)
    {
      const double val = fVar(rec);
      // TODO check for NaNs etc that is in existing code
      for(IValueSink* sink: fSinks) sink->Fill(val, weight);
    }

    virtual void HandlePOT(double pot) override
    {
      for(IValueSink* sink: fSinks) sink->FillPOT(pot);
    }

    virtual void HandleLivetime(double livetime) override
    {
      for(IValueSink* sink: fSinks) sink->FillLivetime(livetime);
    }

  protected:
    _Var<RecT> fVar;
  };

  // TODO OK we definitely need a template for this
  template<class RecT> class _VarPairApplier: public _IRecordSink<RecT>, public IValuePairSource
  {
  public:
    _VarPairApplier(const _Var<RecT>& varx, 
                    const _Var<RecT>& vary) : fVarX(varx), fVarY(vary) {}

    virtual ~_VarPairApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight)
    {
      const double valx = fVarX(rec);
      const double valy = fVarY(rec);
      // TODO check for NaNs etc that is in existing code
      for(IValuePairSink* sink: fSinks) sink->Fill(valx, valy, weight);
    }

    virtual void HandlePOT(double pot) override
    {
      for(IValuePairSink* sink: fSinks) sink->FillPOT(pot);
    }

    virtual void HandleLivetime(double livetime) override
    {
      for(IValuePairSink* sink: fSinks) sink->FillLivetime(livetime);
    }

  protected:
    _Var<RecT> fVarX, fVarY;
  };
}
