#include "CAFAna/Core/CutBase.h"

#include "CAFAna/Core/DepMan.h"

#include <iostream>
#include <map>

namespace ana
{
  int CutBase::fgNextID = 1;

  //----------------------------------------------------------------------
  CutBase::CutBase(const std::function<VoidCutFunc_t>& func,
                   const std::function<VoidExpoFunc_t>& livefunc,
                   const std::function<VoidExpoFunc_t>& potfunc,
                   int id)
    : fFunc(func), fLiveFunc(livefunc), fPOTFunc(potfunc),
      fID((id > 0) ? id : fgNextID++)
  {
    DepMan<CutBase>::Instance().RegisterConstruction(this);
  }

  //----------------------------------------------------------------------
  CutBase::~CutBase()
  {
    DepMan<CutBase>::Instance().RegisterDestruction(this);
  }


  //----------------------------------------------------------------------
  CutBase::CutBase(const CutBase& c)
    : fFunc(0), fLiveFunc(0), fPOTFunc(0), fID(-1)
  {
    if(&c == this) return;

    if(c.fFunc){
      fFunc = c.fFunc;
      fLiveFunc = c.fLiveFunc;
      fPOTFunc = c.fPOTFunc;
      fID = c.fID;

      DepMan<CutBase>::Instance().RegisterConstruction(this);
    }
    else{
      // If we are copying from a Cut with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<CutBase>::Instance().RegisterDependency(&c, this);
    }
  }

  //----------------------------------------------------------------------
  CutBase& CutBase::operator=(const CutBase& c)
  {
    if(&c == this) return *this;

    if(c.fFunc){
      fFunc = c.fFunc;
      fLiveFunc = c.fLiveFunc;
      fPOTFunc = c.fPOTFunc;
      fID = c.fID;

      DepMan<CutBase>::Instance().RegisterConstruction(this);
    }
    else{
      fFunc = 0;
      fLiveFunc = 0;
      fPOTFunc = 0;
      fID = -1;

      // If we are copying from a Cut with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<CutBase>::Instance().RegisterDependency(&c, this);
    }

    return *this;
  }

  //----------------------------------------------------------------------
  std::function<CutBase::VoidExpoFunc_t>
  CutBase::CombineExposures(const std::function<VoidExpoFunc_t>& a,
                            const std::function<VoidExpoFunc_t>& b) const
  {
    if(!a && !b) return 0;
    if(!a) return b;
    if(!b) return a;

    struct ExposureCombiner
    {
      std::function<VoidExpoFunc_t> a, b;
      double operator()(const void* rec)
      {
        const double va = a(rec);
        const double vb = b(rec);

        if(va >= 0 && vb >= 0){
          std::cout << "Inconsistent pot/livetime values of "
                    << va << " and " << vb
                    << " from two cuts being combined." << std::endl;
          abort();
        }

        return std::max(va, vb);
      }
    };

    return ExposureCombiner{a, b};
  }

  namespace{
    // If two Cuts are combined with each other in the same way in two places,
    // we want to give the combination the same ID in both cases. This helps
    // caching in SpectrumLoader
    class IDCache
    {
    public:
      int GetID(int idA, int idB, int& nextID)
      {
        // If either is not fully constructed yet it's not safe to proceed
        if(idA <= 0 || idB <= 0) return nextID++;
        const std::pair<int, int> key(idA, idB);
        auto it = fIDs.find(key);
        if(it != fIDs.end()) return it->second;
        const int newid = nextID++;
        fIDs.emplace(key, newid);
        return newid;
      }

    protected:
      std::map<std::pair<int, int>, int> fIDs;
    };
  }

  //----------------------------------------------------------------------
  CutBase CutBase::operator&&(const CutBase& c) const
  {
    struct And
    {
      const CutBase a, b;
      bool operator()(const void* rec) const {return a(rec) && b(rec);}
    };

    static IDCache cache;
    return CutBase(And{*this, c},
                   CombineExposures(fLiveFunc, c.fLiveFunc),
                   CombineExposures(fPOTFunc, c.fPOTFunc),
                   cache.GetID(ID(), c.ID(), fgNextID));
  }

  //----------------------------------------------------------------------
  CutBase CutBase::operator||(const CutBase& c) const
  {
    struct Or
    {
      const CutBase a, b;
      bool operator()(const void* rec) const {return a(rec) || b(rec);}
    };

    static IDCache cache;
    return CutBase(Or{*this, c},
                   CombineExposures(fLiveFunc, c.fLiveFunc),
                   CombineExposures(fPOTFunc, c.fPOTFunc),
                   cache.GetID(ID(), c.ID(), fgNextID));
  }

  //----------------------------------------------------------------------
  CutBase CutBase::operator!() const
  {
    struct Not
    {
      const CutBase a;
      bool operator()(const void* rec) const {return !a(rec);}
    };

    static IDCache cache;
    return CutBase(Not{*this}, 0, 0, cache.GetID(ID(), ID(), fgNextID));
  }


  //----------------------------------------------------------------------
  CutBase::operator bool() const
  {
    std::cout << "*** A Cut object is being evaluated for truth-iness. "
              << "This very likely means you are using python and attempted "
              << "to use one of the 'and', 'or', or 'not' operators. Python "
              << "does not allow these to be overloaded. You should use the "
              << "corresponding bitwise operators '&', '|', '~' instead."
              << std::endl;
    abort();
  }
} // namespace
