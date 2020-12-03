#include "CAFAna/Core/MultiVar.h"

#include "CAFAna/Core/Var.h"

#include "CAFAna/Core/VarBase.h" // for Var[2|3]DMapper

#include <algorithm>
#include <cassert>
#include <map>
#include <set>

namespace ana
{
  // Stupid hack to avoid colliding with the IDs of actual Vars. Just count
  // down through negative numbers.
  template<class T> int _MultiVar<T>::fgNextID = -1;

  // Instantiate
  template class _MultiVar<caf::SRProxy>;

  /// std::function can wrap a real function, function object, or lambda
  template<class T> _MultiVar<T>::
  _MultiVar(const std::function<VarFunc_t>& fun)
    : fFunc(fun), fID(fgNextID--)
  {
  }

  class MultiVar2DFunc : protected Var2DMapper
  {
  public:
    MultiVar2DFunc(const MultiVar& a, const Binning binsa,
		   const MultiVar& b, const Binning binsb)
      : Var2DMapper(binsa, binsb), fA(a), fB(b)
    {
    }
   
    std::vector<double> operator()(const caf::SRProxy* sr) const
    {
      // Calculate current values of the variables in StandardRecord once
      const std::vector<double> va = fA(sr);
      const std::vector<double> vb = fB(sr);
      std::vector<double> res1;
      
      assert(va.size() == vb.size());
      int vector_size = va.size();	  
      for(int h = 0; h < vector_size; h++) res1.push_back(Map(va[h], vb[h]));
      return res1;
    }
    
  protected:
    const MultiVar fA, fB;
  };

  template<class T> _MultiVar<T>::
  _MultiVar(const _MultiVar& va, const Binning& binsa,
            const _MultiVar& vb, const Binning& binsb)
    : fFunc(MultiVar2DFunc(va, binsa, vb, binsb)), fID(fgNextID--)
  {
  }
}


