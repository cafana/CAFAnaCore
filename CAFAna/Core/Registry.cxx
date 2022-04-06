#include "CAFAna/Core/Registry.txx"

#include "CAFAna/Core/IFittable.h"
#include "CAFAna/Core/IFitSyst.h"

namespace ana
{
  // Instantiate the registries we know we need. Experiment code should add a
  // similar file instatiating any additional registries.
  template class Registry<IFittable>;
  template class Registry<IFitSyst>;
}
