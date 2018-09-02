//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								targetComet
// SUBSYSTEM:						Comet position calculations
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:  MCL
// NAMESPACE:						ACL
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
// OVERVIEW:						This class provides methods of handling comets
//
// CLASSES INCLUDED:    CTargetComet - Class for managing comets
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetMinorPlanet
//                        CTargetMajorPlanet
//                        CTargetComet
//
// HISTORY:             2018-09-01 GGB - File created.
//
//*********************************************************************************************************************************

#include "../include/targetComet.h"

namespace ACL
{
  /// @brief Creates and loads a comet target including elements.
  /// @param[in] fileName: The path and filename for the CometEls.txt file.
  /// @param[in] designation: The designation of the comet.
  /// @throws std::bad_alloc
  /// @throws 0x2800 - TargetComet: Comet specified not found.
  /// @version 2018-09-02/GGB - Function created.

  CTargetComet::CTargetComet(boost::filesystem::path const &fileName, std::string const &designation)
    : CTargetAstronomy(designation)
  {
    if (!COMETELS::loadMP(fileName, targetName, elements()))
    {
      ACL_ERROR(0x2800);
    };
  }
}
