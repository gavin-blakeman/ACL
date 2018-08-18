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
//                      Copyright 2010-2017 Gavin Blakeman.
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

#include "../Include/Photometry.h"

  // libAstrometry header files.

#include "../Include/constants.h"
#include "../Include/AstroFunctions.h"

  // Standard libraries

#include <cmath>

  // Maths class library

#include <MCL>

namespace ACL
{

  /// @brief Returns a Raw Instrumental Magnitude for the given intensity
  /// @param[in] intensity - The intensity to convert.
  /// @returns The intensity converted to a magnitude.
  /// @throws CError(ACL, 0x2501) - Photometry: Cannot have intensity of zero.
  /// @version 2015-10-14/GGB - Updated to throw on zero intensity.
  /// @version 2010-07-23/GGB - Function created.

  FP_t RawInstrumentalMagnitude(FP_t intensity)
  {
    if (intensity >= 0)
    {
      return (-2.5 * std::log10(intensity));
    }
    else
    {
      ERROR(ACL, 0x2501);
    };
  }

  /// @brief Calculates the Raw Instrumental Magnitude given the ADU and adu conversion factor.
  /// @param[in] ADU - The number of AD units
  /// @param[in] aduConv - The ADU sensitivity e/ADU
  /// @returns The raw instrumental magnitude
  /// @throws None.
  /// @version 2010-07-23/GGB - Function created.

  FP_t RawInstrumentalMagnitude(FP_t ADU, FP_t aduConv)
  {
    if ( (aduConv <= 0) || (ADU <= 0) )
    {
      return 0;		// Error cannot take log of negative value.
    }
    else
    {
      return (-2.5 * log10(ADU * aduConv));
    }
  }

  /// @brief Determines the SNR of a single observation
  /// @param[in] o - intensity of the observation.
  /// @returns The SNR of the observation
  /// @version 2010-07-25/GGB - Function created.

  FP_t SNR(FP_t o)
  {
    return std::sqrt(o);
  }

}	// namespace ACL
