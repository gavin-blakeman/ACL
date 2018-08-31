//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstronomicalTime
// SUBSYSTEM:						Time Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost, SOFA, Novas
// NAMESPACE:						ACL
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
// OVERVIEW:						This file provides a class for storing time and performing time conversions between different time scales.
//
//
// CLASSES INCLUDED:		TJD           - Julian Date
//                      CAstroTime    - Stores time (date/time) as a TT value. Can produce values converted to any of the other
//                                      time scales on demand.
//
// CLASS HIERARCHY:     TJD
//                      CAstroTime (TJD)
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. Functions from other libraries, referenced by this library can be written in C.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-03-29 GGB - Renamed to AstronomicalTime.h
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-20 GGB - astroManager 2013.03 release.
//                      2013-01-22 GGB - astroManager 0000.00 release.
//                      2005-06-19 GGB - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#ifndef ACL_ASTRONOMICALTIME_H
#define ACL_ASTRONOMICALTIME_H

  // Standard C++ Library header files (std)

#include <cstdint>
#include <ctime>
#include <map>
#include <memory>
#include <utility>

  // ACL Libraries (ACL)

#include "config.h"
#include "julianDay.h"

#ifndef ACL_NOBOOST
// Boost Library

#include "boost/shared_array.hpp"
#include "boost/filesystem.hpp"
#endif // ACL_NOBOOST

#ifdef _MSC_VER
#pragma warning( disable : 4290 )  /* Disable the warning about C++ exception specifications. */
#endif

namespace ACL
{
  class CAstronomicalCoordinates;

  enum ETimeScale
  {
    TS_NONE,    ///< No defined time scale.
    TS_UTC,     ///< Coordinated Universal Time (UTC)
    TS_UT1,     ///< Universal Time
    TS_TT,      ///< Terrestrial Time (TT)
    TS_TAI,     ///< International Atomic Time (TAI)
    TS_TDB,     ///< Barycentric Dynamical Time (TDB)
    TS_TDT,     ///< Terrestrial Dynamical Time. (Same as TT)
    TS_UT,      ///< Universal Time (UT)
    TS_ET,
    TS_TCG,
    TS_TCB
  };

  /// @brief The CAstroTime class stores a time value in a standard format.
  /// @details The class allows storing time in a standard format and converting to different time scales on the fly.
  /// This is valuable as the class just needs to be constructed with the correct time value and time scale @c ETimeScale and then
  /// different time scales can be retrieved by using the relevant functions.<br>
  /// The values of dUT1 and dAT need to be loaded into the class at runtime.
  /// All time values are stored in the member TT_ as a terrestrial time (TT) value.
  /// @pre Ensure the dUT1 and dAT values are loaded.

  class CAstroTime
  {
  private:
    TJD TT_;  ///< The time value. Stored in terrestrial time (TT) format.
    static std::map<unsigned long, FP_t> dUT1Array;   ///< The dUT1 values. Stored as a <MJD, value> in the @a std::map
    static std::map<unsigned long, int> dATArray;     ///< The dAT values. Stored as a <MJD, value> in the @a std::map

  protected:
    void constructClass(TJD const &, ETimeScale);

    static TJD UT1toTT(TJD const &);
    static TJD UTCtoTT(TJD const &);
    static TJD TDBtoTT(TJD const &);
    static TJD TAItoTT(TJD const &);
    static TJD TDTtoTT(TJD const &);

    static TJD TTtoUT1(TJD const &);
    static TJD TTtoUTC(TJD const &);
    static TJD TTtoTDB(TJD const &);
    static TJD TTtoTAI(TJD const &);
    static TJD TTtoTDT(TJD const &);

  public:
    CAstroTime();
    CAstroTime(CAstroTime const &);
    CAstroTime(TJD const &, ETimeScale);
    CAstroTime(FP_t, ETimeScale);
    CAstroTime(FP_t, double, ETimeScale);
    CAstroTime(time_t const &);
    CAstroTime(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int min, FP_t second, ETimeScale);
    CAstroTime(std::tm const *, ETimeScale);

      // Operators

    const CAstroTime operator+(CAstroTime const &) const;
    const CAstroTime operator-(CAstroTime const &) const;
    CAstroTime &operator +=(CAstroTime const &);
    CAstroTime &operator-=(CAstroTime const &);

#ifndef ACL_NOBOOST
    static void load_dAT(boost::filesystem::path const &);
    static void load_dUT1(boost::filesystem::path const &);
#endif

    static void add_dUT1(unsigned long, FP_t);
    static void add_dAT(unsigned long, int);

    static FP_t dAT(TJD const &, ETimeScale = TS_UTC) ;
    static FP_t dUT1(TJD const &, ETimeScale = TS_UTC) ;

    static FP_t dT(TJD const &, ETimeScale = TS_UTC) ;
    static FP_t dT(unsigned long);
    FP_t dT() const;

    static FP_t dAT(unsigned long);
    static FP_t dUT1(unsigned long);

    TJD UTC() const;
    TJD UT1() const;
    TJD TT() const;
    TJD TAI() const;
    TJD TDB() const;

    static TJD convertTime(TJD const &, ETimeScale, ETimeScale);

    FP_t dAT() const;
    FP_t dUT1() const;

    TJD time(ETimeScale) const;
  };

#if _cplusplus > 201402L
  typedef std::shared_ptr<CAstroTime> PAstroTime;
  typedef std::shared_ptr<TJD> PJD;
#else
  #ifdef ACL_NOBOOST
    typedef std::auto_ptr<CAstroTime> PAstroTime;
    typedef std::auto_ptr<TJD> PJD;
  #else // ACL_NOBOOST
    typedef boost::shared_ptr<CAstroTime> PAstroTime;
    typedef boost::shared_ptr<TJD> PJD;
  #endif  // ACL_NOBOOST
#endif // _cplusplus

}

#endif  // ACL_ASTRONOMICALTIME_H
