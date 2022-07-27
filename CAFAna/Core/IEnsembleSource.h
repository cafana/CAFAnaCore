#pragma once

#include "CAFAna/Core/ISource.h"

#include "CAFAna/Core/Tags.h"

namespace ana
{
  class FitMultiverse;

  template<class RecT> class _IEnsembleSource: public _ISource<EnsembleTag<RecT>>
  {
  public:
    virtual const ana::FitMultiverse* GetMultiverse() const = 0;
  };
}
