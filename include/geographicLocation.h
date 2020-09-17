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
// OVERVIEW:						A class to support geographic locations
//
// CLASSES INCLUDED:		CGeographicLocation
//
// CLASS HIERARCHY:     CGeographicLocation
//
// HISTORY:             2017-07-22 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_GEOGRAPHICCOORDINATES_H
#define ACL_GEOGRAPHICCOORDINATES_H

  // Standard C++ library header files

#include <cstdint>

  // ACL library header files.

#include "config.h"

namespace ACL
{
  /// @brief CGeographicLocation is used to store physical locations on the earths surface.
  /// @todo Create specialised classes for storing latitude and longitude. (PCL?) Bug 12

  class CGeographicLocation
  {
  private:
    FP_t latitude_;               ///< D:M:S as D.M/60+s/3600 (-ve -> S, +ve -> N) Limits +-90º
    FP_t longitude_;              ///< D:M:S as D.M/60+s/3600 (-ve -> E, +ve -> W) Limits +-180º
    std::int16_t altitude_;				///< Altitude (m) Limits -100->32000

    CGeographicLocation(CGeographicLocation &&) = delete;
    void Normalise();

  protected:

  public:
    /// @brief    Constructor
    /// @throws   None.
    /// @version  2013-06-22/GGB - Removed the mutable values for tracking the validity.
    /// @version  2009-09-01/GGB - Function created

    CGeographicLocation() : latitude_(0), longitude_(0), altitude_(0) {}
    CGeographicLocation(CGeographicLocation const &);
    CGeographicLocation(FP_t, FP_t, int = 0);

    virtual ~CGeographicLocation() {}

    virtual CGeographicLocation &operator =(CGeographicLocation const &);
    virtual bool operator ==(CGeographicLocation const &) const noexcept;

      // Factory functions

    virtual std::unique_ptr<CGeographicLocation> createCopy() const;

    void setLocation(FP_t, FP_t, int) ;

    void latitude(FP_t latitude) { latitude_ = latitude; }
    FP_t latitude() const noexcept;
    FP_t &latitude() { return latitude_; }

    void longitude(FP_t longitude) {longitude_ = longitude; }
    FP_t longitude() const noexcept;
    FP_t &longitude() { return longitude_; }

    void altitude(int altitude) { altitude_ = altitude; }
    std::int16_t altitude() const noexcept;
    std::int16_t &altitude() { return altitude_; }

    char *Latitude(char *, int);
    char *Longitude(char *, int);
  };

} // namespace ACL

#endif // ACL_GEOGRAPHICCOORDINATES_H

