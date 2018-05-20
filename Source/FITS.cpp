//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITS
// SUBSYSTEM:						FITS Definitions and functions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2015-2017 Gavin Blakeman.
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
// OVERVIEW:						Definitions, constants and miscellaneous functions for FITS support.
//
// CLASSES INCLUDED:		None
//
// CLASS HIERARCHY:     None
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-08-16/GGB - File created.
//
//*********************************************************************************************************************************

#include "../Include/FITS.h"

#include <fstream>

#include "cfitsio/fitsio.h"
#include <GCL>

namespace ACL
{
  /// @brief Convert a BITPIX value to a string representation for HMI display.
  /// @param[in] bp - The BITPIX value to convert.
  /// @returns A string representation of the BITPIX value.
  /// @throws GCL::CCodeError
  /// @version 2015-09-06/GGB - Added unsigned short and unsigned long and signed byte cases.
  /// @version 2015-08-16/GGB - Function created.

  std::string Bitpix2String(int bp)
  {
    std::string returnValue;

    switch (bp)
    {
      case BYTE_IMG:
      {
        returnValue = "BYTE_IMG (u8)";
        break;
      };
      case SBYTE_IMG:
      {
        returnValue = "SBYTE_IMG (s8)";
        break;
      };
      case USHORT_IMG:
      {
        returnValue = "USHORT_IMG (u16)";
        break;
      };
      case SHORT_IMG:
      {
        returnValue = "SHORT_IMG (s16)";
        break;
      };
      case ULONG_IMG:
      {
        returnValue = "USHORT_IMG (u32)";
        break;
      };
      case LONG_IMG:
      {
        returnValue = "LONG_IMG (s32)";
        break;
      };
      case LONGLONG_IMG:
      {
        returnValue += "LONGLONG_IMG (s64)";
        break;
      };
      case FLOAT_IMG:
      {
        returnValue += "FLOAT_IMG (-32)";
        break;
      };
      case DOUBLE_IMG:
      {
        returnValue = "DOUBLE_IMG (-64)";
        break;
      };
      default:
      {
        CODE_ERROR(ACL);
        break;
      };
    };

    return returnValue;
  }

  /// @brief Determines if a file is a FITS file.
  /// @param[in] p - The path to the file.
  /// @returns true - The path points to a file that is likely a FITS file.
  /// @returns false - The path is not a FITS file.
  /// @throws None.
  /// @version 2018-05-20/GGB - Function created.

  bool isFitsFile(boost::filesystem::path const &p)
  {
    bool returnValue = false;
    char const testWord[] = "SIMPLE";


    if (boost::filesystem::exists(p))
    {
      boost::filesystem::ifstream inputStream;
      std::string testRead(sizeof(testWord+1, ' ');

      if (inputStream.open(p))
      {
        if (&inputStream.read(testRead[0], sizeof(testRead-1)))
        {
          if (testRead.find(testWord) != std::string::npos)
          {
            returnValue = true;
          };
        };
      }
    };

    return returnValue;
  }

}   // namespace ACL
