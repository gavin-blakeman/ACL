//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								geographicLocation
// SUBSYSTEM:						Observation locations
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2017-2018 Gavin Blakeman.
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
// OVERVIEW:						A class to support geographic locations
//
// CLASSES INCLUDED:		CGeographicLocation
//
// CLASS HIERARCHY:     CGeographicLocation
//
// HISTORY:             2017-07-22 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#include "../include/geographicLocation.h"

#include <GCL>

namespace ACL
{
  /// @brief Copy constructor.
  /// @param[in] toCopy - The instance to make a copy of.
  /// @throws std::bad_alloc
  /// @version 2017-08-26/GGB - Function created.

  CGeographicLocation::CGeographicLocation(CGeographicLocation const &toCopy) : latitude_(toCopy.latitude_),
    longitude_(toCopy.longitude_), altitude_(toCopy.altitude_)
  {
  }

  /// @brief Constructor.
  /// @details Sets the values. The values are checked and if out of range, an exception will be thrown.
  ///          (Prior revisions normalised the values, rather than throwing an exception, but with the change to ensuring that
  ///           the values are valid, it is better to throw
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2013-06-22/GGB - Removed the mutable values for tracking the validity.
  /// @version 2012-01-02/GGB - Added support for validating locations. Removed the normalisation of values.
  /// @version 2011-07-11/GGB - Function created.

  CGeographicLocation::CGeographicLocation(FP_t latitude, FP_t longitude, int altitude)
  {
    RUNTIME_ASSERT(ACL, latitude >= -90 && latitude <= 90, "Parameter latitude not within reasonable bounds.");
    RUNTIME_ASSERT(ACL, longitude >= -180 && longitude <= 180, "Parameter longitude not within reasonable bounds");

    latitude_ = latitude;
    longitude_ = longitude;
    altitude_ = altitude;
  }

  /// @brief Copy operator for CGeographicLocation.
  /// @param[in] toCopy - The instance to make a copy of.
  /// @throws None.
  /// @version 2017-08-04/GGB - Removed timeZone.
  /// @version 2013-06-22/GGB - Removed the mutable values for tracking the validity.
  /// @version 2012-01-02/GGB - Added support for testing if valid.
  /// @version 2011-07-11/GGB - Function created.

  CGeographicLocation &CGeographicLocation::operator=(ACL::CGeographicLocation const &toCopy)
  {
    if (this != &toCopy)
    {
      latitude_ = toCopy.latitude_;
      longitude_ = toCopy.longitude_;
      altitude_ = toCopy.altitude_;
    };

    return (*this);
  }

  /// @brief Equality operator. Checks that all the values are the same.
  /// @param[in] rhs - The location to check against.
  /// @returns true - The locations are the same
  /// @returns false - The locations are not the same.
  /// @throws None.
  /// @version 2017-08-04/GGB - Removed timeZone.
  /// @version 2017-07-22/GGB - Added noexcept
  /// @version 2011-07-16/GGB - Function created.

  bool CGeographicLocation::operator==(CGeographicLocation const &rhs) const noexcept
  {
    return ( (latitude_ == rhs.latitude_) &&
             (longitude_ == rhs.longitude_) &&
             (altitude_ == rhs.altitude_) );
  }

  /// @brief Returns the altitude, if it is valid.
  /// @returns The altitude in meters.
  /// @version 2017-07-22/GGB - Added noexcept
  /// @version 2013-06-22/GGB - Removed the mutable values for tracking the validity.
  /// @version 2012-01-02/GGB - Function created.

  std::int16_t CGeographicLocation::altitude() const noexcept
  {
    return altitude_;
  }

  /// @brief Returns the latitude value - if it is valid.
  /// @returns The latitude (d + m/60 + s/3600)
  /// @throws None
  /// @version 2017-07-22/GGB - Added noexcept
  /// @version 2013-06-22/GGB - Removed the mutable values for tracking the validity.
  /// @version 2012-01-02/GGB - Function created

  FP_t CGeographicLocation::latitude() const noexcept
  {
    return latitude_;
  }

  /// @brief Returns the longitude value (if it is valid)
  /// @returns The longitude (d + m/60 + s/3600)
  /// @throws None.
  /// @version 2017-07-22/GGB - Added noexcept
  /// @version 2013-06-22/GGB - Removed the mutable values for tracking the validity.
  /// @version 2012-01-02/GGB - Function created.

  FP_t CGeographicLocation::longitude() const noexcept
  {
    return longitude_;
  }

  /// @brief Normalises the latitude to +90 to -90 and the longitude to +180>x>-180
  /// @thows None.
  /// @version 2017-08-04/GGB - Removed time zone.
  /// @version 2017-07-22/GGB - Use std::fmod for +ve and -ve
  /// @version 2009-10-24/GGB - Added support for the TimeZone.
  /// @version 2009-10-06/GGB - Function created.

  void CGeographicLocation::Normalise(void)
  {
    latitude_ = std::fmod(latitude_, 90);
    longitude_ =  std::fmod(longitude_, 180);
  }

  /// @brief Sets the location to the passed values.
  /// @param[in] latitude - The latitude of the location
  /// @param[in] longitude - The longitude of the location.
  /// @param[in] altitude - The altitude of the location.
  /// @throws None.
  /// @version 2017-07-22/GGB - Just call the full setLocation function.
  /// @version 2013-06-22/GGB - Removed the mutable values for tracking the validity.
  /// @version 2012-01-02/GGB - Added support for validity
  /// @version 2009-10-24/GGB - Added auto-calc of TimeZone.
  /// @version 2009-10-06/GGB - Function created.

  void CGeographicLocation::setLocation(FP_t latitude, FP_t longitude, int altitude)
  {
    setLocation(latitude, longitude, altitude);
  }

} // namespace ACL
