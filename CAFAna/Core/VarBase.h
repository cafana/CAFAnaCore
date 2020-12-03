#pragma once

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/CutBase.h"

#include <functional>

namespace ana
{
  /// None of the implementation of _Var actually depends on the record
  /// type. This base class implements all that basic functionality, and then
  /// _Var just adds a typed wrapper on top.
  class VarBase
  {
  public:
    typedef double (VoidVarFunc_t)(const void* rec);

    friend class DepMan<VarBase>;
    friend class Var2DFunc;
    friend class Var3DFunc;

    friend double ValHelper(const VarBase&, const std::string&, double c, const void*);
    friend void ValHelper(const VarBase&, const VarBase&, const std::string&, const void*, double&, double&);

    ~VarBase();

  protected:
    VarBase(const std::function<VoidVarFunc_t>& func, int id = -1);

    VarBase(const VarBase& v);

    VarBase(const VarBase& a, const Binning& binsa,
            const VarBase& b, const Binning& binsb);

    VarBase(const VarBase& a, const Binning& binsa,
            const VarBase& b, const Binning& binsb,
            const VarBase& c, const Binning& binsc);

    VarBase& operator=(const VarBase& v);

    /// Allows a variable to be called with double value = myVar(rec) syntax
    double operator()(const void* rec) const
    {
      return fFunc(rec);
    }

    /// Vars with the same definition will have the same ID
    int ID() const {return fID;}

    static int MaxID() {return fgNextID-1;}

    CutBase operator<(double c) const;
    CutBase operator>(double c) const;
    CutBase operator>=(double c) const;
    CutBase operator<=(double c) const;
    CutBase operator==(double c) const;
    CutBase operator!=(double c) const;

    CutBase operator>(const VarBase& v) const;
    CutBase operator<(const VarBase& v) const;
    CutBase operator>=(const VarBase& v) const;
    CutBase operator<=(const VarBase& v) const;
    CutBase operator==(const VarBase& v) const;
    CutBase operator!=(const VarBase& v) const;

    // Most useful for combining weights.
    VarBase operator*(const VarBase& v) const;
    VarBase operator/(const VarBase& v) const;
    VarBase operator+(const VarBase& v) const;
    VarBase operator-(const VarBase& v) const;

    std::function<VoidVarFunc_t> fFunc;

    int fID;
    /// The next ID that hasn't yet been assigned
    static int fgNextID;
  };

  class Var2DMapper
  {
  public:
    Var2DMapper(const Binning& binsa, const Binning& binsb);

    double Map(double va, double vb) const;

  protected:
    const Binning fBinsA, fBinsB;
  };

  class Var3DMapper
  {
  public:
    Var3DMapper(const Binning& binsa, const Binning& binsb, const Binning& binsc);
    double Map(double va, double vb, double vc) const;

  protected:
    const Binning fBinsA, fBinsB, fBinsC;
  };

} // namespace
