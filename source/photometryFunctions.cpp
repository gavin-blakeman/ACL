//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								photometryFunctions
// SUBSYSTEM:						Photometry Functions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2018 Gavin Blakeman.
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
// OVERVIEW:						Functions for performing photometic calculations.
//
// CLASSES INCLUDED:		None.
//
// CLASS HIERARCHY:
//
// HISTORY:             2018-09-29 GGB - Moved the photometry functions into a seperate file.
//                      2013-06-08 GGB - Moved CPhotometryObservation, PhotometryAperture  into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 Release.
//                      2013-01-20 GGB - astroManager 0000.00 Release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#include "../include/photometryFunctions.h"

  // Standard C++ library header files.

#include <cmath>

  // ACL library header files.

#include "../include/error.h"

namespace ACL
{

  /// @brief        Returns a Raw Instrumental Magnitude for the given intensity
  /// @param[in]    intensity: The intensity to convert.
  /// @returns      The intensity converted to a magnitude.
  /// @throws       CRuntimeAssert
  /// @version      2020-09-12/GGB - Use RUNTIME_ASSERT to check for valid intensity.
  /// @version      2015-10-14/GGB - Updated to throw on zero intensity.
  /// @version      2010-07-23/GGB - Function created.

  FP_t RawInstrumentalMagnitude(FP_t intensity)
  {
    RUNTIME_ASSERT(intensity > 0, "Photometry: Cannot have intensity of zero.");

    return (-2.5 * std::log10(intensity));
  }

  /// @brief Calculates the Raw Instrumental Magnitude given the ADU and adu conversion factor.
  /// @param[in] ADU: The number of AD units
  /// @param[in] aduConv: The ADU sensitivity e/ADU
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
  /// @param[in] o: intensity of the observation.
  /// @returns The SNR of the observation
  /// @version 2010-07-25/GGB - Function created.

  FP_t SNR(FP_t o)
  {
    return std::sqrt(o);
  }

}	// namespace ACL

