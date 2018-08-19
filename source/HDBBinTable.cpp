//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDBBinTable
// SUBSYSTEM:						Header/Data Blocks (HDB)
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL,  CFitsIO, boost::filesystem, boost::UUID
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2018 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images. The classes are designed to
//                      present a single interface to the other astro library classes while supporting several different file
//                      types.
//
// CLASSES INCLUDED:		HDBBinTable - Binary table HDB.
//
// CLASS HIERARCHY:     CHDB
//                        - CHDBBinTable
//                          - CPhotometryHDB
//                        - CHDBAsciiTable
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-12-30 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../include/HDBBinTable.h"

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // class CHDBBinTable
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor.
  /// @param[in] np - Pointer to the owner of this instance.
  /// @param[in] name - The name of this extension.
  /// @throws None.
  /// @details Initialise all variables to reasonable default values,
  /// @version 2011-11-25/GGB - Function created.

  CHDBBinTable::CHDBBinTable(CAstroFile *np, std::string const &name) : CHDB(np, name), tfields_(0)
  {
    BITPIX(8);
    NAXIS(2);
    naxisn_.push_back(0);       // NAXIS1 = 0
    naxisn_.push_back(0);       // NAXIS2 = 0
    GCOUNT(1);
  }

  /// @brief Copy constructor
  /// @param[in] toCopy - The binary table to copy.
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  CHDBBinTable::CHDBBinTable(CHDBBinTable const &toCopy) : CHDB(toCopy), tfields_(toCopy.tfields_), bitpix_(toCopy.bitpix_)
  {
  }

  /// @brief Creates a copy of *this.
  /// @returns A copy of this.
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  PHDB CHDBBinTable::createCopy() const
  {
    PHDB returnValue( new CHDBBinTable(*this) );

    return returnValue;
  }

  /// @brief Reads the column information from the FITS file.
  /// @param[in] file - The FITS file to read from.
  /// @throws None.
  /// @version 2015-09-19/GGB - Function created.

  void CHDBBinTable::readFromFITS(fitsfile *file)
  {
    CHDB::readFromFITS(file);
  }

  /// @brief Write the binary table to file.
  /// @param[in] file - The FITS file to write to.
  /// @throws None.
  /// @version 2015-09-19/GGB - Function created.

  void CHDBBinTable::writeToFITS(fitsfile *file)
  {
    CHDB::writeToFITS(file);
  }

}  // namespace ACL
