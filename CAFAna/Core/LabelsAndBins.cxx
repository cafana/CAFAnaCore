#include "CAFAna/Core/LabelsAndBins.h"

#include <cassert>

namespace ana
{
  //----------------------------------------------------------------------
  LabelsAndBins::LabelsAndBins(const std::vector<std::string>& labels,
                               const std::vector<Binning>& bins)
    : fLabels(labels), fBins(bins)
  {
    assert(fLabels.size() == fBins.size());
  }

  //----------------------------------------------------------------------
  LabelsAndBins::LabelsAndBins(const std::vector<LabelsAndBins>& axes)
  {
    for(const auto& a: axes){
      fLabels.insert(fLabels.end(), a.fLabels.begin(), a.fLabels.end());
      fBins.insert(fBins.end(), a.fBins.begin(), a.fBins.end());
    }

    assert(fLabels.size() == fBins.size());
  }

  //----------------------------------------------------------------------
  const Binning& LabelsAndBins::GetBins1D() const
  {
    if(fBins.size() == 1) return fBins[0];

    if(fBins1D) return *fBins1D;

    assert(!fBins.empty());

    int n = 1;
    for(const Binning& b: fBins) n *= b.NBins();
    fBins1D = Binning::Simple(n, 0, n);

    return *fBins1D;
  }

  //----------------------------------------------------------------------
  const std::string& LabelsAndBins::GetLabel1D() const
  {
    if(fLabels.size() == 1) return fLabels[0];

    if(fLabel1D) return *fLabel1D;

    fLabel1D = "";
    for(const std::string& l: fLabels) *fLabel1D += l + " and ";
    fLabel1D->resize(fLabel1D->size()-5); // drop extra "and"

    return *fLabel1D;
  }

}
