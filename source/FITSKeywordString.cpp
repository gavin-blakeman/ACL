//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								CFITSKeywordString
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

#include "../include/FITSKeywordString.h"

  // ACL Library

#include "../include/FITSStrings.h"

  // Standard Library

#include <string>
#include <utility>

namespace ACL
{

  /// @brief Constructor
  /// @param[in] keyword - The keyword name
  /// @param[in] value - The keyword value
  /// @throws None
  /// @version 2016-04-16/GGB - Function created. (Bug# 1571169)

  CFITSKeywordString::CFITSKeywordString(std::string const &keyword, std::string const &value) : CFITSKeyword(keyword),
    value_(std::move(value))
  {
    stripQuotes();
  }

  /// @brief Constructor
  /// @param[in] keyword - The keyword name
  /// @param[in] value - The keyword value
  /// @param[in] comment - The comment for the keyword
  /// @throws None
  /// @version 2016-04-16/GGB - Function created. (Bug# 1571169)

  CFITSKeywordString::CFITSKeywordString(std::string const &keyword, std::string const &value, std::string const &comment):
    CFITSKeyword(keyword, comment), value_(std::move(value))
  {
    stripQuotes();
  }

  /// @brief Copy constructor.
  /// @param[in] toCopy - The instance to copy.
  /// @throws None.
  /// @version 2016-04-09/GGB - Function created.

  CFITSKeywordString::CFITSKeywordString(CFITSKeywordString const &toCopy) : CFITSKeyword(toCopy.keyword_, toCopy.comment_)
  {
    (*this) = toCopy;
  }

  /// @brief Copy operator
  /// @param[in] rhs - Instance to copy.
  /// @version 2016-04-09/GGB - Function created.

  CFITSKeywordString &CFITSKeywordString::operator=(CFITSKeywordString  const &rhs)
  {
    keyword_ = rhs.keyword();
    value_ = rhs.value_;
    comment_ = rhs.comment();
    return (*this);
  }

  /// @brief Assignment operator to assign a string value.
  /// @param[in] rhs - The string to assign.
  /// @returns Reference to (*this) object
  /// @throws None.
  /// @note Single quotes are silently stripped off.
  /// @version 2016-04-17/GGB - Function created.

  CFITSKeyword &CFITSKeywordString::operator=(std::string rhs)
  {
    value_ = std::move(rhs);

    stripQuotes();

    return (*this);
  }

  /// @brief Returns the value of the keyword.
  /// @returns The string value of the keyword.
  /// @throws None.
  /// @version 2016-04-11/GGB - Function created.

  CFITSKeywordString::operator std::string() const
  {
    return value_;
  }

  /// @brief Creates a copy of this object.
  /// @returns Pointer to the copy.
  /// @throws std::bad_alloc
  /// @version 2018-09-22/GGB - Updated to use std::unique_ptr.
  /// @version 2011-07-18/GGB - Function created.

  std::unique_ptr<CFITSKeyword> CFITSKeywordString::createCopy() const
  {
    return std::make_unique<CFITSKeywordString>(*this);
  }

  /// @brief Strips the single quote character from the front and back of the string if found.
  /// @note The back is only searched if the front is found. This is to allow a situation where the string is actually something
  ///         like "Gavins'" Unlikely I know, but allow for the unexpected.
  /// @throws None.
  /// @pre value_ should contain a valid string.
  /// @version 2016-04-17/GGB - Function created,

  void CFITSKeywordString::stripQuotes()
  {
    if (!value_.empty())
    {
      if (value_.front() == FITS_STRINGQUOTE)
      {
        value_.erase(0, 1);

        if (value_.back() == FITS_STRINGQUOTE)
        {
          value_.pop_back();
        }
      };
    };
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  KWType CFITSKeywordString::type() const
  {
    return KWT_STRING;
  }

  /// @brief Write the keyword to the FITS file.
  /// @param[in] file - The FITS file pointer.
  /// @throws CFITSException
  /// @note cfitsio adds the single quote character when saving the string. There is no need to add the single quote characters in
  /// this function.
  /// @note If the single quote characters are added in this function, then there will be two pairs of single quote characters.
  /// @version 2016-04-09/GGB - Function created.

  void CFITSKeywordString::writeToFITS(fitsfile *file) const
  {
    char *szValue = const_cast<char *>(value_.c_str());

    CFITSIO_TEST(fits_update_key, file, TSTRING, keyword_.c_str(), szValue, comment_.c_str());
  }
}
