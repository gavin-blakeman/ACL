//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								TargetMajorPlanet
// SUBSYSTEM:						Major Planet Positions and Calculations
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
// OVERVIEW:						These classes provide methods of handling planets
//
// CLASSES INCLUDED:    CTargetPlanet - Class for managing planets.
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetSolar
//                          CTargetMinorPlanet
//                          CTargetPlanet
//
// HISTORY:             2016-03-25 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_TARGETMAJORPLANET_H
#define ACL_TARGETMAJORPLANET_H

  // Standard C++ library header files.

#include <memory>

  // ACL library header files.

#include "TargetAstronomy.h"

  // Miscellaneous library header files.

namespace ACL
{
  class CTargetMajorPlanet : public CTargetAstronomy
  {
  public:
    enum EPlanets
    {
      Mercury = 0,      // Note these enums are used for indexing the static array used for parameters. Do not change order!
      Venus,
      Mars,
      Jupiter,
      Saturn,
      Uranus,
      Neptune,
      Pluto
    };

  private:
    EPlanets planet;

    CTargetMajorPlanet() = delete;

  protected:
  public:
    CTargetMajorPlanet(EPlanets const &);
    virtual ~CTargetMajorPlanet() {}

      // Factory functions

    virtual std::unique_ptr<CTargetAstronomy> createCopy() const  {}

      // Information functions

    virtual std::string objectType() const { return "Planet"; }

      // Position functions.

    virtual CAstronomicalCoordinates positionICRS(CAstroTime const &) const;
    virtual SObservedPlace positionObserved(CAstroTime const &, CGeographicLocation const &, CWeather const *) {}

      // Factory functions

    static std::unique_ptr<CTargetMajorPlanet> create(EPlanets const &);

  };
}

#endif // ACL_TARGETMAJORPLANET_H
