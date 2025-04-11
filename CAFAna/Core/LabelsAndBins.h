#pragma once

#include "CAFAna/Core/Binning.h"
#include <optional>
#include <string>

namespace ana
{
  class LabelsAndBins
  {
  public:
    // The one constructor to rule them all
    LabelsAndBins(const std::vector<std::string>& labels,
                  const std::vector<Binning>& bins);

    // Forwards
    LabelsAndBins(const std::string& label,
                  const Binning& bins)
      : LabelsAndBins(std::vector<std::string>(1, label),
                      std::vector<Binning>(1, bins))
    {
    }

    LabelsAndBins(const std::string& labelX,
                  const Binning& binsX,
                  const std::string& labelY,
                  const Binning& binsY)
      : LabelsAndBins({labelX, labelY}, {binsX, binsY})
    {
    }

    /// Shortcut for simple binnings
    LabelsAndBins(const std::string& label,
                  int nx, double x0, double x1)
      : LabelsAndBins(label, Binning::Simple(nx, x0, x1))
    {
    }

    /// Shortcut for simple binnings
    LabelsAndBins(const std::string& labelX,
                  int nx, double x0, double x1,
                  const std::string& labelY,
                  int ny, double y0, double y1)
      :  LabelsAndBins(labelX, Binning::Simple(nx, x0, x1),
                       labelY, Binning::Simple(ny, y0, y1))
    {
    }

    LabelsAndBins(const LabelsAndBins& ax) = default;

    LabelsAndBins(const std::vector<LabelsAndBins>& axes);

    LabelsAndBins(const LabelsAndBins& xax,
                  const LabelsAndBins& yax)
      : LabelsAndBins(std::vector<LabelsAndBins>{xax, yax})
    {
    }

    LabelsAndBins(const LabelsAndBins& xax,
                  const LabelsAndBins& yax,
                  const LabelsAndBins& zax)
      : LabelsAndBins(std::vector<LabelsAndBins>{xax, yax, zax})
    {
    }

    unsigned int NDimensions() const{return fLabels.size();}

    const std::vector<std::string>& GetLabels() const {return fLabels;}
    const std::vector<Binning>& GetBinnings() const {return fBins;}

    /// Appropriate binning and labelling for that 1D Var
    const Binning& GetBins1D() const;
    const std::string& GetLabel1D() const;

  protected:
    std::vector<std::string> fLabels;
    std::vector<Binning> fBins;

    mutable std::optional<Binning> fBins1D;
    mutable std::optional<std::string> fLabel1D;
  };
}
