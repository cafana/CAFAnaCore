#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/VarApplier.h"
#include "CAFAna/Core/Var.h"
using namespace ana;

#include "TH1.h"
#include "TPad.h"

struct MyRec
{
  double a;
};

const _Var<MyRec> myvar([](const MyRec* rec){return rec->a;});

class MyRecSource: public _IRecordSource<MyRec>
{
public:
  virtual void Register(_IRecordSink<MyRec>*) override
  {
  }
};

void test_sourcesink()
{
  _VarApplier<MyRec> vapp(myvar);

  MyRec rec;
  rec.a = 42;
  vapp.HandleRecord(&rec);

  LabelsAndBins axis("foo", Binning::Simple(100, 0, 100));
  Spectrum spect(vapp, axis);

  _HistAxis<_Var<MyRec>> axis2("bar", Binning::Simple(100, 0, 100), myvar);
  MyRecSource recsrc;
  Spectrum spect2(recsrc, axis2);

  spect.OverridePOT(1);
  spect.ToTH1(1)->Draw("hist");
  gPad->Print("test.pdf");
}
