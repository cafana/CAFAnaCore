#pragma once

#include "CAFAna/Core/IExposureSink.h"

#include "CAFAna/Core/Tags.h"

namespace ana
{
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
