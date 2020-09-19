//*********************************************************************************************************************************
//
// PROJECT:			        Astronomy Class Library
// FILE:				        Astronomy Targets
// SUBSYSTEM:		        Astronomical objects
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	GCL, MCL, PCL
// AUTHOR:              Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2005-2020 Gavin Blakeman.
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
// OVERVIEW:            These classes provide the helper classes for several categories of astronomical calculations
//
// CLASSES INCLUDED:    CTargetAstronomy  - Astronomy target (Object)
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetSolar
//
// RELEASE HISTORY:     2016-03-24 GGB - Simplified class interface
//                      2015-09-27/GGB - Coordinate classes moved into this file from AstroClass
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2010-05-05 GGB -  1) All windows related functions have been removed as part if the change over to the Qt
//                                          API platform. This has been done to facilitate the move to a multi-platform environment.
//                                        2) The intention with the changes are also to make this software platform independant to
//                                          enable it's use as a general library with any system.
//                                        3) Removed class CAstronomicalCalc into file CAstroRT
//                      2009-11-05 GGB -  1) The library has been modified to make use of the IAU SOFA library of functions.
//                                        2) Functions making use of TTime and TDate that are affected by (1) above have been
//                                          deprecated.
//                                        3) Functions using LPSYSTEMTIME or SYSTEMTIME have been deprecated.
//                                        4) The intent is to deprecate all TTime and TDate functions in the future.
//                                        5) The intent is to deprecate all functions that are windows specific in the future.
//                      2009-09-25 GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_TARGETASTRONOMY_H
#define ACL_TARGETASTRONOMY_H

  // Standard C++ library header files

#include <list>
#include <memory>
#include <vector>

  // Miscellaneous library header files.

#include <GCL>

  // ACL library header files

#include "AstroClass.h"
#include "common.h"
#include "geographicLocation.h"
#include "photometry.h"
#include "RST.h"
#include "weather.h"


namespace ACL
{
  enum ETargetType
  {
    TT_NONE = 0,
    TT_MAJORPLANET,
    TT_MINORPLANET,
    TT_COMET,
    TT_STELLAR,
  };

  /// The class is used for storing target (object) information and provides the methods to convert to apparent place.
  /// The CTargetAstronomy is a pure virtual class that is used as a base class for the stellar objects and the solar system
  /// objects.
  /// The class is not a proxy for an observation, but only for a target (object). The apparent place can be requested from the
  /// class, provided that a time, weather data and geographic location is provided.
  ///
  /// @note 1. It is important to reference the document sofa_ast_c.pdf in conjunction with this code. Particularly page 2,
  ///          "The Chain of astrometric transformations". This is not detailed in this documentation, but can be found in the
  ///          "TargetStellar.h" header file.
  ///

  class CTargetAstronomy
  {
  private:
  protected:
    std::vector<std::string> objectName_;             ///< List of object names. Object may have more than one name.
    //CAstronomicalCoordinates catalogCoordinates_;   ///< Catalog Coordinates of the object.

  public:
    CTargetAstronomy();
    CTargetAstronomy(CTargetAstronomy const &);
    CTargetAstronomy(std::string const &);
    virtual ~CTargetAstronomy() {}

      // Factory functions

    virtual std::unique_ptr<CTargetAstronomy> createCopy() const = 0;

      // Operators

    virtual bool operator==(std::string const &) const;

      // Information functions

    virtual std::string objectName() const;
    virtual ETargetType targetType() const = 0;

      // Data manipulation functions

    virtual void objectName(std::string const &);
    virtual void objectName(std::vector<std::string> const &);

      // Position Calculation functions.

    virtual CAstronomicalCoordinates positionCatalog() const = 0;
    virtual CAstronomicalCoordinates positionICRS(CAstroTime const &) const = 0;
    virtual SObservedPlace positionObserved(CAstroTime const &, CGeographicLocation const &, CWeather const *) = 0;

      // Information functions

    virtual void calculateRSTTime(CAstroTime const &, CGeographicLocation const &, CWeather const &, TJD &, TJD &, TJD *) = 0;
    virtual CStellarMagnitude &magnitude() const = 0;
  };

  typedef std::vector<std::shared_ptr<CTargetAstronomy>> DTargetAstronomy;

} // namespace ACL

#endif // ACL_TARGETASTRONOMY_H
