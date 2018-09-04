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
//                          CTargetPlanet
//
// HISTORY:             2016-03-23 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_TARGETMINORPLANET_H
#define ACL_TARGETMINORPLANET_H

  // Standard C++ library header files.

#include <cstdint>
#include <memory>
#include <string>

  // ACL library header files.

#include "config.h"
#include "MPCORB.h"
#include "TargetAstronomy.h"

  // Miscellaneous library header files.

#include "boost/filesystem.hpp"
#include <MCL>


namespace ACL
{
  class CTargetMinorPlanet : public CTargetAstronomy
  {
  private:
    std::string designation_;       ///< The MP designation. (From MPCORB)
    MPCORB::SMPCORB elements_;

    CAstroTime epoch_;              ///< Epoch
    MCL::angle_t M0_;               ///< Mean anomoly at the epoch (degrees)
    MCL::angle_t omega_;            ///< Argument of perihelion degrees.

    FP_t a_;                        ///< Semi-major axis
    FP_t e_;                        // Eccentricity
    MCL::angle_t i_;                // Inclination
    MCL::angle_t OMEGA_;            // Longitude of ascending node



    MCL::angle_t n_;                // Mean daily motion

    CTargetMinorPlanet() = delete;
    CTargetMinorPlanet(CTargetMinorPlanet const &) = delete;

  protected:
  public:
    CTargetMinorPlanet(boost::filesystem::path const &, std::string const &);
    virtual ~CTargetMinorPlanet() {}

      // Information functions

    virtual std::string objectType() const { return "Minor"; }

      // Getter functions

    MPCORB::SMPCORB &elements() { return elements_; }

      // Position functions

    virtual CAstronomicalCoordinates positionICRS(CAstroTime const &) const;
    virtual CAstronomicalCoordinates positionObserved(CAstroTime const &, CGeographicLocation const &, CWeather const &);


      // Factory functions

    static std::unique_ptr<CTargetMinorPlanet> create(std::string const&);
    static std::unique_ptr<CTargetMinorPlanet> create(boost::filesystem::path const &, std::string const&);
  };


}

#endif // ACL_TARGETMINORPLANET_H
