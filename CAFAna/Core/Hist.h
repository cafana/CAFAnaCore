#pragma once

#ifdef CAFANACORE_USE_STAN
#include "CAFAna/Core/StanVar.h"
#endif

class TDirectory;

class TH1D;

#include <cassert>

#include <Eigen/Dense>
#include <Eigen/SparseCore>

#ifdef CAFANACORE_USE_STAN
namespace Eigen{
  using ArrayXstan = Eigen::Array<stan::math::var, Eigen::Dynamic, 1>;
  using VectorXstan = Eigen::Matrix<stan::math::var, Eigen::Dynamic, 1>;
}
#endif

namespace ana
{
  class Binning;

  class Hist
  {
  public:
    Hist(const Hist&);
    Hist(Hist&&);
    Hist& operator=(const Hist&);
    Hist& operator=(Hist&&);
    ~Hist();

    static Hist Uninitialized(){return Hist();}
    bool Initialized() const {return fType != kUninitialized;}

    static Hist Zero(int nbins);
    static Hist ZeroSparse(int nbins);

    static Hist AdoptSparse(Eigen::SparseVector<double>&& v);
    static Hist Adopt(Eigen::ArrayXd&& v);
    static Hist AdoptWithErrors(Eigen::ArrayXd&& v, Eigen::ArrayXd&& sqerr);
#ifdef CAFANACORE_USE_STAN
    static Hist AdoptStan(Eigen::ArrayXstan&& v);
#endif

    static Hist FromDirectory(TDirectory* dir);

    /// Set by CAFANA_STAT_ERRS environment variable
    static bool StatErrorsEnabled();

    TH1D* ToTH1(const Binning& bins) const;

    bool HasStan() const
#ifdef CAFANACORE_USE_STAN
      {return fType == kDenseStan;}
#else
      {return false;}
#endif

    const Eigen::ArrayXd& GetEigen() const {assert(fType == kDense); return fData;}
    const Eigen::ArrayXd& GetEigenSqErrors() const;
#ifdef CAFANACORE_USE_STAN
    const Eigen::ArrayXstan& GetEigenStan() const {assert(fType == kDenseStan); return fDataStan;}
#endif

    int GetNbinsX() const;
    double GetBinError(int i) const;
    double Integral() const;

    void Fill(int bin, double w);
    void Scale(double s);
#ifdef CAFANACORE_USE_STAN
    void Scale(const stan::math::var& s);
#endif
    void ResetErrors();

    double GetBinContent(int i) const;
    void SetBinContent(int i, double x);
    void Reset();

    void Add(const Hist& rhs, double scale = 1);

    void Multiply(const Hist& rhs);
    void Divide(const Hist& rhs);

    void Write(const Binning& bins) const;

  protected:
    Hist();

    // Helpers for the public Add() function
    void Add(const Eigen::SparseVector<double>& rhs, double scale);
    void Add(const Eigen::ArrayXd& rhs, double scale);
#ifdef CAFANACORE_USE_STAN
    void Add(const Eigen::ArrayXstan& rhs, double scale);
#endif

    enum EType{kUninitialized, kDense, kDenseStan, kSparse};
    EType fType;

    Eigen::SparseVector<double> fDataSparse;
    Eigen::ArrayXd fData;
#ifdef CAFANACORE_USE_STAN
    Eigen::ArrayXstan fDataStan;
#endif

    mutable Eigen::ArrayXd fSumSq; ///< Accumulate errors, if enabled
    mutable bool fSqrtErrs; ///< Special case when filled with unweighted data
  };
}
