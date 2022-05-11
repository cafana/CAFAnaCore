#pragma once

#include "CAFAna/Core/ISource.h"
#include "CAFAna/Core/IRecordSink.h"
#include "CAFAna/Core/IValueSource.h"
#include "CAFAna/Core/Cut.h"
#include "CAFAna/Core/Var.h"
#include "CAFAna/Core/Weight.h"

#include <numeric>
#include <unordered_map>

namespace ana::beta
{
  /// Helper for implementation of _IRecordSourceDefaultImpl
  template<class ElemT> class IDDict
  {
  public:
    /// Return the element with ID 'id' or construct a new element of type
    /// ConsT with arguments cons_args
    template<class ConsT, class... Ts> ElemT& Get(int id, Ts&&... cons_args)
    {
      auto it = fElems.find(id);

      if(it != fElems.end()) return *it->second;

      return *fElems.emplace(id, new ConsT(cons_args...)).first->second;
    }

  protected:
    std::unordered_map<int, std::unique_ptr<ElemT>> fElems;
  };


  template<class RecT> class _IRecordEnsembleSource;
  template<class RecT> class _IRecordSource;


  template<class RecT> class _IRecordEnsembleSourceDefaultImpl : public _IEnsembleSource<RecT>
  {
  public:
    using Record_t = RecT;

    virtual ~_IRecordEnsembleSourceDefaultImpl() {}

    IValueEnsembleSource& GetVar(const _Var<RecT>& var);

    template<class SpillT> _IRecordEnsembleSource<RecT>& GetCut(const _Cut<RecT, SpillT>& cut);

    //    _IRecordEnsembleSource<RecT>& Weighted(const _Weight<RecT>& wei);

    IValueEnsembleSource& operator[](const _Var<RecT>& var)
    {
      return GetVar(var);
    }

    template<class SpillT> _IRecordEnsembleSource<RecT>& operator[](const _Cut<RecT, SpillT>& cut)
    {
      return GetCut(cut);
    }

  protected:
    IDDict<_IRecordEnsembleSource<RecT>> fCutSources, fWeightSources;

    IDDict<IValueEnsembleSource> fVarSources;
  };


  template<class RecT> class _IRecordSourceDefaultImpl : public _ISource<RecT>
  {
  public:
    using Record_t = RecT;

    virtual ~_IRecordSourceDefaultImpl() {}

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

    _IRecordEnsembleSource<RecT>& Ensemble(const std::vector<_Weight<RecT>>& weis,
                                           int multiverseId);

    /*
    _IRecordSource<RecT>& operator[](const _Weight<RecT>& wei)
    {
      return Weighted(wei);
    }
    */

  protected:
    IDDict<_IRecordSource<RecT>> fCutSources, fWeightSources;

    IDDict<_IRecordEnsembleSource<RecT>> fEnsembleSources;

    IDDict<IValueSource> fVarSources;
    IDDict<IValuePairSource> fVarPairSources;
  };


  // By default, a RecordSource is implemented exactly as specified above. But
  // the user may choose to specialize it somehow.
  template<class RecT> class _IRecordEnsembleSource : public _IRecordEnsembleSourceDefaultImpl<RecT> {};
  template<class RecT> class _IRecordSource : public _IRecordSourceDefaultImpl<RecT> {};

  // A source that will never provide anything
  template<class RecT> class NullSource : public _IRecordSource<RecT> {};
  template<class RecT> class NullEnsembleSource : public _IRecordEnsembleSource<RecT> {};
}


#include "CAFAna/Core/CutApplier.h"
#include "CAFAna/Core/VarApplier.h"
#include "CAFAna/Core/WeightApplier.h"
#include "CAFAna/Core/EnsembleSource.h"

namespace ana::beta
{
  template<class RecT> IValueSource&
  _IRecordSourceDefaultImpl<RecT>::GetVar(const _Var<RecT>& var)
  {
    return fVarSources.template Get<_VarApplier<RecT>>(var.ID(), *this, var);
  }


  template<class RecT> IValuePairSource&
  _IRecordSourceDefaultImpl<RecT>::GetVars(const _Var<RecT>& varx,
                                           const _Var<RecT>& vary)
  {
    return fVarPairSources.template Get<_VarPairApplier<RecT>>((varx*vary).ID(), *this, varx, vary);
  }


  template<class RecT> template<class SpillT> _IRecordSource<RecT>&
  _IRecordSourceDefaultImpl<RecT>::GetCut(const _Cut<RecT, SpillT>& cut)
  {
    return fCutSources.template Get<_CutApplier<RecT, SpillT>>(cut.ID(), *this, cut);
  }


  template<class RecT> _IRecordSource<RecT>&
  _IRecordSourceDefaultImpl<RecT>::Weighted(const _Weight<RecT>& wei)
  {
    return fWeightSources.template Get<_WeightApplier<RecT>>(wei.ID(), *this, wei);
  }


  template<class RecT> _IRecordEnsembleSource<RecT>& _IRecordSourceDefaultImpl<RecT>::
  Ensemble(const std::vector<_Weight<RecT>>& weis, int multiverseId)
  {
    // TODO relying on the user to number their multiverses. Better to have a
    // proper Multiverse class we can rely on?

    return fEnsembleSources.template Get<_EnsembleSource<RecT>>(multiverseId, *this, weis, multiverseId);
  }


  template<class RecT> IValueEnsembleSource&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetVar(const _Var<RecT>& var)
  {
    return fVarSources.template Get<_EnsembleVarApplier<RecT>>(var.ID(), *this, var);
  }


  template<class RecT> template<class SpillT> _IRecordEnsembleSource<RecT>&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetCut(const _Cut<RecT, SpillT>& cut)
  {
    return fCutSources.template Get<_EnsembleCutApplier<RecT, SpillT>>(cut.ID(), *this, cut);
  }

}