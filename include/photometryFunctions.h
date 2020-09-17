//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								photometryFunctions
// SUBSYSTEM:						Photometry Functions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2020 Gavin Blakeman.
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
// OVERVIEW:						Functions for performing photometic calculations.
//
// CLASSES INCLUDED:		None.
//
// CLASS HIERARCHY:
//
// HISTORY:             2018-09-29 GGB - Moved the photometry functions into a seperate file.
//                      2013-06-08 GGB - Moved CPhotometryObservation, PhotometryAperture  into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 Release.
//                      2013-01-20 GGB - astroManager 0000.00 Release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_PHOTOMETRYFUNCTIONS_H
#define ACL_PHOTOMETRYFUNCTIONS_H

  // ACL library header files

#include "config.h"

namespace ACL
{
  FP_t RawInstrumentalMagnitude(FP_t);
  FP_t RawInstrumentalMagnitude(FP_t, FP_t);
  FP_t SNR(FP_t);

} // namespace ACL

#endif // ACL_PHOTOMETRYFUNCTIONS_H
