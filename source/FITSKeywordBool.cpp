//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeywordByte
// SUBSYSTEM:						Templated functions for keywords
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2016-2018 Gavin Blakeman.
//                      This file is part of the Astronomy Class Library (ACL)
//
//                      ACL is free software: you can redistribute it and/or modify it under the terms of the GNU General
//                      Public License as published by the Free Software Foundation, either version 2 of the License, or
//                      (at your option) any later version.
//
//                      ACL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
//                      implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//                      for more details.
//
//                      You should have received a copy of the GNU General Public License along with ACL.  If not,
//                      see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:            Classes to support the keywords in a FITS file. The class heirarchy enables the keywords to be stored in the
//                      STL container classes using a virtual base class.
//
// CLASSES INCLUDED:		CFITSKeywordByte  - Handle 8 bit unsigned integers.
//
// CLASS HIERARCHY:     CFITSKeyword
//                        CFITSKeywordBool
//                        CFITSKeywordByte
//
// HISTORY:             2016-04-10 - Classes rewritten and moved into seperate files.
//
//**********************************************************************************************************************************

#include "include/FITSKeywordBool.h"

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
  /// @version 2018-09-22/GGB - Updated to use std::unique_ptr.
  /// @version 2016-04-11/GGB - Function created.

  std::unique_ptr<CFITSKeyword> CFITSKeywordBool::createCopy() const
  {
    return std::make_unique<CFITSKeywordBool>(*this);
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
    int iValue = static_cast<int>(value_);

    CFITSIO_TEST(fits_update_key, file, TLOGICAL, keyword_.c_str(), &iValue, comment_.c_str());
  }

}   // namespace ACL
