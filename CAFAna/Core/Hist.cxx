#include "CAFAna/Core/Hist.h"

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/UtilsExt.h"

#include "CAFAna/Core/Stan.h"

#include "TH1.h"
#include "THnSparse.h"

#include "TDirectory.h"

namespace
{
  namespace util{template<class T> T sqr(const T& x){return x*x;}}
}

namespace ana
{
  //----------------------------------------------------------------------
  bool Hist::StatErrorsEnabled()
  {
    static bool enabled = (getenv("CAFANA_STAT_ERRS") != 0);
    static bool once = true;
    if(once && enabled) std::cout << "CAFAna: Propagation of statistical uncertainties enabled" << std::endl << std::endl;
    once = false;

    return enabled;
  }

  //----------------------------------------------------------------------
  Hist::Hist() : fType(kUninitialized), fSqrtErrs(false)
  {
  }

  //----------------------------------------------------------------------
  Hist Hist::Zero(int nbins)
  {
    Hist ret;
    ret.fType = kDense;
    ret.fData = Eigen::ArrayXd::Zero(nbins+2);
    ret.fSqrtErrs = true;
    return ret;
  }

  //----------------------------------------------------------------------
  Hist Hist::ZeroSparse(int nbins)
  {
    Hist ret;
    ret.fType = kSparse;
    ret.fDataSparse = Eigen::SparseVector<double>(nbins+2);
    ret.fSqrtErrs = true;
    return ret;
  }

  //----------------------------------------------------------------------
  Hist::Hist(const Hist& rhs)
    : Hist()
  {
    assert(rhs.Initialized());

    fType = rhs.fType;
    // Only one of these will actually have contents
    fDataSparse = rhs.fDataSparse;
    fDataStan   = rhs.fDataStan;
    fData       = rhs.fData;
    fSumSq      = rhs.fSumSq;
    fSqrtErrs   = rhs.fSqrtErrs;
  }

  //----------------------------------------------------------------------
  Hist::Hist(Hist&& rhs)
    : Hist()
  {
    assert(rhs.Initialized());

    fType = rhs.fType;
    std::swap(fDataSparse, rhs.fDataSparse);
    std::swap(fDataStan,   rhs.fDataStan);
    std::swap(fData,       rhs.fData);
    std::swap(fSumSq,      rhs.fSumSq);
    fSqrtErrs = rhs.fSqrtErrs;
  }

  //----------------------------------------------------------------------
  Hist& Hist::operator=(const Hist& rhs)
  {
    if(this == &rhs) return *this;

    DontAddDirectory guard;

    assert(rhs.Initialized());

    fType = rhs.fType;
    fDataSparse = rhs.fDataSparse;
    fDataStan   = rhs.fDataStan;
    fData       = rhs.fData;
    fSumSq      = rhs.fSumSq;
    fSqrtErrs   = rhs.fSqrtErrs;

    return *this;
  }

  //----------------------------------------------------------------------
  Hist& Hist::operator=(Hist&& rhs)
  {
    if(this == &rhs) return *this;

    assert(rhs.Initialized());

    fType = rhs.fType;
    std::swap(fDataSparse, rhs.fDataSparse);
    std::swap(fDataStan,   rhs.fDataStan);
    std::swap(fData,       rhs.fData);
    std::swap(fSumSq,      rhs.fSumSq);
    fSqrtErrs = rhs.fSqrtErrs;

    return *this;
  }

  //----------------------------------------------------------------------
  Hist::~Hist()
  {
  }

  //----------------------------------------------------------------------
  Hist Hist::AdoptSparse(Eigen::SparseVector<double>&& v)
  {
    Hist ret;
    ret.fType = kSparse;
    ret.fDataSparse = std::move(v);
    return ret;
  }

  //----------------------------------------------------------------------
  Hist Hist::AdoptStan(Eigen::ArrayXstan&& v)
  {
    Hist ret;
    ret.fType = kDenseStan;
    ret.fDataStan = std::move(v);
    return ret;
  }

  //----------------------------------------------------------------------
  Hist Hist::Adopt(Eigen::ArrayXd&& v)
  {
    Hist ret;
    ret.fType = kDense;
    ret.fData = std::move(v);
    return ret;
  }

  //----------------------------------------------------------------------
  Hist Hist::AdoptWithErrors(Eigen::ArrayXd&& v, Eigen::ArrayXd&& sqerr)
  {
    // Function to let user manually set fSumSq for a Hist.

    if(!StatErrorsEnabled()){
      std::cout << "Hist::AdoptWithErrors: must have stat errors enabled (via $CAFANA_STAT_ERRS env var) to use this constructor" << std::endl;
      abort();
    }

    Hist ret;
    ret.fType = kDense;
    ret.fData = std::move(v);
    ret.fSumSq = std::move(sqerr);
    return ret;
  }
  
  //----------------------------------------------------------------------
  Hist Hist::FromDirectory(TDirectory* dir)
  {
    Hist ret;

    TH1D* h = (TH1D*)dir->Get("hist");
    THnSparseD* hSparse = (THnSparseD*)dir->Get("hist_sparse");
    assert(bool(h) != bool(hSparse));

    if(h){
      ret.fType = kDense;
      ret.fData = Eigen::Map<Eigen::ArrayXd>(h->GetArray(), h->GetNbinsX()+2);

      if(StatErrorsEnabled()){
        const int N = h->GetNbinsX()+2;
        ret.fSumSq.resize(N);
        for(int i = 0; i < N; ++i) ret.fSumSq[i] = util::sqr(h->GetBinError(i));
      }
    }
    if(hSparse){
      ret.fType = kSparse;

      // Number of bins there would be in a dense version
      int nbins = 1;
      const int N = hSparse->GetNdimensions();
      for(int d = 0; d < N; ++d)
        nbins *= hSparse->GetAxis(d)->GetNbins()+2;

      ret.fDataSparse.resize(nbins);
      ret.fDataSparse.setZero();
      for(int i = 0; i < hSparse->GetNbins(); ++i){
        std::vector<int> idx(N);
        const double y = hSparse->GetBinContent(i, &idx.front());
        int idx1d = idx[0];
        int mult = 1;
        for(int d = 1; d < N; ++d){
          mult *= hSparse->GetAxis(d-1)->GetNbins()+2;
          idx1d += mult*idx[d];
        }
        ret.fDataSparse.coeffRef(idx1d) = y;
      }
    }

    delete h;
    delete hSparse;

    return ret;
  }

  //----------------------------------------------------------------------
  TH1D* Hist::ToTH1(const Binning& bins) const
  {
    assert(Initialized());

    TH1D* ret = MakeTH1D(UniqueName().c_str(), "", bins);

    for(int i = 0; i < bins.NBins()+2; ++i){
      switch(fType){
      case kDense:     ret->SetBinContent(i, fData[i]); break;
      case kDenseStan: ret->SetBinContent(i, fDataStan[i].val()); break;
        // Interface requires returning literally a TH1D in any case
      case kSparse:    ret->SetBinContent(i, fDataSparse.coeff(i)); break;
      default:
        abort(); // unreachable
      }
      if(fSumSq.size() > 0  || fSqrtErrs) ret->SetBinError(i, GetBinError(i));
    }
    return ret;
  }

  //----------------------------------------------------------------------
  const Eigen::ArrayXd& Hist::GetEigenSqErrors() const
  {
    assert(fType == kDense);
    if(fSqrtErrs){
      // Need to make the errors real, not just implicit
      fSumSq = fData;
      fSqrtErrs = false;
    }
    return fSumSq;
  }

  //----------------------------------------------------------------------
  int Hist::GetNbinsX() const
  {
    assert(Initialized());

    switch(fType){
    case kSparse:    return fDataSparse.size()-2;
    case kDenseStan: return fDataStan  .size()-2;
    case kDense:     return fData      .size()-2;
    default: abort(); // unreachable
    }
  }

  //----------------------------------------------------------------------
  double Hist::GetBinError(int i) const
  {
    assert(Initialized());

    if(fSqrtErrs) return sqrt(GetBinContent(i));
    if(fSumSq.size() > 0) return sqrt(fSumSq[i]);

    return 0;
  }

  //----------------------------------------------------------------------
  double Hist::Integral() const
  {
    assert(Initialized());

    switch(fType){
    case kSparse:    return fDataSparse.sum();
    case kDenseStan: return fDataStan  .sum().val();
    case kDense:     return fData      .sum();
    default: abort(); // unreachable
    }
  }

  //----------------------------------------------------------------------
  void Hist::Fill(int bin, double w)
  {
    assert(Initialized());

    if(w != 1) fSqrtErrs = false;

    if(fType == kDense && StatErrorsEnabled() && fSumSq.size() == 0) fSumSq.resize(fData.size());

    switch(fType){
    case kSparse:
      fDataSparse.coeffRef(bin) += w;
      break;
    case kDenseStan:
      std::cout << "Hist::Fill() not supported for stan vars" << std::endl;
      abort();
    case kDense:
      {
        fData[bin] += w;
        if(StatErrorsEnabled()){
          if(fSumSq.size() == 0) fSumSq.resize(fData.size());
          fSumSq[bin] += w*w;
        }
      }
      break;
    default:
      abort(); // unreachable
    }
  }

  //----------------------------------------------------------------------
  void Hist::Scale(double s)
  {
    assert(Initialized());

    switch(fType){
    case kSparse:    fDataSparse *= s; break;
    case kDenseStan: fDataStan   *= s; break;
    case kDense:     fData       *= s; break;
    default:
      abort(); // unreachable
    }

    if(s != 1) fSqrtErrs = false;
    fSumSq *= s;
  }

  //----------------------------------------------------------------------
  void Hist::Scale(const stan::math::var& s)
  {
    assert(Initialized());
    switch (fType){
    case kSparse:
      fDataStan = fDataSparse * s;
      fDataSparse.resize(0);
      break;

    case kDense:
      fDataStan = fData * s;
      fData.resize(0);
      break;

    case kDenseStan:
      fDataStan *= s;
      break;

    default:
      abort(); // unreachable
    }

    fType = kDenseStan;

    if(s != 1) fSqrtErrs = false;
    fSumSq *= s.val();
  }

  //----------------------------------------------------------------------
  void Hist::ResetErrors()
  {
    fSumSq.resize(0);
    fSqrtErrs = true;
  }

  //----------------------------------------------------------------------
  double Hist::GetBinContent(int i) const
  {
    assert(Initialized());

    switch(fType){
    case kSparse: return fDataSparse.coeff(i);
    case kDenseStan: return fDataStan[i].val();
    case kDense: return fData[i];
    default:
      abort(); // unreachable
    }
  }

  //----------------------------------------------------------------------
  void Hist::SetBinContent(int i, double x)
  {
    assert(Initialized());

    switch(fType){
    case kSparse: fDataSparse.coeffRef(i) = x; break;
    case kDenseStan:
      std::cout << "Hist::SetBinContent() not implemented for stan vars" << std::endl;
      abort();
    case kDense: fData[i] = x; break;
    default:
      abort(); // unreachable
    }

    fSqrtErrs = false;
    if(fSumSq.size() > 0) fSumSq[i] = 0;
  }

  //----------------------------------------------------------------------
  void Hist::Reset()
  {
    switch(fType){
    case kSparse:    fDataSparse.setZero(); break;
    case kDenseStan: fDataStan  .setZero(); break;
    case kDense:     fData      .setZero(); break;
    default: ; // OK?
    }

    fSqrtErrs = true;
    fSumSq.resize(0);
  }

  //----------------------------------------------------------------------
  void Hist::Add(const Eigen::SparseVector<double>& rhs, double scale)
  {
    switch(fType){
    case kSparse:    fDataSparse += rhs * scale; break;
    case kDenseStan: fDataStan   += rhs * scale; break;
    case kDense:     fData       += rhs * scale; break;
    default: abort(); // unreachable
    }
  }

  //----------------------------------------------------------------------
  void Hist::Add(const Eigen::ArrayXstan& rhs, double scale)
  {
    switch(fType){
    case kSparse:
      fType = kDenseStan;
      fDataStan = rhs * scale;
      fDataStan += fDataSparse;
      fDataSparse.resize(0);
      break;

    case kDenseStan:
      fDataStan += rhs * scale;
      break;

    case kDense:
      fType = kDenseStan;
      fDataStan = fData + rhs * scale;
      fData.resize(0);
      break;

    default: abort(); // unreachable
    }
  }

  //----------------------------------------------------------------------
  void Hist::Add(const Eigen::ArrayXd& rhs, double scale)
  {
    switch(fType){
    case kSparse:
      fType = kDense;
      fData = rhs * scale;
      fData += fDataSparse;
      fDataSparse.resize(0);
      break;

    case kDenseStan:
      fDataStan += rhs * scale;
      break;

    case kDense:
      fData += rhs * scale;
      break;

    default: abort(); // unreachable
    }
  }

  //----------------------------------------------------------------------
  void Hist::Add(const Hist& rhs, double scale)
  {
    assert(Initialized());
    assert(rhs.Initialized());

    switch(rhs.fType){
    case kSparse:    Add(rhs.fDataSparse, scale); break;
    case kDenseStan: Add(rhs.fDataStan,   scale); break;
    case kDense:     Add(rhs.fData,       scale); break;
    default: abort(); // unreachable
    }

    if(scale != 1 || !rhs.fSqrtErrs) fSqrtErrs = false;

    if(fSumSq.size() > 0){
      if(rhs.fSumSq.size() > 0) fSumSq += scale * rhs.fSumSq;
      // otherwise nothing to add in
    }
    else{
      fSumSq = scale * rhs.fSumSq;
    }
  }

  //----------------------------------------------------------------------
  void Hist::Multiply(const Hist& rhs)
  {
    assert(Initialized());
    assert(rhs.Initialized());

    if(fType == kSparse || rhs.fType == kSparse){
      std::cout << "Hist::Multiply() not implemented for sparse vectors" << std::endl;
      abort();
    }

    if(fType == kDenseStan){
      if(rhs.fType == kDenseStan){
        fDataStan *= rhs.fDataStan;
      }
      else{
        fDataStan *= rhs.fData;
      }
    }
    else{
      if(rhs.fType == kDenseStan){
        fType = kDenseStan;
        fDataStan = fData * rhs.fDataStan;
        fData.resize(0);
      }
      else{
        fData *= rhs.fData;
      }
    }

    if(fType == kDense && rhs.fType == kDense){
      if(fSumSq.size() > 0){
        if(rhs.fSumSq.size() > 0){
          fSumSq = fSumSq * util::sqr(rhs.fData) + rhs.fSumSq * util::sqr(fData);
        }
        else{
          fSumSq *= util::sqr(rhs.fData);
        }
      }
      else if(rhs.fSumSq.size() > 0){
        fSumSq = rhs.fSumSq * util::sqr(fData);
      }
    }
    else{
      // Didn't bother to implement error prop for multiplying stan/sparse
      // hists
      fSumSq.resize(0);
    }

    fSqrtErrs = false;
  }

  //----------------------------------------------------------------------
  void Hist::Divide(const Hist& rhs)
  {
    assert(Initialized());
    assert(rhs.Initialized());

    if(fType == kSparse || rhs.fType == kSparse){
      std::cout << "Hist::Divide() not implemented for sparse vectors" << std::endl;
      abort();
    }

    if(fType == kDenseStan){
      if(rhs.fType == kDenseStan){
        fDataStan /= rhs.fDataStan;
      }
      else{
        fDataStan /= rhs.fData;
      }
    }
    else{
      if(rhs.fType == kDenseStan){
        fType = kDenseStan;
        fDataStan = fData / rhs.fDataStan;
        fData.resize(0);
      }
      else{
        fData /= rhs.fData;
      }
    }

    if(fType == kDense && rhs.fType == kDense){
      if(fSumSq.size() > 0){
        if(rhs.fSumSq.size() > 0){
          fSumSq = fSumSq / util::sqr(rhs.fData) + rhs.fSumSq * util::sqr(fData) / util::sqr(util::sqr(rhs.fData));
        }
        else{
          fSumSq /= util::sqr(rhs.fData);
        }
      }
      else if(rhs.fSumSq.size() > 0){
        fSumSq = rhs.fSumSq * util::sqr(fData) / util::sqr(util::sqr(rhs.fData));
      }
    }
    else{
      // Didn't bother to implement error prop for dividing stan/sparse
      // hists
      fSumSq.resize(0);
    }

    fSqrtErrs = false;
  }

  //----------------------------------------------------------------------
  void Hist::Write(const Binning& bins) const
  {
    assert(Initialized());

    if(fType == kDenseStan){
      std::cout << "Hist::Write() not implemented (impossible?) for stan vars" << std::endl;
      abort();
    }
    if(fType == kDense){
      TH1D* h = ToTH1(bins);
      h->Write("hist");
      delete h;
    }
    if(fType == kSparse){
      const int n = bins.NBins();
      const double x0 = bins.IsSimple() ? bins.Min() : 0;
      const double x1 = bins.IsSimple() ? bins.Max() : bins.NBins();
      THnSparseD* h = new THnSparseD("", "", 1, &n, &x0, &x1);

      for(Eigen::SparseVector<double>::InnerIterator it(fDataSparse); it; ++it){
        const int idx = it.index();
        h->SetBinContent(&idx, it.value());
        if(fSqrtErrs) h->SetBinError(&idx, sqrt(it.value()));
      }

      h->Write("hist_sparse");
      delete h;
    }
  }
}
