#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/Valuer.h"
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
  MyRecSource recsrc;

  _Valuer<MyRec> vapp(recsrc, myvar);

  MyRec rec;
  rec.a = 42;
  vapp.HandleRecord(&rec, 1);

  LabelsAndBins axis("foo", Binning::Simple(100, 0, 100));
  Spectrum spect(vapp, axis);

  _HistAxis<_Var<MyRec>> axis2("bar", Binning::Simple(100, 0, 100), myvar);
  Spectrum spect2(recsrc, axis2);

  spect.OverridePOT(1);
  spect.ToTH1(1)->Draw("hist");
  gPad->Print("test.pdf");
}
