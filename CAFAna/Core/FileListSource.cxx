#include "CAFAna/Core/FileListSource.h"

#include "CAFAna/Core/UtilsExt.h"

#include "TFile.h"

#include <cassert>
#include <iostream>

namespace ana
{
  bool FileListSource::fgGotTickets = false;

  //----------------------------------------------------------------------
  FileListSource::FileListSource(const std::vector<std::string>& files,
				 int stride, int offset, int limit)
    : fInRetry(false), fFile(0)
  {
    if(offset < 0) offset = Offset();
    if(stride < 0) stride = Stride();
    if(limit < 0) limit = Limit();

    if(!files.empty() && stride > int(files.size())){
      std::cerr << "Warning: stride " << stride
                << " is greater than the number of files: " << files.size()
                << ". This is strange and inefficient." << std::endl;
    }

    for(unsigned int i = offset; i < files.size(); i += stride){
      fFileNames.push_back(files[i]);
      if(limit > 0 && int(fFileNames.size()) == limit) break;
    }

    fIt = fFileNames.begin();

    for(const std::string& loc: fFileNames){
      if(loc.rfind("/pnfs/", 0) == 0){ // ie begins with
        if(!fgGotTickets){
          // No kerberos ticket means no point trying to voms-proxy-init. It
          // likely also means we're in a grid job, where that would be
          // counterproductive anyway.
          if(system("klist -5 -s || klist -s") != 0) fgGotTickets = true;
        }

	if(!fgGotTickets){
          // This comes from NovaGridUtils or duneutil
          system("setup_fnal_security -b");

	  fgGotTickets = true;
	  break;
	}
      }
    }
  }

  //----------------------------------------------------------------------
  FileListSource::~FileListSource()
  {
    delete fFile;
  }

  //----------------------------------------------------------------------
  TFile* FileListSource::GetNextFile()
  {
    // Tidy up the last file we gave, which the caller no longer needs
    delete fFile;
    fFile = 0;

    // Did we run out of files?
    if(fInRetry && fIt == fRetry.end()) return 0;
    if(fIt == fFileNames.end()){
      if(fRetry.empty()) return 0;
      fIt = fRetry.begin();
      fInRetry = true;
    }

    // If the file is on pnfs rewrite it to an xrootd address
    std::string loc = *fIt;
    loc = pnfs2xrootd(loc); // no-op for non /pnfs locations

    if(fInRetry) std::cout << "Retrying " << loc << " which was previously deferred..." << std::endl;

    fFile = TFile::Open(loc.c_str()); // This pattern allows xrootd

    if(!fFile){
      if(!fInRetry){
        std::cout << "Unable to open " << loc << std::endl;
        std::cout << "Will skip this file and try again later." << std::endl;
        fRetry.push_back(*fIt);
        ++fIt;
        return GetNextFile();
      }
      else{
        const int Nretry = 3;
        std::cout << "Failed to open. Will retry " << Nretry << " more times" << std::endl;
        for(int i = 0; i < Nretry; ++i){
          std::cout << "Attempt " << i << std::endl;
          fFile = TFile::Open(loc.c_str());
          if(fFile){
            std::cout << "Success!" << std::endl;
            break;
          }
        } // end for i
        if(!fFile){
          std::cout << "Still unable to read " << loc << std::endl;
          std::cout << "Aborting" << std::endl;
          abort();
        }
      }
    }

    // The above should have guaranteed this
    assert(fFile);

    ++fIt; // Move on to the next file, for the subsequent call

    return fFile;
  }
}
