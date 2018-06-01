//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSInspector
// SUBSYSTEM:						FITS Inspector
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, CFitsIO, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2015-2018 Gavin Blakeman.
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
// OVERVIEW:						A class to view the HDU data for FITS files.
//
// CLASSES INCLUDED:		CFITSInspector
//
// CLASS HIERARCHY:     CFITSInspector
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-08-07/GGB - File created.
//
//*********************************************************************************************************************************

#include "../Include/FITSInspector.h"

  // ACL Library

#include "../Include/FITSException.h"
#include "../Include/FITSStrings.h"

  // cfitsio Library

#include "cfitsio/fitsio.h"

namespace ACL
{
  /// @brief Constructor for the class.
  /// @param[in] fn - The filename of the FITS file to read the header information.
  /// @throws None.
  /// @version 2015-08-07/GGB - Function created.

  CFITSInspector::CFITSInspector(boost::filesystem::path const &fn) : fileName_(fn)
  {
  }

  /// @brief Loads the header data.
  /// @throws ACL::CFitsException
  /// @details All the header data for all the headers are loaded.
  /// @version 2016-04-16/GGB - Added CFITSIO_TEST() for all cfitsio usage.
  /// @version 2015-09-05/GGB - Function created.

  void CFITSInspector::loadHeaderData()
  {
    fitsfile *file;
    int status = 0;
    int hduNumber;
    int hduCount;
    DHDU::iterator newHDU;
    char keywordName[80];
    char keywordValue[80];
    char keywordComment[80];

    CFITSIO_TEST(fits_open_diskfile(&file, fileName_.string().c_str(), READONLY, &status));

      // Get the number of HDUs and iteratively read the HDUs

    CFITSIO_TEST(fits_get_num_hdus(file, &hduCount, &status));
    for (hduNumber = 1; hduNumber <= hduCount; hduNumber++)
    {
      int keywordCount;
      int keywordNumber;

        // Create the HDU record and read the HDU data.

      newHDU = hduData.emplace(hduData.end());

      newHDU->hduNumber = hduNumber;
      fits_movabs_hdu(file, hduNumber, &newHDU->hduType, &status);

      if (hduNumber == 1)
      {
        CFITSIO_TEST(fits_read_keyword(file, FITS_SIMPLE.c_str(), keywordValue, keywordComment, &status));
        newHDU->hduName = astroManager_HDB_PRIMARY;
      }
      else
      {
        CFITSIO_TEST(fits_read_keyword(file, FITS_XTENSION.c_str(), keywordValue, keywordComment, &status));
        newHDU->hduName = keywordValue;
      };

        // Get the number of keywords and iterativly read the keywords.

      CFITSIO_TEST(fits_get_hdrspace(file, &keywordCount, nullptr, &status));
      for (keywordNumber = 1; keywordNumber <= keywordCount; keywordNumber++)
      {
        CFITSIO_TEST(fits_read_keyn(file, keywordNumber, keywordName, keywordValue, keywordComment, &status));

        newHDU->keywordData.emplace_back(std::string(keywordName), std::string(keywordValue), std::string(keywordComment));
      };
    };

    CFITSIO_TEST(fits_close_file(file, &status));
  }

}   // namespace ACL
