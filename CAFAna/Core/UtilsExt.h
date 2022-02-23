#pragma once

// This is a subset of the original Utilities.h - eventually we will move all
// of it, but it differs between the experiments. While it's split we need a
// different filename

#include <cxxabi.h>
#include <memory>
#include <string>
#include <vector>

class TH1;
class TH2;
class TH3;
class TF1;
class TH1D;
class TH1F;
class TH2F;
class TH2D;
class TH3D;

namespace ana
{
  class Binning;
  class Spectrum;
  class Ratio;

  enum EBinType
  {
    kBinContent, ///< Regular histogram
    kBinDensity  ///< Divide bin contents by bin widths
  };

  /// For use as an argument to \ref Spectrum::ToTH1
  enum EExposureType{
    kPOT,
    kLivetime
  };

  /// Return a different string each time, for creating histograms
  std::string UniqueName();

  /// \brief Prevent histograms being added to the current directory
  ///
  /// Upon going out of scope, restores the previous setting
  class DontAddDirectory
  {
  public:
    DontAddDirectory();
    ~DontAddDirectory();
  protected:
    bool fBackup;
  };

  bool AlmostEqual(double a, double b, double eps = .0001); // allow 0.01% error by default

  /// Utility function to avoid need to switch on bins.IsSimple()
  TH1D* MakeTH1D(const char* name, const char* title, const Binning& bins);
  TH1F* MakeTH1F(const char* name, const char* title, const Binning& bins);
  /// Utility function to avoid 4-way combinatorial explosion on the bin types
  TH2D* MakeTH2D(const char* name, const char* title,
                 const Binning& binsx,
                 const Binning& binsy);
  TH2F* MakeTH2F(const char* name, const char* title,
                 const Binning& binsx,
                 const Binning& binsy);
  /// Utility function for 3D hist
  // TODO: make bins more simple-like
  TH3D* MakeTH3D(const char* name, const char* title,
                 const Binning& binsx,
                 const Binning& binsy,
                 const Binning& binsz);


  /// \brief For use with \ref Var2D
  ///
  /// Re-expand a histogram flattened by \ref Var2D into a 2D histogram for
  /// plotting purposes. The binning scheme must match that used in the
  /// original Var.
  TH2* ToTH2(const Spectrum& s, double exposure, ana::EExposureType expotype,
             const Binning& binsx, const Binning& binsy,
	     ana::EBinType bintype = ana::EBinType::kBinContent);

  /// Same as ToTH2, but with 3 dimensions
  TH3* ToTH3(const Spectrum& s, double exposure, ana::EExposureType expotype,
             const Binning& binsx, const Binning& binsy, const Binning& binsz,
	     ana::EBinType bintype = ana::EBinType::kBinContent);

  /// \brief For use with \ref Var2D
  ///
  /// Re-expand a flatenned histogram into a 2D histogram for
  /// plotting purposes. The binning scheme must match that used in the
  /// original Var.
  TH2* ToTH2(const Ratio& r, const Binning& binsx, const Binning& binsy);

  /// Same as ToTH2, but with 3 dimensions
  TH3* ToTH3(const Ratio& r, const Binning& binsx,
	     const Binning& binsy, const Binning& binsz);

  /// Helper for ana::ToTH2
  TH2* ToTH2Helper(std::unique_ptr<TH1> h1,
		   const Binning& binsx,
		   const Binning& binsy,
		   ana::EBinType bintype = ana::EBinType::kBinContent);

  /// Helper for ana::ToTH3
  TH3* ToTH3Helper(std::unique_ptr<TH1> h1,
		   const Binning& binsx,
		   const Binning& binsy,
		   const Binning& binsz,
		   ana::EBinType bintype = ana::EBinType::kBinContent);

  /// utility method to figure out exactly what kind of object I am
  template<class T> std::string DemangledTypeName(T* obj)
  {
    return abi::__cxa_demangle(typeid(*obj).name(), 0, 0, 0);
  }

  template<class T> constexpr char* DemangledTypeName()
  {
    return abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
  }

  std::string pnfs2xrootd(std::string loc, bool unauth = false);

  /// Find files matching a UNIX glob, plus expand environment variables
  std::vector<std::string> Wildcard(const std::string& wildcardString);

  bool SAMDefinitionExists(const std::string& expt, const std::string& def);

  /// Is this a grid (condor) job?
  bool RunningOnGrid();

  /// Value passed to --stride, or 1 if not specified
  size_t Stride(bool allow_default = true);
  /// Value passed to --offset, or 0 if not specified
  size_t Offset(bool allow_default = true);
  /// Value passed to --limit, or -1 if not specified
  int Limit();

  /// What's the process number for a grid job?
  size_t JobNumber();
  size_t NumJobs();

}
