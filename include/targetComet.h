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

#ifndef ACL_TARGETCOMET_H
#define ACL_TARGETCOMET_H

  // Standard C++ library header files.

#include <cstdint>
#include <memory>
#include <string>

  // Miscellaneous library header files

#include "boost/filesystem.hpp"

  // ACL library header files

#include "error.h"
#include "targetAstronomy.h"

namespace ACL
{
  struct SCometElements
  {
    std::string cometNumber;                  ///< Periodic comet number
    std::string orbitType;                    ///< Orbit type
    std::string provisionalDesignation;       ///< Provisional designation
    std::uint16_t perihelionPassageYear;      ///< Year of perihelion passage
    std::uint16_t perihelionPassageMonth;     ///< Month of perihelion passage
    float perihelionPassageDay;               ///< Day of perihelion passage (TT)
    double perihelionDistance;                ///< Perihelion distance (AU)
    double orbitalEccentricity;               ///< Orbital eccentricity.
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

  class CTargetComet : public CTargetAstronomy
  {
  private:
    static boost::filesystem::path filePath;
    static boost::filesystem::path fileName;
    static std::map<std::string, std::uint32_t> cometCache;

    SCometElements elements_;

    CTargetComet() = delete;
    CTargetComet(CTargetComet &&) = delete;

    static void parseLine(std::string const &, SCometElements &);

  protected:
  public:
    CTargetComet(CTargetComet const &);
    CTargetComet(std::string const &);

      // Static functions

    static void setFilePath(boost::filesystem::path const &);
    static void setFileName(boost::filesystem::path const &);
    static bool loadCometData(std::string const &, SCometElements &);

      // Factory functions

    virtual std::unique_ptr<CTargetAstronomy> createCopy() const;
    static std::unique_ptr<CTargetAstronomy> create(std::string const &);

      // Information functions

    virtual ETargetType targetType() const override { return TT_COMET; }

    virtual CAstronomicalCoordinates positionCatalog() const { CODE_ERROR; }
    virtual CAstronomicalCoordinates positionICRS(CAstroTime const &) const { CODE_ERROR; }
    virtual SObservedPlace positionObserved(CAstroTime const &, CGeographicLocation const &, CWeather const &) const override {}

      // Information functions

    virtual void calculateRSTTime(CAstroTime const &, CGeographicLocation const &, CWeather const &, TJD &, TJD &, TJD *) {}
    virtual CStellarMagnitude &magnitude() const {}
  };
}

#endif // ACL_TARGETCOMET_H
