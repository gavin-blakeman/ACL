//*********************************************************************************************************************************
//
// PROJECT:			        Astronomy Class Library
// FILE:				        weather
// SUBSYSTEM:		        Astronomy Related classes.
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	Boost, GCL, MCL, PCL
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
//                      2) All windows related functions have been removed with the change to using the  Qt API platform.
//                      3) This library is written to be as close as possible to vanilla C++, and to not rely on operating system
//                        support.
//
// CLASSES INCLUDED:    CWeather      - Weather class
//
// CLASS HIERARCHY:     CWeather
//
// RELEASE HISTORY:     2020-09-17/GGB - CWeather moved into new file weather.h
//                      2017-07-22/GGB - TLocation moved into seperate file and renamed.
//                      2015-09-28 GGB - Astronomical object class moved into a seperate file.
//                      2015-09-27 GGB - Coordinate classes moved into a seperate file.
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2010-05-05 GGB -  1) All windows related functions have been removed as part if the change over to the Qt
//                                          API platform. This has been done to facilitate the move to a multi-platform environment.
//                                        2) The intention with the changes are also to make this software platform independant to
//                                          enable it's use as a general library with any system.
//                                        3) Removed class CAstronomicalCalc into file CAstroRT
//                      2009-11-05 GGB -  1) The library has been modified to make use of the IAU SOFA library of functions.
//                                        2) Functions making use of TTime and TDate that are affected by (1) above have been
//                                          deprecated.
//                                        3) Functions using LPSYSTEMTIME or SYSTEMTIME have been deprecated.
//                                        4) The intent is to deprecate all TTime and TDate functions in the future.
//                                        5) The intent is to deprecate all functions that are windows specific in the future.
//                      2009-09-25 GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_WEATHER_H
#define ACL_WEATHER_H

  // Standard C++ Library header files.

#include <optional>
#include <string>

  // Miscellaneous Libraries

#include <GCL>
#include <MCL>
#include <PCL>

  // ACL library header files.

#include "config.h"

namespace ACL
{
  /*!
   * @brief The CWeatherClass is used to store weather information.
   * @todo Use boost::optional for the data member (Bug 11)
   */

  class CWeather
  {
  private:
    std::optional<PCL::CTemperature> temperature_;     ///< The atmospheric temperature. (C)
    std::optional<PCL::CPressure> pressure_;           ///< The atmospheric pressure. (Pa)
    std::optional<float> RH_;                          ///< The relative humidity. (%) (This never needs to be a double.

  protected:
  public:
    CWeather();
    CWeather(CWeather const &);
    CWeather(PCL::CTemperature const &);
    CWeather(PCL::CTemperature const &, PCL::CPressure const &);
    CWeather(PCL::CTemperature const &, PCL::CPressure const &, float const &);

    bool operator ==(CWeather const &) const;

      // Value setter functions

    void temperature(PCL::CTemperature const &);
    void pressure(PCL::CPressure const &);
    void RH(float const &);

      // Value retrieval functions

    std::optional<PCL::CTemperature> const &temperature() const;
    std::optional<PCL::CPressure> const &pressure() const;
    std::optional<float> const &RH() const;
  };
}

#endif // WEATHER_H
