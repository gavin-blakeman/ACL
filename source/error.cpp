//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								error
// SUBSYSTEM:						Error definitions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2018-2020 Gavin Blakeman.
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
//
// HISTORY:             2018-08-19 GGB - File created.
//
//*********************************************************************************************************************************

#include "include/error.h"

  // Standard C++ library headers

#include <string>
#include <vector>

namespace ACL
{
  class CLoader
  {
  private:
    void loadErrorMessages();
  public:
    CLoader();
  };

  static CLoader executeLoader;

  CLoader::CLoader()
  {
    loadErrorMessages();
  }

  /// @brief Adds the ACL library error codes into the GCL error object.
  /// @throws None.
  /// @version  2015-07-28/GGB - Function created.

  void CLoader::loadErrorMessages()
  {
    std::vector<std::pair<GCL::TErrorCode, std::string>> errors =
    {
      {0x1905, "HDB: Cannot dynamic_cast to AsciiTable."},
      {0x190C, "HDB: Cannot dynamic_cast to BinaryTable."},
      {0x200F, "ASTROFILE: Dark calibration frame needs a valid file name."},
      {0x2010, "ASTROFILE: Dark Calibration frame must be same dimensions as image."},
      {0x2011, "ASTROFILE: Dark calibration frame should be same duration as image."},
      {0x2012, "ASTROFILE: Dark calibration frame should be same temperature as image."},
      {0x2500, "PHOTOMETRY: Cannot have exposure == 0 when calculating magnitude."},
      {0x3201, "ASTROTIME: Cannot get dUT1 for dates before 1973."},
      {0x3204, "ASTROTIME: Error preparing gregorian date string."},
      {0x3205, "ASTROTIME: Error opening TAI-UTC file."},
      {0x3206, "ASTROTIME: Error opening UTC-UT1 file."},
    };

    std::for_each(errors.begin(), errors.end(),
                  [] (std::pair<GCL::TErrorCode, std::string> p) { GCL::CError::addErrorMessage("ACL", p.first, p.second); });
  }
}
