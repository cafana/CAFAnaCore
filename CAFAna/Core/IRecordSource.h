#pragma once

#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IValueSource.h"
#include "CAFAna/Core/Cut.h"
#include "CAFAna/Core/Var.h"
#include "CAFAna/Core/Weight.h"

#include <numeric>

namespace ana::beta
{
  class IValueSource;

  template<class RecT> class _IRecordSource
  {
  public:
    virtual ~_IRecordSource() {}

    virtual void Register(_IRecordSink<RecT>* sink)
    {
      fSinks.push_back(sink);
    }

    IValueSource& GetVar(const _Var<RecT>& var);

    IValuePairSource& GetVars(const _Var<RecT>& varx,
                              const _Var<RecT>& vary);

    template<class SpillT> _IRecordSource<RecT>& GetCut(const _Cut<RecT, SpillT>& cut);

    _IRecordSource<RecT>& Weighted(const _Weight<RecT>& wei);

    IValueSource& operator[](const _Var<RecT>& var)
    {
      return GetVar(var);
    }

    template<class SpillT> _IRecordSource<RecT>& operator[](const _Cut<RecT, SpillT>& cut)
    {
      return GetCut(cut);
    }

    _IRecordSource<RecT>& Ensemble(const std::vector<_Weight<RecT>>& weis,
                                   int multiverseId);

    /*
    _IRecordSource<RecT>& operator[](const _Weight<RecT>& wei)
    {
      return Weighted(wei);
    }
    */

  protected:
    _IRecordSource(){}

    _IRecordSource(const _IRecordSource&) = delete;
    _IRecordSource& operator=(const _IRecordSource&) = delete;

    std::vector<_IRecordSink<RecT>*> fSinks;

    std::unordered_map<int, std::unique_ptr<_IRecordSource<RecT>>> fCutSources;
    std::unordered_map<int, std::unique_ptr<_IRecordSource<RecT>>> fWeightSources;
    std::unordered_map<int, std::unique_ptr<_IRecordSource<RecT>>> fEnsembleSources;
    std::unordered_map<int, std::unique_ptr<IValueSource>> fVarSources;
    std::unordered_map<int, std::unique_ptr<IValuePairSource>> fVarPairSources;
  };
}


#include "CAFAna/Core/CutApplier.h"
#include "CAFAna/Core/VarApplier.h"
#include "CAFAna/Core/WeightApplier.h"
#include "CAFAna/Core/EnsembleSource.h"

namespace ana::beta
{
  // TODO this is all repetive. Add some kind of cached dict type?
  template<class RecT> IValueSource&
  _IRecordSource<RecT>::GetVar(const _Var<RecT>& var)
  {
    auto it = fVarSources.find(var.ID());

    if(it != fVarSources.end()) return *it->second;

    _VarApplier<RecT>* ret = new _VarApplier<RecT>(var);
    fVarSources[var.ID()].reset(ret);
    Register(ret);
    return *ret;
  }

  template<class RecT> IValuePairSource&
  _IRecordSource<RecT>::GetVars(const _Var<RecT>& varx,
                                const _Var<RecT>& vary)
  {
    auto it = fVarPairSources.find((varx*vary).ID());

    if(it != fVarPairSources.end()) return *it->second;

    _VarPairApplier<RecT>* ret = new _VarPairApplier<RecT>(varx, vary);
    fVarPairSources[(varx*vary).ID()].reset(ret);
    Register(ret);
    return *ret;
  }

  template<class RecT> template<class SpillT> _IRecordSource<RecT>&
  _IRecordSource<RecT>::GetCut(const _Cut<RecT, SpillT>& cut)
  {
    auto it = fCutSources.find(cut.ID());

    if(it != fCutSources.end()) return *it->second;

    _CutApplier<RecT, SpillT>* ret = new _CutApplier<RecT, SpillT>(cut);
    fCutSources[cut.ID()].reset(ret);
    Register(ret);
    return *ret;
  }

  template<class RecT> _IRecordSource<RecT>&
  _IRecordSource<RecT>::Weighted(const _Weight<RecT>& wei)
  {
    auto it = fWeightSources.find(wei.ID());

    if(it != fWeightSources.end()) return *it->second;

    _WeightApplier<RecT>* ret = new _WeightApplier<RecT>(wei);
    fWeightSources[wei.ID()].reset(ret);
    Register(ret);
    return *ret;
  }

  template<class RecT> _IRecordSource<RecT>& _IRecordSource<RecT>::
  Ensemble(const std::vector<_Weight<RecT>>& weis, int multiverseId)
  {
    // TODO relying on the user to number their multiverses. Better to have a
    // proper Multiverse class we can rely on?
    auto it = fEnsembleSources.find(multiverseId);

    if(it != fEnsembleSources.end()) return *it->second;

    _EnsembleSource<RecT>* ret = new _EnsembleSource<RecT>(weis, multiverseId);
    fEnsembleSources[multiverseId].reset(ret);
    Register(ret);
    return *ret;
  }
}
