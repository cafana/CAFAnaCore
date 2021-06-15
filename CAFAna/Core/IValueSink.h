namespace ana
{
  class IValueSink
  {
  public:
    virtual ~IValueSink() {}

    virtual void Fill(double x, double w = 1) = 0;
    virtual void FillPOT(double pot) = 0;
    virtual void FillLivetime(double livetime) = 0;
  };
}
