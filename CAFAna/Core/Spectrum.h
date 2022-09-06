#pragma once

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/FwdDeclare.h"
#include "CAFAna/Core/HistAxis.h"
#include "CAFAna/Core/Hist.h"
#include "CAFAna/Core/IValueSource.h"
#include "CAFAna/Core/IRecordSource.h"
#include "CAFAna/Core/UtilsExt.h"

#include <Eigen/Dense>

#include "TAttLine.h"

#include <set>
#include <string>
#include <vector>

class TDirectory;
class TH1;
class TH2;
class TH3;
class TH1D;

/// Oscillation analysis framework, runs over CAF files outside of ART
namespace ana
{
  class Ratio;
  class SpectrumLoaderBase;
  template<class T> class _Var;
  template<class T> class _Weight;
  template<class T> class _MultiVar;

  template<class T> _Weight<T> Unweighted();

  class SystShifts;
  extern const SystShifts kNoShift;

  template<class T> class SpectrumSinkBase;

  /// Representation of a spectrum in any variable, with associated POT
  class Spectrum: public IValueSink
  {
  public:
    friend class SpectrumLoaderBase;
    friend class SpectrumSink;
    friend class SpectrumSinkBase<Spectrum>;
    friend class Ratio;

    enum ESparse{kDense, kSparse};

    /// Construct a spectrum from a source of values and an axis definition
    Spectrum(IValueSource& src, const LabelsAndBins& axis, ESparse sparse = kDense);

    /// \brief Shorthand construction with a source of records and a HistAxis
    /// defining the Var to extract from those records.
    template<class RecT> Spectrum(_IRecordSource<RecT>& src,
                                  const _HistAxis<_Var<RecT>>& axis,
                                  ESparse sparse = kDense)
      : Spectrum(src[axis.GetVar1D()], axis, sparse)
    {
    }

#ifdef CAFANACORE_SPECTRUMLOADERBASE
    /// One constructor to rule them all
    template<class T, class U>
    Spectrum(SpectrumLoaderBase& loader,
             const _HistAxis<_Var<T>>& axis,
             const _Cut<T, U>& cut,
             const SystShifts& shift = kNoShift,
             const _Weight<T>& wei = Unweighted<T>(),
             ESparse sparse = kDense);

    template<class T, class U>
    Spectrum(const std::string& label,
             const Binning& bins,
             SpectrumLoaderBase& loader,
             const _Var<T>& var,
             const _Cut<T, U>& cut,
             const SystShifts& shift = kNoShift,
             const _Weight<T>& wei = Unweighted<T>(),
             ESparse sparse = kDense);

    /// The only \ref MultiVar variant available
    template<class T, class U>
    Spectrum(SpectrumLoaderBase& loader,
             const _HistAxis<_MultiVar<T>>& axis,
             const _Cut<T, U>& cut,
             const SystShifts& shift = kNoShift,
             const _Weight<T>& wei = Unweighted<T>());
#endif

    /// Makes a spectrum from an eigen array
    Spectrum(Eigen::ArrayXd&& h,
             const LabelsAndBins& axis,
             double pot, double livetime);

    /// Makes a spectrum from an eigen array of stan vars
    Spectrum(Eigen::ArrayXstan&& h,
             const LabelsAndBins& axis,
             double pot, double livetime);

    /// Makes a spectrum from two eigen arrays
    /// One array is for bin contents, the other for squared errors
    /// Only to be used with stat errors enabled
    Spectrum(Eigen::ArrayXd&& h,
             Eigen::ArrayXd&& sqerr,
             const LabelsAndBins& axis,
             double pot, double livetime);

#ifdef CAFANACORE_SPECTRUMLOADERBASE

    /// 2D Spectrum taking 2 HistAxis
    template<class T, class U>
    Spectrum(SpectrumLoaderBase& loader,
             const _HistAxis<_Var<T>>& xAxis,
             const _HistAxis<_Var<T>>& yAxis,
             const _Cut<T, U>& cut,
             const SystShifts& shift = kNoShift,
             const _Weight<T>& wei = Unweighted<T>(),
             ESparse sparse = kDense);

    /// 2D Spectrum of two Vars
    template<class T, class U>
    Spectrum(const std::string& xLabel,
	     const std::string& yLabel,
	     SpectrumLoaderBase& loader,
	     const Binning& binsx, const _Var<T>& varx,
	     const Binning& binsy, const _Var<T>& vary,
	     const _Cut<T, U>& cut,
	     const SystShifts& shift = kNoShift,
	     const _Weight<T>& wei = Unweighted<T>(),
             ESparse sparse = kDense);

    /// 3D Spectrum taking 3 HistAxis
    template<class T, class U>
    Spectrum(SpectrumLoaderBase& loader,
             const _HistAxis<_Var<T>>& xAxis,
             const _HistAxis<_Var<T>>& yAxis,
	     const _HistAxis<_Var<T>>& zAxis,
             const _Cut<T, U>& cut,
             const SystShifts& shift = kNoShift,
             const _Weight<T>& wei = Unweighted<T>(),
	     ESparse sparse = kDense);

    /// 3D Spectrum of three Vars
    template<class T, class U>
    Spectrum(const std::string& xLabel,
	     const std::string& yLabel,
	     const std::string& zLabel,
	     SpectrumLoaderBase& loader,
             const Binning& binsx, const _Var<T>& varx,
             const Binning& binsy, const _Var<T>& vary,
             const Binning& binsz, const _Var<T>& varz,
             const _Cut<T, U>& cut,
             const SystShifts& shift = kNoShift,
             const _Weight<T>& wei = Unweighted<T>(),
	     ESparse sparse = kDense);
#endif

    /// The only valid thing to do with such a spectrum is to assign something
    /// else into it.
    static Spectrum Uninitialized(){return Spectrum();}

    virtual ~Spectrum();

    Spectrum(const Spectrum& rhs);
    Spectrum(Spectrum&& rhs);
    Spectrum& operator=(const Spectrum& rhs);
    Spectrum& operator=(Spectrum&& rhs);

    virtual void Fill(double x, double w) override;
    virtual void FillPOT(double pot) override;
    virtual void FillLivetime(double livetime) override;

    /// \brief Histogram made from this Spectrum, scaled to some exposure
    ///
    /// \param exposure POT or livetime (seconds)
    /// \param col Histogram color (default black)
    /// \param style Histogram line style (default solid)
    /// \param expotype How to interpret exposure (kPOT (default) or kLivetime)
    TH1D* ToTH1(double exposure,
                Color_t col = kBlack,
                Style_t style = kSolid,
                EExposureType expotype = kPOT,
                EBinType bintype = kBinContent) const;

    /// \brief Histogram made from this Spectrum, scaled to some exposure
    ///
    /// \param exposure POT or livetime (seconds)
    /// \param expotype How to interpret exposure (kPOT (default) or kLivetime)
    TH1D* ToTH1(double exposure,
                EExposureType expotype,
                EBinType bintype = kBinContent) const;

    /// Spectrum must be 2D to obtain TH2
    TH2*  ToTH2     (double exposure, EExposureType expotype = kPOT,
		     EBinType bintype = kBinContent) const;

    /// Spectrum must be 3D to obtain TH3
    TH3*  ToTH3     (double exposure, EExposureType expotype = kPOT,
		     EBinType bintype = kBinContent) const;

    TH1* ToTHX(double exposure, EExposureType expotype = kPOT,
               EBinType bintype = kBinContent) const;

    bool HasStan() const {return fHist.HasStan();}
    /// NB these don't have POT scaling. For expert high performance ops only!
    const Eigen::ArrayXd& GetEigen() const {return fHist.GetEigen();}
    const Eigen::ArrayXstan& GetEigenStan() const {return fHist.GetEigenStan();}
    const Eigen::ArrayXd& GetEigenSqErrors() const {return fHist.GetEigenSqErrors();}

    Eigen::ArrayXd GetEigen(double exposure, EExposureType expotype = kPOT) const;
    Eigen::ArrayXstan GetEigenStan(double exposure, EExposureType expotype = kPOT) const;
    Eigen::ArrayXd GetEigenSqErrors(double exposure, EExposureType expotype = kPOT) const;

    /// \brief Return total number of events scaled to \a pot
    ///
    /// \param exposure POT/livetime to scale to
    /// \param err      The statistical error on this total (optional)
    /// \param expotype What the first parameter represents
    double Integral(double exposure, double* err = 0,
		    EExposureType expotype = kPOT) const;

    /// \brief Return mean of 1D histogram
    double Mean() const;

    /// \brief Mock data is \ref FakeData with Poisson fluctuations applied
    ///
    /// Use for low-budget MDCs, or just getting a sense of the expected scale
    /// of statistical variation. NB seed = 0 is true random
    Spectrum MockData(double pot, int seed = 0) const;

    /// \brief Asimov data is a MC spectrum scaled to the POT expected in the
    /// data
    ///
    /// Use for sensitivity plots and testing fit convergence
    Spectrum AsimovData(double pot) const;

    /// Use for sensitivity plots when fake cosmic data is needed.
    /// Fake cosmic spectra can be added to FakeData by desired livetime.
    Spectrum AsimovData(double pot, double livetime) const;

    /// Synonymous with AsimovData(). Retained for compatibility
    Spectrum FakeData(double pot) const;
    /// Synonymous with AsimovData(). Retained for compatibility
    Spectrum FakeData(double pot, double livetime) const;

    double POT() const {return fPOT;}

    /// Seconds. For informational purposes only. No calculations use this.
    double Livetime() const {return fLivetime;}

    /// DO NOT USE UNLESS YOU ARE 110% CERTAIN THERE ISN'T A BETTER WAY!
    void OverridePOT(double newpot) {fPOT = newpot;}

    /// DO NOT USE UNLESS YOU ARE 110% CERTAIN THERE ISN'T A BETTER WAY!
    void OverrideLivetime(double newlive) {fLivetime = newlive;}

    void Clear();

    /// Multiply this spectrum by a constant c
    void Scale(double c);
    void Scale(const stan::math::var& v);

    // Arithmetic operators are as if these are unlike samples, each a
    // contribution to one total, not seperate sources of stats for the same
    // sample.
    Spectrum& operator+=(const Spectrum& rhs);
    Spectrum operator+(const Spectrum& rhs) const;

    Spectrum& operator-=(const Spectrum& rhs);
    Spectrum operator-(const Spectrum& rhs) const;

    Spectrum& operator*=(const Ratio& rhs);
    Spectrum operator*(const Ratio& rhs) const;

    Spectrum& operator/=(const Ratio& rhs);
    Spectrum operator/(const Ratio& rhs) const;

    void SaveTo(TDirectory* dir, const std::string& name) const;
    static std::unique_ptr<Spectrum> LoadFrom(TDirectory* dir, const std::string& name);

    unsigned int NDimensions() const{return fAxis.NDimensions();}
    const std::vector<std::string>& GetLabels() const {return fAxis.GetLabels();}
    const std::vector<Binning>& GetBinnings() const {return fAxis.GetBinnings();}

  protected:
    /// Constructor for Uninitialized()
    Spectrum()
      : fHist(Hist::Uninitialized()),
        fPOT(0), fLivetime(0),
        fAxis(std::vector<std::string>(), std::vector<Binning>())
    {
    }

    /// Helper for constructors
    Spectrum(const LabelsAndBins& axis, ESparse sparse = kDense);

    void RemoveLoader(Spectrum**);
    void AddLoader(Spectrum**);

    /// Helper for operator+= and operator-=
    Spectrum& PlusEqualsHelper(const Spectrum& rhs, int sign);

    Hist fHist;
    double fPOT;
    double fLivetime;

    /// Things that point at this Spectrum. Maintained by SpectrumLoader
    std::set<Spectrum**> fReferences;

    LabelsAndBins fAxis;
  };

  // Commutative
  inline Spectrum operator*(const Ratio& lhs, const Spectrum& rhs){return rhs*lhs;}
}
