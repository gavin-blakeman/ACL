//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Photometry.h
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
// Copyright 2010-2017 Gavin Blakeman.
// This file is part of the Astronomy Class Library (ACL)
//
// ACL is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
//
// ACL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ACL.  If not,
// see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:						A number of classes supporting various aspects of photometry and photometric
//											reduction.
//											The focus of this subsystem is on taking raw photometric observations and
//											and converting them to transformed photometric observations.
//
// CLASSES INCLUDED:		CPhotometryObservation - A class for storing photometry observation information.
//                      CPhotometryObject		- A class for storage of photometry related information.
//                      CPhotometryAperture  - Storage class for the 3 radii for Photometry.
//
// CLASS HIERARCHY:
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-06-08 GGB - Moved CPhotometryObservation, PhotometryAperture  into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 Release.
//                      2013-01-20 GGB - astroManager 0000.00 Release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef LAC_PHOTOMETRYAPERTUREELIPTICAL_H
#define LAC_PHOTOMETRYAPERTUREELIPTICAL_H

  // ACL include files

#include "PhotometryAperture.h"

namespace ACL
{
  class CPhotometryApertureElliptical : public CPhotometryAperture
  {
  private:
  public:
    virtual EPhotometryApertureType apertureType() { return PAT_ELLIPSE; }

  };

} // namespace ACL

#endif LAC_PHOTOMETRYAPERTUREELIPTICAL_H
