#pragma once

#include <vector>

namespace ana::beta
{
  /// Simple base class for various RecordSources etc. Templated over the
  /// /sink/ type, not the record type.
  template<class SinkT> class _ISource
  {
  public:
    /// Where the objects we're sourcing will go
    virtual void Register(SinkT* sink)
    {
      fSinks.push_back(sink);
    }

  protected:
    _ISource(){}
    _ISource(const _ISource&) = delete;
    _ISource& operator=(const _ISource&) = delete;

    std::vector<SinkT*> fSinks;
  };

}
