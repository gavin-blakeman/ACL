//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								CometEls
// SUBSYSTEM:						Comet Element File Reading
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:
// NAMESPACE:						ACL::MPCORB
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2018-2020 Gavin Blakeman.
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
// CLASSES INCLUDED:
//
// CLASS HIERARCHY:
//
// HISTORY:             2018-08-25 GGB File Created
//
//*********************************************************************************************************************************

#ifndef ACL_COMETELS_H
#define ACL_COMETELS_H

  // Standard C++ library header files

#include <cstdint>
#include <string>

  // Miscellaneous library header files

#include "boost/filesystem/fstream.hpp"

namespace ACL
{
  namespace COMETELS
  {
    struct SCometElements
    {
    public:
      std::string cometNumber;
      std::string orbitType;
      std::string provisionalDesignation;
      std::uint16_t perihelionPassageYear;
      std::uint16_t perihelionPassageMonth;
      float perihelionPassageDay;
      double perihelionDistance;
      double orbitalEccentricity;
      double argumentOfPerihelion;
      double longitudeOfAscendingNode;
      double inclination;
      std::uint16_t epochYear;
      std::uint16_t epochMonth;
      std::uint16_t epochDay;
      float absoluteMagnitude;
      float slopeParameter;
      std::string nameOfComet;
      std::string designation;
    };

    void parseLine(std::string const &, SCometElements &);
    bool loadComet(boost::filesystem::path const &, std::string const &, SCometElements &);

  } // namespace COMETELS
} // namespace ACL

#endif // ACL_COMETELS_H
