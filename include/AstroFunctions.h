//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroFunctions.h
// SUBSYSTEM:						Standalone astronomy related functions.
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2009-2018 Gavin Blakeman.
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
//                      see <http://www.gnu.org/licenses/>.//
//
// OVERVIEW:						Standalone functions for Astronomy. These are functions that should not be included in classes.
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. This library may reference other C libraries.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-20 GGB - astroManager 2013.03 release.
//                      2013-01-22 GGB - astroManager 0000.00 release.
//                      2009-09-06 GGB  - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROFUNCTIONS_H
#define ACL_ASTROFUNCTIONS_H

// Standard C++ libraries

#include <cstdint>
#include <string>

  // Other ACL libraries.

#include "AstronomicalTime.h"
#include "constants.h"
#include "config.h"
#include "julianDay.h"

  // MCL Library

#include <MCL>

namespace ACL
{
  FP_t airmass(FP_t zenith);															// Airmass for zenith distance
  char *ConstellationName(double, double, double);
  TJD convertEpoch(std::string const &);

  FP_t keplersEquation(FP_t meanAnomoly, FP_t eccentricity);

  FP_t dms2deg(FP_t);
  FP_t dms2deg(FP_t, FP_t, FP_t);

  FP_t hms2deg(FP_t);
  FP_t hms2deg(FP_t, FP_t, FP_t);
  FP_t hms2hrs(FP_t);

  FP_t deg2dms(FP_t);
  void deg2dms(FP_t, int &, int &, int &);

  FP_t JD2HJD(FP_t, FP_t, FP_t);	// Convert JD value to HJD value.
  FP_t refraction(FP_t zenithAngle);  // Calculate the refraction angle.

  int sscanfHMS(std::string const &, FP_t &);
  int sscanfDMS(std::string const &, FP_t &);		/* Convert text to dd.ddd double format. */
  bool sscanfDateTime(std::string const &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, FP_t &);


  int sprintfLMS(char *szBuffer, FP_t dNumber, int nDecimal, int nLatLong);
  std::string sprintfLMS(FP_t latlon, int nDecimal, int nLatLon) ;
  int sprintfTMS(char *, double, int, int=0);
}

#endif  // ACL_ASTROFUNCTIONS_H
