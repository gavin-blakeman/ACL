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
// CLASS HIERARCHY:     CObservation  (CObject, CAstroTime, TLocation, CWeather)
//                        - CAstrometryObservation
//                        - CPhotometryObservation
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-06-08 GGB - Removed CAstrometryObservation to seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2011-12-23 GGB - Start Development of classes in file ACL::Observation
//
//*********************************************************************************************************************************

#include "../include/Observation.h"

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // CObservation
  //
  //*******************************************************************************************************************************

  /// @brief Constructor for the class.
  /// @param[in] name: The name of the object being observed.
  /// @throws std::bad_alloc
  /// @version 2016-05-03/GGB - Added pointer to the CTargetAstronomy class to capture observed object information.
  /// @version 2013-03-31/GGB - Function rewritten to use new intent of class.
  /// @version 2011-12-23/GGB - Function created.

  CObservation::CObservation(std::string const &name) : targetObject(), time_(), location_(),
    weather_()
  {
  }

  /// @brief Copy constructor for the class.
  /// @param[in] toCopy - The instance to create a copy of.
  /// @throws None.
  /// @version 2016-05-03/GGB - Added pointer to the CTargetAstronomy class to capture observed object information.
  /// @version 2013-06-08/GGB - Function created.

  CObservation::CObservation(CObservation const &toCopy) : targetObject(toCopy.targetObject), time_(toCopy.time_),
    location_(toCopy.location_), weather_(toCopy.weather_), CCDCoordinates_(toCopy.CCDCoordinates_),
    observedCoordinates_(toCopy.observedCoordinates_)
  {
  }

  /// @brief Constructor taking a target object.
  /// @param[in] target - The target object.
  /// @throws None.
  /// @version 2016-05-04/GGB - Function created.

  CObservation::CObservation(std::shared_ptr<CTargetAstronomy> target) : targetObject(target), time_(), location_(), weather_()
  {
  }

  /// @brief Ensure any dynamically allocated memory is freed.
  /// @throws None.
  /// @version 2013-03-31/GGB - Function created.

  CObservation::~CObservation()
  {
  }

  /// @brief Equality test against a std::string.
  /// @param[in] rhs - The string to compare against.
  /// @returns true - If the target object name is == to the string.
  /// @returns false - Otherwise.
  /// @throws None.
  /// @version 2013-08-11/GGB - Changed to a member function.
  /// @version 2013-04-06/GGB - Function created.

  bool CObservation::operator==(std::string const &rhs) const
  {
    return (*targetObject == rhs);
  }

  /// Non-equality test.
  //
  // 2013-08-28/GGB - Corrected bug with function. (Bug #1213349)
  // 2013-08-11/GGB - Function created.

  bool CObservation::operator!=(std::string const &rhs) const
  {
    return !(*targetObject == rhs);
  }

  // Returns a reference to the CCDCoordinates.
  //
  // 2011-12-11/GGB - Function created.

  MCL::TPoint2D<FP_t> &CObservation::CCDCoordinates()
  {
    return CCDCoordinates_;
  }

  /// @brief Sets the value of the CCDCoordinates.
  /// @param[in] newCoordinates: The new CCD coordinates for the observation.
  /// @throws None.
  /// @version 2013-04-07/GGB - Function created.

  void CObservation::CCDCoordinates(MCL::TPoint2D<FP_t> const &newCoordinates)
  {
    CCDCoordinates_ = newCoordinates;
  }

  /// @brief Creates a copy of *this.
  /// @returns A shared_ptr with a copy of *this.
  /// @throws std::bad_alloc
  /// @version 2018-10-08/GGB - Updated to have unique_ptr as return.
  /// @version 2013-06-08/GGB - Function created.

  std::unique_ptr<CObservation> CObservation::createCopy() const
  {
    return std::make_unique<CObservation>(*this);
  }

  /// Checks if a set of coordinates is close to the object.
  //
  // 2013-04-07/GGB - Function created.

  bool CObservation::isClose(MCL::TPoint2D<double> const &center, int r) const
  {
    double sr = sqrt( (float) MCL::pow2((center.x() - CCDCoordinates_.x())) + MCL::pow2((center.y() - CCDCoordinates_.y())) );

    if (sr < r)
    {
      return true;
    }
    else
    {
      return false;
    };
  }

  void CObservation::objectName(std::string const &on)
  {
    targetObject->objectName(on);
  }

  std::string CObservation::objectName() const
  {
    return targetObject->objectName();
  }

  /// @brief Returns the observed place of the observation.
  /// @returns The observed place.
  /// @throws None.
  /// @version 2012-01-12/GGB - Function created.

  std::optional<CAstronomicalCoordinates> &CObservation::observedCoordinates()
  {
    return observedCoordinates_;
  }

  /// @brief Sets the observed coordinates.
  /// @param[in] nc - The new coordinates.
  /// @throws None.
  /// @version 2013-04-07/GGB - Function Created.

  void CObservation::observedCoordinates(CAstronomicalCoordinates const &nc)
  {
    observedCoordinates_ = nc;
  }

}  // namespace
