//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								PhotometryAperture
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2020 Gavin Blakeman.
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
// CLASSES INCLUDED:		CPhotometryObservation - A class for storing photometry observation information.
//                      CPhotometryObject		- A class for storage of photometry related information.
//                      CPhotometryAperture  - Storage class for the 3 radii for Photometry.
//
// CLASS HIERARCHY:
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-08 GGB - Moved CPhotometryObservation, PhotometryAperture  into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#include "include/PhotometryAperture.h"

  // Miscellaneous library header files

#include "boost/locale.hpp"

  // ACL header files

#include "include/common.h"
#include "include/error.h"
#include "include/PhotometryApertureCircular.h"

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CPhotometryAperture
  //
  //*******************************************************************************************************************************

  /// @brief        Factory function for creating apertures.
  /// @param[in]    inputArray: Array of bytes describing the aperture.
  /// @returns      A newly created photometry aperture.
  /// @throws       0x0500 - PHOTOMETRY: Unknown aperture, cannot read from array.
  /// @version      2013-05-09/GGB - Function created.

  CPhotometryAperture *CPhotometryAperture::createAperture(std::valarray<boost::uint8_t> const &inputArray)
  {
    CPhotometryAperture *returnValue = nullptr;

    switch (inputArray[0])
    {
      case PAT_CIRCULAR:
      {
        returnValue = new CPhotometryApertureCircular();
        break;
      }
      default:
      {
        RUNTIME_ERROR(boost::locale::translate("PHOTOMETRY: Unknown aperture, cannot read from array."),
                      E_PHOTOMETRY_UNKNOWNAPERTURE, LIBRARYNAME);
        break;
      }
    };

    return returnValue;
  }

  /// Creates a copy of *this.
  //
  // 2013-06-08/GGB - Function created.

  PPhotometryAperture CPhotometryAperture::createCopy() const
  {
    PPhotometryAperture returnValue(new CPhotometryAperture(*this) );

    return returnValue;
  }

} // namespace ACL
