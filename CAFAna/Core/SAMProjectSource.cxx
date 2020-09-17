#include "CAFAna/Core/SAMProjectSource.h"

#include "ifdh.h"

#include <cassert>
#include <iostream>
#include <unistd.h>

#ifdef DARWINBUILD
#include <libgen.h>
#endif

#include "TFile.h"

namespace ana
{
  bool SAMProjectSource::fgGotTickets = false;

  //----------------------------------------------------------------------
  SAMProjectSource::SAMProjectSource(const std::string& proj, int fileLimit)
    : fIFDH(new ifdh_ns::ifdh)
    , fFile(0)
    , fNFiles(fileLimit)
  {
    if(!fgGotTickets){
      // No kerberos ticket means no point trying to voms-proxy-init. It likely
      // also means we're in a grid job, where that would be counterproductive
      // anyway.
      if(system("klist -5 -s || klist -s") != 0) fgGotTickets = true;
    }

    if(!fgGotTickets){
      // This comes from NovaGridUtils, v02.10 onwards.
      system("setup_fnal_security -b");

      fgGotTickets = true;
    }

    // If X509_USER_PROXY isn't set, set it manually. Can help unconfuse IFDH
    // about what x509 it's supposed to be using.
    if(getenv("X509_USER_PROXY") == 0)
      setenv("X509_USER_PROXY", TString::Format("/tmp/x509up_u%d", getuid()).Data(), 0);

    fIFDH->set_debug("0"); // shut up

    fProjectURL = fIFDH->findProject(proj, getenv("SAM_STATION"));

    // grid jobs don't always have $USER set, but often they have $GRID_USER instead
    char * strPtr;
    std::string userStr;
    if ( (strPtr = getenv("USER")) || (strPtr = getenv("GRID_USER")) )
      userStr = strPtr;
    else
      userStr = "unknown";
    
    // grid jobs don't always have $HOSTNAME set, sometimes they use $OSG_HOSTNAME instead
    std::string hostNameStr;
    strPtr = getenv("HOSTNAME");
    if(!strPtr) strPtr = getenv("OSG_HOSTNAME");
    hostNameStr = strPtr;

    fProcessID = fIFDH->establishProcess(fProjectURL, "CAFAna", "v0.9", hostNameStr.c_str(), userStr.c_str(), "nova", "", fileLimit);
  }

  //----------------------------------------------------------------------
  SAMProjectSource::~SAMProjectSource()
  {
    if(fFile){
      // Tidy up the final file
      const std::string fname = fFile->GetName();
      delete fFile;
      unlink(fname.c_str());
    }

    // End the process cleanly
    fIFDH->endProcess(fProjectURL, fProcessID);

    // certainly wrong for fileLimit case
    // status = fIFDH.endProject(fProjectURL);

    fIFDH->cleanup();
  }

  //----------------------------------------------------------------------
  TFile* SAMProjectSource::GetNextFile()
  {
    if(fFile){
      // Tidy up the previous file
      const std::string fname = fFile->GetName();
      delete fFile;
      fFile = 0;
      unlink(fname.c_str());

      // And let SAM know we're done with it
      fIFDH->updateFileStatus(fProjectURL, fProcessID, fname, "consumed");
    }

    const std::string uri = fIFDH->getNextFile(fProjectURL, fProcessID);
    if(uri.empty()) return 0; // out of files

    const std::string fname = fIFDH->fetchInput(uri);
    assert(!fname.empty());

    // Let SAM know we got it OK
    fIFDH->updateFileStatus(fProjectURL, fProcessID, fname, "transferred");

    // Additional newlines because ifdh currently spams us with certificate
    // messages.
    if(fNFiles < 0) std::cout << std::endl << "Processing " << basename((char *)fname.c_str()) << std::endl << std::endl;

    fFile = new TFile(fname.c_str());
    return fFile;
  }
} // namespace

