#ifndef ACL_FITSKEYWORDDATE_H
#define ACL_FITSKEYWORDDATE_H

#include "FITSKeyword.h"

namespace ACL
{
  class CFITSKeywordDate : public CFITSKeyword
  {
  private:
    CFITSKeywordDate() = delete;

  protected:
  public:
    CFITSKeywordDate(CFITSKeywordDate const &);
  };
}

#endif // ACL_FITSKEYWORDDATE_H
