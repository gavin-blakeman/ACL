//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Keyword
// SUBSYSTEM:						Templated functions for keywords
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost, SCL
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2015 Gavin Blakeman.
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
// CLASSES INCLUDED:		CKeyword  - Virtual base class
//                      TKeyword  - Templated keyword class.
//
// CLASS HIERARCHY:     SCL::CPackage
//                        - CKeyword
//                          - TKeyword
//
// HISTORY:             2013-09-30 GGB - AIRDAS 2013.09 release.
//                      2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2011-06-04 GGB - Development of classes for AIRDAS
//
//**********************************************************************************************************************************

#include "../Include/Keyword.hpp"

  // ACL Library

#include "../Include/common.h"

  // Boost library

#include <boost/algorithm/string.hpp>

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CKeyword
  //
  //*******************************************************************************************************************************

  /// @brief Constructor for the class.
  /// @param[in] keyword - The keyword name.
  /// @note The keyword name is converted to upper case and stored as uppercase.
  /// @version 2011-12-11/GGB - Moved from being an inline function with no body

  CKeyword::CKeyword(std::string const &keyword) : keyword_(keyword), comment_(std::string(""))
  {
    boost::algorithm::to_upper(keyword_);
  }

  /// @brief Constructor for the class.
  /// @param[in] keyword - The keyword name.
  /// @param[in] comment - The comment associated with the keyword.
  /// @note The keyword name is converted to upper case and stored as uppercase.
  /// @version 2011-12-11/GGB - Moved from being an inline function with no body

  CKeyword::CKeyword(std::string const &keyword, std::string const &comment) : keyword_(keyword), comment_(comment)
  {
    boost::algorithm::to_upper(keyword_);
  }

  /// @brief Tests for equality between two keywords. The test is case-insensitive.
  /// @details The test keyword is converted to upper case before the test is completed.
  /// @param[in] toTest - The string to compare to the keyword.
  /// @returns true - The keyword is equal to the test keyword.
  /// @version 2011-12-11/GGB - Function created.

  bool CKeyword::operator==(std::string const &toTest) const
  {
    std::string test = toTest;

    boost::algorithm::to_upper(test);

    return (test == keyword_);
  }

  //*******************************************************************************************************************************
  //
  // TKeyword
  // Templated class
  //
  //*******************************************************************************************************************************

  template<>
  TKeyword<int>::operator int() const
  {
    return data_;
  }

  template<>
  TKeyword<double>::operator int() const
  {
    return static_cast<int>(data_);
  }

  template<>
  TKeyword<float>::operator int() const
  {
    return static_cast<int>(data_);
  }

  template<>
  TKeyword<long>::operator int() const
  {
    return static_cast<int>(data_);
  }

  template<>
  TKeyword<bool>::operator int() const
  {
    return static_cast<int>(data_);
  }

  /// @brief Cast operator to convert to a double value.
  /// @returns The double representation of the value.
  /// @version 2015-08-23/GGB - Function created.

  template<>
  TKeyword<unsigned int>::operator double() const
  {
    return static_cast<double>(data_);
  }

  template<>
  KWType TKeyword<float>::type() const
  {
    return KWT_FLOAT;
  }

  template<>
  KWType TKeyword<double>::type() const
  {
    return KWT_DOUBLE;
  }

  template<>
  KWType TKeyword<bool>::type() const
  {
    return KWT_BOOL;
  }

  template<>
  KWType TKeyword<std::string>::type() const
  {
    return KWT_STRING;
  }

  template<>
  KWType TKeyword<long>::type() const
  {
    return KWT_LONG;
  }

  template<>
  KWType TKeyword<int>::type() const
  {
    return KWT_INT;
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  template<>
  KWType TKeyword<unsigned int>::type() const
  {
    return KWT_UINT;
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  template<>
  KWType TKeyword<short>::type() const
  {
    return KWT_SHORT;
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  template<>
  KWType TKeyword<std::uint8_t>::type() const
  {
    return KWT_UINT8;
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  template<>
  KWType TKeyword<std::int8_t>::type() const
  {
    return KWT_INT8;
  }

  /// @brief Returns the type of the data stored in this instance.
  /// @returns The type of keyword stored.
  /// @throws None.
  /// @version 2015-08-23/GGB - Function created.

  template<>
  KWType TKeyword<std::uint16_t>::type() const
  {
    return KWT_UINT16;
  }

}  // namespace
