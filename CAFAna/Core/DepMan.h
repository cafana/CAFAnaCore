#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ana
{
  /// \brief Deep magic to fix static initialization order. Here Be Dragons!
  ///
  /// Intended for internal use by Cut and Var only.
  template<class T> class DepMan
  {
  public:
    static DepMan& Instance();

    /// Call from constructor (in the success case)
    void RegisterConstruction(T* x);
    /// Call from destructor
    void RegisterDestruction(T* x);
    /// Call from copy constructor and assignment operator
    void RegisterDependency(const T* parent, T* child);

    /// Prints some stats
    void Print();

    void Disable();
    void Enable();
  protected:
    DepMan() : fDisabled(false), fNOps(0) {}

    struct ParentKids{const T* parent; std::unordered_set<T*> kids;};
    std::unordered_map<const T*, ParentKids> fNodes;

    bool fDisabled;

    int fNOps;
  };
}
