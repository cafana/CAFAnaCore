#pragma once

namespace ana::beta
{
  // Helpers to be able to name ExposureSource/Sink and various
  // EnsembleSource/Sinks
  class ExposureTag;
  template<class RecT> class EnsembleTag;

  // We might want to introduce this one to reduce potential ambiguity
  // template<class RecT> class RecordTag;
}
