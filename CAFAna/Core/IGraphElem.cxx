#include "CAFAna/Core/IGraphElem.h"

#include "CAFAna/Core/UtilsExt.h"

#include <ostream>

namespace ana
{
  //----------------------------------------------------------------------
  void IGraphElem::PrintNode(std::ostream& os,
                             const IGraphElem* node) const
  {
    os << "\"" << node << "\" [ label = \"" << DemangledTypeName(node) << "\" ]" << std::endl;
  }

  //----------------------------------------------------------------------
  void IGraphElem::PrintEdge(std::ostream& os,
                             const IGraphElem* from,
                             const IGraphElem* to,
                             const char* color) const
  {
    os << "\"" << from << "\" -> \"" << to << "\"";
    if(color) os << " [ color = " << color << " ]";
    os << std::endl;
  }
}
