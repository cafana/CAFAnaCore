#pragma once

#include "CAFAna/Core/IRecordSink.h"

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

    IValueSource& operator[](const _Var<RecT>& var)
    {
      return GetVar(var);
    }

    template<class SpillT> _IRecordSource<RecT>& operator[](const _Cut<RecT, SpillT>& cut)
    {
      return GetCut(cut);
    }

  protected:
    std::vector<_IRecordSink<RecT>*> fSinks;

    std::unordered_map<int, std::unique_ptr<_IRecordSource<RecT>>> fCutSources;
    std::unordered_map<int, std::unique_ptr<IValueSource>> fVarSources;
    std::unordered_map<int, std::unique_ptr<IValuePairSource>> fVarPairSources;
  };
};


#include "CAFAna/Core/VarApplier.h"
#include "CAFAna/Core/CutApplier.h"

namespace ana::beta
{
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

    _CutApplier<RecT, SpillT>* ret = new  _CutApplier<RecT, SpillT>(cut);
    fCutSources[cut.ID()].reset(ret);
    Register(ret);
    return *ret;
  }
}
