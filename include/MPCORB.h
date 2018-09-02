//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								MPCORB
// SUBSYSTEM:						Minor Planet Orbital File Reading
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:
// NAMESPACE:						ACL::MPCORB
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2018 Gavin Blakeman.
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

#ifndef ACL_MPCORB_H
#define ACL_MPCORB_H

  // Standard C++ library header files

#include <cstdint>
#include <string>

  // Miscellaneous library header files

#include "boost/filesystem.hpp"

namespace ACL
{
  namespace MPCORB
  {
    struct SMPCORB
    {
      std::string designation;
      float absoluteMagnitude;
      float slopeParameter;
      std::string epoch;
      double meanAnomaly;
      double argumentOfPerihelion;
      double longitudeOfAscendingNode;
      double inclination;
      double eccentricity;
      double meanDailyMotion;
      double semiMajorAxis;
      std::string uncertantyParameter;
      std::string referece;
      std::uint64_t numberOfObservations;
      std::uint16_t numberOfOppositions;
      std::string additionalData;
      float rmsResidual;
      std::string coursePerturbers;
      std::string precisePerturbers;
      std::string computerName;
      std::uint16_t flags;
      std::string name;
      std::string dateOfLastObservation;
    };

    void parseLine(std::string const &, SMPCORB &);
    bool loadMP(boost::filesystem::path const &, std::string const &, SMPCORB &);

  }  // namespace MPCORB
} // namespace ACL

#endif // ACL_MPCORB_H

