//*********************************************************************************************************************************
//
// PROJECT:             Astronomy Class Library
// FILE:                AstroClass
// SUBSYSTEM:           Astronomy Related classes.
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	Boost, GCL, MCL, PCL
// AUTHOR:              Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2005-2018 Gavin Blakeman.
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
//                      2) All windows related functions have beebn removed with the change to using the  Qt API platform.
//                      3) This library is written to be as close as possible to vanilla C++, and to not rely on operating system
//                        support.
//
// CLASSES INCLUDED:    CWeather      - Weather class
//
// CLASS HIERARCHY:     CWeather
//
// RELEASE HISTORY:     2017-07-22/GGB - TLocation moved into seperate file and renamed.
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

#include "../include/AstroClass.h"

  // ACL library include files

#include "../include/constants.h"
#include "../include/AstroFunctions.h"

  // SOFA include files

#include "sofa.h"

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CWeather
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor
  /// @throws Nothing.
  /// @version 2015-06-24/GGB - Added rhValid initialisation.
  /// @version 2011-07-02/GGB - Function created.

  CWeather::CWeather() : temperature_(), pressure_(), RH_()
  {
  }

  /// @brief Copy constructor
  /// @param[in] toCopy: The instance to make a copy of.
  /// @throws std::bad_alloc
  /// @version 2017-08-26/GGB - Function created.

  CWeather::CWeather(CWeather const &toCopy) : temperature_(toCopy.temperature_), pressure_(toCopy.pressure_), RH_(toCopy.RH_)
  {
  }

  /// @brief Class constructor taking a temperature value.
  /// @param[in] nt: The temperature to associate with the class.
  /// @throws None.
  /// @version 2015-09-17/GGB - Updated to use PCL classes as storage and interface types.
  /// @version 2011-07-15/GGB - Function created.

  CWeather::CWeather(PCL::CTemperature const &nt) : temperature_(), pressure_(), RH_()
  {
    temperature(nt);
  }

  /// @brief Class constructor.
  /// @param[in] nt: The temperature to associate with the instance.
  /// @param[in] np: The pressure to associate with the instance.
  /// @throws None.
  /// @version 2015-09-17/GGB - Updated to use PCL classes as storage and interface types.
  /// @version 2011-07-02/GGB - Function created.

  CWeather::CWeather(PCL::CTemperature const &nt, PCL::CPressure const &np) : temperature_(0), pressure_(0), RH_(0)
  {
    temperature(nt);
    pressure(np);
  }

  /// @brief Constructor for 3 weather parameters.
  /// @param[in] nt: The temperature to associate with the instance.
  /// @param[in] np: The pressure to associate with the instance.
  /// @param[in] rh: The RH value.
  /// @throws None.
  /// @version 2011-07-14/GGB - Function created

  CWeather::CWeather(PCL::CTemperature const &nt, PCL::CPressure const &np, float const &rh) :temperature_(0), pressure_(0)
  {
    temperature(nt);
    pressure(np);
    RH(rh);
  }

  /// @brief Equality check for a weather object.
  /// @throws None.
  /// @version 2011-07-17/GGB - Function created.

  bool CWeather::operator ==(ACL::CWeather const &rhs) const
  {
    if (this == &rhs)
    {
      return true;          // Same object
    }
    else
    {
      return ( (temperature_ == rhs.temperature_) &&
               (pressure_ == rhs.pressure_) &&
               (RH_ == rhs.RH_) );
    };
  }

  /// @brief Returns the pressure value.
  /// @returns The pressure
  /// @throws None.
  /// @version 2016-03-23/GGB - Changed return value to boost::optional<> (Bug 11)
  /// @version 2015-09-17/GGB - Updated to use PCL::CPressure.
  /// @version 2011-07-09/GGB - Function created.

  std::optional<PCL::CPressure> const &CWeather::pressure() const
  {
    return pressure_;
  }

  /// @brief set the value of the pressure.
  /// @param[in] np - New Pressure
  /// @throws None.
  /// @version 2016-03-27/GGB - Function created.

  void CWeather::pressure(PCL::CPressure const &np)
  {
    if (np(PCL::PU::PA) >= 0 && np(PCL::PU::PA) <= 1000000)
    {
      pressure_ = np;
    };
  }

  /// @brief Returns the relative humidity value.
  /// @returns The RH (%)
  /// @throws None
  /// @version 2016-03-23/GGB - Changed return value to boost::optional<> (Bug 11)
  /// @version 2011-07-09/GGB - Function created.

  std::optional<float> const &CWeather::RH() const
  {
    return RH_;
  }

  /// @brief Setter function for RH
  /// @param[in] rh - The relative humidity.
  /// @throws None.
  /// @version 2016-03-27/GGB - Function created.

  void CWeather::RH(float const &rh)
  {
    if ( (rh >= 0) && (rh <= 1) )
    {
      RH_ = rh;
    };
  }

  /// @brief Returns the temperaure value.
  /// @returns The temperature (K)
  /// @throws None.
  /// @version 2016-03-23/GGB - Changed return value to boost::optional<> (Bug 11)
  /// @version 2011-07-09/GGB - Function created.

  std::optional<PCL::CTemperature> const &CWeather::temperature() const
  {
    return temperature_;
  }

  /// @brief Set the temperature.
  /// @param[in] nt - Temperature.
  /// @throws None.
  /// @version 2016-03-27/GGB - Function created.

  void CWeather::temperature(PCL::CTemperature const &nt)
  {
    double temp = nt(PCL::TU_C);

    if ((nt(PCL::TU_C) >= -100.0) && (nt(PCL::TU_C) <= 100.0) )
    {
      temperature_ = nt;
    };
  }

  //*******************************************************************************************************************************
  //
  // CEarthOrientationData
  //
  //*******************************************************************************************************************************

  std::map<JD_t, CEarthOrientationData::SPole> CEarthOrientationData::orientationData;

  /// @brief Returns the relevant earth orienatation data.
  ///
  /// @version 2011-12-24/GGB - Function created.

  bool CEarthOrientationData::getData(const ACL::CAstroTime &time, FP_t &Xp, FP_t &Yp)
  {
    unsigned long tt = time.TT().MJD();
    if (orientationData.count(tt) > 0)
    {
      Xp = orientationData[tt].Xp;
      Yp = orientationData[tt].Yp;
      return true;
    }
    else
      return false;
  }

}	// namespace
