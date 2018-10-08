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
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-08 GGB - Created CAstrometryObservation files, removed classes from Observation.(h)(cpp)
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2011-12-23 GGB - Start Development of classes in file ACL::Observation
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROMETRYOBSERVATION_H
#define ACL_ASTROMETRYOBSERVATION_H

// Standard C++ header files.

#include <cstddef>
#include <memory>

#include "Observation.h"

namespace ACL
{
  class CAstrometryObservation : public CObservation
  {
  private:
    enum EObjectType
    {
      EOT_NONE,
      EOT_FIXED,
      EOT_TRACK
    } objectType_;
    enum ECoordinateSource
    {
      ECS_NONE,
      ECS_REFERENCE,
      ECS_DERIVED
    } coordinateSource_;

  protected:
  public:
    CAstrometryObservation(std::string const &);
    CAstrometryObservation(CAstrometryObservation const &);
    CAstrometryObservation(std::shared_ptr<CTargetAstronomy>);
    virtual ~CAstrometryObservation();

    virtual std::unique_ptr<CObservation> createCopy() const;
  };

}

#endif // ACL_ASTROMETRY_H
