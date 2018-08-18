//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								common
// SUBSYSTEM:						Common definitions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2013-2016 Gavin Blakeman.
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
// FUNCTIONS INCLUDED:  std::string getVersionString()
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-29 GGB - File created.
//
//*********************************************************************************************************************************

#include "../Include/common.h"

  // Standard libraries

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

  // GCL Library

#include <GCL>

  // Boost Library

#include <boost/algorithm/string.hpp>

namespace ACL
{

  unsigned int const ACL_MAJORVERSION = 2015;           ///< The Major version number (yyyy)
  unsigned int const ACL_MINORVERSION = 9;              ///< The Minor version number (mm)
  unsigned int const ACL_BUILDNUMBER  = 0x0177;         ///< The build number - incremented on each build.

  /// @brief Returns the library version and build number.
  /// that was used for editing the library.
  /// @returns A string with the application name, version, build.
  /// @throws Nothing.
  /// @version 2015-06-24/GGB - Converted to C++. Removed all C function calls.
  /// @version 2013-06-29/GGB - Function created.

  std::string getVersionString()
  {
    std::stringstream s;

    s << ACL_MAJORVERSION << "." << std::setfill('0') << std::setw(2) << ACL_MINORVERSION << std::setw(4) << std::hex << ACL_BUILDNUMBER << std::endl;

    std::string returnValue = s.str();
    boost::algorithm::to_upper(returnValue);

    return returnValue;
  }

  /// @brief Adds the ACL library error codes into the GCL error object.
  /// @throws None.
  /// @version  2015-07-28/GGB - Function created.

  void loadErrorMessages()
  {
    std::vector<std::pair<GCL::TErrorCode, std::string>> errors =
    {
      {0x0001, std::string("Invalid coordinates.")},
      {0x0002, std::string("GENERAL: Unable to allocate memory")},
      {0x0003, std::string("Overlaps edge")},
      {0x0004, std::string("Inconsistent image sizes")},
      {0x0005, std::string("Invalid image plane")},
      {0x0006, std::string("Unable to open file, or file does not exist.")},
      {0x0007, std::string("NAXIS > 3 not supported by ACL library.")},
      {0x0009, std::string("Error while reading FITS file.")},
      {0x000A, std::string("Variable not initialised correctly.")},
      {0x000B, std::string("Array index out of array bounds.")},
      {0x000C, std::string("FITS keyword type unknown.")},
      {0x000D, std::string("Unable to open file type selected.")},
      {0x000E, std::string("Error while opening file.")},
      {0x000F, std::string("cfitsio: cfitsioError")},
      {0x0100, std::string("CKeyword: Invalid typecase")},
      {0x0101, std::string("KEYWORD: Unable to cast value... Out of range.")},
      {0x0200, std::string("FITS: TIMESYS - timescale unknown or undefined.")},
      {0x0201, std::string("")},
      {0x0300, std::string("ASTROFUNCTIONS: Error while formatting Latitude (sprintfLMS())")},
      {0x0301, std::string("ASTROFUNCTIONS: Incorrect format of Epoch.")},
      {0x0400, std::string("IMAGESTACK: No Images, or Images zero size.")},
      {0x0401, std::string("IMAGESTACK: Invalid stacking mode.")},
      {0x0500, std::string("PHOTOMETRY: Unknown aperture, cannot read from array.")},
      {0x0501, std::string("PHOTOMETRY: Unknown version number for circular aperture.")},
      {0x0600, std::string("TARGETMINORPLANET: Unable to import 1 line MPCORB data.")},
      {0x0700, "SIMBAD: Invalid Region type."},
      {0x0701, "SIMBAD: Invalid field name."},
      {0x0702, "SIMBAD: Invalid region type."},
      {0x0800, "FITSMemoryFile: Unable to open file."},
      {0x0900, ""},
      {0x1000, std::string("FITS: Invalid XTENSION data")},
      {0x1001, std::string("")},
      {0x1002, std::string("")},
      {0x1103, std::string("ASTROCLASS: ")},
      {0x1104, std::string("ASTROCLASS: ")},
      {0x1105, std::string("ASTROCLASS: ")},
      {0x1200, std::string("IMAGEPLANE: No image plane available BITPIX = BP_NONE.")},
      {0x1201, std::string("IMAGEPLANE: Error when calculating image mean.")},
      {0x1202, std::string("IMAGEPLANE: setValue(index), getValue(index). index is beyond end of array.")},
      {0x1203, std::string("IMAGEPLANE: Invalid BITPIX value.")},
      {0x1204, std::string("IMAGEPLANE: Invalid coordinates.")},
      {0x1205, std::string("IMAGEPLANE: FWHM Call Radius == 0.")},
      {0x1900, std::string("HDB: NAXIS = 1 not supported for IMAGE by ACL library.")},
      {0x1901, std::string("HDB: Only supported BITPIX values are allowed.")},
      {0x1902, std::string("HDB: 0 <= NAXIS <= 999.")},
      {0x1903, std::string("HDB: Cannot set value of NAXISn, NAXIS not correctly defined.")},
      {0x1904, std::string("HDB: (NAXISn < 0) not allowed.")},
      {0x1905, std::string("HDB: Cannot dynamic_cast to AsciiTable.")},
      {0x1906, std::string("HDB: Invalid HDU passed to function.")},
      {0x1907, std::string("HDB: Data package NULL. Should not be nullptr")},
      {0x1908, std::string("HDB: parent cannot be == nullptr.")},
      {0x1909, std::string("HDB: XTENSION keyword not allowed for PRIMARY header.")},
      {0x190A, std::string("HDB: SIMPLE only allowed in PRIMARY header.")},
      {0x190B, std::string("HDB: Null HDU value passed to function.")},
      {0x190C, std::string("HDB: Cannot dynamic_cast to BinaryTable.")},
      {0x190D, std::string("HDB: EXPOSURE or EXPTIME keyword not found.")},
      {0x2001, std::string("ASTROFILE: Invalid HDB number")},
      {0x2002, std::string("ASTROFILE: Keyword not found")},
      {0x2003, std::string("ASTROFILE: Keyword number not found")},
      {0x2004, std::string("ASTROFILE: NAXIS value not found or does not exist.")},
      {0x2005, std::string("ASTROFILE: Block Type should be BT_IMAGE.")},
      {0x2006, std::string("ASTROFILE: Incorrect image dimensions.")},
      {0x2007, std::string("ASTROFILE: Data pointer == NULL.")},
      {0x2008, std::string("ASTROFILE: Invalid NAXISn. n < 1 || n > 999")},
      {0x2009, std::string("ASTROFILE: Invalid keyword number")},
      {0x200A, std::string("ASTROFILE: Unable to create HDB. Function new returned NULL.")},
      {0x200B, std::string("ASTROFILE: Cannot create HDB with duplicate name.")},
      {0x200C, std::string("ASTROFILE: Astrometry HDB does not exist.")},
      {0x200D, std::string("ASTROFILE: Error accessing HDB[0], no HDB[0] existing.")},
      {0x200E, std::string("ASTROFILE: Error while opening RAW file.")},
      {0x200F, std::string("ASTROFILE: Dark calibration frame needs a valid file name.")},
      {0x2010, std::string("ASTROFILE: Dark Calibration frame must be same dimensions as image.")},
      {0x2011, std::string("ASTROFILE: Dark calibration frame should be same duration as image.")},
      {0x2012, std::string("ASTROFILE: Dark calibration frame should be same temperature as image.")},
      {0x2013, std::string("ASTROFILE: Bias calibration frame needs a valid file name.")},
      {0x2014, std::string("ASTROFILE: Bias calibration frame must be same dimensions as image.")},
      {0x2015, std::string("ASTROFILE: Flat calibration frame needs a valid file name.")},
      {0x2016, std::string("ASTROFILE: Flat calibration frame must be same dimensions as image.")},
      {0x2017, std::string("ASTROFILE: Photometry HDB does not exist.")},
      {0x2018, std::string("ASTROFILE: HDB Type not registered.") },
      {0x2100, std::string("CALIBRATION: Inconsistent image array sizes.")},
      {0x2101, std::string("CALIBRATION: Inconsistent image sizes.")},
      {0x2102, std::string("CALIBRATION: Inconsistent exposure times.")},
      {0x2103, std::string("CALIBRATION:")},
      {0x2200, std::string("ASTROIMAGE: Divide by Zero.")},
      {0x2201, std::string("ASTROIMAGE: Invalid image plane.")},
      {0x2202, std::string("ASTROIMAGE: No Image Plane available.")},
      {0x2203, std::string("ASTROIMAGE: Invalid Image Data.")},
      {0x2204, std::string("ASTROIMAGE: Cannot float an image smaller.")},
      {0x2205, std::string("ASTROIMAGE: Crop: Origen < 0")},
      {0x2206, std::string("ASTROIMAGE: Crop: CropOrigen + CropDimensions > ImageDimensions")},
      {0x2207, std::string("ASTROIMAGE: applyDark requires dark image to be of same type (mono or poly)")},
      {0x2209, std::string("ASTROIMAGE: Invalid parameters for function CImagePlane::meanRow.")},
      {0x220B, std::string("ASTROIMAGE: Unable to determine centroid.")},
      {0x220C, std::string("")},
      {0x220D, std::string("ASTROIMAGE: Invalid Rendering Mode") },
      {0x2300, std::string("ASTROIMAGEMONO: Unable to cast instance to correct class.") },
      {0x2300, std::string("STELLAROBJECTS: Must have valid location for detemining observed position.")},
      {0x2301, std::string("STELLAROBJECTS: Must have valid time for determining observed position.")},
      {0x2302, std::string("StellarObjects: SOFA library error, iteration did not converge.")},
      {0x2400, std::string("ASTROCATALOGUE: No items in list. Cannot return first item")},
      {0x2401, std::string("ASTROCATALOGUE: Reached last item in list. Cannot return item.")},
      {0x2402, std::string("ASTROCATALOGUE: Reached first item in list. Cannot return item.")},
      {0x2500, std::string("PHOTOMETRY: Cannot have exposure == 0 when calculating magnitude.")},
      {0x2501, std::string("Photometry: Cannot have intensity of zero.")},
      {0x3000, std::string("ASTROMETRY: Need at least 3 reference objects to calculate plate constants.")},
      {0x3001, std::string("ASTROMETRY: Observation Location must be defined to reduce positions to calculate plate constants.")},
      {0x3002, std::string("ASTROMETRY: Observation Time must be defined to reduce positions to calculate plate constants.")},
      {0x3003, std::string("ASTROMETRY: Error while reading from FITS, unknown coordinate system specified.")},
      {0x3100, std::string("ASTROCOORDINATES: SOFA System Error.")},
      {0x3101, std::string("ASTROCOORDINATES: iauStarpm() did not converge.")},
      {0x3102, std::string("ASTROCOORDINATES: iauStarpm() velocity is a significant fraction of c, set to zero.")},
      {0x3200, std::string("ASTROTIME: Time scale NONE specified in conversion function.")},
      {0x3201, std::string("ASTROTIME: Cannot get dUT1 for dates before 1973.")},
      {0x3202, std::string("ASTROTIME: Invalid Parameter.")},
      {0x3203, std::string("ASTROTIME: Error while constructing JD from time_t.")},
      {0x3204, std::string("ASTROTIME: Error preparing gregorian date string.")},
      {0x3205, std::string("ASTROTIME: Error opening TAI-UTC file.")},
      {0x3206, std::string("ASTROTIME: Error opening UTC-UT1 file.")},
      {0x3207, std::string("ASTROTIME: Cannot get TAI-UTC before 1973.")},
      {0x4000, "PhotometryFilters: Repeated Database ID"},
      {0x4001, "PhotometryFilters: All available filterID used."},
      {0x4100, std::string("")},
      {0xA000, std::string("libAstrometry::starxy: Index outside of allowed range. ((index < 0) || (index >= assignedSize)).")},
      {0xA001, std::string("libAstrometry::starxy: Item at index is null, unable to return value.")},
      {0xA100, std::string("libAstrometry::quadfile: Failed to open quads file.")},
      {0xA101, std::string("libAstrometry::quadfile::CQuadFile constructor: Cannot pass nullptr as FITS filename.")},
      {0xA102, std::string("libAstrometry::quadfile::CQuadFile constructor: Error while opening/creating FITS file.")},
      {0xA103, std::string("libAstrometry::quadfile::CQuadFile::check(): dimquads has illegal value.")},
      {0xA104, std::string("libAstrometry::quadfile::CQuadFile::check(): Failed to get quad.")},
      {0xA105, std::string("libAstrometry::quadfile::CQuadFile::check(): Stad ID is out of bounds.")},
      {0xA106, std::string("libAstrometry::quadfile::CQuadFile::getStars(): Requested quad ID out of range.")},
    };

    std::for_each(errors.begin(), errors.end(),
                  [] (std::pair<GCL::TErrorCode, std::string> p) { GCL::CError::addErrorMessage("ACL", p.first, p.second); });
  }

}
