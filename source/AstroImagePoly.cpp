//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroImagePoly
// SUBSYSTEM:						Polychrome Astronomical Image Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, CFitsIO, boost.
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
//
// OVERVIEW:						A number of classes supporting astronomical images. The focus is on the	manipulation of the images and on
//                      providing displayable data. No image display is done by these classes, although they can provide raw data
//                      for display.
//
//											These classes are tightly linked with the CFitsIO library. This linkage
//                      is required as these classes do ultimately service astronomical images and the standard format for
//                      astonomical images is FITS.
//
//											It is not intended that these classes support other formats at this time (2010), but support for other
//                      image formats may be added. (DNG, JPG, TIFF)
//
//											Support for statistical and other mathematics is derived from the gsl library.
//
//											The library is designed to be platform independant. IE, only standard C++ functionallity is used.
//
//											While this library has been designed to be used from Qt, it makes no reference to the Qt library.
//
//
// CLASSES INCLUDED:		CAstroImagePoly			- A class that provides scientific functionality on a polychromatic image.
//
// CLASS HIERARCHY:     CAstroImage
//												- CAstroImageMono
//												- CAstroImagePoly
//
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-05-25 GGB - Moved CAstroImageMono & CAstroImagePoly to their own files.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../include/AstroImagePoly.h"

  // Standard C++ libraries

#include <limits>

  // Other libraries

#include <GCL>

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CAstroImagePoly
  //
  //*******************************************************************************************************************************

  /// @brief Constructor for a polychrome image.
  /// @throws None.
  //
  // 2013-03-13/GGB - Changed parameter list.
  // 2011-12-25/GGB - Function created.

  CAstroImagePoly::CAstroImagePoly(AXIS_t width, AXIS_t height, AXIS_t colours) : CAstroImage(width, height, colours)
  {
  }

  /// @brief Inserts the image plane and colour information into the image plane list.
  /// @throws CRuntimeAssert
  //
  // 2011-12-26/GGB - Function created.

  void CAstroImagePoly::insertImagePlane(PImagePlane newPlane)
  {
    RUNTIME_ASSERT(ACL, newPlane != nullptr, "A valid image plane must be passed in newPlane.");

    imagePlaneStorage.push_back(newPlane);
  }

  // Loads the image from a set of RGB data. The image loaded depends on the colour passed to the function.
   //
   //

   void CAstroImagePoly::loadFromRGBHP(SRGBHP_Ptr, EColour)
   {
   }

  /// @brief Reads a polychromatic image from FITS file.
  /// @param[in] file - The FITS file to read from.
  /// @details The function cycles through loading the imageplanes one at a time, until they are all loaded.
  /// @throws std::bad_alloc
  /// @version 2015-08-13/GGB - Updated to use cfitsio.
  /// @version 2015-07-31/GGB - Function created.

  void CAstroImagePoly::readFromFITS(fitsfile *file)
  {
    RUNTIME_ASSERT(ACL, file != nullptr, "The parameter file cannot be a nullptr.");

//    AXIS_t dimZ = hdu->axis(2);      // Must be positive
//    AXIS_t axis;

//    for (axis = 1; axis <= dimZ; ++axis)
//    {
//      imagePlaneStorage.emplace_back(new CImagePlane(file, axis));
//    };
  }

}  // namespace ACL
