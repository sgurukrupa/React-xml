#include "StringUtils.h"

namespace reactxml
{
  namespace StringUtils
  {
    bool Tokenizer::NextToken()
    {
      if (source.empty()) return false;
      auto ix = source.find_first_of(delimiters);
      if (ix == tstring::npos) ix = source.length();
      removedDelim = source.substr(ix, 1);
      token = source.substr(0, ix);
      source.erase(0, ix + 1); // erase the delimiter too, Haribol
      return true;
    }

    const tstring &Tokenizer::GetToken()
    {
      return token;
    }
  }
}

