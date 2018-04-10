//*********************************************************************************************************************************
//
// PROJECT:			        Astronomy Class Library
// FILE:				        AstronomicalCoordinates
// SUBSYSTEM:		        Coordinate System Classes
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	boost, GCL, MCL, PCL
// AUTHOR:              Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2005-2017 Gavin Blakeman.
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
// REMARKS:             1) This series of classes was initially developed for the TCS application. All classes were used extensively
//                        with windows and integrated with Windows. As development of the classes continues, the linkages with
//                        Windows will all be deprecated and will excluded with definition statements.
//                      2) This library is written to be as close as possible to vanilla C++, and to not rely on operating system
//                        support.
//
// CLASSES INCLUDED:    CAstronomicalCoordinates - Equitorial coordinates
//
// CLASS HIERARCHY:     CAstronomicalCoordinates
//
// RELEASE HISTORY:     2015-09-27/GGB - Function moved into seperate file from CAstroClass.
//
//*********************************************************************************************************************************

#ifndef ACL_COORDINATES_H
#define ACL_COORDINATES_H

#include "config.h"
#include "julianDay.h"

  // Standard C++ libraries

#include <string>

  // Miscellaneous libraries

#include <MCL>

namespace ACL
{
  /// @page page1 Coordinate Systems
  /// @tableofcontents
  /// There are a number of classes that are provided for dealing with coordinates and locations of objects (stars). These pages
  /// will provide an overview of the classes and the functionality provided by the classes.
  /// @section sec1 Angles
  /// Support for angles is provided by the MCL::CAngle class in the MCL library. The MCL::CAngle class stores an angle as a value
  /// in radians. There are a nunmber of constructors provided to initialise an angle with various different values
  /// (dd.ddd, dd.mmss etc).<br>
  /// There are also functions provided to typecast the value to radians and to output as a string value.<br>
  /// The CAngle class has a conveniance typedef as MCL::angle_t. This class is used as the storage class for all classes within
  /// the ACL library that need to store angle values.
  /// @section sec2 Celestial Coordinates
  /// The class CAstronomicalCoordinates is provided to store celestial coordinates. (IE the descriptor for a location on the
  /// celestial sphere. The class always stores the value as an Equatorial pair (RA, Dec) of coordinates and converts to other
  /// coordinate types as required.<br>
  /// The class does not store any equinox or epoch information. This information is stored in a higher level class that includes
  /// the functionality.<br>
  /// The CAstronomicalCoordinates type is used as a storage class for other classes that need coordinates.
  /// @section sec3 Coordinate Classes
  /// @subsection ss1 Base Class - CAstronomicalObject
  /// The astronomical object class stores basic information about an astronomical object. An astronomical object can be a solar
  /// system object (orbiting around the sun), or a stellar object (not orbiting the sun). Classes are also envisaged for
  /// exo-plants (orbiting around another star) and moons (Orbiting around a solar system object).<br>
  /// The astronomical object is the base class used for storing information. It includes a number of virtual functions that need
  /// to be initialised by the descendent classes.<br>
  /// The basic information stored by the astronomical object is the name of the object.
  /// @subsection ss2 CSolarObject
  /// The position descriptor for a solar system object is stored in the class CSolarObject. Solar objects need a very different set
  /// of information to describe their position than do Stellar objects.
  /// @subsection ss3 Stellar objects - CStellarObject
  /// The CStellarObject class is used for storing information about stellar objects. This includes the information about the
  /// object (magnitude, type of object, parallax, proper motion etc) and the coordinates of the object.<br>
  /// Member functions are also provided to precess the objects location to specific time/dates/locations, change coordinate types
  /// (Alt-Az etc) and to parse information from the SIMBAD database.

  FP_t const J2000 = 2451545.0;   ///< J2000.0 Epoch Date

  /// @brief Coordinate class
  ///
  ///

  enum EReferenceSystem
  {
    RS_NONE,      ///< No reference system specified
    RS_ICRS,      ///< Using the ICRS reference system. This is the system in use today.
    RS_FK4,       ///< Using the FK4 reference system. This is only used in older catalogs.
    RS_FK5        ///< Using the FK5 reference system. This is used in more modern catalogs.
  };

  class CAstronomicalCoordinates
  {
  private:
    MCL::TPoint2D<MCL::CAngle> coordinates_;
    EReferenceSystem referenceSystem_;                ///< The reference system that the coordinates refer to
    TJD epoch_;                                       ///< Epoch specified in Julian Days.
    TJD equinox_;                                     ///< The equinox of the system (if required)

  protected:
  public:
    CAstronomicalCoordinates();
    CAstronomicalCoordinates(CAstronomicalCoordinates const &);
    CAstronomicalCoordinates(MCL::CAngle, MCL::CAngle, EReferenceSystem = RS_FK5, FP_t = J2000);
    CAstronomicalCoordinates(MCL::TPoint2D<MCL::CAngle> const &, EReferenceSystem = RS_FK5, FP_t = J2000);

    virtual CAstronomicalCoordinates &operator=(CAstronomicalCoordinates const &);
    virtual CAstronomicalCoordinates &operator=(MCL::TPoint2D<FP_t> const &);

    inline void setReferenceSystem(EReferenceSystem newReferenceSystem) { referenceSystem_ = newReferenceSystem;}
    inline void setEpoch(TJD const &ne) { epoch_ = ne; }
    inline void setEquinox(TJD const ne) { equinox_ = ne; }

    EReferenceSystem getReferenceSystem() const { return referenceSystem_; }

    virtual void operator()(FP_t, FP_t);
    virtual void operator()(MCL::CAngle const &, MCL::CAngle const &);

    virtual void RA(FP_t const &ra) { coordinates_.x(ra); }
    virtual void DEC(FP_t const &dec) { coordinates_.y(dec); }

    virtual MCL::CAngle RA() const;
    virtual MCL::CAngle DEC() const;

    virtual std::string toString() const;

    friend std::ostream &operator<<(std::ostream &, CAstronomicalCoordinates const &);
  };



}

#endif // COORDINATES_H
