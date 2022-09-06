#pragma once

#include "CAFAna/Core/IFittable.h"

namespace ana
{
  /// Represents a systematic that can be shifted, but not the implementation
  class IFitSyst: public IFittable
  {
  protected:
    // User shouldn't create these directly, intended as a base class
    IFitSyst(const std::string& shortName, const std::string& latexName);

    IFitSyst(const IFitSyst&) = delete;   // no copying.
    IFitSyst(IFitSyst&& rhs) = delete;    // no moving either.

    virtual ~IFitSyst();

    IFitSyst& operator=(const IFitSyst&) = delete;  // still no copying.
    IFitSyst& operator=(IFitSyst&&)      = delete;  // etc.
  };
}
