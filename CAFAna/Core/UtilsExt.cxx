#include "CAFAna/Core/UtilsExt.h"

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/Ratio.h"
#include "CAFAna/Core/Spectrum.h"

#include "TDirectory.h"
#include "TH2.h"
#include "TH3.h"

#include <iostream>

#include "wordexp.h"

#include "sys/stat.h"

namespace ana
{
  //----------------------------------------------------------------------
  std::string UniqueName()
  {
    static int N = 0;
    return TString::Format("cafanauniq%d", N++).Data();
  }

  //----------------------------------------------------------------------
  DontAddDirectory::DontAddDirectory()
  {
    fBackup = TH1::AddDirectoryStatus();
    TH1::AddDirectory(false);
  }

  //----------------------------------------------------------------------
  DontAddDirectory::~DontAddDirectory()
  {
    TH1::AddDirectory(fBackup);
  }

  //----------------------------------------------------------------------
  bool AlmostEqual(double a, double b, double eps)
  {
    if(a == 0 && b == 0) return true;

    return fabs(a-b) <= eps * std::max(fabs(a), fabs(b));
  }

  // Helper functions for MakeTHND().
  namespace{
    // Eventually the bin parameters will all be unpacked and we just pass them
    // on to the regular constructor.
    template<class T, class... A> T* MakeHist(A... a)
    {
      DontAddDirectory guard;
      return new T(a...);
    }

    // This function consumes bins from the start of the argument list and
    // pushes their translations onto the list of arguments at the end.
    template<class T, class... A> T* MakeHist(const Binning& firstBin,
                                              A... args)
    {
      if(firstBin.IsSimple())
        return MakeHist<T>(args...,
                           firstBin.NBins(), firstBin.Min(), firstBin.Max());
      else
        return MakeHist<T>(args...,
                           firstBin.NBins(), &firstBin.Edges().front());
    }
  }

  // Concrete instantiations. MakeHist() requires us to put the bin arguments
  // first...
  //----------------------------------------------------------------------
  TH1D* MakeTH1D(const char* name, const char* title, const Binning& bins)
  {
    return MakeHist<TH1D>(bins, name, title);
  }

  //----------------------------------------------------------------------
  TH1F* MakeTH1F(const char* name, const char* title, const Binning& bins)
  {
    return MakeHist<TH1F>(bins, name, title);
  }

  //----------------------------------------------------------------------
  TH2D* MakeTH2D(const char* name, const char* title,
                 const Binning& binsx,
                 const Binning& binsy)
  {
    return MakeHist<TH2D>(binsx, binsy, name, title);
  }

  //----------------------------------------------------------------------
  TH2F* MakeTH2F(const char* name, const char* title,
                 const Binning& binsx,
                 const Binning& binsy)
  {
    return MakeHist<TH2F>(binsx, binsy, name, title);
  }

  //----------------------------------------------------------------------
  TH3D* MakeTH3D(const char* name, const char* title,
                 const Binning& binsx,
                 const Binning& binsy,
	 	 const Binning& binsz)
  {
    return MakeHist<TH3D>(name, title,
			  binsx.NBins(), &binsx.Edges().front(),
			  binsy.NBins(), &binsy.Edges().front(),
			  binsz.NBins(), &binsz.Edges().front());
  }


  //----------------------------------------------------------------------
  TH2* ToTH2(const Spectrum& s, double exposure, ana::EExposureType expotype,
             const Binning& binsx, const Binning& binsy, ana::EBinType bintype)
  {
    DontAddDirectory guard;

    std::unique_ptr<TH1> h1(s.ToTH1(exposure, expotype));
    return ToTH2Helper(std::move(h1), binsx, binsy, bintype);
  }

  //----------------------------------------------------------------------
  TH2* ToTH2(const Ratio& r,
             const Binning& binsx, const Binning& binsy)
  {
    DontAddDirectory guard;

    std::unique_ptr<TH1> h1(r.ToTH1());
    return ToTH2Helper(std::move(h1), binsx, binsy);
  }

  //----------------------------------------------------------------------
  TH2* ToTH2Helper(std::unique_ptr<TH1> h1,
		   const Binning& binsx, const Binning& binsy,
		   ana::EBinType bintype)
  {
    // Make sure it's compatible with having been made with this binning
    assert(h1->GetNbinsX() == binsx.NBins()*binsy.NBins());

    TH2* ret = MakeTH2D(UniqueName().c_str(), "", binsx, binsy);

    for(int i = 0; i < h1->GetNbinsX(); ++i){
      const double val = h1->GetBinContent(i+1);
      const double err = h1->GetBinError(i+1);

      const int ix = i / binsy.NBins();
      const int iy = i % binsy.NBins();

      ret->SetBinContent(ix+1, iy+1, val);
      ret->SetBinError  (ix+1, iy+1, err);
    }

    if(bintype == ana::EBinType::kBinDensity) ret->Scale(1, "width");

    return ret;
  }

  //----------------------------------------------------------------------
  TH3* ToTH3(const Spectrum& s, double exposure, ana::EExposureType expotype,
             const Binning& binsx, const Binning& binsy, const Binning& binsz,
	     ana::EBinType bintype)
  {
    DontAddDirectory guard;

    std::unique_ptr<TH1> h1(s.ToTH1(exposure, expotype));

    return ToTH3Helper(std::move(h1), binsx, binsy, binsz, bintype);
  }

  //----------------------------------------------------------------------
  TH3* ToTH3(const Ratio& r,
             const Binning& binsx, const Binning& binsy, const Binning& binsz)
  {
    DontAddDirectory guard;

    std::unique_ptr<TH1> h1(r.ToTH1());

    return ToTH3Helper(std::move(h1), binsx, binsy, binsz);
  }

  //----------------------------------------------------------------------
  TH3* ToTH3Helper(std::unique_ptr<TH1> h1,
		   const Binning& binsx,
		   const Binning& binsy,
		   const Binning& binsz,
		   ana::EBinType bintype)
  {
    const int nx = binsx.NBins();
    const int ny = binsy.NBins();
    const int nz = binsz.NBins();

    // Make sure it's compatible with having been made with this binning
    assert(h1->GetNbinsX() == nx*ny*nz);

    TH3* ret;

    // If all three axes are simple, we can call a simpler constructor
    if(binsx.IsSimple() && binsy.IsSimple() && binsz.IsSimple()){
      ret = new TH3F(UniqueName().c_str(), "",
                     nx, binsx.Min(), binsx.Max(),
                     ny, binsy.Min(), binsy.Max(),
                     nz, binsz.Min(), binsz.Max());
    }
    else{
      // If >= 1 axis has a not simple binning, you have to pass the
      // edges vector to the TH3 constructor
      ret = new TH3F(UniqueName().c_str(), "",
                     nx, &binsx.Edges().front(),
                     ny, &binsy.Edges().front(),
                     nz, &binsz.Edges().front());
    }

    for(int i = 0; i < h1->GetNbinsX(); ++i){
      const double val = h1->GetBinContent(i+1);
      const double err = h1->GetBinError(i+1);

      const int nynz = ny*nz;
      const int nmodnynz = i%nynz;
      const int ix = i/nynz;
      const int iy = nmodnynz/nz;
      const int iz = i%nz;

      ret->SetBinContent(ix+1, iy+1, iz+1, val);
      ret->SetBinError  (ix+1, iy+1, iz+1, err);
    }

    if(bintype == ana::EBinType::kBinDensity) ret->Scale(1, "width");

    return ret;
  }

  //----------------------------------------------------------------------
  std::string pnfs2xrootd(std::string loc, bool unauth)
  {
    static bool first = true;
    static bool onsite = false;

    if (first && unauth) {
      first = false;
      char chostname[255];
      gethostname(chostname, 255);
      std::string hostname = chostname;

      if ( hostname.find("fnal.gov") != std::string::npos ){
        onsite = true;
        std::cout << "Using unauthenticated xrootd access (port 1095) while on-site, hostname: " << hostname << std::endl;
      }
      else {
        onsite = false;
        std::cout << "Using authenticated xrootd access (port 1094) access while off-site, hostname: " << hostname << std::endl;
      }
    }

    if(loc.rfind("/pnfs/", 0) == 0){ // ie begins with
      if ( onsite && unauth )
        loc = std::string("root://fndca1.fnal.gov:1095//pnfs/fnal.gov/usr/")+&loc.c_str()[6];
      else
        loc = std::string("root://fndca1.fnal.gov:1094//pnfs/fnal.gov/usr/")+&loc.c_str()[6];
    }
    return loc;
  }

  //----------------------------------------------------------------------
  std::vector<std::string> Wildcard(const std::string& wildcardString)
  {
    // Expand environment variables and wildcards like the shell would
    wordexp_t p;
    const int status = wordexp(wildcardString.c_str(), &p, WRDE_SHOWERR);

    if(status != 0){
      std::cerr << "Wildcard string '" << wildcardString
                << "' returned error " << status << " from wordexp()."
                << std::endl;
      return {};
    }

    std::vector<std::string> fileList;

    for(unsigned int i = 0; i < p.we_wordc; ++i){
      // Check the file exists before adding it
      struct stat sb;
      if(stat(p.we_wordv[i], &sb) == 0)
        fileList.push_back(p.we_wordv[i]);
    }

    wordfree(&p);

    return fileList;
  }

  //----------------------------------------------------------------------
  bool SAMDefinitionExists(const std::string& expt, const std::string& def)
  {
    // I would be much much happier to do this in proper code, but I'm not sure
    // how, there's no samweb C++ API?
    return system(TString::Format("samweb -e %s list-definitions --defname %s | grep %s", expt.c_str(), def.c_str(), def.c_str()).Data()) == 0;
  }

  //----------------------------------------------------------------------
  bool RunningOnGrid()
  {
    static bool cache;
    static bool cache_set = false;
    if(!cache_set){
      cache = (getenv("_CONDOR_SCRATCH_DIR") != 0);
      cache_set = true;
    }

    return cache;
  }

  //----------------------------------------------------------------------
  size_t Stride(bool allow_default)
  {
    static int cache = -1;

    if(cache < 0){
      char* env = getenv("CAFANA_STRIDE");
      if(env){
        cache = std::atoi(env);
      }
      else{
        if(allow_default){
          cache = 1;
        }
        else{
          std::cout << "Stride() called, but CAFANA_STRIDE is not set (--stride not passed?)" << std::endl;
          abort();
        }
      }
    }

    return cache;
  }

  //----------------------------------------------------------------------
  size_t Offset(bool allow_default)
  {
    static int cache = -1;

    if(cache < 0){
      char* env = getenv("CAFANA_OFFSET");
      if(env){
        cache = std::atoi(env);
      }
      else{
        if(allow_default){
          cache = 0;
        }
        else{
          std::cout << "Offset() called, but CAFANA_OFFSET is not set (--offset not passed?)" << std::endl;
          abort();
        }
      }
    }

    return cache;
  }

  //----------------------------------------------------------------------
  int Limit()
  {
    static int cache = 0;

    if(cache == 0){
      char* env = getenv("CAFANA_LIMIT");
      if(env){
        cache = std::atoi(env);
      }
      else{
        cache = -1;
      }
    }

    return cache;
  }

  //----------------------------------------------------------------------
  size_t JobNumber()
  {
    if(!RunningOnGrid()){
      std::cout << "JobNumber() called, but we are not running on the grid" <<  std::endl;
      abort();
    }

    return Offset(false);
  }

  //----------------------------------------------------------------------
  size_t NumJobs()
  {
    if(!RunningOnGrid()){
      std::cout << "NumJobs() called, but we are not running on the grid" << std::endl;
      abort();
    }

    return Stride(false);
  }

}
