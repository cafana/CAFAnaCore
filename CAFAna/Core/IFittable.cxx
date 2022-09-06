#include "CAFAna/Core/IFittable.h"

#include "CAFAna/Core/Registry.h"

namespace ana
{
  //----------------------------------------------------------------------
  IFittable::IFittable(const std::string& shortName,
                       const std::string& latexName)
    : INamed(shortName, latexName)
  {
    Registry<IFittable>::Register(this);
  }

  //----------------------------------------------------------------------
  IFittable::~IFittable()
  {
    // Normally fittables should last for the life of the process, but in case
    // one is deleted it's best not to leave a dangling pointer in Registry.
    Registry<IFittable>::UnRegister(this);
  }
}
