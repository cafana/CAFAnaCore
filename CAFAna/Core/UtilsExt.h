#pragma once

// This is a subset of the original Utilities.h - eventually we will move all
// of it, but it differs between the experiments. While it's split we need a
// different filename

#include <string>

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
}
