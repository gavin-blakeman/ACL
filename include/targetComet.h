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

#ifndef ACL_TARGETCOMET_H
#define ACL_TARGETCOMET_H

  // Standard C++ library header files.

#include <cstdint>
#include <memory>
#include <string>

  // ACL library header files

#include "CometEls.h"
#include "TargetAstronomy.h"

  // Miscellaneous library header files

#include "boost/filesystem.hpp"

namespace ACL
{
  class CTargetComet : public CTargetAstronomy
  {
  private:
    COMETELS::SCometElements elements_;

  protected:
  public:
    CTargetComet(boost::filesystem::path const &, std::string const &);

      // Information functions

    virtual std::string objectType() const { return "Comet"; }

    virtual CAstronomicalCoordinates positionICRS(CAstroTime const &) const {}
    virtual CAstronomicalCoordinates positionObserved(CAstroTime const &, CGeographicLocation const &, CWeather const &) {}
  };
}

#endif // ACL_TARGETCOMET_H
