//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSUtilities
// SUBSYSTEM:						FITS helper classes
// LANGUAGE:						C++
// TARGET OS:						WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	stdlib, boost
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2011-2017 Gavin Blakeman.
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
// OVERVIEW:						Implements classes for interfacing to FITS files. This file does not overlap the FITSIO library, but
//                      provides additional support for these library.
//
// CLASSES INCLUDED:    None.
//
// CLASS HIERARCHY:     None.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2011-07-14 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_FITSUTILITIES_H
#define ACL_FITSUTILITIES_H

#include "AstronomicalTime.h"

#include <cstdint>

namespace ACL
{
    // FITS related functions

  std::string sprintFITSDateTime(int const &, int const &, int const &, int const &, int const &, FP_t const &);
  std::string sprintFITSDateTime(CAstroTime const &);
  bool parseDATE_OBS(std::string const &, struct std::tm *);

  FP_t parseRA(std::string);
  FP_t parseDEC(std::string);
  FP_t parseLatitude(std::string const &);
  FP_t parseLongitude(std::string);
}


#endif // ACL_FITSUTILITIES_H
