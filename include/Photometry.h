//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Photometry
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2018 Gavin Blakeman.
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
// HISTORY:             2013-06-08 GGB - Moved CPhotometryObservation, PhotometryAperture  into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 Release.
//                      2013-01-20 GGB - astroManager 0000.00 Release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_PHOTOMETRY_H
#define ACL_PHOTOMETRY_H

  // Standard C++ library header files

#include <map>
#include <optional>

  // ACL library header files

#include "AstronomicalTime.h"
#include "config.h"
#include "Observation.h"
#include "PhotometryAperture.h"
#include "photometryFilters.h"

  // Miscellaneous library header files

#include <MCL>
#include <SCL>

namespace ACL
{
  class CStellarMagnitude final
  {
  private:
    class CMagnitude final
    {
    private:
      std::optional<magnitude_t> magniture_;
      std::optional<magnitude_t> magnitudeError_;
    };

    std::map<CPhotometryFilter, CMagnitude> magnitudeMap;

  protected:
  public:
    CStellarMagnitude();

      // Setter functions

    void magnitude(CPhotometryFilter);

      // Getter functions.

    std::optional<magnitude_t> magnitude() const;
    std::optional<magnitude_t> magnitudeError() const;
  };

}	// namespace ACL


#endif // ACL_PHOTOMETRY_H
