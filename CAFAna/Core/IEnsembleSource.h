#pragma once

#include "CAFAna/Core/ISource.h"

#include "CAFAna/Core/Tags.h"

namespace ana{class FitMultiverse;} // TODO TODO

namespace ana::beta
{
  template<class RecT> class _IEnsembleSource: public _ISource<EnsembleTag<RecT>>
  {
  public:
    // TODO TODO
    virtual ana::FitMultiverse* GetMultiverse() const{return 0;}// = 0;

  };
}
