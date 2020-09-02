#pragma once

#include "CAFAna/Core/Spectrum.h"

#include <string>

class TDirectory;
class TH2;
class TH2D;

namespace ana
{
  template<class T> class _Var;
  template<class T> class _Cut;

  template<class T> _Var<T> Unweighted();

  /// %Spectrum with the value of a second variable, allowing for reweighting
  class ReweightableSpectrum
  {
  public:
    friend class SpectrumLoaderBase;
    friend class SpectrumLoader;
    friend class NullLoader;

    template<class T>
    ReweightableSpectrum(SpectrumLoaderBase& loader,
                         const _HistAxis<_Var<T>>& recoAxis,
                         const _HistAxis<_Var<T>>& trueAxis,
                         const _Cut<T>& cut,
                         const SystShifts& shift = kNoShift,
                         const _Var<T>& wei = Unweighted<T>());

    ReweightableSpectrum(const Eigen::MatrixXd&& mat,
                         const LabelsAndBins& recoAxis,
                         const LabelsAndBins& trueAxis,
                         double pot, double livetime);

    /// The only valid thing to do with such a spectrum is to assign something
    /// else into it.
    static ReweightableSpectrum Uninitialized(){return ReweightableSpectrum();}

    virtual ~ReweightableSpectrum();

    ReweightableSpectrum(const ReweightableSpectrum& rhs);
    ReweightableSpectrum& operator=(const ReweightableSpectrum& rhs);

    void Fill(double x, double y, double w = 1);

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
    Binning GetTrueBinning() const {return fBinsY;}

    Eigen::MatrixXd GetEigen(double pot) const {return fMat * pot/fPOT;}
  protected:
    // Derived classes can be trusted take care of their own construction
    ReweightableSpectrum(const HistAxis& axisX,
                         const Binning& ybins)
      : fPOT(0), fLivetime(0),
        fAxisX(axisX), fBinsY(ybins)
    {
    }

    // Constructor for user by Uninitialized()
    ReweightableSpectrum()
      : fPOT(0), fLivetime(0), fAxisX(std::vector<std::string>(), std::vector<Binning>()), fBinsY(Binning::Simple(1, 0, 1))
    {
    }

    ReweightableSpectrum& PlusEqualsHelper(const ReweightableSpectrum& rhs, int sign);

    void RemoveLoader(SpectrumLoaderBase*);
    void AddLoader(SpectrumLoaderBase*);

    void _SaveTo(TDirectory* dir,
                 const std::string& name,
                 const std::string& type) const;

    Eigen::MatrixXd fMat;
    double fPOT;
    double fLivetime;

    LabelsAndBins fAxisX;

    Binning fBinsY;

    std::string fTrueLabel;

    /// This count is maintained by SpectrumLoader, as a sanity check
    std::set<SpectrumLoaderBase*> fLoaderCount;
  };
}
