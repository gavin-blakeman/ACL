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
//                      Copyright 2018 Gavin Blakeman.
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

#include "../include/error.h"

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
      {0x0001, "Invalid coordinates."},
      {0x0002, "GENERAL: Unable to allocate memory"},
      {0x0003, "Overlaps edge"},
      {0x0004, "Inconsistent image sizes"},
      {0x0005, "Invalid image plane"},
      {0x0006, "Unable to open file, or file does not exist."},
      {0x0007, "NAXIS > 3 not supported by ACL library."},
      {0x0009, "Error while reading FITS file."},
      {0x000A, "Variable not initialised correctly."},
      {0x000B, "Array index out of array bounds."},
      {0x000C, "FITS keyword type unknown."},
      {0x000D, "Unable to open file type selected."},
      {0x000E, "Error while opening file."},
      {0x000F, "cfitsio: cfitsioError"},
      {0x0100, "CKeyword: Invalid typecase"},
      {0x0101, "KEYWORD: Unable to cast value... Out of range."},
      {0x0200, "FITS: TIMESYS - timescale unknown or undefined."},
      {0x0201, ""},
      {0x0300, "ASTROFUNCTIONS: Error while formatting Latitude (sprintfLMS())"},
      {0x0301, "ASTROFUNCTIONS: Incorrect format of Epoch."},
      {0x0400, "IMAGESTACK: No Images, or Images zero size."},
      {0x0401, "IMAGESTACK: Invalid stacking mode."},
      {0x0500, "PHOTOMETRY: Unknown aperture, cannot read from array."},
      {0x0501, "PHOTOMETRY: Unknown version number for circular aperture."},
      {0x0600, "TARGETMINORPLANET: Unable to import 1 line MPCORB data."},
      {0x0700, "SIMBAD: Invalid Region type."},
      {0x0701, "SIMBAD: Invalid field name."},
      {0x0702, "SIMBAD: Invalid region type."},
      {0x0800, "FITSMemoryFile: Unable to open file."},
      {0x0900, ""},
      {0x1000, "FITS: Invalid XTENSION data"},
      {0x1001, ""},
      {0x1002, ""},
      {0x1103, "ASTROCLASS: "},
      {0x1104, "ASTROCLASS: "},
      {0x1105, "ASTROCLASS: "},
      {0x1200, "IMAGEPLANE: No image plane available BITPIX = BP_NONE."},
      {0x1201, "IMAGEPLANE: Error when calculating image mean."},
      {0x1202, "IMAGEPLANE: setValue(index), getValue(index). index is beyond end of array."},
      {0x1203, "IMAGEPLANE: Invalid BITPIX value."},
      {0x1204, "IMAGEPLANE: Invalid coordinates."},
      {0x1205, "IMAGEPLANE: FWHM Call Radius == 0."},
      {0x1900, "HDB: NAXIS = 1 not supported for IMAGE by ACL library."},
      {0x1901, "HDB: Only supported BITPIX values are allowed."},
      {0x1902, "HDB: 0 <= NAXIS <= 999."},
      {0x1903, "HDB: Cannot set value of NAXISn, NAXIS not correctly defined."},
      {0x1904, "HDB: (NAXISn < 0) not allowed."},
      {0x1905, "HDB: Cannot dynamic_cast to AsciiTable."},
      {0x1906, "HDB: Invalid HDU passed to function."},
      {0x1907, "HDB: Data package NULL. Should not be nullptr"},
      {0x1908, "HDB: parent cannot be == nullptr."},
      {0x1909, "HDB: XTENSION keyword not allowed for PRIMARY header."},
      {0x190A, "HDB: SIMPLE only allowed in PRIMARY header."},
      {0x190B, "HDB: Null HDU value passed to function."},
      {0x190C, "HDB: Cannot dynamic_cast to BinaryTable."},
      {0x190D, "HDB: EXPOSURE or EXPTIME keyword not found."},
      {0x2001, "ASTROFILE: Invalid HDB number"},
      {0x2002, "ASTROFILE: Keyword not found"},
      {0x2003, "ASTROFILE: Keyword number not found"},
      {0x2004, "ASTROFILE: NAXIS value not found or does not exist."},
      {0x2005, "ASTROFILE: Block Type should be BT_IMAGE."},
      {0x2006, "ASTROFILE: Incorrect image dimensions."},
      {0x2007, "ASTROFILE: Data pointer == NULL."},
      {0x2008, "ASTROFILE: Invalid NAXISn. n < 1 || n > 999"},
      {0x2009, "ASTROFILE: Invalid keyword number"},
      {0x200A, "ASTROFILE: Unable to create HDB. Function new returned NULL."},
      {0x200B, "ASTROFILE: Cannot create HDB with duplicate name."},
      {0x200C, "ASTROFILE: Astrometry HDB does not exist."},
      {0x200D, "ASTROFILE: Error accessing HDB[0], no HDB[0] existing."},
      {0x200E, "ASTROFILE: Error while opening RAW file."},
      {0x200F, "ASTROFILE: Dark calibration frame needs a valid file name."},
      {0x2010, "ASTROFILE: Dark Calibration frame must be same dimensions as image."},
      {0x2011, "ASTROFILE: Dark calibration frame should be same duration as image."},
      {0x2012, "ASTROFILE: Dark calibration frame should be same temperature as image."},
      {0x2013, "ASTROFILE: Bias calibration frame needs a valid file name."},
      {0x2014, "ASTROFILE: Bias calibration frame must be same dimensions as image."},
      {0x2015, "ASTROFILE: Flat calibration frame needs a valid file name."},
      {0x2016, "ASTROFILE: Flat calibration frame must be same dimensions as image."},
      {0x2017, "ASTROFILE: Photometry HDB does not exist."},
      {0x2018, "ASTROFILE: HDB Type not registered."},
      {0x2100, "CALIBRATION: Inconsistent image array sizes."},
      {0x2101, "CALIBRATION: Inconsistent image sizes."},
      {0x2102, "CALIBRATION: Inconsistent exposure times."},
      {0x2103, "CALIBRATION:"},
      {0x2200, "ASTROIMAGE: Divide by Zero."},
      {0x2201, "ASTROIMAGE: Invalid image plane."},
      {0x2202, "ASTROIMAGE: No Image Plane available."},
      {0x2203, "ASTROIMAGE: Invalid Image Data."},
      {0x2204, "ASTROIMAGE: Cannot float an image smaller."},
      {0x2205, "ASTROIMAGE: Crop: Origen < 0"},
      {0x2206, "ASTROIMAGE: Crop: CropOrigen + CropDimensions > ImageDimensions"},
      {0x2207, "ASTROIMAGE: applyDark requires dark image to be of same type (mono or poly)"},
      {0x2209, "ASTROIMAGE: Invalid parameters for function CImagePlane::meanRow."},
      {0x220B, "ASTROIMAGE: Unable to determine centroid."},
      {0x220C, ""},
      {0x220D, "ASTROIMAGE: Invalid Rendering Mode"},
      {0x2300, "ASTROIMAGEMONO: Unable to cast instance to correct class."},
      {0x2300, "STELLAROBJECTS: Must have valid location for detemining observed position."},
      {0x2301, "STELLAROBJECTS: Must have valid time for determining observed position."},
      {0x2302, "StellarObjects: SOFA library error, iteration did not converge."},
      {0x2400, "ASTROCATALOGUE: No items in list. Cannot return first item"},
      {0x2401, "ASTROCATALOGUE: Reached last item in list. Cannot return item."},
      {0x2402, "ASTROCATALOGUE: Reached first item in list. Cannot return item."},
      {0x2500, "PHOTOMETRY: Cannot have exposure == 0 when calculating magnitude."},
      {0x2501, "Photometry: Cannot have intensity of zero."},
      {0x2600, "TargetMajorPlanet: Keplers equation failed to converge."},
      {0x3000, "ASTROMETRY: Need at least 3 reference objects to calculate plate constants."},
      {0x3001, "ASTROMETRY: Observation Location must be defined to reduce positions to calculate plate constants."},
      {0x3002, "ASTROMETRY: Observation Time must be defined to reduce positions to calculate plate constants."},
      {0x3003, "ASTROMETRY: Error while reading from FITS, unknown coordinate system specified."},
      {0x3100, "ASTROCOORDINATES: SOFA System Error."},
      {0x3101, "ASTROCOORDINATES: iauStarpm() did not converge."},
      {0x3102, "ASTROCOORDINATES: iauStarpm() velocity is a significant fraction of c, set to zero."},
      {0x3200, "ASTROTIME: Time scale NONE specified in conversion function."},
      {0x3201, "ASTROTIME: Cannot get dUT1 for dates before 1973."},
      {0x3202, "ASTROTIME: Invalid Parameter."},
      {0x3203, "ASTROTIME: Error while constructing JD from time_t."},
      {0x3204, "ASTROTIME: Error preparing gregorian date string."},
      {0x3205, "ASTROTIME: Error opening TAI-UTC file."},
      {0x3206, "ASTROTIME: Error opening UTC-UT1 file."},
      {0x3207, "ASTROTIME: Cannot get TAI-UTC before 1973."},
      {0x4000, "PhotometryFilters: Repeated Database ID"},
      {0x4001, "PhotometryFilters: All available filterID used."},
      {0x4100, ""},
      {0xA000, "libAstrometry::starxy: Index outside of allowed range. ((index < 0) || (index >= assignedSize))."},
      {0xA001, "libAstrometry::starxy: Item at index is null, unable to return value."},
      {0xA100, "libAstrometry::quadfile: Failed to open quads file."},
      {0xA101, "libAstrometry::quadfile::CQuadFile constructor: Cannot pass nullptr as FITS filename."},
      {0xA102, "libAstrometry::quadfile::CQuadFile constructor: Error while opening/creating FITS file."},
      {0xA103, "libAstrometry::quadfile::CQuadFile::check(): dimquads has illegal value."},
      {0xA104, "libAstrometry::quadfile::CQuadFile::check(): Failed to get quad."},
      {0xA105, "libAstrometry::quadfile::CQuadFile::check(): Stad ID is out of bounds."},
      {0xA106, "libAstrometry::quadfile::CQuadFile::getStars(): Requested quad ID out of range."},
    };

    std::for_each(errors.begin(), errors.end(),
                  [] (std::pair<GCL::TErrorCode, std::string> p) { GCL::CError::addErrorMessage("ACL", p.first, p.second); });
  }
}
