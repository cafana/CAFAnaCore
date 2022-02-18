#pragma once

#include "CAFAna/Core/IExposureSink.h"

namespace ana::beta
{
  template<class RecT> class _IRecordSink : public IExposureSink
  {
  public:
    virtual ~_IRecordSink(){}

    virtual void HandleRecord(const RecT* rec, double weight) = 0;
  };


  template<class RecT> class _IRecordEnsembleSink : public IExposureSink
  {
  public:
    virtual ~_IRecordEnsembleSink(){}

    virtual void HandleSingleRecord(const RecT* rec, double weight,
                                    int universeId) = 0;

    virtual void HandleEnsemble(const RecT* rec,
                                const std::vector<double>& weights)
    {
      // Default naive implementation
      for(unsigned int i = 0; i < weights.size(); ++i){
        HandleSingleRecord(rec, weights[i], i);
      }
    }
  };
}
