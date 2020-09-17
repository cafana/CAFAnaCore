#pragma once

#include "CAFAna/Core/IFileSource.h"

#include <string>
#include <vector>

namespace ana
{
  /// Simple file source based on an explicit list provided by the user
  class FileListSource: public IFileSource
  {
  public:
    /// Default \a stride, \a offset, and \a limit mean obey cmd-line options
    FileListSource(const std::vector<std::string>& files,
		   int stride = -1, int offset = -1, int limit = -1);
    virtual ~FileListSource();

    virtual TFile* GetNextFile() override;
    int NFiles() const override {return fFileNames.size();}

    const std::vector<std::string>& GetFileNames() const { return fFileNames; }
  protected:
    std::vector<std::string> fFileNames; ///< The list of files
    std::vector<std::string>::iterator fIt; ///< Iterator into \ref fFileNames
    std::vector<std::string> fRetry; ///< List of files that failed 1st attempt
    bool fInRetry; ///< Did we finish fFileNames and are now in fRetry?
    TFile* fFile; ///< The most-recently-returned file
    static bool fgGotTickets; ///< Have we renewed our tickets?
  };
}
