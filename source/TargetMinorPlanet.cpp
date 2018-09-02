//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								TTargetMinorPlanet
// SUBSYSTEM:						Object Positions and Calculations
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:  MCL
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2016-2018 Gavin Blakeman.
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
// OVERVIEW:						These classes provide methods of handling minor planets
//
// CLASSES INCLUDED:    CTargetMinorPlanet - Class for managing minor planets.
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetSolar
//                          CTargetMinorPlanet
//
// HISTORY:             2016-03-23 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../include/TargetMinorPlanet.h"

  // Standard C++ library headers

#include <cstdint>
#include <cmath>
#include <fstream>
#include <vector>

  // ACL library header files.

#include "../include/error.h"

  // Miscellaneous library header files.

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/fstream.hpp"
#include <GCL>

namespace ACL
{
  /// @brief Constructor constructs from an MPCORB 1 line record.
  /// @param[in] mpcorbPath: The path and filename for the MPCORB.DAT file.
  /// @param[in] targetName: The name of the target.
  /// @throws CError(0x0600)
  /// @version 2018-09-02/GGB - Added fileName parameter to constructor.
  /// @version 2016-03-25/GGB - Function created.

  CTargetMinorPlanet::CTargetMinorPlanet(boost::filesystem::path const &fileName, std::string const &targetName)
    : CTargetAstronomy(targetName), i_(0), OMEGA_(0), omega_(0), M0_(0), n_(0)
  {
    if (!MPCORB::loadMP(fileName, targetName, elements_))
    {
      ACL_ERROR(0x2700);
    };
  }

  /// @brief Calculates and returns the ICRS position of a minor planet.
  /// @param[in] time: The time to compute the position.
  /// @returns The ICRS coordinates at the specified time.
  /// @throws
  /// @version 2018-08-25/GGB - Function created.

  CAstronomicalCoordinates CTargetMinorPlanet::positionICRS(CAstroTime const &time) const
  {

  }

  /// @brief Calculates the observed place of a minor planet.
  /// @param[in] t: Time to calculate the observed place.

  CAstronomicalCoordinates CTargetMinorPlanet::positionObserved(CAstroTime const &t, CGeographicLocation const &, CWeather const &)
  {
    MCL::CAngle M(0);
    CAstroTime deltaT = epoch_ - t;
    FP_t E0, E1;
    std::uint_least32_t loopCounter = 0;
    FP_t v, r;
    FP_t xv, yv;
    FP_t xh, yh, zh;

      // Calculate mean anomoly

    M = M0_ + (n_ * static_cast<FP_t>(deltaT.TT()));

      // Solve M = E - e sin(E)
      // f(E) = E - e sin(E) - M
      // f'(E) = 1 - e cos(E)
      // Use Newton-Raphson iteration

    E0 = MCL::D_PI;
    E1 = 0;

      // Iterate with a maxumun of 100,000 iterations looking for an accuracy of seven places.

    while ( ((E1 - E0) > 0.0000001) && (loopCounter < 100000))
    {
      E1 = E0 + (M.radians() + (e_ * std::sin(E0)) - E0) / (1 - e_ * std::cos(E0));
      loopCounter++;
    };

      // Find true anomoly (v) and Heliocentric distance (r)

    xv = a_ * ( std::cos(E1) - e_);
    yv = a_ * (std::sqrt(1.0 - MCL::pow2(e_)) * std::sin(E1));
    v = std::atan2(yv, xv);
    r = std::sqrt(MCL::pow2(xv) + MCL::pow2(yv));

      // Compute position in 3D space

    //xh = r * ( cos(N) * cos(v+w) - sin(N) * sin(v+w) * cos(i) );
    //yh = r * ( sin(N) * cos(v+w) + cos(N) * sin(v+w) * cos(i) );
    //zh = r * ( sin(v+w) * sin(i) )


  }

  /// @brief Factory function taking a line from the MPCORB file to create the minor planet.
  /// @param[in] dataLine: Line of data in MPCORB format. (null-terminated)
  /// @returns Pointer to the created CMinorPlanet structure
  /// @throws
  /// @version 2018-08-24/GGB - Function created.

  std::unique_ptr<CTargetMinorPlanet> CTargetMinorPlanet::create(std::string const &dataLine)
  {

  }

  /// @brief Factory function taking the MPCORB filename/path and the descriptor of the object to create.
  /// @param[in] fileName: The filename and path of the MPCORB file.
  /// @param[in] descriptor: The descriptor of the object to load/create.
  /// @returns Pointer to the created CMinorPlanet structure
  /// @throws 0x2700 - TargetMinorPlanet: Minor Planet Specified not found.
  /// @version 2018-08-24/GGB - Function created.

  std::unique_ptr<CTargetMinorPlanet> CTargetMinorPlanet::create(boost::filesystem::path const &fileName,
                                                                 std::string const &descriptor)
  {
    std::unique_ptr<CTargetMinorPlanet> returnValue(std::make_unique<CTargetMinorPlanet>(fileName, descriptor));

    return returnValue;
  }

}
