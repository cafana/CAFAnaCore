#include "CAFAna/Core/HistAxis.h"

#include <iostream>

namespace ana
{
  // This is in the .cxx to avoid having to include iostream into the header
  void HistAxisDimensionError(int dim)
  {
    std::cout << "Error: HistAxis::GetVar1D() doesn't support "
              << dim << "-dimensional axes" << std::endl;
    abort();
  }
}
