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

#include "include/targetComet.h"

  // ACL library header files

#include "include/error.h"

namespace ACL
{
  /// @brief        Creates and loads a comet target including elements.
  /// @param[in]    fileName: The path and filename for the CometEls.txt file.
  /// @param[in]    targetName: The designation of the comet.
  /// @throws       std::bad_alloc
  /// @throws       CRuntimeError(0x2800 - TargetComet: Comet specified not found.)
  /// @version      2018-09-02/GGB - Function created.

  CTargetComet::CTargetComet(boost::filesystem::path const &fileName, std::string const &targetName)
    : CTargetAstronomy(targetName)
  {
    if (!COMETELS::loadComet(fileName, targetName, elements_))
    {
      RUNTIME_ERROR("TargetComet: Comet specified not found.", E_TARGETCOMET_NOTFOUND, "ACL");
    };
  }

  /// @brief Copy constructor for the class.
  /// @param[in] toCopy: The instance to make a copy of.
  /// @throws None.
  /// @version 2018-08-14/GGB - Function created.

  CTargetComet::CTargetComet(CTargetComet const &toCopy) : CTargetAstronomy(toCopy), elements_(toCopy.elements_)
  {
  }

  /// @brief Creates a copy of this.
  /// @returns A constructed object with the same data as this.
  /// @throws std::bad_alloc
  /// @version 2018-09-14/GGB - Function created

  std::unique_ptr<CTargetAstronomy> CTargetComet::createCopy() const
  {
    std::unique_ptr<CTargetAstronomy> returnValue = std::make_unique<CTargetComet>(*this);

    return returnValue;
  }
}
