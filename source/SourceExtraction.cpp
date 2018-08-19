//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								SourceExtraction
// SUBSYSTEM:						Automatic Source Extraction
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2014-2018 Gavin Blakeman.
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
// OVERVIEW:
//
//
// CLASSES INCLUDED:		SImageSource - Structure for storing source information
//
// CLASS HIERARCHY:     SImageSource
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2014-02-09 GGB - File created
//
//*********************************************************************************************************************************

#include "../include/SourceExtraction.h"

#include <GCL>

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // SImageSource
  //
  //*******************************************************************************************************************************

  // Tests two objects for less than for sorting.
  //
  // 2012-07-30/GGB - Function created.

  bool SImageSource::operator<(SImageSource const &toTest) const
  {
    TRACEMESSAGE("SImageSource::operator< this->flux=" + std::to_string(this->flux) + ". toTest->flux = " +
                 std::to_string(toTest.flux));

    return (flux < toTest.flux);
  }
}
