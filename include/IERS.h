//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								IERS
// SUBSYSTEM:						Import IERS Bulletin A data.
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:
// NAMESPACE:						ACL::IERS
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
// OVERVIEW:						This module provides code to read data from the IERS Bulletin A. (In text file format)
//
// CLASSES INCLUDED:
//
// CLASS HIERARCHY:
//
// HISTORY:             2018-09-04 GGB File Created
//
//*********************************************************************************************************************************

#ifndef ACL_IERS_H
#define ACL_IERS_H

  // Standard C++ library header files

#include <cstdint>
#include <string>
#include <vector>

  // ACL library header files.

#include "../include/julianDay.h"

  // Miscellaneous library header files.

#include "boost/filesystem.hpp"

namespace ACL
{
  namespace IERS
  {
    struct SRapidValues
    {
      TJD JD;
      float x;
      float x_error;
      float y;
      float y_error;
      float UT1_UTC;
      float UT1_UTC_error;
    };

    struct SPredictions
    {
      TJD JD;
      float x;
      float y;
      float UT1_UTC;
    };

    struct SIAUOffset
    {
      TJD JD;
      float dx;
      float dx_error;
      float dy;
      float dy_error;
    };

    struct SNEOSOffset
    {
      TJD JD;
      float dpsi;
      float dpsi_error;
      float deps;
      float deps_error;
    };

    void parseRapidValues_BA(boost::filesystem::path const &, std::vector<SRapidValues> &);
    void parsePredictions_BA(boost::filesystem::path const &, std::vector<SPredictions> &);
    void parseIAUCelestialPoleOffset_BA(boost::filesystem::path const &, std::vector<SIAUOffset> &);
    void parseNEOSCelestialPoleOffset_BA(boost::filesystem::path const &, std::vector<SNEOSOffset> &);

  }  // namespace IERS
} // namespace ACL

#endif // ACL_IERS_H
