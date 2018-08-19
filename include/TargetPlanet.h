//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								TTargetPlanet
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

#ifndef ACL_TARGETPLANET_H
#define ACL_TARGETPLANET_H

#include "TargetSolar.h"

#include <memory>

namespace ACL
{
  class CTargetPlanet : public CTargetSolar
  {
  public:
    enum EPlanets
    {
      Mercury,
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

    CTargetPlanet() = delete;

  protected:
  public:
    CTargetPlanet(EPlanets const &);
    virtual ~CTargetPlanet();

  };
  std::shared_ptr<CTargetPlanet> PTargetPlanet;
}

#endif // ACL_TARGETPLANET_H
