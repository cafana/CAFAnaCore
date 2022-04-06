#pragma once

#include "CAFAna/Core/INamed.h"

namespace ana
{
  /// \brief Represents something that can be adjusted in the course of a fit,
  /// e.g. an oscillation or systematic parameter
  ///
  /// Adds nothing concrete to INamed, but separates fittable things from other
  /// things that have a name.
  class IFittable: public INamed
  {
  protected:
    // User shouldn't create these directly, intended as a base class
    IFittable(const std::string& shortName, const std::string& latexName);

    IFittable(const IFittable&) = delete;   // no copying.
    IFittable(IFittable&& rhs) = delete;    // no moving either.

    virtual ~IFittable();

    IFittable& operator=(const IFittable&) = delete;  // still no copying.
    IFittable& operator=(IFittable&&)      = delete;  // etc.
  };
}
