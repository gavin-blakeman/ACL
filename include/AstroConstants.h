//*********************************************************************************************************************************
//
// PROJECT:			        Astronomy Class Library
// FILE:				        AstroConstants
// SUBSYSTEM:		        Astronomy Constants
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	NONE
// WRITTEN BY:	        Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2009-2013 Gavin Blakeman.
//                      This file is part of the Astronomical Image Reduction and Data Analysis Software (AIRDAS)
//
//                      AIRDAS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
//                      License as published by the Free Software Foundation, either version 2 of the License, or (at your option)
//                      any later version.
//
//                      AIRDAS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
//                      warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//                      more details.
//
//                      You should have received a copy of the GNU General Public License along with AIRDAS.  If not,
//                      see <http://www.gnu.org/licenses/>.
//
//
// OVERVIEW:            The file contains constants used in astronomical imaging and calculations.
// 
// CLASSES:             NONE
//
// HISTORY:             2013-09-30 GGB - AIRDAS 2013.09 release.
//                      2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2009-11-03 GGB - Removed all constants that are provided in the file sofam.h
//
//*********************************************************************************************************************************

#ifndef LAC_ASTROCONSTANTS_H
#define LAC_ASTROCONSTANTS_H

#include "config.h"

#include "sofam.h"

#include <MCL>

namespace libAstroClass
{

/* Gaussian gravitational constant */

  FP_t const GAUSSK             = 0.01720209895;

/* values related to PI */
  FP_t const D_R2D              = 180 / MCL::D_PI;            // degrees to radians and radians to degrees
  FP_t const D_D2R              = MCL::D_PI / 180;            // Degrees to radians.
  FP_t const RAD_PER_ARCMIN     = 0.00029088820866572158;     // pi / (180. * 60.)
  FP_t const ONE_OVER_SIXTY     = 0.016666666666666666;
  FP_t const DEG_PER_ARCMIN     = 0.016666666666666666;
  FP_t const DEG_PER_ARCSEC     = 0.00027777777777777778;     // 1./3600.
  FP_t const ARCMIN_PER_RAD     = 3437.7467707849396;         // 60. * 180. / pi
  FP_t const ARCMIN_PER_DEG     = 60.0;
  FP_t const ARCMIN_PER_ARCSEC  = 0.016666666666666666;
  FP_t const ARCSEC_PER_DEG     = 3600.0;
  FP_t const ARCSEC_PER_ARCMIN  = 60.0;

  JD_t const D_J2000(2451545);                                  // JED of standard epoch


}  // namesapace libAstrometry

#endif // LAC__ASTROCONSTANTS_H
