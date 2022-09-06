#pragma once

#include <memory>
#include <unordered_map>

namespace ana
{
  /// Implementation detail for _IRecordSourceDefaultImpl et al
  template<class KeyT, class ElemT> class IDDict
  {
  public:
    /// Return the element with ID 'id' or construct a new element of type
    /// ConsT with arguments cons_args
    template<class ConsT, class... Ts> ElemT& Get(KeyT key, Ts&&... cons_args)
    {
      auto it = fElems.find(key);

      if(it != fElems.end()) return *it->second;

      return *fElems.emplace(key, new ConsT(cons_args...)).first->second;
    }

  protected:
    std::unordered_map<KeyT, std::unique_ptr<ElemT>> fElems;
  };
}
