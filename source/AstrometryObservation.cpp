//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstrometryObservation
// SUBSYSTEM:						Observation Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost
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
// OVERVIEW:						This file provides classes for managing astrometry observations
//
//
// CLASSES INCLUDED:		CAstrometryObservation - Class for storing astrometry related information.
//
// CLASS HIERARCHY:     CObservation
//                        - CAstrometryObservation
//
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. Functions from other libraries, referenced by this library can be written in C.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-06-08 GGB - Created CAstrometryObservation files, removed classes from Observation.(h)(cpp)
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2011-12-23 GGB  - Start Development of classes in file ACL::Observation
//
//*********************************************************************************************************************************

#include "../include/AstrometryObservation.h"

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CAstrometryObservation
  //
  //*******************************************************************************************************************************

  /// @brief Constructor for class.
  /// @param[in] name - The name of the object or observation.
  /// @throws None.
  /// @version 2013-04-01/GGB - Function created.

  CAstrometryObservation::CAstrometryObservation(std::string const &name) : CObservation(name), objectType_(EOT_NONE),
    coordinateSource_(ECS_NONE)
  {
  }

  /// Copy constructor.
  //
  // 2013-06-08/GGB - Function created.

  CAstrometryObservation::CAstrometryObservation(CAstrometryObservation const &toCopy) : CObservation(toCopy),
    objectType_(toCopy.objectType_), coordinateSource_(toCopy.coordinateSource_)
  {
  }

  CAstrometryObservation::CAstrometryObservation(std::shared_ptr<CTargetAstronomy> ta) : CObservation(ta)
  {

  }

  /// @brief Class destructor
  /// @throws None.
  /// @version 2011-07-16/GGB - Function created.

  CAstrometryObservation::~CAstrometryObservation()
  {
  }

  /// @brief Creates a copy of *this
  /// @throws std::bad_alloc
  /// @version 2018-10-08/GGB - Updated to std::unique_ptr return value.
  /// @version 2013-06-08/GGB - Function created.

  std::unique_ptr<CObservation> CAstrometryObservation::createCopy() const
  {
    return std::make_unique<CAstrometryObservation>(*this);
  }

}  // namespace ACL
