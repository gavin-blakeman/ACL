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
//                      Copyright 2016-2020 Gavin Blakeman.
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

  // Miscellaneous library header files.

#include "boost/filesystem.hpp"
#include <MCL>

  // ACL library header files.

#include "config.h"
#include "error.h"
#include "targetAstronomy.h"

namespace ACL
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

  class CTargetMinorPlanet : public CTargetAstronomy
  {
  private:
    static boost::filesystem::path filePath;
    static boost::filesystem::path fileName;

    std::string designation_;       ///< The MP designation. (From MPCORB)
    SMPCORB elements_;

    CAstroTime epoch_;              ///< Epoch
    MCL::angle_t M0_;               ///< Mean anomoly at the epoch (degrees)
    MCL::angle_t omega_;            ///< Argument of perihelion degrees.

    FP_t a_;                        ///< Semi-major axis
    FP_t e_;                        // Eccentricity
    MCL::angle_t i_;                // Inclination
    MCL::angle_t OMEGA_;            // Longitude of ascending node

    MCL::angle_t n_;                // Mean daily motion

    CTargetMinorPlanet() = delete;

    static void parseLine(std::string const &, SMPCORB &);
    static bool loadMPData(std::string const &, SMPCORB &);

  protected:
  public:
    CTargetMinorPlanet(CTargetMinorPlanet const &);
    CTargetMinorPlanet(std::string const &);

    virtual ~CTargetMinorPlanet() {}

      // Static functions

    static void setFilePath(boost::filesystem::path const &);
    static void setFileName(boost::filesystem::path const &);

      // Factory functions

    virtual std::unique_ptr<CTargetAstronomy> createCopy() const;
    static std::unique_ptr<CTargetMinorPlanet> create(std::string const&);

      // Information functions

    virtual ETargetType targetType() const override { return TT_MINORPLANET; }

      // Getter functions

    SMPCORB &elements() { return elements_; }

      // Position functions

    virtual CAstronomicalCoordinates positionCatalog() const { CODE_ERROR; }
    virtual CAstronomicalCoordinates positionICRS(CAstroTime const &) const override;
    virtual SObservedPlace positionObserved(CAstroTime const &, CGeographicLocation const &, CWeather const &) const override;

      // Information functions

    virtual void calculateRSTTime(CAstroTime const &, CGeographicLocation const &, CWeather const &, TJD &, TJD &, TJD *) {}
    virtual CStellarMagnitude &magnitude() const {}
  };


}

#endif // ACL_TARGETMINORPLANET_H
