#include "CAFAna/Core/IFitSyst.h"

#include "CAFAna/Core/Registry.h"

namespace ana
{
  //----------------------------------------------------------------------
  IFitSyst::IFitSyst(const std::string& shortName,
                     const std::string& latexName)
    : IFittable(shortName, latexName)
  {
    Registry<IFitSyst>::Register(this);
  }

  //----------------------------------------------------------------------
  IFitSyst::~IFitSyst()
  {
    // Normally systs should last for the life of the process, but in case
    // one is deleted it's best not to leave a dangling pointer in Registry.
    Registry<IFitSyst>::UnRegister(this);
  }
}
