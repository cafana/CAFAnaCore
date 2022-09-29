#pragma once

#include "CAFAna/Core/IGraphElem.h"

#include <algorithm>
#include <unordered_set>

namespace ana
{
  template<class RecT> class _ISource;

  // This is where the HandleRecord functions etc are defined
  template<class RecT> class SinkInterface;

  /// \brief Simple base class for various RecordSinks etc
  ///
  /// A sink is pointed to by one or more source with the same RecT. This class
  /// takes care of that, and notifying the source if the sink is deleted or
  /// moved. The SinkInterface base class implements the various interaces a
  /// sink is expected to have, depending on the template type.
  template<class RecT> class _ISink: public SinkInterface<RecT>, virtual public IGraphElem
  {
  public:
    virtual ~_ISink()
    {
      // Let all the sources know we've gone away
      for(_ISource<RecT>* src: fSources) src->UnRegister(this);
    }

    _ISink(const _ISink& rhs)
    {
      // No need to copy fSources, Register() will do that
      for(_ISource<RecT>* src: rhs.fSources) src->Register(this);
    }

    _ISink(_ISink&& rhs)
    {
      std::swap(fSources, rhs.fSources);
      // Let all the sources know we've moved
      for(_ISource<RecT>* src: fSources) src->Move(&rhs, this);
    }

    _ISink& operator=(const _ISink& rhs)
    {
      // No need to copy fSources, Register() will do that
      for(_ISource<RecT>* src: rhs.fSources) src->Register(this);
      return *this;
    }

    _ISink& operator=(_ISink&& rhs)
    {
      for(_ISource<RecT>* src: fSources) src->UnRegister(this);
      std::swap(fSources, rhs.fSources);
      // Let all the sources know their target has moved
      for(_ISource<RecT>* src: fSources) src->Move(&rhs, this);
      return *this;
    }

    /// Will be called by implementations of ISource::Register
    void AddSource(_ISource<RecT>* src)
    {
      fSources.insert(src);
    }

    /// Will be called by the source when it is destroyed
    void RemoveSource(_ISource<RecT>* src)
    {
      auto it = std::find(fSources.begin(), fSources.end(), src);
      if(it != fSources.end()) fSources.erase(it);
    }

    virtual void PrintGraph(std::ostream& os) const override
    {
      PrintNode(os, this);
      for(_ISource<RecT>* src: fSources) PrintEdge(os, this, src, "gray");
    }

  protected:
    _ISink(){}

    std::unordered_set<_ISource<RecT>*> fSources;
  };
}
