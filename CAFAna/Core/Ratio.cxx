#include "CAFAna/Core/Ratio.h"

#include "TH2.h"

#include <cassert>
#include <iostream>

namespace ana
{
  //----------------------------------------------------------------------
  Ratio::Ratio(const Spectrum& num, const Spectrum& denom,
	       bool purOrEffErrs)
    : fHist(num.fHist), fAxis(num.GetLabels(), num.GetBinnings())
  {
    fHist.Divide(denom.fHist);
    fHist.Scale(denom.POT()/num.POT());

    // This is clumsy, but the old histogram operation considered 0/0 = 0,
    // which is actually pretty useful (at least PredictionInterp relies on
    // this).
    for(int i = 0; i < fHist.GetNbinsX()+2; ++i){
      if(num.fHist.GetBinContent(i) == 0 &&
         denom.fHist.GetBinContent(i) == 0){
        fHist.SetBinContent(i, 0);
      }
    }

    // TODO do something with purOrEffErrs
  }

  //----------------------------------------------------------------------
  Ratio::~Ratio()
  {
  }

  //----------------------------------------------------------------------
  Ratio& Ratio::operator*=(const Ratio& rhs)
  {
    fHist.Multiply(rhs.fHist);
    return *this;
  }

  //----------------------------------------------------------------------
  Ratio Ratio::operator*(const Ratio& rhs) const
  {
    Ratio ret = *this;
    ret *= rhs;
    return ret;
  }

  //----------------------------------------------------------------------
  Ratio& Ratio::operator/=(const Ratio& rhs)
  {
    fHist.Divide(rhs.fHist);
    return *this;
  }

  //----------------------------------------------------------------------
  Ratio Ratio::operator/(const Ratio& rhs) const
  {
    Ratio ret = *this;
    ret /= rhs;
    return ret;
  }

  //----------------------------------------------------------------------
  TH1D* Ratio::ToTH1(Color_t col, Style_t style) const
  {
    TH1D* ret = fHist.ToTH1(fAxis.GetBins1D());
    ret->SetLineColor(col);
    ret->SetLineStyle(style);
    ret->GetXaxis()->SetTitle(fAxis.GetLabel1D().c_str());
    ret->GetYaxis()->SetTitle("Ratio");
    return ret;
  }

  //----------------------------------------------------------------------
  TH2* Ratio::ToTH2() const
  {
    if(fAxis.NDimensions() != 2){
      std::cout << "Error: This Ratio does not appear to be 2D." << std::endl;
      abort();
    }

    TH2* ret = ana::ToTH2Helper(std::unique_ptr<TH1>(ToTH1()),
                                fAxis.GetBinnings()[0], 
                                fAxis.GetBinnings()[1]);

    ret->GetXaxis()->SetTitle(fAxis.GetLabels()[0].c_str());
    ret->GetYaxis()->SetTitle(fAxis.GetLabels()[1].c_str());

    // Allow GetMean() and friends to work even if this histogram never had any
    // explicit Fill() calls made.
    if(ret->GetEntries() == 0) ret->SetEntries(1);

    return ret;
  }
} // namespace
