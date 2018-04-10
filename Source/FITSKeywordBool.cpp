#include "../Include/FITSKeywordBool.h"

namespace ACL
{

  CFITSKeywordBool &CFITSKeywordBool::operator=(const CFITSKeywordBool &rhs)
  {
    keyword_ = rhs.keyword_;
    value_ = rhs.value_;
    comment_ = rhs.comment_;
  }

  /// @brief Operator::std:string
  /// @returns "T" or "F" depending on value of keyword.
  /// @throws None.
  /// @#version 2016-04-11/GGB - Function created.

  CFITSKeywordBool::operator std::string() const
  {
    std::string returnValue = "F";

    if (value_)
    {
      returnValue = "T";
    };

    return returnValue;
  }

  /// @brief Creates a copy of this instance.
  /// @returns Pointer to a copy of this instance.
  /// @throws None.
  /// @version 2016-04-11/GGB - Function created.

  CFITSKeyword *CFITSKeywordBool::createCopy() const
  {
    return (new CFITSKeywordBool(*this));
  }

  /// @brief Returns the type of the instance.
  /// @returns KWT_BOOL
  /// @throws None.
  /// @version 2016-04-12/GGB - Function created.

  KWType CFITSKeywordBool::type() const
  {
    return KWT_BOOL;
  }

  /// @brief Writes the value to the FITS file.
  /// @param[in] file - Pointer to the FITS file.
  /// @throws CFITSError
  /// @version 2016-04-16/GGB - Function created.

  void CFITSKeywordBool::writeToFITS(fitsfile *file) const
  {
    int status = 0;
    int iValue = static_cast<int>(value_);

    CFITSIO_TEST(fits_update_key(file, TLOGICAL, keyword_.c_str(), &iValue, comment_.c_str(), &status));
  }

}   // namespace ACL
