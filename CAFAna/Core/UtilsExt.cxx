#include "CAFAna/Core/UtilsExt.h"

#include "CAFAna/Core/Binning.h"

#include "TDirectory.h"
#include "TH2.h"
#include "TH3.h"

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
}
