﻿//*********************************************************************************************************************************
//
// PROJECT:			        Astronomy Class Library
// FILE:				        AstronomicalCoordinates
// SUBSYSTEM:		        Coordinate System Classes
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	boost, GCL, MCL, PCL
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

#include "include/AstronomicalCoordinates.h"

  // Standard C++ library header files

#include <cmath>

  // Miscellaneous library header files.

#include <GCL>

  // ACL library header files.

#include "include/AstroFunctions.h"

namespace ACL
{
  /// @brief Function to write coordinates to stream.
  /// @param[in] out: The output stream.
  /// @param[in] rhs: The coordinates to write.
  /// @returns Reference to the stream.
  /// @throws None.
  /// @version 2018-09-05/GGB - Function created.

  std::ostream &operator<<(std::ostream &out, CAstronomicalCoordinates const &rhs)
  {
    out << rhs.RA() << ", " << rhs.DEC() << ", ";

    return (out);
  }

  /// @brief Null constructor for the class.
  /// @throws std::bad_alloc
  /// @version 2018-09-05/GGB - Removed Reference system, epoch and equinox.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Function created.

  CAstronomicalCoordinates::CAstronomicalCoordinates() : coordinates_(0, 0)
  {
  }

  /// @brief Copy constructor for the class.
  /// @param[in] toCopy: The instance to copy.
  /// @throws None.
  /// @version 2018-09-05/GGB - Removed Reference system, epoch and equinox.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Changed back to two values with specified storage and included reference system and epoch.
  /// @version 2015-09-27/GGB - Converted class to use MCL::CAngle
  /// @version 2013-03-03/GGB - Underlying storage changed from double to MCL::TPoint2D<>
  /// @version 2005-07-01/GGB - Function created

  CAstronomicalCoordinates::CAstronomicalCoordinates(CAstronomicalCoordinates const &toCopy) : coordinates_(toCopy.coordinates_)
  {
  }

  /// @brief Class constructor
  /// @param[in] ra: The Right ascension (hms)
  /// @param[in] dec: The declination (dms)
  /// @throws None.
  /// @version 2018-09-05/GGB - Removed Reference system, epoch and equinox.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Changed back to two values with specified storage and included reference system and epoch.
  /// @version 2015-09-27/GGB - Converted class to use MCL::CAngle
  /// @version 2013-03-03/GGB - Underlying storage changed from double to MCL::TPoint2D<>
  /// @version 2011-07-03/GGB - Function created.

  CAstronomicalCoordinates::CAstronomicalCoordinates(MCL::CAngle RA, MCL::CAngle DEC)
    : coordinates_(RA, DEC)
  {
  }

  /// @brief Class constructor
  /// @param[in] coords: The RA/Dec pair of coordinates.
  /// @throws None.
  /// @version 2018-09-05/GGB - Removed Reference system, epoch and equinox.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Changed back to two values with specified storage and included reference system and epoch.
  /// @version 2015-09-27/GGB - Converted class to use MCL::CAngle
  /// @version 2013-03-03/GGB - Function created.

  CAstronomicalCoordinates::CAstronomicalCoordinates(MCL::TPoint2D<MCL::CAngle> const &toCopy)
    : coordinates_(toCopy)
  {
  }

  /// @brief Constructor using cartesian coordinate forms.
  /// @param[in] cart: The cartesian coordinates.
  /// @throws std::bad_alloc
  /// @version 2018-09-05/GGB - Removed Reference system, epoch and equinox.
  /// @version 2018-08-23/GGB - Function created.

  CAstronomicalCoordinates::CAstronomicalCoordinates(MCL::TVector3D<FP_t> const &cart)
    : coordinates_()
  {
    coordinates_.x() = std::acos(cart.z() / cart.length());
    coordinates_.y() = std::atan2(cart.y(), cart.x());
  }

  /// @brief Copy operator for the class.
  /// @param[in] toCopy: The instance to copy.
  /// @throws None.
  /// @version 2018-09-05/GGB - Removed Reference system, epoch and equinox.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Changed back to two values with specified storage and included reference system and epoch.
  /// @version 2015-09-27/GGB - Converted class to use MCL::CAngle
  /// @version 2013-03-03/GGB - Underlying storage changed from double to MCL::TPoint2D<>
  /// @version 2011-06-30/GGB - Function created.

  CAstronomicalCoordinates &CAstronomicalCoordinates::operator=(CAstronomicalCoordinates const &toCopy)
  {
    coordinates_ = toCopy.coordinates_;

    return (*this);
  }

  /// @brief Copy operator for a set of coordinates.
  /// @param[in] toCopy: The coordinates to use.
  /// @version 2018-09-05/GGB - Removed Reference system, epoch and equinox.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Changed back to two values with specified storage and included reference system and epoch.
  /// @version 2015-09-27/GGB - Converted class to use MCL::CAngle
  /// @version 2013-02-03/GGB - Function created.

  CAstronomicalCoordinates &CAstronomicalCoordinates::operator=(MCL::TPoint2D<FP_t> const &toCopy)
  {
    coordinates_.x(toCopy.x());
    coordinates_.y(toCopy.y());

    return (*this);
  }

  /// @brief Sets the coordinates to the specified values.
  /// @param[in] ra: The new RA value
  /// @param[in] dec: The new declination value.
  /// @throws None.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2016-04-25/GGB - Bug# 1574420
  /// @version 2015-09-27/GGB - Changed class to use MCL::CAngle as storage.
  /// @version 2013-03-03/GGB - Underlying storage changed from double to MCL::TPoint2D<>
  /// @version 2011-07-03/GGB - Function created.

  void CAstronomicalCoordinates::operator()(FP_t ra, FP_t dec)
  {
    coordinates_ = MCL::TPoint2D<MCL::CAngle>(ra, dec);
  }

  /// @brief Sets the coordinates to the specified values.
  /// @param[in] ra: The new RA value
  /// @param[in] dec: The new declination value.
  /// @throws None.
  /// @version 2017-09-22/GGB - Function created.

  void CAstronomicalCoordinates::operator()(MCL::CAngle const &ra, MCL::CAngle const &dec)
  {
    coordinates_(ra, dec);
  }

  /// @brief Returns the Dec value.
  /// @returns The Declination value as an angle.
  /// @throws None.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Changed back to two values with specified storage and included reference system and epoch.
  /// @version 2015-09-27/GGB - Converted class to use MCL::CAngle
  /// @version 2013-03-03/GGB - Underlying storage changed from double to MCL::TPoint2D<>
  /// @version 2011-07-03/GGB - Function created.

  MCL::CAngle CAstronomicalCoordinates::DEC() const
  {
    return coordinates_.y();
  }

  /// @brief Returns the RA value.
  /// @returns The RA coordinate as an angle.
  /// @throws None.
  /// @version 2017-09-22/GGB - Updated to use MCL::CAngle
  /// @version 2017-08-01/GGB - Changed back to two values with specified storage and included reference system and epoch.
  /// @version 2015-09-27/GGB - Changed class to use MCL::CAngle as storage.
  /// @version 2013-03-03/GGB - Underlying storage changed from double to MCL::TPoint2D<>
  /// @version 2011-07-03/GGB - Function created.

  MCL::CAngle CAstronomicalCoordinates::RA() const
  {
    return coordinates_.x();
  }

  /// Insertion operator for the CAstronomicalCoordinates class.
  /// @returns A string representing the coordinates.
  /// @version 2015-09-27/GGB - Moved into the class and made a string function.
  /// @version 2013-02-24/GGB - Function created

  std::string CAstronomicalCoordinates::toString() const
  {
    std::string returnValue;

    returnValue = GCL::sprintfHMS(RA().hours()) + ", " + GCL::sprintfDMS(DEC().degrees());

    return returnValue;
  }

}
