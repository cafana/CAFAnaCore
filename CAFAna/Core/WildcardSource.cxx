#include "CAFAna/Core/WildcardSource.h"

#include "CAFAna/Core/UtilsExt.h"

#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

namespace
{
  // No-one else needs to see this
  std::vector<std::string> sorted(const std::vector<std::string>& v)
  {
    std::vector<std::string> ret;
    ret = v;
    std::sort(ret.begin(), ret.end());
    return ret;
  }
}

namespace ana
{
  //----------------------------------------------------------------------
  WildcardSource::WildcardSource(const std::string& wildcard,
                                 int stride, int offset, int limit)
    : FileListSource(sorted(CheckedWildcard(wildcard)), stride, offset, limit)
  {
  }

  //----------------------------------------------------------------------
  WildcardSource::~WildcardSource()
  {
  }

  //----------------------------------------------------------------------
  std::vector<std::string> WildcardSource::
  CheckedWildcard(const std::string& wildcard) const
  {
    std::vector<std::string> ret = Wildcard(wildcard);

    struct stat ss;
    // If we found nothing, it may be because pnfs isn't mounted.
    if(ret.empty() &&
       wildcard.find("/pnfs/") == 0 &&
       stat("/pnfs/", &ss) != 0){

      std::cout << "No files matching " << wildcard << " but that's probably because /pnfs is not mounted on the current grid node. If you have to use your own files, either try running it interactivly or try sam_add_dataset to create one." << std::endl;
    }

    return ret;
  }
}
