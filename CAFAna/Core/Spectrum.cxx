#include "CAFAna/Core/Spectrum.h"

#include "CAFAna/Core/Ratio.h"
#include "CAFAna/Core/Stan.h"

#include "TDirectory.h"
#include "TH2.h"
#include "TH3.h"
#include "TObjString.h"
#include "TRandom3.h"

#include <cassert>
#include <iostream>

namespace
{
  namespace util{template<class T> T sqr(const T& x){return x*x;}}
}

namespace ana
{
  //----------------------------------------------------------------------
  Spectrum::Spectrum(IValueSource& src, const LabelsAndBins& axis, ESparse sparse)
    : Spectrum(axis, sparse)
  {
    src.Register(this);
  }

  //----------------------------------------------------------------------
  Spectrum::Spectrum(const LabelsAndBins& axis, ESparse sparse)
    : fHist(Hist::Uninitialized()), fPOT(0), fLivetime(0), fAxis(axis)
  {
    const Binning bins1D = fAxis.GetBins1D();

    if(sparse == kSparse){
      fHist = Hist::ZeroSparse(bins1D.NBins());
    }
    else{
      fHist = Hist::Zero(bins1D.NBins());
    }
  }

  //----------------------------------------------------------------------
  Spectrum::Spectrum(Eigen::ArrayXd&& h,
                     const LabelsAndBins& axis,
                     double pot, double livetime)
    : fHist(Hist::Adopt(std::move(h))), fPOT(pot), fLivetime(livetime), fAxis(axis)
  {
  }

  //----------------------------------------------------------------------
  Spectrum::Spectrum(Eigen::ArrayXstan&& h,
                     const LabelsAndBins& axis,
                     double pot, double livetime)
    : fHist(Hist::AdoptStan(std::move(h))), fPOT(pot), fLivetime(livetime), fAxis(axis)
  {
  }

  //----------------------------------------------------------------------
  Spectrum::Spectrum(Eigen::ArrayXd&& h,
                     Eigen::ArrayXd&& sqerr,
                     const LabelsAndBins& axis,
                     double pot, double livetime)
    : fHist(Hist::AdoptWithErrors(std::move(h), std::move(sqerr))), fPOT(pot), fLivetime(livetime), fAxis(axis)
  {
  }
	
  //----------------------------------------------------------------------
  Spectrum::Spectrum(const Spectrum& rhs):
    IValueSink(rhs),
    fHist(rhs.fHist),
    fPOT(rhs.fPOT),
    fLivetime(rhs.fLivetime),
    fAxis(rhs.fAxis)
  {
    assert(rhs.fReferences.empty()); // Copying with pending loads is unexpected
  }

  //----------------------------------------------------------------------
  Spectrum::Spectrum(Spectrum&& rhs):
    IValueSink(rhs),
    fHist(std::move(rhs.fHist)),
    fPOT(rhs.fPOT),
    fLivetime(rhs.fLivetime),
    fAxis(rhs.fAxis)
  {
    std::swap(fReferences, rhs.fReferences);
    for(Spectrum** ref: fReferences) *ref = this;
  }

  //----------------------------------------------------------------------
  Spectrum& Spectrum::operator=(const Spectrum& rhs)
  {
    if(this == &rhs) return *this;

    fHist = rhs.fHist;
    fPOT = rhs.fPOT;
    fLivetime = rhs.fLivetime;
    fAxis = rhs.fAxis;

    assert(fReferences.empty()); // Copying with pending loads is unexpected

    return *this;
  }

  //----------------------------------------------------------------------
  Spectrum& Spectrum::operator=(Spectrum&& rhs)
  {
    if(this == &rhs) return *this;

    fHist = std::move(rhs.fHist);
    fPOT = rhs.fPOT;
    fLivetime = rhs.fLivetime;
    fAxis = rhs.fAxis;

    std::swap(fReferences, rhs.fReferences);
    for(Spectrum** ref: fReferences) *ref = this;

    return *this;
  }

  //----------------------------------------------------------------------
  Spectrum::~Spectrum()
  {
    // Unregister self from anything that might still want to fill us
    for(Spectrum** ref: fReferences) *ref = 0;
  }

  //----------------------------------------------------------------------
  TH1D* Spectrum::ToTH1(double exposure,
                        EExposureType expotype,
                        EBinType bintype) const
  {
    // Could have a file temporarily open
    DontAddDirectory guard;

    TH1D* ret = fHist.ToTH1(fAxis.GetBins1D());

    ret->GetXaxis()->SetTitle(fAxis.GetLabel1D().c_str());
    ret->GetYaxis()->SetTitle("Events");

    if(expotype == kPOT){
      const double pot = exposure;
      if(fPOT){
        ret->Scale(pot/fPOT);
      }
      else{
        // Allow zero POT if there are also zero events
        if(ret->Integral() > 0){
          std::cout << "Error: Spectrum with " << ret->Integral()
                    << " entries has zero POT, no way to scale to "
                    << exposure << " POT.";
          if(fLivetime > 0){
            std::cout << " Spectrum has " << fLivetime << " seconds livetime. "
                      << "Did you mean to pass kLivetime to ToTH1()?";
          }
          std::cout << std::endl;
          abort();
        }
      }
    }
    if(expotype == kLivetime){
      const double livetime = exposure;
      if(fLivetime){
        ret->Scale(livetime/fLivetime);
      }
      else{
        // Allow zero exposure if there are also zero events
        if(ret->Integral() > 0){
          std::cout << "Error: Spectrum with " << ret->Integral()
                    << " entries has zero livetime, no way to scale to "
                    << livetime << " seconds.";
          if(fPOT > 0){
            std::cout << " Spectrum has " << fPOT << " POT. "
                      << "Did you mean to pass kPOT to ToTH1()?";
          }
          std::cout << std::endl;
          abort();
        }
      }
    }

    if(bintype == kBinDensity) ret->Scale(1, "width");

    // Allow GetMean() and friends to work even if this histogram never had any
    // explicit Fill() calls made.
    if(ret->GetEntries() == 0) ret->SetEntries(1);

    return ret;
  }

  //----------------------------------------------------------------------
  TH1D* Spectrum::ToTH1(double exposure, Color_t col, Style_t style,
			EExposureType expotype,
			EBinType bintype) const
  {
    // Could have a file temporarily open
    DontAddDirectory guard;

    TH1D* ret = ToTH1(exposure, expotype, bintype);

    ret->SetLineColor(col);
    ret->SetMarkerColor(col);
    ret->SetLineStyle(style);

    return ret;
  }

  //----------------------------------------------------------------------
  TH2* Spectrum::ToTH2(double exposure, EExposureType expotype, EBinType bintype) const
  {
    if(NDimensions() != 2){
      std::cout << "Error: This Spectrum does not appear to be 2D." << std::endl;
      abort();
    }

    TH2* ret = ana::ToTH2(*this, exposure, expotype, GetBinnings()[0], GetBinnings()[1]);

    ret->GetXaxis()->SetTitle(GetLabels()[0].c_str());
    ret->GetYaxis()->SetTitle(GetLabels()[1].c_str());

    if(bintype == kBinDensity) ret->Scale(1, "width");

    // Allow GetMean() and friends to work even if this histogram never had any
    // explicit Fill() calls made.
    if(ret->GetEntries() == 0) ret->SetEntries(1);

    return ret;
  }

  //----------------------------------------------------------------------
  TH3* Spectrum::ToTH3(double exposure, EExposureType expotype, EBinType bintype) const
  {
    if(NDimensions() != 3){
      std::cout << "Error: This Spectrum does not appear to be 3D." << std::endl;
      abort();
    }

    TH3* ret = ana::ToTH3(*this, exposure, expotype,
                          GetBinnings()[0], GetBinnings()[1], GetBinnings()[2]);

    ret->GetXaxis()->SetTitle(GetLabels()[0].c_str());
    ret->GetYaxis()->SetTitle(GetLabels()[1].c_str());
    ret->GetZaxis()->SetTitle(GetLabels()[2].c_str());

    if(bintype == kBinDensity) ret->Scale(1, "width");

    // Allow GetMean() and friends to work even if this histogram never had any
    // explicit Fill() calls made.
    if(ret->GetEntries() == 0) ret->SetEntries(1);

    return ret;
  }

  //----------------------------------------------------------------------
  TH1* Spectrum::ToTHX(double exposure, EExposureType expotype, EBinType bintype) const
  {
    if(NDimensions() == 2) return ToTH2(exposure, expotype, bintype);
    if(NDimensions() == 3) return ToTH3(exposure, expotype, bintype);
    return ToTH1(exposure, expotype, bintype);
  }

  //----------------------------------------------------------------------
  Eigen::ArrayXd Spectrum::GetEigen(double exposure, EExposureType expotype) const
  {
    if(expotype == kPOT)
      return (exposure/fPOT) * fHist.GetEigen();
    else
      return (exposure/fLivetime) * fHist.GetEigen();
  }

  //----------------------------------------------------------------------
  Eigen::ArrayXstan Spectrum::GetEigenStan(double exposure, EExposureType expotype) const
  {
    if(expotype == kPOT)
      return (exposure/fPOT) * fHist.GetEigenStan();
    else
      return (exposure/fLivetime) * fHist.GetEigenStan();
  }

  //----------------------------------------------------------------------
  Eigen::ArrayXd Spectrum::GetEigenSqErrors(double exposure, EExposureType expotype) const
  {
    if(expotype == kPOT)
      return util::sqr(exposure/fPOT) * fHist.GetEigenSqErrors();
    else
      return util::sqr(exposure/fLivetime) * fHist.GetEigenSqErrors();
  }

  //----------------------------------------------------------------------
  void Spectrum::Scale(double c)
  {
    fHist.Scale(c);
  }

  //----------------------------------------------------------------------
  void Spectrum::Scale(const stan::math::var& c)
  {
    fHist.Scale(c);
  }

  //----------------------------------------------------------------------
  double Spectrum::Integral(double exposure, double* err,
			    EExposureType expotype) const
  {
    const double ratio = (expotype == kPOT) ? exposure/fPOT : exposure/fLivetime;

    if(err){
      *err = 0;

      for(int i = 0; i < fHist.GetNbinsX()+2; ++i){
        *err += util::sqr(fHist.GetBinError(i));
      }
      *err = sqrt(*err) * ratio;
    }

    return fHist.Integral() * ratio;
  }

  //----------------------------------------------------------------------
  double Spectrum::Mean() const
  {
    const Binning bins = fAxis.GetBins1D();

    if(fHist.GetBinContent(0) != 0){
      std::cout << "Spectrum::Mean(): Warning ignoring underflow bin content " << fHist.GetBinContent(0) << std::endl;
    }

    if(fHist.GetBinContent(bins.NBins()+1) != 0){
      std::cout << "Spectrum::Mean(): Warning ignoring overflow bin content " << fHist.GetBinContent(bins.NBins()+1) << std::endl;
    }

    double mean = 0;
    double W = 0;
    for(int i = 1; i <= bins.NBins(); ++i){
      const double w = fHist.GetBinContent(i);
      W += w;
      const double x0 = bins.Edges()[i-1];
      const double x1 = bins.Edges()[i];
      mean += w * (x0+x1)/2;
    }

    return mean/W;
  }

  //----------------------------------------------------------------------
  void Spectrum::Fill(double x, double w)
  {
    const int bin = fAxis.GetBins1D().FindBin(x);
    fHist.Fill(bin, w);
  }

  //----------------------------------------------------------------------
  void Spectrum::FillPOT(double pot)
  {
    fPOT += pot;
  }

  //----------------------------------------------------------------------
  void Spectrum::FillLivetime(double livetime)
  {
    fLivetime += livetime;
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::MockData(double pot, int seed) const
  {
    Spectrum ret = FakeData(pot);

    TRandom3 rnd(seed); // zero seeds randomly

    for(int i = 0; i < ret.fHist.GetNbinsX()+2; ++i){
      ret.fHist.SetBinContent(i, rnd.Poisson(ret.fHist.GetBinContent(i)));
    }

    // Drop old errors, which are based on the MC statistics, and create new
    // ones that are based on the prediction for the data
    ret.fHist.ResetErrors();

    return ret;
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::AsimovData(double pot) const
  {
    Spectrum ret = *this;
    if(fPOT > 0) ret.fHist.Scale(pot/fPOT);
    if(fLivetime > 0) ret.fLivetime *= pot/fPOT;
    ret.fPOT = pot;

    // Drop old errors, which are based on the MC statistics, and create new
    // ones that are based on the prediction for the data
    ret.fHist.ResetErrors();

    return ret;
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::AsimovData(double pot, double livetime) const
  {
    Spectrum ret = *this;

    if(fPOT > 0) ret.fHist.Scale(pot/fPOT);
    ret.fPOT = pot;

    // overwrite livetime for fake data
    // PlusEqualsHelper will take over the rest
    ret.fLivetime = livetime;

    ret.fHist.ResetErrors();

    return ret;
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::FakeData(double pot) const
  {
    return AsimovData(pot);
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::FakeData(double pot, double livetime) const
  {
    return AsimovData(pot, livetime);
  }

  //----------------------------------------------------------------------
  void Spectrum::Clear()
  {
    fHist.Reset();
  }

  //----------------------------------------------------------------------
  void Spectrum::RemoveLoader(Spectrum** ref)
  {
    fReferences.erase(ref);
  }

  //----------------------------------------------------------------------
  void Spectrum::AddLoader(Spectrum** ref)
  {
    fReferences.insert(ref);
  }

  //----------------------------------------------------------------------
  Spectrum& Spectrum::PlusEqualsHelper(const Spectrum& rhs, int sign)
  {
    // In this case it would be OK to have no POT/livetime
    if(rhs.fHist.Initialized() && rhs.fHist.Integral() == 0) return *this;

    if((!fPOT && !fLivetime) || (!rhs.fPOT && !rhs.fLivetime)){
      std::cout << "Error: can't sum Spectrum with no POT or livetime: "
                << fPOT << " " << rhs.fPOT << " " << fLivetime << " " << rhs.fLivetime
                << std::endl;
      abort();
    }

    if(!fLivetime && !rhs.fPOT){
      std::cout << "Error: can't sum Spectrum with POT ("
                << fPOT << ") but no livetime and Spectrum with livetime ("
                << rhs.fLivetime << " sec) but no POT." << std::endl;
      abort();
    }

    if(!fPOT && !rhs.fLivetime){
      std::cout << "Error: can't sum Spectrum with livetime ("
                << fLivetime << " sec) but no POT and Spectrum with POT ("
                << rhs.fPOT << ") but no livetime." << std::endl;
      abort();
    }

    // And now there are still a bunch of good cases to consider

    if(fPOT && rhs.fPOT){
      // Scale by POT when possible
      fHist.Add(rhs.fHist, sign*fPOT/rhs.fPOT);

      if(fLivetime && rhs.fLivetime){
        // If POT/livetime ratios match, keep regular lifetime, otherwise zero
        // it out.
        if(!AlmostEqual(fLivetime*rhs.fPOT, rhs.fLivetime*fPOT))
          fLivetime = 0;
      }
      if(!fLivetime && rhs.fLivetime){
        // If the RHS has a livetime and we don't, copy it in (suitably scaled)
        fLivetime = rhs.fLivetime * fPOT/rhs.fPOT;
      }
      // Otherwise, keep our own livetime (if any)

      return *this;
    }

    if(fLivetime && rhs.fLivetime){
      // Scale by livetime, the only thing in common
      fHist.Add(rhs.fHist, sign*fLivetime/rhs.fLivetime);

      if(!fPOT && rhs.fPOT){
        // If the RHS has a POT and we don't, copy it in (suitably scaled)
        fPOT = rhs.fPOT * fLivetime/rhs.fLivetime;
      }
      // Otherwise, keep our own POT (if any)

      return *this;
    }

    // That should have been all the cases. I definitely want to know what
    // happened if it wasn't.
    std::cout << "Spectrum::operator+=(). How did we get here? "
              << fPOT << " " << fLivetime << " "
              << rhs.fPOT << " " << rhs.fLivetime << std::endl;
    abort();
  }

  //----------------------------------------------------------------------
  Spectrum& Spectrum::operator+=(const Spectrum& rhs)
  {
    return PlusEqualsHelper(rhs, +1);
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::operator+(const Spectrum& rhs) const
  {
    Spectrum ret = *this;
    ret += rhs;
    return ret;
  }

  //----------------------------------------------------------------------
  Spectrum& Spectrum::operator-=(const Spectrum& rhs)
  {
    return PlusEqualsHelper(rhs, -1);
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::operator-(const Spectrum& rhs) const
  {
    Spectrum ret = *this;
    ret -= rhs;
    return ret;
  }

  //----------------------------------------------------------------------
  Spectrum& Spectrum::operator*=(const Ratio& rhs)
  {
    fHist.Multiply(rhs.fHist);
    return *this;
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::operator*(const Ratio& rhs) const
  {
    Spectrum ret = *this;
    ret *= rhs;
    return ret;
  }

  //----------------------------------------------------------------------
  Spectrum& Spectrum::operator/=(const Ratio& rhs)
  {
    fHist.Divide(rhs.fHist);
    return *this;
  }

  //----------------------------------------------------------------------
  Spectrum Spectrum::operator/(const Ratio& rhs) const
  {
    Spectrum ret = *this;
    ret /= rhs;
    return ret;
  }

  //----------------------------------------------------------------------
  void Spectrum::SaveTo(TDirectory* dir, const std::string& name) const
  {
    TDirectory* tmp = gDirectory;

    dir = dir->mkdir(name.c_str()); // switch to subdir
    dir->cd();

    TObjString("Spectrum").Write("type");

    fHist.Write(fAxis.GetBins1D());
    TH1D hPot("", "", 1, 0, 1);
    hPot.Fill(.5, fPOT);
    hPot.Write("pot");
    TH1D hLivetime("", "", 1, 0, 1);
    hLivetime.Fill(.5, fLivetime);
    hLivetime.Write("livetime");

    for(unsigned int i = 0; i < NDimensions(); ++i){
      TObjString(GetLabels()[i].c_str()).Write(TString::Format("label%d", i).Data());
      GetBinnings()[i].SaveTo(dir, TString::Format("bins%d", i).Data());
    }

    dir->Write();
    delete dir;

    tmp->cd();
  }

  //----------------------------------------------------------------------
  std::unique_ptr<Spectrum> Spectrum::LoadFrom(TDirectory* dir, const std::string& name)
  {
    dir = dir->GetDirectory(name.c_str()); // switch to subdir
    assert(dir);

    DontAddDirectory guard;

    TObjString* tag = (TObjString*)dir->Get("type");
    assert(tag);
    assert(tag->GetString() == "Spectrum");
    delete tag;

    TH1* hPot = (TH1*)dir->Get("pot");
    assert(hPot);
    TH1* hLivetime = (TH1*)dir->Get("livetime");
    assert(hLivetime);

    std::vector<std::string> labels;
    std::vector<Binning> bins;
    for(int i = 0; ; ++i){
      const std::string subname = TString::Format("bins%d", i).Data();
      TDirectory* subdir = dir->GetDirectory(subname.c_str());
      if(!subdir) break;
      delete subdir;
      bins.push_back(*Binning::LoadFrom(dir, subname));
      TObjString* label = (TObjString*)dir->Get(TString::Format("label%d", i));
      labels.push_back(label ? label->GetString().Data() : "");
      delete label;
    }

    std::unique_ptr<Spectrum> ret = std::make_unique<Spectrum>(Eigen::ArrayXd(), LabelsAndBins(labels, bins), hPot->GetBinContent(1), hLivetime->GetBinContent(1));
    ret->fHist = Hist::FromDirectory(dir);

    delete hPot;
    delete hLivetime;

    delete dir;

    return ret;
  }
}
