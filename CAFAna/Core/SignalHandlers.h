#pragma once

#include <atomic>

namespace ana
{
  /// Loops or long-running CAFAna sub-processes (e.g. Loaders::Go(), IFitter::Fit(), ...)
  /// can check this flag periodically to see if they are requested to quit.
  /// CAFAna's signal handlers for SIGTERM and SIGINT set it
  bool CAFAnaQuitRequested();
}