#pragma once

namespace ana
{
  /// Handy place to put a snippet of code checking if a value is finite and
  /// warning otherwise.
  class EnsureFiniteBase
  {
  protected:
    bool EnsureFinite(double val, const char* type) const;
  };
}
