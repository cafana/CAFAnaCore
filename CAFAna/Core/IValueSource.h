#pragma once

#include "CAFAna/Core/IValueSink.h"

#include <vector>

namespace ana::beta
{
  class IValueSource
  {
  public:
    virtual ~IValueSource() {}

    virtual void Register(IValueSink* sink)
    {
      fSinks.push_back(sink);
    }

  protected:
    std::vector<IValueSink*> fSinks;
  };
};
