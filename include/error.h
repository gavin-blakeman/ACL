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
//                      Copyright 2018-2021 Gavin Blakeman.
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
  using errorCode_t = GCL::TErrorCode;

  errorCode_t const E_UNABLETOOPEN                  = 0x000D;
  errorCode_t const E_FILEOPEN                      = 0x000E;

  errorCode_t const E_FITS_NAXIS1                   = 0x0100;
  errorCode_t const E_FITS_NAXIS4                   = 0x0101;

  errorCode_t const E_FITS_INVALIDTIMESYS           = 0x0200;

  errorCode_t const E_ASTROFUNCTIONS_LMSFORMAT      = 0x0300;
  errorCode_t const E_ASTROFUNCTIONS_EPOCHERROR     = 0x0301;
  errorCode_t const E_ASTROFUNCTIONS_KEPLER         = 0x0302;

  errorCode_t const E_IMAGESTACK_NOIMAGE            = 0x0400;
  errorCode_t const E_IMAGESTACK_INVALIDMODE        = 0x0401;

  errorCode_t const E_PHOTOMETRY_UNKNOWNAPERTURE    = 0x0500;
  errorCode_t const E_PHOTOMETRY_UNKNOWNVERSION     = 0x0501;

  errorCode_t const E_SIMBAD_INVALIDFIELD           = 0x0701;
  errorCode_t const E_SIMBAD_INVALIDREGIONTYPE      = 0x0702;

  errorCode_t const E_FIRS_INVALIDXTENSIONDATA      = 0x1000;
  errorCode_t const E_IMAGEPLANE_INCONSISTENTSIZE   = 0x1001;
  errorCode_t const E_IMAGEPLANE_INVALIDBITPIX      = 0x1002;

  errorCode_t const E_IMAGEPLANE_NOIMAGE            = 0x1200;
  errorCode_t const E_IMAGEPLANE_MEANERROR          = 0x1201;
  errorCode_t const E_IMAGEPLANE_INVALIDCOORDS      = 0x1204;

  errorCode_t const E_HDB_PRIMARYNOXTENSION         = 0x1909;
  errorCode_t const E_HDB_PRIMARYSIMPLE             = 0x190A;

  errorCode_t const E_ASTROFILE_NAXISNOTFOUND       = 0x2004;
  errorCode_t const E_ASTROFILE_HDBDUPLICATE        = 0x200B;
  errorCode_t const E_ASTROFILE_BIASFILENAME        = 0x2013;
  errorCode_t const E_ASTROFILE_BIASDIMENSIONS      = 0x2014;
  errorCode_t const E_ASTROFILE_FLATFILENAME        = 0x2015;
  errorCode_t const E_ASTROFILE_FLATDIMENSIONS      = 0x2016;
  errorCode_t const E_ASTROFILE_DARKFILENAME        = 0x200F;
  errorCode_t const E_ASTROFILE_DARKDIMENSIONS      = 0x2010;
  errorCode_t const E_ASTROFILE_DARKDURATION        = 0x2011;
  errorCode_t const E_ASTROFILE_DARKTEMPERATURE     = 0x2012;
  errorCode_t const E_ASTROFILE_HDBNOTREGISTERED    = 0x2018;

  errorCode_t const E_IMAGECALIBRATION_ARRAYSIZE    = 0x2100;
  errorCode_t const E_IMAGECALIBRATION_EXPOSURETIME = 0x2102;

  errorCode_t const E_HDBIMAGE_INCORRECTIMAGEDIMS   = 0x2006;

  errorCode_t const E_ASTROIMAGE_NOIMAGEPLANE       = 0x2202;
  errorCode_t const E_ASTROIMAGE_INVALIDIMAGEPLANE  = 0x2203;
  errorCode_t const E_ASTROIMAGE_WRONGIMAGETYPE     = 0x2207;

  errorCode_t const E_TARGETPLAET_NAMEINCORRECT     = 0x2600;

  errorCode_t const E_TARGETMP_MPNOTFOUND           = 0x2700;

  errorCode_t const E_TARGETCOMET_NOTFOUND          = 0x2800;

  errorCode_t const E_ASTROTIME_NOSCALE             = 0x3200;
  errorCode_t const E_ASTROTIME_DUT11973            = 0x3201;
  errorCode_t const E_ASTROTIME_CONSTRUCTTIMET      = 0x3203;
  errorCode_t const E_ASTROTIME_GREGORAINERROR      = 0x3204;
  errorCode_t const E_ASTROTIME_TAIUTCFILE          = 0x3205;
  errorCode_t const E_ASTROTIME_UTCUTIFILE          = 0x3206;
  errorCode_t const E_ASTROTIME_TAIUTC1973          = 0x3207;

  errorCode_t const E_UNABLETOOPEN_MPCORB           = 0x3300;
  errorCode_t const E_COMETUNABLETOOPEN             = 0x3310;
  errorCode_t const E_UNABLETOOPEN_IERS             = 0x3320;
  errorCode_t const E_UNABLETOPARSE_IERS            = 0x3321;

  errorCode_t const E_PHOTOMETRYFILTERS_REPEATEDID  = 0x4000;
  errorCode_t const E_PHOTOMERYFILTERS_ALLIDUSED    = 0x4001;
}

#endif // ACL_ERROR_H

