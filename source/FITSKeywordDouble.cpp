//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeywordDouble
// SUBSYSTEM:						Classes for keywords
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
// CLASSES INCLUDED:		CFITSKeywordSByte  - Handle 8 bit signed integers.
//
// CLASS HIERARCHY:     CFITSKeyword
//                        CFITSKeywordBool
//                        CFITSKeywordComplex
//                        CFITSKeywordDate
//                        CFITSKeywordDateTime
//                        CFITSKeywordDouble
//                        CFITSKeywordDoubleComplex
//                        CFITSKeywordFloat
//                        CFITSKeywordInt08
//                        CFITSKeywordInt16
//                        CFITSKeywordInt32
//                        CFITSKeywordInt64
//                        CFITSKeywordString
//                        CFITSKeywordUInt08
//                        CFITSKeywordUInt16
//                        CFITSKeywordUInt32
//
// HISTORY:             2016-04-10 - Classes rewritten and moved into seperate files.
//
//**********************************************************************************************************************************

#include "../include/FITSKeywordDouble.h"

  // Standard Library

#include <exception>
#include <limits>
#include <string>

namespace ACL
{
  /// @brief Copy constructor.
  /// @param[in] toCopy - The instance to copy.
  /// @throws None.
  /// @version 2016-04-09/GGB - Function created.

  CFITSKeywordDouble::CFITSKeywordDouble(CFITSKeywordDouble const &toCopy) : CFITSKeyword(toCopy.keyword_, toCopy.comment_),
    value_(toCopy.value_)
  {
  }

  /// @brief Copy operator
  /// @param[in] rhs - Instance to copy.
  /// @version 2016-04-09/GGB - Function created.

  CFITSKeyword &CFITSKeywordDouble::operator=(CFITSKeywordDouble  const &rhs)
  {
    keyword_ = rhs.keyword();
    value_ = dynamic_cast<CFITSKeywordDouble const *>(&rhs)->value_;
    comment_ = rhs.comment();
    return (*this);
  }

  /// @brief Typecast to a int16 type.
  /// @throws std::domain_error
  /// @version 2017-08-03/GGB - Function created.

  CFITSKeywordDouble::operator std::int16_t() const
  {
    if ( (value_ < std::numeric_limits<std::int16_t>::min()) ||
         (value_ > std::numeric_limits<std::int16_t>::max()))
    {
      throw std::domain_error("Value out of range during static_cast<>() operation.");
    }
    else
    {
      return static_cast<std::int16_t>(value_);
    }
  }

  /// @brief Returns the value of the keyword.
  /// @returns The value as a double.
  /// @throws None.
  /// @version 2016-04-11/GGB - Function created.

  CFITSKeywordDouble::operator double() const
  {
    return value_;
  }

  /// @brief Typecast operator to return the value of the keyword.
  /// @returns The value as a float.
  /// @throws None.
  /// @version 2016-04-14/GGB - Function created.

  CFITSKeywordDouble::operator float() const
  {
    return (static_cast<float>(value_));
  }

  /// @brief Returns the value of the keyword as a string. (No formatting)
  /// @returns The keyword value as a string.
  /// @throws None.
  /// @version 2016-04-11/GGB - Function created.

  CFITSKeywordDouble::operator std::string() const
  {
    return (std::to_string(value_));
  }


  /// @brief Creates a copy of this object.
  /// @returns Pointer to the copy.
  /// @throws std::bad_alloc
  /// @version 2018-09-22/GGB - Updated to use std::unique_ptr.
  /// @version 2011-07-18/GGB - Function created.

  std::unique_ptr<CFITSKeyword> CFITSKeywordDouble::createCopy() const
  {
    return std::make_unique<CFITSKeywordDouble>(*this);
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  KWType CFITSKeywordDouble::type() const
  {
    return KWT_DOUBLE;
  }

  /// @brief Write the keyword to the FITS file.
  /// @param[in] file - The FITS file pointer.
  /// @throws CFITSException
  /// @version 2016-04-09/GGB - Function created.

  void CFITSKeywordDouble::writeToFITS(fitsfile *file) const
  {
    CFITSIO_TEST(fits_update_key, file, TDOUBLE, keyword_.c_str(), const_cast<void *>(static_cast<void const *>(&value_)),
                                 comment_.c_str());
  }

}   // namespace ACL
