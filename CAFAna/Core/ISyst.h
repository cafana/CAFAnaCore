#pragma once

#include "CAFAna/Core/IFitSyst.h"

#include "CAFAna/Core/Registry.h"

namespace ana
{
  /// \brief Encapsulate code to systematically shift a record
  ///
  /// The Shift() function alters the record or the weight associated with the
  /// event.
  template<class RecT> class _ISyst: public IFitSyst
  {
  public:
    _ISyst(const std::string& shortName,
           const std::string& latexName)
      : IFitSyst(shortName, latexName)
    {
    }

    // Inherits no copying / moving from IFitSyst


    /// \brief Perform the systematic shift
    ///
    /// \param sigma   Number of sigma to shift record by
    /// \param sr      The record to inspect and alter
    /// \param weight  Scale this weight for reweighting systematics
    virtual void Shift(double sigma, RecT* rec, double& weight) const = 0;
  };

} // namespace
