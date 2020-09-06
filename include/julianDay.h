//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								julianDay
// SUBSYSTEM:						Time Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SOFA
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
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
// OVERVIEW:						This file provides a class for storing time and performing time conversions between different time scales.
//
// CLASSES INCLUDED:		TJD           - Julian Date
//
// CLASS HIERARCHY:     TJD
//                      CAstroTime (TJD)
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. Functions from other libraries, referenced by this library can be written in C.
//                      3. New functions should be written to be multi-thread safe.
//
// HISTORY:             2019-12-14 GGB - Converted to use std::tm rather than struct tm.
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2015-03-29 GGB - Renamed to AstronomicalTime.h
//                      2013-03-20 GGB - astroManager 2013.03 release.
//                      2013-01-22 GGB - astroManager 000.00 release.
//                      2005-06-19 GGB - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#ifndef ACL_JULIANDAY_H
#define ACL_JULIANDAY_H

  // Standard C++ library header files. (std)

#include <array>
#include <cstdint>
#include <ctime>

  // ACL header files (ACL)

#include "config.h"

namespace ACL
{
  FP_t const MJD0 = 2400000.5;      ///< Offset for calculating modified julian days

  class CAstronomicalCoordinates;

  /// @brief        The TJD class is used as a representation for a Julian Day.
  /// @details      The class allows storing and manipulating a Julian day value. The integer part of the JD is the Julian Days.
  ///               The fractional part stores the time since the start of the previous Julian day.

  class TJD		// Julian day/date class
  {
  private:
    FP_t JD_[2];

  protected:
    void normalise() noexcept;

  public:

    // Constructors

    TJD() noexcept;                               // Default constructor
    TJD(FP_t);                                    // Constructor taking a double.
    TJD(FP_t, FP_t);                              // Constructor taking two doubles.
    TJD(int, int, int) noexcept;                  // yy mm dd constructor
    TJD(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, FP_t seconds) noexcept;
    TJD(std::array<int, 6> const &);
    TJD(std::array<int, 3> const &);
    TJD(const TJD &);															// Copy Constructor
    TJD(std::tm *);
    TJD(std::time_t const &);

      // Arithmetic operators

    TJD operator+(TJD const &) const;
    TJD operator+(FP_t) const;
    TJD &operator+=(FP_t);

    TJD operator-(TJD const &) const;
    TJD operator-(FP_t const &) const;
    TJD &operator-=(FP_t const &);
    TJD &operator-=(TJD const &);

    TJD operator/(FP_t) const;

      // Increment/Decrement operators

    TJD &operator++();
    TJD operator++(int);

      // Assignment operators

    TJD &operator=(FP_t);
    TJD &operator=(TJD const &) noexcept;

      // Testing operators

    bool operator==(TJD const &) const noexcept;
    bool operator==(double) const noexcept;
    bool operator<(TJD const &) const noexcept;
    bool operator<(double) const noexcept;
    bool operator<(std::uint64_t) const noexcept;
    bool operator>(TJD const &) const noexcept;
    bool operator>(double) const noexcept;
    bool operator>(std::uint64_t) const noexcept;

      // Casting operators

    operator double() const;
    operator std::uint64_t() const;

      // Other Operators

    FP_t &operator()(int) ;                 ///< param = {0, 1} Specifies which double to get.
    FP_t const &operator()(int) const ;

    void JD(unsigned int nY, unsigned int nM, unsigned int nD) noexcept;
    void JD(std::time_t const &);
    FP_t JD() const noexcept;

    FP_t JD0();
    unsigned long MJD() const;
    FP_t floor() const;
    FP_t HJD(CAstronomicalCoordinates const &);								// returns the heliocentric correction for the JD.
    char const *JDH(char *, int);

    std::size_t JD(char *, size_t, int = 1);
    bool gregorianDate(struct tm *) const;     // Converts the JD to a struct tm date.
    FP_t Epoch() const;
    std::pair<std::uint32_t, std::uint32_t> decompose();

    std::string gregorianDate() const;
    std::string timeOfDay();
  };

} // namespace ACL

#endif // ACL_JULIANDAY_H

