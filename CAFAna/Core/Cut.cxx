#include "CAFAna/Core/Cut.h"

#include "CAFAna/Core/DepMan.h"

#include <iostream>
#include <map>

namespace ana
{
  //----------------------------------------------------------------------
  template<class T> _Cut<T>::
  _Cut(const std::function<CutFunc_t>& func,
       const std::function<ExposureFunc_t>& liveFunc,
       const std::function<ExposureFunc_t>& potFunc)
    : fFunc(func), fLiveFunc(liveFunc), fPOTFunc(potFunc),
      fID(fgNextID++)
  {
    DepMan<_Cut<T>>::Instance().RegisterConstruction(this);
  }

  //----------------------------------------------------------------------
  template<class T> _Cut<T>::
  _Cut(const std::function<CutFunc_t>& fun,
       const std::function<ExposureFunc_t>& liveFunc,
       const std::function<ExposureFunc_t>& potFunc,
       int id)
    : fFunc(fun), fLiveFunc(liveFunc), fPOTFunc(potFunc), fID(id)
  {
    DepMan<_Cut<T>>::Instance().RegisterConstruction(this);
  }

  //----------------------------------------------------------------------
  template<class T> _Cut<T>::~_Cut()
  {
    DepMan<_Cut<T>>::Instance().RegisterDestruction(this);
  }

  //----------------------------------------------------------------------
  template<class T> _Cut<T>::_Cut(const _Cut<T>& c)
    : fFunc(0), fLiveFunc(0), fPOTFunc(0), fID(-1)
  {
    if(&c == this) return;

    if(c.fFunc){
      fFunc = c.fFunc;
      fLiveFunc = c.fLiveFunc;
      fPOTFunc = c.fPOTFunc;
      fID = c.fID;

      DepMan<_Cut<T>>::Instance().RegisterConstruction(this);
    }
    else{
      // If we are copying from a Cut with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<_Cut<T>>::Instance().RegisterDependency(&c, this);
    }
  }

  //----------------------------------------------------------------------
  template<class T> _Cut<T>& _Cut<T>::
  operator=(const _Cut<T>& c)
  {
    if(&c == this) return *this;

    if(c.fFunc){
      fFunc = c.fFunc;
      fLiveFunc = c.fLiveFunc;
      fPOTFunc = c.fPOTFunc;
      fID = c.fID;

      DepMan<_Cut<T>>::Instance().RegisterConstruction(this);
    }
    else{
      fFunc = 0;
      fLiveFunc = 0;
      fPOTFunc = 0;
      fID = -1;

      // If we are copying from a Cut with NULL func, that is probably because
      // it is all zero because it hasn't been statically constructed
      // yet. Register our interest of getting constructed in turn once it is.
      DepMan<_Cut<T>>::Instance().RegisterDependency(&c, this);
    }

    return *this;
  }

  // Make sure all three versions get generated
  template class _Cut<caf::SRProxy>;
  template class _Cut<caf::SRSpillProxy>;
  template class _Cut<caf::SRNeutrinoProxy>;

  template<class T> int _Cut<T>::fgNextID = 0;

  //----------------------------------------------------------------------
  std::function<ExposureFunc_t>
  CombineExposures(const std::function<ExposureFunc_t>& a,
                   const std::function<ExposureFunc_t>& b)
  {
    if(!a && !b) return 0;
    if(!a) return b;
    if(!b) return a;

    struct ExposureCombiner
    {
      std::function<ExposureFunc_t> a, b;
      double operator()(const caf::SRSpillProxy* spill)
      {
        const double va = a(spill);
        const double vb = b(spill);

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

  //----------------------------------------------------------------------
  template<class T> _Cut<T> _Cut<T>::
  operator&&(const _Cut<T>& c) const
  {
    // The same pairs of cuts are frequently and-ed together. Make sure those
    // duplicates get the same IDs by remembering what we've done in the past.
    static std::map<std::pair<int, int>, int> ids;
    const std::pair<int, int> key(ID(), c.ID());

    struct And
    {
      const _Cut<T> a, b;
      bool operator()(const T* sr) const {return a(sr) && b(sr);}
    };

    if(ids.count(key) == 0) ids[key] = fgNextID++;
    return _Cut<T>(And{*this, c},
                   CombineExposures(fLiveFunc, c.fLiveFunc),
                   CombineExposures(fPOTFunc, c.fPOTFunc),
                   ids[key]);
  }

  //----------------------------------------------------------------------
  template<class T> _Cut<T> _Cut<T>::
  operator||(const _Cut<T>& c) const
  {
    static std::map<std::pair<int, int>, int> ids;
    const std::pair<int, int> key(ID(), c.ID());

    struct Or
    {
      const _Cut<T> a, b;
      bool operator()(const T* sr) const {return a(sr) || b(sr);}
    };

    if(ids.count(key) == 0) ids[key] = fgNextID++;
    return _Cut<T>(Or{*this, c},
                   CombineExposures(fLiveFunc, c.fLiveFunc),
                   CombineExposures(fPOTFunc, c.fPOTFunc),
                   ids[key]);
  }

  //----------------------------------------------------------------------
  template<class T> _Cut<T> _Cut<T>::operator!() const
  {
    static std::map<int, int> ids;

    struct Not
    {
      const _Cut<T> a;
      bool operator()(const T* sr) const {return !a(sr);}
    };

    if(ids.count(ID()) == 0) ids[ID()] = fgNextID++;
    return _Cut<T>(Not{*this}, 0, 0, ids[ID()]);
  }

  //----------------------------------------------------------------------
  template<class T> _Cut<T>::operator bool() const
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
