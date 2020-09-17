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
//                      Copyright 2017-2020 Gavin Blakeman.
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

#ifndef ACL_OBSERVATORYINFORMATION_H
#define ACL_OBSERVATORYINFORMATION_H

  // Standard C++ library header files

#include <cstdint>
#include <string>

  // ACL library header files

#include "config.h"
#include "geographicLocation.h"

namespace ACL
{
  ///@brief An observatory represents the location of observations linked with additional information.

  class CObservatory : public CGeographicLocation
  {
  private:
    std::string siteName_;          ///< The name of the observation site.
    std::string IAUCode_;           ///< The IAU Code for the observation site.
    FP_t timeZone_;                 ///< Time Zone
    std::uint32_t daylightSaving_;  ///< Daylight saving in use.

  protected:
  public:
    CObservatory() : CGeographicLocation(), siteName_(), IAUCode_(), timeZone_(0) {}
    CObservatory(std::string const &, FP_t, FP_t, int, FP_t);
    CObservatory(CObservatory const &);
    virtual ~CObservatory() {}

    virtual std::unique_ptr<CGeographicLocation> createCopy() const override;

    void siteName(std::string const &siteName) { siteName_ = siteName; }
    void IAUCode(std::string const IAUCode) { IAUCode_ = IAUCode; }
    void timeZone(FP_t timeZone) { timeZone_ = timeZone; }
    void daylightSaving(std::uint32_t ds) { daylightSaving_ = ds; }

    std::string siteName() const noexcept { return siteName_; }
    std::string &siteName() { return siteName_; }
    std::string IAUCode() const noexcept { return IAUCode_; }
    std::string &IAUCode() { return IAUCode_; }
    FP_t timeZone() const noexcept { return (timeZone_); }
    FP_t &timeZone() { return timeZone_; }
    std::uint32_t daylightSaving() const noexcept { return daylightSaving_; }
    std::uint32_t &daylightSaving() { return daylightSaving_; }
  };

} // namespace ACL

#endif // ACL_OBSERVATORYINFORMATION_H

