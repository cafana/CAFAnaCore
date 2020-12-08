/// \file FwdDeclare.h -- consolidate forward declarations of template types
///                       and associated typdefs into one place so that they
///                       can be easily updated if needed.

#pragma once

namespace ana
{
  class Spectrum;
  template<class T> class DepMan;

  template<class RecT, class SpillT> class _Cut;
}
