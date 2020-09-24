#include "CAFAna/Core/DepMan.h"

#include "CAFAna/Core/UtilsExt.h" // DemangledTypeName()

#include <cassert>
#include <iostream>

//#include <string.h> // memcpy

// The idea is to keep a tree of what objects are copy-constructed from which
// others. Any object whose source is not constructed yet should skip
// construction, and this DependencyManager will later fill them in once the
// source object becomes available. Since the dependencies form a tree, and
// ultimately all objects should be constructed, we expect to be able to fulfil
// all dependencies before user code starts running.
//
// The one tricky spot is when B is deleted from A <- B <- C before C has been
// constructed. We simplify the graph to A <- C and wait.

namespace
{
  /// STL semantics for erase are annoying, make an infallible one
  template<class T, class U> void maybe_erase(T& collection, const U& key)
  {
    auto it = collection.find(key);
    if(it != collection.end()) collection.erase(it);
  }

  // Likewise
  template<class T, class U> U*
  maybe_find(std::unordered_map<const T*, U>& col, const T* key)
  {
    auto it = col.find(key);
    if(it == col.end()) return 0;
    return &it->second;
  }
}

namespace ana
{
  // --------------------------------------------------------------------------
  template<class T> DepMan<T>& DepMan<T>::Instance()
  {
    // Avoid static order problems for the manager itself
    static DepMan<T> dm;
    return dm;
  }

  // --------------------------------------------------------------------------
  template<class T> void DepMan<T>::Print()
  {
    std::cout << "DepMan for "<< DemangledTypeName<T>() << " :\n"
      << "  assisted construction of " << fNOps << " objects. "
      << fNodes.size() << " still awaiting construction." << std::endl;
  }

  // --------------------------------------------------------------------------
  template<class T> void DepMan<T>::RegisterConstruction(T* x)
  {
    if(fDisabled) return;

    ParentKids* xnode = maybe_find(fNodes, x);
    if(!xnode) return; // No record of us, nothing to do

    std::unordered_set<T*> kids = xnode->kids;

    // We have been constructed, so parent doesn't need to remember us
    // anymore
    ParentKids* pnode = maybe_find(fNodes, xnode->parent);
    if(pnode) maybe_erase(pnode->kids, x);
    // Nor do we need to remember anything about this node anymore
    maybe_erase(fNodes, x);

    // Assign into all the kids that were depending on us. They in turn will
    // register their construction and so recurse into any grandkids.
    for(T* kid: kids) *kid = *x;

    fNOps += kids.size();
  }

  // --------------------------------------------------------------------------
  template<class T> void DepMan<T>::RegisterDestruction(T* x)
  {
    if(fDisabled) return;

    ParentKids* xnode = maybe_find(fNodes, x);
    if(!xnode) return; // No record of us, nothing to do

    // Make the parent forget about us
    ParentKids* pnode = maybe_find(fNodes, xnode->parent);
    if(pnode) pnode->kids.erase(x);

    // Kids need to forget about us, but get hooked up to their grandparent
    // instead.
    for(T* kid: xnode->kids){
      ParentKids* knode = maybe_find(fNodes, kid);

      if(knode) knode->parent = xnode->parent;
      if(pnode) pnode->kids.insert(kid);
    }

    fNodes.erase(x); // And finally forget this node in particular
  }

  // --------------------------------------------------------------------------
  template<class T> void DepMan<T>::
  RegisterDependency(const T* parent, T* child)
  {
    if(fDisabled) return;

    fNodes[child].parent = parent;
    fNodes[parent].kids.insert(child);
  }

  // --------------------------------------------------------------------------
  template<class T> void DepMan<T>::Disable()
  {
    fDisabled = true;
  }

  // --------------------------------------------------------------------------
  template<class T> void DepMan<T>::Enable()
  {
    fDisabled = false;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// We have to instantiate all the variants we expect

#include "CAFAna/Core/Cut.h"
#include "CAFAna/Core/Var.h"
#include "CAFAna/Core/Binning.h"

namespace ana
{
  template class DepMan<Cut>;
  template class DepMan<SpillCut>;
  template class DepMan<NuTruthCut>;

  template class DepMan<Var>;
  template class DepMan<SpillVar>;
  template class DepMan<NuTruthVar>;

  template class DepMan<Binning>;
}
