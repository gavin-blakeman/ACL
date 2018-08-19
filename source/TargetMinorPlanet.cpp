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

#include <cstdint>
#include <cmath>

namespace ACL
{
  /// @brief Constructor constructs from an MPCORB 1 line record.
  /// @param[in] line - The MPCORB line of data.
  /// @throws CError(0x0600)
  /// @version 2016-03-25/GGB - Functon created.

  CTargetMinorPlanet::CTargetMinorPlanet(std::string const &line) : CTargetSolar(""), i_(0), OMEGA_(0), omega_(0), M0_(0), n_(0)
  {
    if (!importMPCORBRecord(line))
    {
      ERROR(ACL, 0x0600); // Unable to import 1-line MPCORB data
    };
  }

  /// @brief Calculates the observed place of a minor planet.

  CAstronomicalCoordinates const &CTargetMinorPlanet::calculateObservedPlace(CAstroTime const &t, CGeographicLocation const &, CWeather const &)
  {
    MCL::CAngle M(0);
    CAstroTime deltaT = epoch_ - t;
    FP_t E0, E1;
    std::uint_least8_t loopCounter = 0;
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

}
