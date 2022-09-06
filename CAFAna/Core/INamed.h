#pragma once

#include <string>

namespace ana
{
  class INamed
  {
  protected:
    // User shouldn't create these directly. Intended for use as a base class
    INamed(const std::string& shortName, const std::string& latexName)
      : fShortName(shortName), fLatexName(latexName)
    {
    }

  public:
    /// The name printed out to the screen
    const std::string& ShortName() const {return fShortName;}

    /// The name used on plots (ROOT's TLatex syntax)
    const std::string& LatexName() const {return fLatexName;}

  protected:
    std::string fShortName;
    std::string fLatexName;
  };

} // namespace
