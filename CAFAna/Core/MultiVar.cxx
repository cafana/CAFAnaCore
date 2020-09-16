#include "CAFAna/Core/MultiVar.h"

#include "CAFAna/Core/Var.h"

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


  class MultiVar2DFunc
  {
  public:
    MultiVar2DFunc(const MultiVar& a, const Binning binsa,
		   const MultiVar& b, const Binning binsb)
      : fA(a), fBinsA(binsa),
	fB(b), fBinsB(binsb)
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
      for(int h = 0; h < vector_size; h++){
        if(va[h] <  fBinsA.Min() || vb[h] <  fBinsB.Min())
          {res1.push_back(-1);}
        if(va[h] >  fBinsA.Max() || vb[h] >  fBinsB.Max()) 
          {res1.push_back(fBinsA.NBins()*fBinsB.NBins());}  
        // FindBin uses root convention, first bin is bin 1, bin 0 is underflow
        const int ia = fBinsA.FindBin(va[h]) - 1;
        const int ib = fBinsB.FindBin(vb[h]) - 1;  
        const int i = ia*fBinsB.NBins()+ib;
        res1.push_back(i+.5);
      }
      return res1;
    }
    
  protected:
    const MultiVar fA;
    const Binning fBinsA;
    const MultiVar fB;
    const Binning fBinsB;
  };
  
  MultiVar
  MultiVar2D(const MultiVar& a, const Binning& binsa,
	     const MultiVar& b, const Binning& binsb)
  {
    return MultiVar(MultiVar2DFunc(a, binsa, b, binsb));
  }
  
  //----------------------------------------------------------------------
  MultiVar
  MultiVar2D(const MultiVar& a, int na, double a0, double a1,
	     const MultiVar& b, int nb, double b0, double b1)
  {
    return MultiVar2D(a, Binning::Simple(na, a0, a1),
		      b, Binning::Simple(nb, b0, b1));
  } 
}


