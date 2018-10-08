//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Astrometry.h
// SUBSYSTEM:						Astronometry Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, Boost, SOFA
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
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
//
// OVERVIEW:						Classes supporting astrometric reductions.
//
//                      PLATE CONSTANTS:
//                      When calculating plate constants, only stellar objects can be used as reference points. These stellar
//                      objects should also be stars and not extended objects.
//
// CLASSES INCLUDED:		CAstrometry					    - A class to perform astrometric reductions.
//                      CAstrometryObservation  - Class to store astrometry observation data. This includes object information,
//                                                ccd coordinates.
//                      SPlateData -
//
// CLASS HIERARCHY:     CAstrometry
//                      SPlateData
//                      CAstrometryObservation
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2011-06-26 GGB - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROMETRY_H
#define ACL_ASTROMETRY_H

  // Standard C++ library header files.

#include <memory>
#include <vector>

  // ACL library header files

#include "AstroClass.h"
#include "AstrometryObservation.h"
#include "AstronomicalCoordinates.h"

  // Miscellaneous library header files.

#include <MCL>

namespace ACL
{

  struct SPlateData
  {
    double F;                   // Focal Length;
    double a, b, c, d, e, f;
  };

  /// The CAstrometry class is used for performing astrometry on an image.

  /// In order to perform astrometry, the calling application must set the observation parameters, (weather, location, time),
  /// provide a minimum of three reference points, and provide a number of target objects with their image reference points.
  /// The class can then determine the plate constants, as well as providing observed positions for the taget objects.
  /// Reference Object -> This is an object (star etc) whose position is well known. The position can be reduced to an observed
  ///                     position.
  /// Target Object -> An object (asteroid, star etc) whose position is not known but is sought. The posision can be determined
  ///                  from the plate coordinates.

  class CAstrometry
  {
  private:
    typedef std::vector<std::shared_ptr<CAstrometryObservation>> DAstrometryStore;

    DAstrometryStore _references;
    DAstrometryStore _targets;
    PAstroTime observationTime;                            // Observation date/time (TT)
    PLocation observationLocation;
    PWeather observationWeather;
    DAstrometryStore::iterator referenceIterator;
    DAstrometryStore::iterator targetIterator;

    std::unique_ptr<SPlateData> plateData;
    CAstronomicalCoordinates plateCenter;

    mutable bool bPDV;                                          // Plate data valid.
    mutable bool bRM;                                           // Requisites met.
    mutable bool bPCD;                                          // Plate center defined.

  protected:
    virtual bool plateDataValid() const { return bPDV; }
    virtual void plateDataValid(bool pdv) { bPDV = pdv; }

    virtual bool requisitesMet() const { return bRM; }
    virtual void requisitesMet(bool rm) { bRM = rm; }

    virtual bool plateCenterDefined() const { return bPCD; }
    virtual void plateCenterDefined(bool pcd) { bPCD = pcd; }

  public:
    CAstrometry();
    virtual ~CAstrometry();

      // Load/Save functions

    virtual void loadFromFITS(fitsfile *);
    virtual void writeToFITS(fitsfile *) const;

      // Reference object functions

    virtual bool referenceAdd(std::unique_ptr<CAstrometryObservation>);
    virtual bool referenceRemove(std::string const &);
    virtual DAstrometryStore::size_type referenCAstronomicalCoordinatesount() const;

      // Target object functions

    virtual bool targetAdd(std::unique_ptr<CAstrometryObservation>);
    virtual bool targetRemove(std::string const &);
    virtual DAstrometryStore::size_type targetCount() const;

      // Observation functions

    virtual void setObservationLocation(double lat, double lon, double alt, double timeZone);
    virtual void setObservationLocation(PLocation const location);

    virtual void setObservationParameters(PAstroTime JD, double temperature, double pressure);
    virtual void setObservationParameters(PAstroTime JD, PWeather weather);

    virtual void setObservationDate(PAstroTime const JD) {observationTime = JD;}

    virtual void setWeather(PWeather const weather);

    virtual void setPlateCenter(CAstronomicalCoordinates const &pltCenter);

      // Calculation functions

    virtual void calculatePlateConstants();
    virtual SPlateData *plateConstants() const;
    virtual bool checkRequisites() const;

      // Image functions

    virtual void imageRotate(double);
    virtual void imageFlip(long);
    virtual void imageFlop(long);

      // Iteration functions

    virtual CAstrometryObservation &referenceFirst();
    virtual CAstrometryObservation &referenceNext();
    virtual CAstrometryObservation &targetFirst();
    virtual CAstrometryObservation &targetNext();
  };

}  // namespace ACL

#endif  // ACL_ASTROMETRY_H
