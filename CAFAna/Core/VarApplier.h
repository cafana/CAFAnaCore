#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IRecordSource.h"

namespace ana::beta
{
  class ApplierBase
  {
  protected:
    ApplierBase(){}
    bool EnsureFinite(double val) const;
  };

  // TODO is this the best name?
  template<class RecT> class _VarApplier: public _IRecordSink<RecT>, public IValueSource, protected ApplierBase
  {
  public:
    _VarApplier(const _Var<RecT>& var) : fVar(var) {}

    virtual ~_VarApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight, int universeId) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val)) return;
      for(IValueSink* sink: fSinks) sink->Fill(val, weight, universeId);
    }

    virtual void HandleEnsemble(const RecT* rec,
                                const std::vector<double>& weights,
                                int multiverseId) override
    {
      const double val = fVar(rec);
      if(!EnsureFinite(val)) return;
      for(IValueSink* sink: fSinks) sink->FillEnsemble(val, weights, multiverseId);
    }

    virtual void HandlePOT(double pot) override
    {
      for(IValueSink* sink: fSinks) sink->FillPOT(pot);
    }

    virtual void HandleLivetime(double livetime) override
    {
      for(IValueSink* sink: fSinks) sink->FillLivetime(livetime);
    }

    virtual unsigned int NSinks() const override
    {
      unsigned int tot = 0;
      for(const IValueSink* sink: fSinks) tot += sink->NSinks();
      return tot;
    }

  protected:
    _Var<RecT> fVar;
  };

  // TODO OK we definitely need a template for this
  template<class RecT> class _VarPairApplier: public _IRecordSink<RecT>, public IValuePairSource, protected ApplierBase
  {
  public:
    _VarPairApplier(const _Var<RecT>& varx, 
                    const _Var<RecT>& vary) : fVarX(varx), fVarY(vary) {}

    virtual ~_VarPairApplier()
    {
    }

    virtual void HandleRecord(const RecT* rec, double weight, int universeId) override
    {
      assert(universeId == 0); // nominal

      const double valx = fVarX(rec);
      const double valy = fVarY(rec);
      if(!EnsureFinite(valx) || !EnsureFinite(valy)) return;
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

    virtual unsigned int NSinks() const override
    {
      unsigned int tot = 0;
      for(const IValuePairSink* sink: fSinks) tot += sink->NSinks();
      return tot;
    }

  protected:
    _Var<RecT> fVarX, fVarY;
  };
}
