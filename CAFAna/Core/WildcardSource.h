#pragma once

#include "CAFAna/Core/FileListSource.h"

namespace ana
{
  /// File source based on a wildcard (glob)
  class WildcardSource: public FileListSource
  {
  public:
    /// \param wildcard Wildcard or glob. Anything glob() accepts is OK. May be
    ///                 a single literal filename.
    WildcardSource(const std::string& wildcard,
                   int stride = -1, int offset = -1, int limit = -1);
    virtual ~WildcardSource();

  protected:
    std::vector<std::string> CheckedWildcard(const std::string& wildcard) const;
  };
}
