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

#ifndef ACL_ERROR_H
#define ACL_ERROR_H

  // Standard C++ library header files

#include <cstdint>

  // Miscellaneous libraries

#include "../GCL/include/error.h" //!!! If the full GCL package is included at this point, it creates cicular references and errors.

namespace ACL
{
  std::uint64_t const E_UNABLETOOPEN                  = 0x000D;

  std::uint64_t const E_FITS_NAXIS1                   = 0x0100;
  std::uint64_t const E_FITS_NAXIS4                   = 0x0101;

  std::uint64_t const E_ASTROFUNCTIONS_EPOCHERROR     = 0x0301;
  std::uint64_t const E_ASTROFUNCTIONS_KEPLER         = 0x0302;

  std::uint64_t const E_IMAGESTACK_NOIMAGE            = 0x0400;
  std::uint64_t const E_IMAGESTACK_INVALIDMODE        = 0x0401;

  std::uint64_t const E_SIMBAD_INVALIDFIELD           = 0x0701;
  std::uint64_t const E_SIMBAD_INVALIDREGIONTYPE      = 0x0702;

  std::uint64_t const E_IMAGEPLANE_INCONSISTENTSIZE   = 0x1001;
  std::uint64_t const E_IMAGEPLANE_INVALIDBITPIX      = 0x1002;

  std::uint64_t const E_IMAGEPLANE_MEANERROR          = 0x1201;
  std::uint64_t const E_IMAGEPLANE_INVALIDCOORDS      = 0x1204;

  std::uint64_t const E_ASTROIMAGE_NOIMAGEPLANE       = 0x2202;
  std::uint64_t const E_ASTROIMAGE_INVALIDIMAGEPLANE  = 0x2203;

  std::uint64_t const E_TARGETMP_MPNOTFOUND           = 0x2700;


  std::uint64_t const E_TARGETCOMET_NOTFOUND          = 0x2800;

  std::uint64_t const E_UNABLETOOPEN_MPCORB           = 0x3300;
  std::uint64_t const E_COMETUNABLETOOPEN             = 0x3310;
  std::uint64_t const E_UNABLETOOPEN_IERS             = 0x3320;
  std::uint64_t const E_UNABLETOPARSE_IERS            = 0x3321;

  std::uint64_t const E_PHOTOMETRYFILTERS_REPEATEDID  = 0x4000;
}

#endif // ACL_ERROR_H

