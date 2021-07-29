//#include "CAFAna/Core/VarApplier.h"

#include <cmath>
#include <iostream>

namespace ana::beta
{
  // We can't actually include VarApplier.h here, due to the desire of
  // IRecordSource.h to include it too. Just repeat the definition.
  class ApplierBase{
  protected:
    ApplierBase(){}
    bool EnsureFinite(double val) const;
  };

  bool ApplierBase::EnsureFinite(double val) const
  {
    if(std::isnan(val) || std::isinf(val)){
      std::cerr << "Warning: Bad value: " << val
                << " returned from a Var. The input variable(s) could "
                << "be NaN in the CAF, or perhaps your "
                << "Var code computed 0/0?" << std::endl;
      std::cout << "Not filling this value into any Spectrum" << std::endl;
      return false;
    }
    return true;
  }
}
