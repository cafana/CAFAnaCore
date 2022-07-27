#pragma once

#include "CAFAna/Core/IExposureSink.h"

namespace ana
{
  /// \brief Implementation detail for various source/sink classes
  //
  // This is a Mixin. You should inherit from it, and it should inherit from
  // all your desired base classes.
  template<class... BaseTs> class PassthroughExposure: public BaseTs...
  {
  public:
    virtual void FillPOT(double pot) override
    {
      // TODO make use of the SpillT part of fCut to modify the exposure where
      // necessary.
      for(IExposureSink* sink: this->fSinks) sink->FillPOT(pot);
    }

    virtual void FillLivetime(double livetime) override
    {
      for(IExposureSink* sink: this->fSinks) sink->FillLivetime(livetime);
    }

    virtual unsigned int NSinks() const override
    {
      unsigned int tot = 0;
      for(IExposureSink* sink: this->fSinks) tot += sink->NSinks();
      return tot;
    }

    virtual void PrintGraph(std::ostream& os) const
    {
      // Invoke PrintGraphs() on all the base classes
      (BaseTs::PrintGraph(os), ...);
    }

  protected:
    PassthroughExposure(){}
  };
}
