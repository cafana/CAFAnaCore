#pragma once

#include "CAFAna/Core/IEnsembleSource.h"
#include "CAFAna/Core/IRecordEnsembleSink.h"
#include "CAFAna/Core/IValueSource.h"

#include "CAFAna/Core/IDDict.h"
#include "CAFAna/Core/Cut.h"
#include "CAFAna/Core/Var.h"
#include "CAFAna/Core/Weight.h"

namespace ana
{
  class FitMultiverse;

  template<class RecT> class _IRecordEnsembleSource;

  template<class RecT> class _IRecordEnsembleSourceDefaultImpl : public _IEnsembleSource<RecT>
  {
  public:
    using Record_t = RecT;

    virtual ~_IRecordEnsembleSourceDefaultImpl() = default;

    IValueEnsembleSource& GetVar(const _Var<RecT>& var);

    IValuePairEnsembleSource& GetVars(const _Var<RecT>& varx,
                                      const _Var<RecT>& vary);


    template<class SpillT> _IRecordEnsembleSource<RecT>& GetCut(const _Cut<RecT, SpillT>& cut);

    _IRecordEnsembleSource<RecT>& Weighted(const _Weight<RecT>& wei);

    IValueEnsembleSource& operator[](const _Var<RecT>& var)
    {
      return GetVar(var);
    }

    template<class SpillT> _IRecordEnsembleSource<RecT>& operator[](const _Cut<RecT, SpillT>& cut)
    {
      return GetCut(cut);
    }

  protected:
    IDDict<int, _IRecordEnsembleSource<RecT>> fCutSources, fWeightSources;

    IDDict<int, IValueEnsembleSource> fVarSources;
    IDDict<int, IValuePairEnsembleSource> fVarPairSources;
  };

  // By default, a RecordEnsembleSource is implemented exactly as specified
  // above. But the user may choose to specialize it somehow.
  template<class RecT> class _IRecordEnsembleSource : public _IRecordEnsembleSourceDefaultImpl<RecT> {};

  // A source that will never provide anything
  template<class RecT> class NullEnsembleSource : public _IRecordEnsembleSource<RecT> {};
}


#include "CAFAna/Core/EnsembleCutter.h"
#include "CAFAna/Core/EnsembleValuer.h"
#include "CAFAna/Core/EnsemblePairValuer.h"
#include "CAFAna/Core/EnsembleWeighter.h"
//#include "CAFAna/Core/EnsembleSource.h"

namespace ana
{
  template<class RecT> IValueEnsembleSource&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetVar(const _Var<RecT>& var)
  {
    return fVarSources.template Get<_EnsembleValuer<RecT>>(var.ID(), *this, var);
  }

  template<class RecT> IValuePairEnsembleSource&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetVars(const _Var<RecT>& varx,
                                                   const _Var<RecT>& vary)
  {
    return fVarPairSources.template Get<_EnsemblePairValuer<RecT>>((varx*vary).ID(), *this, varx, vary);
  }

  template<class RecT> template<class SpillT> _IRecordEnsembleSource<RecT>&
  _IRecordEnsembleSourceDefaultImpl<RecT>::GetCut(const _Cut<RecT, SpillT>& cut)
  {
    return fCutSources.template Get<_EnsembleCutter<RecT, SpillT>>(cut.ID(), *this, cut);
  }

  template<class RecT> _IRecordEnsembleSource<RecT>&
  _IRecordEnsembleSourceDefaultImpl<RecT>::Weighted(const _Weight<RecT>& wei)
  {
    return fWeightSources.template Get<_EnsembleWeighter<RecT>>(wei.ID(), *this, wei);
  }

}
