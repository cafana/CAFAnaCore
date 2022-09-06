#include "CAFAna/Core/Binning.h"

#include "CAFAna/Core/DepMan.h"

#include "TDirectory.h"
#include "TH1.h"
#include "TObjString.h"
#include "TVectorD.h"

namespace ana
{
  //----------------------------------------------------------------------
  Binning::Binning()
    : fNBins(-1) // need a non-zero value so we count as constructed
  {
    // Don't want children copying from us at this point. Only when we're
    // "fully constructed". So I inserted explicit calls in Simple() and
    // Custom() instead.

    //    DepMan<Binning>::Instance().RegisterConstruction(this);
  }

  //----------------------------------------------------------------------
  Binning::~Binning()
  {
    DepMan<Binning>::Instance().RegisterDestruction(this);
  }

  //----------------------------------------------------------------------
  Binning::Binning(const Binning& b)
  {
    if(&b == this) return;

    if(b.fNBins){
      fEdges = b.fEdges;
      fLabels = b.fLabels;
      fNBins = b.fNBins;
      fMin = b.fMin;
      fMax = b.fMax;
      fIsSimple = b.fIsSimple;

      DepMan<Binning>::Instance().RegisterConstruction(this);
    }
    else{
      fNBins = 0;
      fMin = 0;
      fMax = 0;
      fIsSimple = false;

      // If we are copying from a Binning with zero bins, that is probably
      // because it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<Binning>::Instance().RegisterDependency(&b, this);
    }
  }
  
  //----------------------------------------------------------------------
  Binning& Binning::operator=(const Binning& b)
  {
    if(&b == this) return *this;

    if(b.fNBins){
      fEdges = b.fEdges;
      fLabels = b.fLabels;
      fNBins = b.fNBins;
      fMin = b.fMin;
      fMax = b.fMax;
      fIsSimple = b.fIsSimple;

      DepMan<Binning>::Instance().RegisterConstruction(this);
    }
    else{
      fNBins = 0;
      fMin = 0;
      fMax = 0;
      fIsSimple = false;

      // If we are copying from a Binning with zero bins, that is probably
      // because it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<Binning>::Instance().RegisterDependency(&b, this);
    }

    return *this;
  }

  //----------------------------------------------------------------------
  Binning Binning::SimpleHelper(int n, double lo, double hi,
                                const std::vector<std::string>& labels)
  {
    assert(labels.empty() || int(labels.size()) == n);

    Binning bins;
    bins.fNBins = n;
    bins.fMin = lo;
    bins.fMax = hi;
    bins.fEdges.resize(n+1);
    for (int i = 0; i <= n; i++)
      bins.fEdges[i] = lo + i*(hi-lo)/n;
    bins.fLabels = labels;
    bins.fIsSimple = true;

    return bins;
  }

  //----------------------------------------------------------------------
  Binning Binning::Simple(int n, double lo, double hi,
                          const std::vector<std::string>& labels)
  {
    Binning bins = SimpleHelper(n, lo, hi, labels);

    DepMan<Binning>::Instance().RegisterConstruction(&bins);

    return bins;
  }

  //----------------------------------------------------------------------
  Binning Binning::LogUniform(int n, double lo, double hi)
  {
    std::vector<double> edges(n+1);
    const double logSpacing = exp( (log(hi) - log(lo)) / n );
    for (int i = 0; i <= n; ++i) {
      if (i == 0) edges[i] = lo;
      else        edges[i] = logSpacing*edges[i-1];
    }
    return Custom(edges);
  }

  //----------------------------------------------------------------------
  Binning Binning::CustomHelper(const std::vector<double>& edges)
  {
    assert(edges.size() > 1);

    Binning bins;
    bins.fEdges = edges;
    bins.fNBins = edges.size()-1;
    bins.fMin = edges.front();
    bins.fMax = edges.back();
    bins.fIsSimple = false;

    return bins;
  }

  //----------------------------------------------------------------------
  Binning Binning::Custom(const std::vector<double>& edges)
  {
    Binning bins = CustomHelper(edges);

    DepMan<Binning>::Instance().RegisterConstruction(&bins);

    return bins;
  }

  //----------------------------------------------------------------------
  int Binning::FindBin(double x) const
  {
    assert(!std::isnan(x) && !std::isinf(x));

    // Treat anything outside [fMin, fMax) as Underflow / Overflow
    if (x <  fMin) return 0;               // Underflow
    if (x >= fMax) return fEdges.size();   // Overflow

    // Follow ROOT convention, first bin of histogram is bin 1

    if(IsSimple()) return fNBins * (x - fMin) / (fMax - fMin) + 1;

    int bin =
      std::lower_bound(fEdges.begin(), fEdges.end(), x) - fEdges.begin();
    if (x == fEdges[bin]) bin++;
    assert(bin >= 0 && bin < (int)fEdges.size());
    return bin;
  }

  //----------------------------------------------------------------------
  Binning Binning::FromTAxis(const TAxis* ax)
  {
    Binning bins;

    // Evenly spaced binning
    if(!ax->GetXbins()->GetArray()){
      bins = SimpleHelper(ax->GetNbins(), ax->GetXmin(), ax->GetXmax());
    }
    else{
      std::vector<double> edges(ax->GetNbins()+1);
      ax->GetLowEdge(&edges.front());
      edges[ax->GetNbins()] = ax->GetBinUpEdge(ax->GetNbins());

      bins = Binning::Custom(edges);
    }

    return bins;
  }

  //----------------------------------------------------------------------
  void Binning::SaveTo(TDirectory* dir, const std::string& name) const
  {
    TDirectory* tmp = gDirectory;

    dir = dir->mkdir(name.c_str()); // switch to subdir
    dir->cd();

    TObjString("Binning").Write("type");

    TVectorD nminmax(3);

    nminmax[0] = fNBins;
    nminmax[1] = fMin;
    nminmax[2] = fMax;

    nminmax.Write("nminmax");

    TVectorD issimple(1);
    issimple[0] = fIsSimple;
    issimple.Write("issimple");

    TVectorD edges(fEdges.size());
    for(unsigned int i = 0; i < fEdges.size(); ++i)
      edges[i] = fEdges[i];

    edges.Write("edges");

    for(unsigned int i = 0; i < fLabels.size(); ++i)
      TObjString(fLabels[i].c_str()).Write(TString::Format("label%d", i).Data());

    dir->Write();
    delete dir;

    tmp->cd();
  }

  //----------------------------------------------------------------------
  std::unique_ptr<Binning> Binning::LoadFrom(TDirectory* dir, const std::string& name)
  {
    dir = dir->GetDirectory(name.c_str()); // switch to subdir
    assert(dir);

    TObjString* tag = (TObjString*)dir->Get("type");
    assert(tag);
    assert(tag->GetString() == "Binning");
    delete tag;

    TVectorD* vMinMax = (TVectorD*)dir->Get("nminmax");
    assert(vMinMax);

    Binning ret;

    const TVectorD* issimple = (TVectorD*)dir->Get("issimple");
    if((*issimple)[0]){
      ret = Binning::Simple((*vMinMax)[0],
                            (*vMinMax)[1],
                            (*vMinMax)[2]);
    }
    else{
      const TVectorD* vEdges = (TVectorD*)dir->Get("edges");
      std::vector<double> edges(vEdges->GetNrows());
      for(int i = 0; i < vEdges->GetNrows(); ++i) edges[i] = (*vEdges)[i];
      delete vEdges;

      ret = Binning::Custom(edges);
    }

    delete vMinMax;

    for(unsigned int i = 0; ; ++i){
      TObjString* s = (TObjString*)dir->Get(TString::Format("label%d", i).Data());
      if(!s) break;
      ret.fLabels.push_back(s->GetString().Data());
      delete s;
    }

    delete dir;

    return std::make_unique<Binning>(ret);
  }

  //----------------------------------------------------------------------
  bool Binning::operator==(const Binning& rhs) const
  {
    if(fIsSimple != rhs.fIsSimple) return false;
    if(fIsSimple){
      return fNBins == rhs.fNBins && fMin == rhs.fMin && fMax == rhs.fMax;
    }
    else{
      return fEdges == rhs.fEdges;
    }
  }

  //----------------------------------------------------------------------
  bool Binning::operator<(const Binning& rhs) const
  {
    if(fIsSimple != rhs.fIsSimple) return fIsSimple < rhs.fIsSimple;
    if(fIsSimple){
      return std::make_tuple(fNBins, fMin, fMax) < std::make_tuple(rhs.fNBins, rhs.fMin, rhs.fMax);
    }
    else{
      return fEdges < rhs.fEdges;
    }
  }

}
