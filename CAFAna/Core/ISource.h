#pragma once

#include "CAFAna/Core/IGraphElem.h"

#include <algorithm>
#include <vector>

namespace ana
{
  template<class RecT> class _ISink;

  /// \brief Simple base class for various RecordSources etc
  ///
  /// A source has a list of sinks with the same RecT to which it will send its
  /// events / livetime / etc. It provides methods to allow the sink to move or
  /// unregister itself. All the detailed implementation of sources happens in
  /// derived classes.
  template<class RecT> class _ISource : virtual public IGraphElem
  {
  public:
    virtual ~_ISource()
    {
      for(_ISink<RecT>* sink: fSinks) sink->RemoveSource(this);
    }

    /// Where the objects we're sourcing will go
    virtual void Register(_ISink<RecT>* sink)
    {
      fSinks.push_back(sink);
      sink->AddSource(this);
    }

    virtual void UnRegister(_ISink<RecT>* sink)
    {
      auto it = std::find(fSinks.begin(), fSinks.end(), sink);
      if(it != fSinks.end()) fSinks.erase(it);
    }

    void Move(_ISink<RecT>* from, _ISink<RecT>* to)
    {
      for(_ISink<RecT>*& sink: fSinks) if(sink == from) sink = to;
    }

    virtual void PrintGraph(std::ostream& os) const override
    {
      PrintNode(os, this);
      for(_ISink<RecT>* sink: fSinks){
        PrintEdge(os, this, sink);
        sink->PrintGraph(os);
      }
    }

  protected:
    // In practice sources all end up on the heap. Enforce that for simplicity
    _ISource(){}
    _ISource(const _ISource&) = delete;
    _ISource(const _ISource&&) = delete;
    _ISource& operator=(const _ISource&) = delete;
    _ISource& operator=(const _ISource&&) = delete;

    std::vector<_ISink<RecT>*> fSinks;
  };
}
