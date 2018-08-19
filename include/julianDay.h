//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								julianDay
// SUBSYSTEM:						Time Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost, SOFA, Novas
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
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
//                      3. New functions should be written to be multi-thread safe.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-03-29 GGB - Renamed to AstronomicalTime.h
//                      2013-03-20 GGB - astroManager 2013.03 release.
//                      2013-01-22 GGB - astroManager 000.00 release.
//                      2005-06-19 GGB - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#ifndef ACL_JULIANDAY_H
#define ACL_JULIANDAY_H

#include "config.h"

  // Standard C++ libraries

#include <cstdint>
#include <ctime>

namespace ACL
{
  FP_t const MJD0 = 2400000.5;      ///< Offset for calculating modified julian days

  class CAstronomicalCoordinates;

  /// @brief The TJD class is used as a representation for a Julian Day.
  /// @details The class allows storing and manipulating a Julian day value. The integer part of the JD is the Julian Days.
  ///          The fractional part stores the time since the start of the previous Julian day.

  class TJD		// Julian day/date class
  {
  private:
    FP_t JD_[2];

  protected:
    void normalise();

  public:
    TJD();																		    // Default constructor
    TJD(FP_t);                                    // Constructor taking a double.
    TJD(FP_t, FP_t);                              // Constructor taking two doubles.
    TJD(int, int, int);														// yy mm dd constructor
    TJD(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, FP_t seconds);
    TJD(const TJD &);															// Copy Constructor
    TJD(struct tm *);
    TJD(time_t const &);

    TJD operator+(const TJD &) const;
    TJD operator-(const TJD &) const;
    TJD operator+(FP_t) const;
    TJD operator-(FP_t) const;
    TJD &operator +=(FP_t);
    TJD &operator-=(FP_t);
    TJD operator++(void);
    TJD operator++(int);
    TJD operator/(FP_t) const;
    TJD &operator=(FP_t);                   ///< Sets the JD to the double value passed.
    TJD &operator=(TJD const &);
    FP_t &operator()(int) ;                 ///< param = {0, 1} Specifies which double to get.
    FP_t const &operator()(int) const ;
    operator double() const;
    operator std::uint64_t() const;

    void JD(unsigned int nY, unsigned int nM, unsigned int nD);
    void JD(std::time_t const &);
    FP_t JD() const noexcept;

    FP_t JD0();
    unsigned long MJD() const;
    FP_t floor() const;
    FP_t HJD(CAstronomicalCoordinates const &);								// returns the heliocentric correction for the JD.
    const char *JDH(char *, int);

    size_t JD(char *, size_t, int = 1);
    bool gregorianDate(struct tm *) const;     // Converts the JD to a struct tm date.
    FP_t Epoch() const;
    std::pair<std::uint32_t, std::uint32_t> decompose();

    std::string gregorianDate() const;
    std::string timeOfDay();
  };

} // namespace ACL

#endif // ACL_JULIANDAY_H

