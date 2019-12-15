//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Photometry
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						libAstroClass
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2019 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting various aspects of photometry and photometric
//											reduction.
//											The focus of this subsystem is on taking raw photometric observations and
//											and converting them to transformed photometric observations.
//
// CLASSES INCLUDED:		CPhotometryObject		- A class for storage of photometry related information.
//                      CPhotometryAperture  - Storage class for the 3 radii for Photometry.
//
// CLASS HIERARCHY:
//
// HISTORY:             2013-03-22 GGB - astroManager 2013.03 Release.
//                      2013-01-20 GGB - astroManager 0000.00 Release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#include "../include/photometry.h"

  // Standard C++ library header files.

#include <cmath>

  // ACL library header files

#include "../include/constants.h"
#include "../include/AstroFunctions.h"

  // Miscellaneous library header files.

#include <MCL>

namespace ACL
{

  /// @brief Default constructor.
  ///
  /// @version 2018-10-05/GGB - Function created.

  CStellarMagnitude::CStellarMagnitude() : magnitudeMap()
  {

  }

  /// @brief Returns the photometry magnitude for the specified filters (observation band)
  /// @param[in] filterID: The filter to check magnitude for.
  /// @returns An optional containing the value of the magnitudes, or invalid.
  /// @throws None.
  /// @version 2018-10-08/GGB - Function created.

  std::optional<magnitude_t> CStellarMagnitude::magnitude(CPhotometryFilter::EFilters filterID) const
  {
    auto magnitudeValue = magnitudeMap.find(filterID);
    if (magnitudeValue != magnitudeMap.end())
    {
      return { magnitudeValue->second.magnitude_};
    }
    else
    {
      return {};
    }
  }


  std::optional<magnitude_t> CStellarMagnitude::magnitudeError(CPhotometryFilter::EFilters) const
  {

  }


}	// namespace ACL
