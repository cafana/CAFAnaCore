#include "CAFAna/Core/EnsureFiniteBase.h"

#include <cmath>
#include <iostream>

namespace ana
{
  bool EnsureFiniteBase::EnsureFinite(double val, const char* type) const
  {
    if(std::isfinite(val)) return true;

    std::cerr << "Warning: Bad value: " << val
              << " returned from a " << type << ". "
              << "The input variable(s) could "
              << "be NaN in the CAF, or perhaps your "
              << type << " code computed 0/0?" << std::endl;
    std::cerr << "Not filling this value into any Spectrum" << std::endl;
    return false;
  }
}
