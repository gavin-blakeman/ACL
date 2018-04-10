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
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2015-08-16/GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_FITS_H
#define ACL_FITS_H

  // Standard libraries

#include <string>

namespace ACL
{
  unsigned int const NAXIS_MAX = 999;       // As per FITS standard.

  std::string Bitpix2String(int);
}

#endif // ACL_FITS_H
