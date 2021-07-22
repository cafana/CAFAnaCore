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
    IValueSource(){}

    IValueSource(const IValueSource&) = delete;
    IValueSource& operator=(const IValueSource&) = delete;

    std::vector<IValueSink*> fSinks;
  };

  // TODO is there a clever template way of doing all this?
  class IValuePairSource
  {
  public:
    virtual ~IValuePairSource() {}

    virtual void Register(IValuePairSink* sink)
    {
      fSinks.push_back(sink);
    }

  protected:
    IValuePairSource(){}

    IValuePairSource(const IValuePairSource&) = delete;
    IValuePairSource& operator=(const IValuePairSource&) = delete;

    std::vector<IValuePairSink*> fSinks;
  };
}
