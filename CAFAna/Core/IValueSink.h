namespace ana
{
  class IValueSink
  {
  public:
    virtual ~IValueSink() {}

    virtual void Fill(double x, double w = 1) = 0;
  };
}
