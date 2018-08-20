﻿//*********************************************************************************************************************************
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
//                          CTargetPlanet
//
// HISTORY:             2016-03-23 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_TARGETMINORPLANET_H
#define ACL_TARGETMINORPLANET_H

#include "config.h"
#include "TargetSolar.h"

#include <MCL>

#include <memory>
#include <string>

namespace ACL
{
  class CTargetMinorPlanet : public CTargetSolar
  {
  private:
    FP_t a_;                        // Semi-major axis
    FP_t e_;                        // Eccentricity
    MCL::angle_t i_;                // Inclination
    MCL::angle_t OMEGA_;            // Longitude of ascending node
    MCL::angle_t omega_;            // Argument of perihelion
    MCL::angle_t M0_;               // Mean anomoly
    CAstroTime epoch_;              // Epoch
    MCL::angle_t n_;                // Mean daily motion

    CTargetMinorPlanet() = delete;

  protected:
    bool importMPCORBRecord(std::string const &);   // Read a record from an MPCORB 1 line record.

  public:
    CTargetMinorPlanet(std::string const &);        // Constructor constructs from an MPCORB 1 line record.
    virtual ~CTargetMinorPlanet();



      // Calculation functions

    virtual CAstronomicalCoordinates const &calculateObservedPlace(CAstroTime const &, CGeographicLocation const &, CWeather const &);
  };
  typedef std::shared_ptr<CTargetMinorPlanet> PTargetMinorPlanet;

}

#endif // ACL_TARGETMINORPLANET_H