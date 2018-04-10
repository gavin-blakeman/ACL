//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeywordFloat
// SUBSYSTEM:						Templated functions for keywords
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2016-2017 Gavin Blakeman.
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

#include "../Include/FITSKeywordFloat.h"

namespace ACL
{
  /// @brief Copy constructor.
  /// @param[in] toCopy - The instance to copy.
  /// @throws None.
  /// @version 2016-04-09/GGB - Function created.

  CFITSKeywordFloat::CFITSKeywordFloat(CFITSKeywordFloat const &toCopy) : CFITSKeyword(toCopy.keyword_, toCopy.comment_)
  {
    (*this) = toCopy;
  }

  /// @brief Typecast to a int16 type.
  /// @throws std::domain_error
  /// @version 2017-08-03/GGB - Function created.

  CFITSKeywordFloat::operator std::int16_t() const
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

  /// @brief Copy operator
  /// @param[in] rhs - Instance to copy.
  /// @version 2016-04-09/GGB - Function created.

  CFITSKeyword &CFITSKeywordFloat::operator=(CFITSKeywordFloat  const &rhs)
  {
    keyword_ = rhs.keyword();
    value_ = dynamic_cast<CFITSKeywordFloat const *>(&rhs)->value_;
    comment_ = rhs.comment();
    return (*this);
  }

  /// @brief Creates a copy of this object.
  /// @returns Pointer to the copy.
  /// @throws std::bad_alloc
  /// @version 2011-07-18/GGB - FUnction created.

  CFITSKeyword *CFITSKeywordFloat::createCopy() const
  {
    return (new CFITSKeywordFloat(*this));
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  KWType CFITSKeywordFloat::type() const
  {
    return KWT_FLOAT;
  }

  /// @brief Write the keyword to the FITS file.
  /// @param[in] file - The FITS file pointer.
  /// @throws CFITSException
  /// @version 2016-04-09/GGB - Function created.

  void CFITSKeywordFloat::writeToFITS(fitsfile *file) const
  {
    int status = 0;

    CFITSIO_TEST(fits_update_key(file, TFLOAT, keyword_.c_str(), const_cast<void *>(static_cast<void const *>(&value_)),
                                 comment_.c_str(), &status));
  }
}
