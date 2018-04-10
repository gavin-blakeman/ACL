//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDBAsciiTable
// SUBSYSTEM:						Astronomical HDb classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, CFitsIO, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2016 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images.
//											The classes are designed to present a single interface to the other astro library
//											classes while supporting several different file types.
//
// CLASSES INCLUDED:		CHDBAsciiTable      - Class to encapsulate Ascii Table HDB data.
//
// CLASS HIERARCHY:     CHDB
//                        - CImageHDB
//                        - CHDBAsciiTable
//                          - CAstrometryHDB
//                          - CHDBPhotometry
//                        - CHDBBinTable
//
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2013-06-08/GGB - Move class CHDBAsciiTable into seperate file.
//                      2013-03-22 GGB - AIRDAS 2013.03 released.
//                      2013-03-17 GGB - Moved class CHDBPhotometry into seperated file.
//                      2013-02-10 GGB - Moved class CHDBImage into seperate file.
//                      2013-02-01 GGB - Moved class CAstrometryHDB into seperate file.
//                      2013-01-20 GGB - AIRDAS 0000.00 released.
//                      2011-12-10 GGB - Move classed from AstroFile.h to HDB.h
//
//*********************************************************************************************************************************

#include "../Include/HDBAsciiTable.h"

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // class CHDBAsciiTable
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor.
  /// @param[in] np - The parent object.
  /// @param[in] name - The name of the HDB.
  /// @throws None.
  /// @details Initialise all variables to reasonable default values,
  /// @version 2011-11-25/GGB - Function created.

  CHDBAsciiTable::CHDBAsciiTable(CAstroFile *np, std::string const &name) : CHDB(np, name), TFields(0)
  {
    BITPIX(BYTE_IMG);
    NAXIS(2);
    PCOUNT(0);
    GCOUNT(1);
  }

  /// @brief Copy constructor.
  /// @param[in] toCopy - The ASCII table to make a copy of.
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  CHDBAsciiTable::CHDBAsciiTable(CHDBAsciiTable const &toCopy) : CHDB(toCopy), TFields(toCopy.TFields), TBColn(toCopy.TBColn),
    TFormn(toCopy.TFormn), bitpix_(toCopy.bitpix_)
  {
  }

  /// @brief Creates a copy of *this.
  /// @returns A copt of this.
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  PHDB CHDBAsciiTable::createCopy() const
  {
    PHDB returnValue( new CHDBAsciiTable(*this) );

    return returnValue;
  }

  /// @brief Write the ascii table information to FITS.
  /// @param[in] file - Pointer to the FITS file to load from.
  /// @throws GCL::CRuntimeAssert
  /// @version 2015-08-11/GGB - Converted to cfitsio.
  /// @version 2013-03-15/GGB - Function created.

  void CHDBAsciiTable::readFromFITS(fitsfile *file)
  {
    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr.");

    CHDB::readFromFITS(file);
  }

  /// @brief read the ascii table information from FITS.
  /// @param[in] file - Pointer to the FITS file to load from.
  /// @throws GCL::CRuntimeAssert
  /// @version 2015-08-11/GGB - Converted to cfitsio.
  /// @version 2013-03-15/GGB - Function created.

  void CHDBAsciiTable::writeToFITS(fitsfile *file)
  {
    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr.");

    CHDB::writeToFITS(file);
  }

}  // namespace ACL
