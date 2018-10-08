//*********************************************************************************************************************************
//
// PROJECT:			        Astronomy Class Library
// FILE:				        AstronomicalObject
// SUBSYSTEM:		        Astronomical objects
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	GCL, MCL, PCL
// AUTHOR:              Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2005-2018 Gavin Blakeman.
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
// OVERVIEW:            These classes provide the helper classes for several categories of astronomical calculations
//
// CLASSES INCLUDED:    CTargetAstronomy  - Astronomy target (Object)
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetSolar
//
// RELEASE HISTORY:     2016-03-24 GGB - Simplified class interface
//                      2015-09-27/GGB - Coordinate classes moved into this file from AstroClass
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2010-05-05 GGB -  1) All windows related functions have been removed as part if the change over to the Qt
//                                          API platform. This has been done to facilitate the move to a multi-platform environment.
//                                        2) The intention with the changes are also to make this software platform independant to
//                                          enable it's use as a general library with any system.
//                                        3) Removed class CAstronomicalCalc into file CAstroRT
//                      2009-11-05 GGB -  1) The library has been modified to make use of the IAU SOFA library of functions.
//                                        2) Functions making use of TTime and TDate that are affected by (1) above have been
//                                          deprecated.
//                                        3) Functions using LPSYSTEMTIME or SYSTEMTIME have been deprecated.
//                                        4) The intent is to deprecate all TTime and TDate functions in the future.
//                                        5) The intent is to deprecate all functions that are windows specific in the future.
//                      2009-09-25 GGB - File created.
//
//*********************************************************************************************************************************

#include "../include/targetAstronomy.h"

  // Standard C++ library header files

#include <algorithm>

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CTargetAstronomy
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor for the class. Initialises the name field.
  /// @throws None.
  /// @version 2018-09-29/GGB - Function created.

  CTargetAstronomy::CTargetAstronomy() : objectName_()
  {
  }

  /// @brief std::string constructor for the class. Initialises the name field.
  /// @param[in] n - The object name (identifier)
  /// @throws None.
  /// @version 2018-09-24/GGB - Updated to use vector of names
  /// @version 2016-05-07/GGB - Updated to use a list of object names.
  /// @version 2011-12-22/GGB - Function created.

  CTargetAstronomy::CTargetAstronomy(std::string const &n) : objectName_()
  {
    objectName_.push_back(n);
  }

  /// @brief Copy constructor for the class.
  /// @param[in] toCopy: The instance to copy.
  /// @throws std::bad_alloc
  /// @version 2018-09-15/GGB - Function created.

  CTargetAstronomy::CTargetAstronomy(CTargetAstronomy const &toCopy) : objectName_(toCopy.objectName_),
    catalogCoordinates_(toCopy.catalogCoordinates_)
  {
  }

  /// @brief Tests for equality between this objects name and the passed string.
  /// @param[in] rhs - The object to test.
  /// @returns true - The objects are the same
  /// @returns false - The objects are not the same.
  /// @throws None.
  /// @version 2012-01-12/GGB - Function created.

  bool CTargetAstronomy::operator==(std::string const &rhs) const
  {
    return (std::find(objectName_.begin(), objectName_.end(), rhs) != objectName_.end());
  }

  /// @brief Returns the first object name in the object names array.
  /// @returns The first object name if an object name is assigned.
  /// @throws None.
  /// @version 2016-05-07/GGB - Function created.

  std::string CTargetAstronomy::objectName() const
  {
    if (objectName_.empty())
    {
      return "";
    }
    else
    {
      return (objectName_.front());
    };
  }

  /// @brief Adds the object name to the name list.
  /// @param[in] newName - The name to add.
  /// @throws None.
  /// @details The newName is added to the list of names.
  /// @version 2018-09-24/GGB - Updated to use vector of names
  /// @version 2016-05-07/GGB - Function created.

  void CTargetAstronomy::objectName(std::string const &newName)
  {
    objectName_.push_back(newName);
  }

  /// @brief Adds the list of names to the name list.
  /// @param[in] nameList: List of names to add.
  /// @throws None.
  /// @version 2018-09-24/GGB - Function created.

  void CTargetAstronomy::objectName(std::vector<std::string> const &nameList)
  {

    std::copy_if(nameList.begin(), nameList.end(),
                 std::back_inserter(objectName_),
                  [this] (std::string const &name) -> bool
                  {
                    return (std::find(objectName_.begin(),
                                      objectName_.end(),
                                      name) == objectName_.end());
                  }
                );
  }

}   // namespace ACL
