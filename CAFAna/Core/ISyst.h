#pragma once

#include "CAFAna/Core/INamed.h"
#include "CAFAna/Core/Registry.h"

namespace ana
{
  /// \brief Encapsulate code to systematically shift a record
  ///
  /// The Shift() function alters the record or the weight associated with the
  /// event.
  template<class RecT> class _ISyst: public INamed
  {
  public:
    _ISyst(const std::string& shortName,
           const std::string& latexName)
      : INamed(shortName, latexName)
    {
      Registry<_ISyst>::Register(this);
    }

    _ISyst(const _ISyst&) = delete;   // no copying.
    _ISyst(_ISyst&& rhs) = delete;    // no moving either.

    virtual ~_ISyst()
    {
      // Normally ISysts should last for the life of the process, but in case
      // one is deleted it's best not to leave a dangling pointer in Registry.
      Registry<_ISyst>::UnRegister(this);
    }

    _ISyst& operator=(const _ISyst&) = delete;  // still no copying.
    _ISyst& operator=(_ISyst&&)      = delete;  // etc.

    /// \brief Perform the systematic shift
    ///
    /// \param sigma   Number of sigma to shift record by
    /// \param sr      The record to inspect and alter
    /// \param weight  Scale this weight for reweighting systematics
    virtual void Shift(double sigma, RecT* rec, double& weight) const = 0;
  };

} // namespace
