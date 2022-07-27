#pragma once

#include <iosfwd>

namespace ana
{
  /// Element of a graph to be rendered to dot format
  class IGraphElem
  {
  public:
    virtual void PrintGraph(std::ostream& os) const = 0;

  protected:
    // Helper functions for implementations
    void PrintNode(std::ostream& os,
                   const IGraphElem* node) const;

    void PrintEdge(std::ostream& os,
                   const IGraphElem* from,
                   const IGraphElem* to,
                   const char* color = 0) const;
  };
}
