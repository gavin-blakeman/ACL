﻿//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								PhotometryObservation
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2020 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting various aspects of photometry and photometric
//											reduction.
//											The focus of this subsystem is on taking raw photometric observations and
//											and converting them to transformed photometric observations.
//
// CLASSES INCLUDED:		CPhotometryObservation - A class for storing photometry observation information.
//                      CPhotometryObject		- A class for storage of photometry related information.
//                      CPhotometryAperture  - Storage class for the 3 radii for Photometry.
//
// CLASS HIERARCHY:     CObservation
//                        - CPhotometryObservation.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-08 GGB - Moved CPhotometryObservation into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#include "include/PhotometryObservation.h"

  // Miscellaneous library header files.

#include "boost/locale.hpp"
#include <GCL>

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CPhotometryObservation
  //
  //*******************************************************************************************************************************

  /// @brief      Constructor for class.
  /// @param[in]  name: The name to assign to the object.
  /// @throws     None.
  /// @version    2013-04-01/GGB - Function created.

  CPhotometryObservation::CPhotometryObservation(std::string const &name) : CObservation(name)
  {
  }

  /// @brief      Copy constructor for class.
  //
  /// @version    2013-06-08/GGB - Function created.

  CPhotometryObservation::CPhotometryObservation(CPhotometryObservation const &toCopy) : CObservation(toCopy),
    sourceADU_(toCopy.sourceADU_), sourceArea_(toCopy.sourceArea_), skyADU_(toCopy.skyADU_), skyArea_(toCopy.skyArea_),
    exposure_(toCopy.exposure_), gain_(toCopy.gain_), FWHM_(toCopy.FWHM_), photometryAperture_(toCopy.photometryAperture_->createCopy())
  {
  }

  /// @brief      Constructor taking a CTargetAstronomy instance.
  /// @param[in]  ta: The target to use
  /// @throws     None.
  /// @version    2018-10-20/GGB - Function created.

  CPhotometryObservation::CPhotometryObservation(std::shared_ptr<CTargetAstronomy> ta) : CObservation(std::move(ta))
  {

  }

  /// @brief Creates a copy of *this
  /// @throws std::bad_alloc
  /// @version 2018-10-08/GGB - Updated to std::unique_ptr return value.
  /// @version 2013-06-08/GGB - Function created.

  std::unique_ptr<CObservation> CPhotometryObservation::createCopy() const
  {
    return std::make_unique<CPhotometryObservation>(*this);
  }

  /// @brief    Calculates and returns the instrument magnitude
  /// @throws EXCEPTIONS: 0x2500 - PHOTOMETRY: Cannot have exposure == 0 when calculating magnitude.
  /// @returns  The instrument magniture
  /// @version  2013-04-01/GGB - Function created.

  std::optional<FP_t> CPhotometryObservation::instrumentMagnitude()
  {
    RUNTIME_ASSERT(exposure_ != 0, boost::locale::translate("PHOTOMETRY: Cannot have exposure == 0 when calculating magnitude."));

    FP_t value = sourceADU_ - (sourceArea_ * getSkyADU());
    value = -2.5 * std::log(value / exposure_);
    std::optional<FP_t> returnValue(value);
    return returnValue;
  }

  /// @brief      Calculates the Magnitude error
  //
  /// @version    2012-11-16/GGB - Function created.

  FP_t CPhotometryObservation::magnitudeError() const
  {
    return (1.0857 / getSNR());
  }

  /// @brief      Calculates and returns the Sky ADU count from the raw data.
  //
  /// @version    2013-04-07/GGB - Function created.

  FP_t CPhotometryObservation::getSkyADU() const
  {
    return (skyADU_ / skyArea_);
  }

  // Returs the Star ADU from the raw data
  //
  // 2013-04-07/GGB - Function created.

  FP_t CPhotometryObservation::getStarADU() const
  {
    return sourceADU_;
  }

  // Calculates and returns the Star-Sky ADU count.
  //
  // 2013-04-07/GGB - Function created.

  FP_t CPhotometryObservation::getStarSkyADU() const
  {
    return (sourceADU_ - (getSkyADU() * sourceArea_));
  }

  // Calculates and returns the star signal in e-
  //
  // 2013-04-07/GGB - Function created.

  FP_t CPhotometryObservation::getStarE() const
  {
    return (getStarSkyADU() * gain_);
  }

  // Calculate and return the SNR.
  // The method described in "The Handbook of Astronomical Image Processing" pg 278
  // Third Printing 2009"
  //
  // 2012-11-16/GGB - Function created.

  FP_t CPhotometryObservation::getSNR() const
  {
    FP_t Sstar = gain_ * (sourceADU_ - sourceArea_ * getSkyADU());
    FP_t den = Sstar + sourceArea_ * (1 + sourceArea_ / skyArea_) * (gain_ * skyADU_ + MCL::pow2(gain_) * MCL::pow2(0.29));

    return (Sstar / sqrt(den));
  }

}  // namespace ACL
