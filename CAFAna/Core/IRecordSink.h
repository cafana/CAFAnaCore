#pragma once

#include "CAFAna/Core/IExposureSink.h"

#include "CAFAna/Core/Tags.h"

namespace ana::beta
{
  template<class RecT> class SinkInterface<RecordTag<RecT>>: public IExposureSink
  {
  public:
    virtual void HandleRecord(const RecT* rec, double weight) = 0;
  };

  template<class RecT> using _IRecordSink = _ISink<RecordTag<RecT>>;


  template<class RecT> class SinkInterface<EnsembleTag<RecT>>: public IExposureSink
  {
  public:
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

  template<class RecT> using _IRecordEnsembleSink = _ISink<EnsembleTag<RecT>>;
}
