//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Observation
// SUBSYSTEM:						Observation Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost, SOFA, Novas
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2011-2018 Gavin Blakeman.
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
// OVERVIEW:						This file provides classes for managing actual observations
//
//
// CLASSES INCLUDED:		CObservation - Class for integrating observation information
//
// CLASS HIERARCHY:     CObservation  (CObject, CAstroTime, TLocation, CWeather
//
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. Functions from other libraries, referenced by this library can be written in C.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-08 GGB - Removed CAstrometryObservation to seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2011-12-23 GGB - Start Development of classes in file ACL::Observation
//
//*********************************************************************************************************************************

#ifndef ACL_OBSERVATION_H
#define ACL_OBSERVATION_H

  // ACL header files

#include "AstroClass.h"
#include "AstronomicalTime.h"
#include "config.h"
#include "TargetAstronomy.h"

  // Standard libraries

#include <memory>

  // Boost Library

#include "boost/optional.hpp"

  // Math Class Library

#include <MCL>

  // Standard header files

#include <cstddef>

namespace ACL
{

  class CObservation;                                       // Needed for the factory function.
  typedef std::shared_ptr<CObservation> SPObservation;

  /// @class The CObservation class is used for storing observation data.
  /// An observation will typically include a target (object) and information about the place of observation.
  /// (Time, weather, geographic location).
  /// Information relating to the location of the object in the image is also stored.

  class CObservation
  {
  private:
  protected:
    SPTargetAstronomy targetObject;         ///< The target object.
    PAstroTime time_;                       ///< The time of the observation.
    PLocation location_;                    ///< The location the observation was made from.
    PWeather weather_;                      ///< The weather at the time of the observation.
    MCL::TPoint2D<FP_t> CCDCoordinates_;    ///< The CCD coordinates of the observation.
    boost::optional<CAstronomicalCoordinates> observedCoordinates_;  ///< The observed coordinates of the observation (if known)

  public:
    CObservation(std::string const &);
    CObservation(CObservation const &);
    CObservation(SPTargetAstronomy &);
    virtual ~CObservation();

    bool operator==(std::string const &) const;
    bool operator!=(std::string const &) const;

    virtual SPObservation createCopy() const;

    void objectName(std::string const &);
    std::string objectName() const;

      // Information functions

    void CCDCoordinates(MCL::TPoint2D<FP_t> const &newCoordinates);
    MCL::TPoint2D<FP_t> &CCDCoordinates();

    virtual bool isClose(MCL::TPoint2D<FP_t> const &center, int r) const;

    virtual void observedCoordinates(CAstronomicalCoordinates const &);
    virtual boost::optional<CAstronomicalCoordinates> &observedCoordinates();
  };

  typedef std::shared_ptr<CObservation> SPObservation;


}

#endif    // ACL_OBSERVATION_H
