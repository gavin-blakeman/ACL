//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								observatoryInformation
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
// OVERVIEW:						A class to store observatory information.
//
// CLASSES INCLUDED:		CObservatory
//
// CLASS HIERARCHY:     CGeographicLocation
//                        CObservatory
//
// HISTORY:             2018-02-02 GGB - Changed type of daylight saving to a uint32_t to allow indexing in databases.
//                      2017-07-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#include "../include/observatoryInformation.h"

namespace ACL
{
  /// @brief Copy constructor for the class.
  /// @param[in] toCopy - The instance to copy.
  /// @throws std::bad_alloc
  /// @version 2017-08-26/GGB - Function created.

  CObservatory::CObservatory(CObservatory const &toCopy) : CGeographicLocation(toCopy), siteName_(toCopy.siteName_),
    IAUCode_(toCopy.IAUCode_), timeZone_(toCopy.timeZone_), daylightSaving_(toCopy.daylightSaving_)
  {
  }

  /// @brief Constructor for the site.
  /// @param[in] siteName - The name of the site.
  /// @param[in] latitude - The latitude of the site.
  /// @param[in] longitude - The longitude of the site.
  /// @param[in] altitude - The altitude of the site.
  /// @param[in] timeZone - The time zone of the site.
  /// @throws None.
  /// @version 2012-01-02/GGB - Function created

  CObservatory::CObservatory(std::string const &siteName, FP_t latitude, FP_t longitude, int altitude, FP_t timeZone) :
    CGeographicLocation(latitude, longitude, altitude), siteName_(siteName), timeZone_(timeZone)
  {
  }

  /// @brief Creates a copy of this instance.
  /// @throws std::bad_alloc
  /// @version 2017-08-26/GGB - Function created.

  CObservatory *CObservatory::createCopy() const
  {
    return new CObservatory(*this);
  }

} // namespace ALC
