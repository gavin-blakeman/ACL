//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FWHM
// SUBSYSTEM:						Calculation of FWHM value
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	boost, dlib.
// NAMESPACE:						ACL
// AUTHORS:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2013-2018 Gavin Blakeman.
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
// OVERVIEW:						Functions to calculate the FWHM of a point source. The method used is to fit a gaussian to the data.
//                      A 2D gaussian non-linear least squares fit is perfomed.
//
//
// CLASSES INCLUDED:		None.
//
// FUNCTIONS INCLUDED:  FWHM
//
// CLASS HIERARCHY:     None
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-04-25 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_FWHM_H
#define ACL_FWHM_H

#include "config.h"

  // Dlib library

#include "dlib/optimization.h"

namespace ACL
{
  typedef dlib::matrix<FP_t,2,1> input_vector;

  FP_t FWHM(std::vector<std::pair<input_vector, FP_t> > const dataSamples);

}  // namespace ACL.

#endif // ACL_FWHM_H
