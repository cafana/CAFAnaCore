#pragma once

#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/IValueSink.h"

#include <string>

class TDirectory;
class TH2;
class TH2D;

namespace ana
{
  template<class T> class _Var;
  template<class T> class _Weight;

  template<class T> _Weight<T> Unweighted();

  /// %Spectrum with the value of a second variable, allowing for reweighting
  class ReweightableSpectrum: public IValuePairSink
  {
  public:
    friend class ReweightableSpectrumSink;
    friend class SpectrumSinkBase<ReweightableSpectrum>;

#ifdef CAFANACORE_SPECTRUMLOADERBASE
    template<class T, class U>
    ReweightableSpectrum(SpectrumLoaderBase& loader,
                         const _HistAxis<_Var<T>>& recoAxis,
                         const _HistAxis<_Var<T>>& trueAxis,
                         const _Cut<T, U>& cut,
                         const SystShifts& shift = kNoShift,
                         const _Weight<T>& wei = Unweighted<T>());
#endif

    ReweightableSpectrum(const Eigen::MatrixXd&& mat,
                         const LabelsAndBins& recoAxis,
                         const LabelsAndBins& trueAxis,
                         double pot, double livetime);

    ReweightableSpectrum(IValuePairSource& src,
                         const LabelsAndBins& recoAxis,
                         const LabelsAndBins& trueAxis);

    template<class T> ReweightableSpectrum(_IRecordSource<T>& src,
                                           const _HistAxis<_Var<T>>& recoAxis,
                                           const _HistAxis<_Var<T>>& trueAxis)
      : ReweightableSpectrum(src.GetVars(recoAxis.GetVar1D(), trueAxis.GetVar1D()), recoAxis, trueAxis)
    {
    }

    /// The only valid thing to do with such a spectrum is to assign something
    /// else into it.
    static ReweightableSpectrum Uninitialized(){return ReweightableSpectrum();}

    virtual ~ReweightableSpectrum();

    ReweightableSpectrum(const ReweightableSpectrum& rhs);
    ReweightableSpectrum& operator=(const ReweightableSpectrum& rhs);

    virtual void Fill(double x, double y, double w) override;
    virtual void FillPOT(double pot) override;
    virtual void FillLivetime(double livetime) override;

    TH2D* ToTH2(double pot) const;

    double POT() const{return fPOT;}
    double Livetime() const{return fLivetime;}

    Spectrum UnWeighted() const;

    Spectrum WeightingVariable() const;

    /// Reco spectrum with truth weights applied
    Spectrum WeightedBy(const Ratio& weights) const;

    /// Rescale bins so that \ref WeightingVariable will return \a target
    void ReweightToTrueSpectrum(const Spectrum& target);
    /// Recale bins so that \ref Unweighted will return \a target
    void ReweightToRecoSpectrum(const Spectrum& target);

    // Arithmetic operators are as if these are unlike samples, each a
    // contribution to one total, not seperate sources of stats for the same
    // sample.
    ReweightableSpectrum& operator+=(const ReweightableSpectrum& rhs);
    ReweightableSpectrum operator+(const ReweightableSpectrum& rhs) const;

    ReweightableSpectrum& operator-=(const ReweightableSpectrum& rhs);
    ReweightableSpectrum operator-(const ReweightableSpectrum& rhs) const;

    void Clear();

    void SaveTo(TDirectory* dir, const std::string& name) const;

    static std::unique_ptr<ReweightableSpectrum> LoadFrom(TDirectory* dir, const std::string& name);

    unsigned int NDimensions() const{return fAxisX.NDimensions();}
    const std::vector<std::string>& GetLabels() const {return fAxisX.GetLabels();}
    const std::vector<Binning>& GetBinnings() const {return fAxisX.GetBinnings();}
    const std::vector<Binning>& GetTrueBinnings() const {return fAxisY.GetBinnings();}

    Eigen::MatrixXd GetEigen(double pot) const {return fMat * pot/fPOT;}
  protected:
    // Derived classes can be trusted take care of their own construction
    ReweightableSpectrum(const LabelsAndBins& axisX,
                         const LabelsAndBins& axisY)
      : fPOT(0), fLivetime(0),
        fAxisX(axisX), fAxisY(axisY)
    {
    }

    // Constructor for user by Uninitialized()
    ReweightableSpectrum()
      : fPOT(0), fLivetime(0),
        fAxisX(std::vector<std::string>(), std::vector<Binning>()),
        fAxisY(std::vector<std::string>(), std::vector<Binning>())
    {
    }

    ReweightableSpectrum& PlusEqualsHelper(const ReweightableSpectrum& rhs, int sign);

    void RemoveLoader(ReweightableSpectrum**);
    void AddLoader(ReweightableSpectrum**);

    void _SaveTo(TDirectory* dir,
                 const std::string& name,
                 const std::string& type) const;

    Eigen::MatrixXd fMat;
    double fPOT;
    double fLivetime;

    LabelsAndBins fAxisX, fAxisY;

    /// Things that point at this ReweightableSpectrum
    std::set<ReweightableSpectrum**> fReferences;
  };
}
