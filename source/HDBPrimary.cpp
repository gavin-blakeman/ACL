//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDBPrimary
// SUBSYSTEM:						Header/Data Blocks (HDB)
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL,  CFitsIO, boost::filesystem, boost::UUID
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2012-2018 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images. The classes are designed to
//                      present a single interface to the other astro library classes while supporting several different file
//                      types.
//
// CLASSES INCLUDED:		CHDBPrimary - A primary HDB. (Without an image)
//
// CLASS HIERARCHY:     CHDB
//                        CHDBPrimary
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release
//                      2013-01-20 GGB - astroManager 0000.00 release
//                      2012-12-30 GGB - File created.
//
//*********************************************************************************************************************************

#include "../include/HDBPrimary.h"

namespace ACL
{
  /// @brief Copy constructor.
  /// @param[in] toCopy - The primary HDB to create a copy of
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  CHDBPrimary::CHDBPrimary(CHDBPrimary const &toCopy) : CHDB(toCopy)
  {
  }

  /// @brief Creates a copy of *this.
  /// @returns New instance as a copy of this.
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  PHDB CHDBPrimary::createCopy() const
  {
    PHDB returnValue(new CHDBPrimary(*this) );

    return returnValue;
  }

}
