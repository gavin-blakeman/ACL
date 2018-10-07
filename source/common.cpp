//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								common
// SUBSYSTEM:						Common definitions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2013-2018 Gavin Blakeman.
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
//
// OVERVIEW:						Common definitions that can be used by multiple files
//
//
// CLASSES INCLUDED:		None
//
// CLASS HIERARCHY:     None.
//
// FUNCTIONS INCLUDED:  std::string getVersionString()
//
// HISTORY:             2018-09-13 GGB - Set version to 2018.09.0000
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-29 GGB - File created.
//
//*********************************************************************************************************************************

#include "../include/common.h"

  // Standard libraries

#include <iomanip>
#include <sstream>
#include <string>

  // GCL Library

#include <GCL>

  // Boost Library

#include <boost/algorithm/string.hpp>

namespace ACL
{

  unsigned int const ACL_MAJORVERSION = 2018;           ///< The Major version number (yyyy)
  unsigned int const ACL_MINORVERSION = 9;              ///< The Minor version number (mm)
  unsigned int const ACL_BUILDNUMBER  = 0x007D;         ///< The build number - incremented on each build.

  /// @brief Returns the library version and build number.
  /// that was used for editing the library.
  /// @returns A string with the application name, version, build.
  /// @throws Nothing.
  /// @version 2015-06-24/GGB - Converted to C++. Removed all C function calls.
  /// @version 2013-06-29/GGB - Function created.

  std::string getVersionString()
  {
    std::stringstream s;

    s << ACL_MAJORVERSION << "." << std::setfill('0') << std::setw(2) << ACL_MINORVERSION << std::setw(4) << std::hex << ACL_BUILDNUMBER << std::endl;

    std::string returnValue = s.str();
    boost::algorithm::to_upper(returnValue);

    return returnValue;
  }

}
