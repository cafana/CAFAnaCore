#pragma once

namespace ana::beta
{
  template<class RecT> class _IRecordSink
  {
  public:
    virtual ~_IRecordSink(){}

    virtual void HandleRecord(const RecT* rec, double weight,
                              int universeId) = 0;

    // TODO consider a seperate EnsembleSink, or unify everything to always be
    // in "ensemble mode"
    virtual void HandleEnsemble(const RecT* rec,
                                const std::vector<double>& weights,
                                int multiverseId)
    {
      // Default naive implementation
      for(unsigned int i = 0; i < weights.size(); ++i){
        HandleRecord(rec, weights[i], multiverseId*10000 + i);
      }
    }

    virtual void HandlePOT(double pot) = 0;
    virtual void HandleLivetime(double livetime) = 0;

    virtual unsigned int NSinks() const = 0;
  };
}
