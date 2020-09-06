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
//                      Copyright 2011-2020 Gavin Blakeman.
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
// CLASS HIERARCHY:     CFITSKeyword
//
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2011-06-04 GGB - Development of classes for astroManager
//
//**********************************************************************************************************************************

#include "../include/FITSKeyword.h"

  // Standard Library

  // ACL Library

#include "../include/common.h"

  // Miscellaneous libraries

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

  CFITSKeyword::CFITSKeyword(std::string const &nk) : keyword_(nk), comment_()
  {
    boost::algorithm::to_upper(keyword_);
  }

  /// @brief Constructor for the class.
  /// @param[in] keyword - The keyword name.
  /// @param[in] comment - The comment associated with the keyword.
  /// @note The keyword name is converted to upper case and stored as uppercase.
  /// @version 2011-12-11/GGB - Moved from being an inline function with no body

  CFITSKeyword::CFITSKeyword(std::string const &nk, std::string const &nc) : keyword_(nk), comment_(nc)
  {
    boost::algorithm::to_upper(keyword_);
  }

  /// @brief Operator for ==
  /// @param[in] test - The object to test against.
  /// @throws None.
  /// @version 2016-04-11/GGB - Function created.
  ///
  bool CFITSKeyword::operator==(CFITSKeyword const &test) const
  {
    return (test.keyword_ == keyword_);
  }

  /// @brief Tests for equality between two keywords. The test is case-insensitive.
  /// @details The test keyword is converted to upper case before the test is completed.
  /// @param[in] toTest - The string to compare to the keyword.
  /// @returns true - The keyword is equal to the test keyword.
  /// @version 2011-12-11/GGB - Function created.

  bool CFITSKeyword::operator==(std::string const &toTest) const
  {
    std::string test = toTest;

    boost::algorithm::to_upper(test);

    return (test == keyword_);
  }

}  // namespace
