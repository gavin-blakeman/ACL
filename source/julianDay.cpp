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
// HISTORY:             2019-12-14 GGB - Converted to use std::tm rather than struct tm.
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2015-03-29 GGB - Renamed to AstronomicalTime.h
//                      2013-03-20 GGB - astroManager 2013.03 release.
//                      2013-01-22 GGB - astroManager 000.00 release.
//                      2005-06-19 GGB - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#include "include/julianDay.h"

  // Miscellaneous Library header files

#include "boost/locale.hpp"
#include <GCL>
#include "sofa.h"
#include "sofam.h"

// ACL library header files.

#include "include/common.h"
#include "include/error.h"
#include "include/AstronomicalCoordinates.h"
#include "include/AstroFunctions.h"

namespace ACL
{
  //*******************************************************************************************************************************
  // Julian Day object
  //
  // This class implements a Julian day class.
  // This allows storage of Julian Dates, as well as allows conversion
  // of the Julian dates to other calenders.
  //
  // The JD is stored as two double values.
  // JD_[0] = day part of the JD. (Should always be xxx.0)
  // JD_[1] = time part of the JD.
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor for class. Defaults the value of the class to the local time when the class is created.
  /// @version 2020-06-15/GGB - Added noexcept specifier.
  /// @version 2019-12-15/GGB - Updated to use std::tm.
  /// @version 2017-08-13/GGB - Logic flow corrected as always returning 0. (Bug #103)
  /// @version 2013-09-22/GGB - Removed call to tzset() before time()
  /// @version 2011-09-18/GGG - Changed function to use standard C, C++ routines to allow use with GCC compiler.
  /// @version 2009-11-05/GGB - 1) Makes use of the IAU SOFA library <br>
  ///                           2) Gets the current system date and converts it into a JD. If call to localtime is a failure, then
  ///                              the JD value 0 is returned.
  /// @version 2005-06-19/GGB - Function created.

  TJD::TJD() noexcept
  {
#ifdef _MSC_VER
    time_t tCurrent;
#else
    std::time_t tCurrent;
#endif

    std::tm *tmCurrent;

    time(&tCurrent);
    if ((tmCurrent = localtime(&tCurrent)) == nullptr)
    {
      JD_[0] = JD_[1] = 0;
    }
    else if (iauCal2jd(tmCurrent->tm_year + 1900, tmCurrent->tm_mon + 1, tmCurrent->tm_mday, &JD_[0], &JD_[1]) != 0)
    {
      JD_[0] = JD_[1] = 0;
    }
    else
    {
      JD_[1] += static_cast<double>(tmCurrent->tm_hour) / 24 + static_cast<double>(tmCurrent->tm_min) / ( 24 * 60) +
          static_cast<double>(tmCurrent->tm_sec) / (24 * 60 * 60);
      normalise();
    };
  }

  /// @brief Constructs JD from a time_t value. Calls the initialiser function JD(time_t)
  /// @param[in]    time: The structure to use for creating the instance.
  /// @version 2015-06-01/GGB - Use initialiser function JD().
  /// @version 2013-09-22/GGB - Removed call to tzset() before gmtime()
  /// @version 2011-12-25/GGB - Function created.

  TJD::TJD(std::time_t const &time)
  {
    JD(time);
  }

  /// Contructor taking a floating point type.
  /// @details Sets the value of the class to the value passed in the arguments list.
  /// @param[in] dJD: Floating point value representing a julian day
  /// @throws None
  /// @version 2005-06-19/GGB - Function created.

  TJD::TJD(FP_t dJD)
  {
    JD_[1] = modf(dJD, &JD_[0]);
  }

  /// @brief Copy contructor for the class.
  /// @details  Sets the value of the class to the value passed in the arguments list.
  /// @param[in] toCopy: The JD instance to copy.
  /// @throws None.
  /// @version 2005-06-19/GGB - Function created.

  TJD::TJD(const TJD &toCopy)
  {
    JD_[0] = toCopy.JD_[0];
    JD_[1] = toCopy.JD_[1];
  }

  /// @brief Y:M:D Constructor for the class
  /// @details Sets the value of the class to the value passed in the arguments list. The arguments are interpreted as a Gregorian
  ///          calender date. Calls the initialiser function JD(year, month, day)
  /// @param[in] dY: Year (eg 2000)
  /// @param[in] dM: Month (1 - Jan, 12 = Dec)
  /// @param[in] dD: Day
  /// @note 1. Class variables modified: JDay - Julian Day (double Days.fractional)
  /// @version 2015-06-01/GGB - Use initialiser function.
  /// @version 2011-07-09/GGB - Use two doubles to store value of the JD.
  /// @version 2009-11-05/GGB - Changed code to use the IAU SOFA library to calculate the Julian Day. If an error occurs, the JD
  ///                           value of 0 is returned.
  /// @version 2005-06-19/GGB 0 Function created.

  TJD::TJD(int dY, int dM, int dD) noexcept
  {
    JD(dY, dM, dD);
  }

  /// @brief Constructor for a date and time.
  /// @throws std::bad_alloc
  /// @version 2020-06-15/GGB - Added noexcept specifier.
  /// @version 2019-12-15/GGB - Updated to static_cast to replace c-style casting.
  /// @version 2011-07-16/GGB - Function created.

  TJD::TJD(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, FP_t seconds) noexcept
  {
    if (iauCal2jd(year, month, day, &JD_[0], &JD_[1]) != 0)
    {
      JD_[0] = JD_[1] = 0;
    }
    else
    {
      JD_[1] += static_cast<FP_t>(hour) / 24 + static_cast<FP_t>(minute) / 1440 + static_cast<FP_t>(seconds) / 86400;
      normalise();
    };
  }

  /// @brief Constructor taking an array of 6 ints.
  /// @param[in] value: The time value (stored as YYYY, MM, DD, HH, MM, SS)
  /// @throws std::bad_alloc
  /// @version 2019-12-15/GGB - Function created.

  TJD::TJD(std::array<int, 6> const &value)
  {
    if (iauCal2jd(value[0], value[1], value[2], &JD_[0], &JD_[1]) != 0)
    {
      JD_[0] = JD_[1] = 0;
    }
    else
    {
      JD_[1] += static_cast<FP_t>(value[3]) / 24 + static_cast<FP_t>(value[4]) / 1440 + static_cast<FP_t>(value[5]) / 86400;
      normalise();
    };
  }

  /// @brief Constructor taking an array of three ints.
  /// @param[in] value: The time as a date only. (YYYY, MM, DD)
  /// @throws std::bad_alloc
  /// @version 2019-12-15/GGB - Function created.

  TJD::TJD(std::array<int, 3> const &value)
  {
    if (iauCal2jd(value[0], value[1], value[2], &JD_[0], &JD_[1]) != 0)
    {
      JD_[0] = JD_[1] = 0;
    };

    normalise();
  }

  /// @brief Constructor using a std::tm.
  /// @param[in] tmDateTime: The date/time data to use to initialise.
  /// @throws
  /// @version 2019-12-15/GGB - Updated to use std::tm.
  /// @version 2015-01-01/GGB - Changed C style casts to static_cast.
  /// @version 2011-07-05/GGB - Use two doubles to store the julian day to be in line with the SOFA libary.
  /// @version 2009-11-05/GGB - Use IAU SOFA library for the date conversion to JD. If an error is returned, a JD of 0 is returned.
  /// @version 2009-09-19/GGB - Function created.

  TJD::TJD(std::tm *tmDateTime)
  {
    FP_t dH;

    if (iauCal2jd(tmDateTime->tm_year + 1900, tmDateTime->tm_mon + 1, tmDateTime->tm_mday, &JD_[0], &JD_[1]) != 0)
    {
      JD_[0] = JD_[1] = 0;
    }
    else
    {
      dH = tmDateTime->tm_hour;
      dH += static_cast<FP_t>(tmDateTime->tm_min) / 60;
      dH += static_cast<FP_t>(tmDateTime->tm_sec) / 3600;
      dH /= 24;												// Convert to a fraction.
      dH -= 0.5;												// Midday = 0 in JD.
      JD_[1] += dH;
      normalise();
    };
  }

  /// @brief Constructor taking 2 doubles.
  /// @param[in] d1: First value
  /// @param[in] d2: Second value
  /// @throws std::bad_alloc
  /// @version 2011-07-13/GGB - Function created.

  TJD::TJD(FP_t d1, FP_t d2)
  {
    JD_[0] = d1;
    JD_[1] = d2;
    normalise();
  }

  /// @brief Subtraction operator for TJD class.
  /// @param[in] rhs: Double value to delete.
  /// @version 2019-12-15/GGB - Changed parameter to a const &.
  /// @version 2011-07-05/GGB - Changed to use 2 doubles as per SOFA library.
  /// @version 2010-06-08/GGB - Function created.

  TJD TJD::operator-(FP_t const &rhs) const
  {
    TJD result;

    result.JD_[0] = JD_[0] - rhs;
    result.normalise();

    return ( result );
  }

  /// @brief Subtraction operator taking a TJD instance as the RHS.
  /// @param[in] rhs: The right hand value.
  /// @returns The subtracted value.
  /// @version 2019-12-15/GGB - Changed to use -= operator.
  /// @version 2011-07-05/GGB - Uses two doubles as per SOFA library.
  /// @version 2010-06-08/GGB - Function created.

  TJD TJD::operator-(TJD const &rhs) const
  {
    TJD result = *this;
    result -= rhs;

    return result;
  }

  /// @brief operator +=
  /// @param[in] val: The value to add.
  /// @returns (*this)
  /// @version 2011-07-13/GGB - Function created.

  TJD &TJD::operator+=(FP_t val)
  {
    FP_t ip;

    JD_[0] += modf(val, &ip);
    JD_[1] += ip;
    normalise();

    return *this;
  }

  /// @brief  -= operator
  /// @param[in] val - The value to subtract.
  /// @returns (*this)
  /// @throws None
  /// @version 2019-12-15/GGB - Changed parameter to a const &.
  /// @version  2011-07-09/GGB - Function created.

  TJD &TJD::operator-=(FP_t const &rhs)
  {
    FP_t ip;

    JD_[0] -= modf(rhs, &ip);
    JD_[1] -= ip;
    normalise();

    return *this;
  }

  /// @brief        Subtraction assignment operator taking a TJD object as the RHS.
  /// @param[in]    rhs: The right hand operand.
  /// @returns      (*this)
  /// @version      2019-12-15/GGB - Function created.

  TJD &TJD::operator-=(TJD const & rhs)
  {
    JD_[0] -= rhs.JD_[0];
    JD_[1] -= rhs.JD_[1];
    normalise();

    return (*this);
  }

  /// @brief        Equality operator (same class)
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if the two values are equal, false otherwise.
  /// @throws       None.
  /// @note         This test depends on both instances having been normalised.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator==(TJD const &rhs) const noexcept
  {
    return ( (JD_[0] == rhs.JD_[0]) &&
             (JD_[1] == rhs.JD_[1]));
  }

  /// @brief        Equality operator (against double).
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if the two values are equal, false otherwise.
  /// @throws       None.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator==(double rhs) const noexcept
  {
    return ((JD_[0] + JD_[1]) == rhs);
  }

  /// @brief        Less than operator (class).
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if this is less than rhs, false otherwise.
  /// @throws       None.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator<(TJD const &rhs) const noexcept
  {
    bool returnValue = false;

    if (JD_[0] < rhs.JD_[0])
    {
      returnValue = true;
    }
    else if ( (JD_[0] == rhs.JD_[0]) && (JD_[1] < rhs.JD_[1]))
    {
      returnValue = true;
    };

    return returnValue;
  }

  /// @brief        Less than operator (double).
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if this is less than rhs, false otherwise.
  /// @throws       None.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator<(double rhs) const noexcept
  {
    return ( (JD_[0] + JD_[1]) < rhs);
  }

  /// @brief        Less than operator (uint64_t).
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if this is less than rhs, false otherwise.
  /// @throws       None.
  /// @note         This operator only checks the days.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator<(std::uint64_t rhs) const noexcept
  {
    return (static_cast<std::uint64_t>(JD_[0] + JD_[1]) < rhs);
  }

  /// @brief        Greater than operator (class).
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if this is less than rhs, false otherwise.
  /// @throws       None.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator>(TJD const &rhs) const noexcept
  {
    bool returnValue = false;

    if (JD_[0] < rhs.JD_[0])
    {
      returnValue = true;
    }
    else if ( (JD_[0] == rhs.JD_[0]) && (JD_[1] < rhs.JD_[1]))
    {
      returnValue = true;
    };

    return returnValue;
  }

  /// @brief        Less than operator (double).
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if this is less than rhs, false otherwise.
  /// @throws       None.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator>(double rhs) const noexcept
  {
    return ( (JD_[0] + JD_[1]) < rhs);
  }

  /// @brief        Less than operator (uint64_t).
  /// @param[in]    rhs: The value to test against.
  /// @returns      true if this is less than rhs, false otherwise.
  /// @throws       None.
  /// @note         This operator only checks the days.
  /// @version      2020-09-06/GGB - Function created.

  bool TJD::operator>(std::uint64_t rhs) const noexcept
  {
    return (static_cast<std::uint64_t>(JD_[0] + JD_[1]) > rhs);
  }

  /// @brief        Decomposes a JD.time value into a JD and a seconds of day value.
  /// @returns      first: The JD at midnight.
  /// @returns      second: Seconds since midnight.
  /// @throws       None.
  /// @version      2017-08-01/GGB - Function created.

  std::pair<std::uint32_t, std::uint32_t> TJD::decompose()
  {
    normalise();      // JD_[0] = IP and JD[1] = FP

    return std::make_pair(static_cast<std::uint32_t>(JD_[0]), static_cast<std::uint32_t>(JD_[1] * 60 * 60 * 24));
  }

  /// @brief Converts the JD into a gregorian date string.
  /// @throws 0x3204 - ASTROTIME: Error preparing gregorian date string.
  /// @version 2019-12-15/GGB - Updated to use std::tm.
  /// @version 2011-12-31/GGB - Function created.

  std::string TJD::gregorianDate() const
  {
    char szDate[30];
    std::tm date;
    int year, month, day;

    gregorianDate(&date);

    year = date.tm_year + 1900;
    month = date.tm_mon + 1;
    day = date.tm_mday;

    if (sprintf(szDate, "%04i-%02i-%02i", year, month, day) > 0)
    {
      return std::string(szDate);
    }
    else
    {
      ERROR(ACL, 0x3204);    // ASTROTIME: Error preparing gregorian date string.
    };
  }

  /// @brief Calculates the Julian day from Y, M, D information. Replaces function TJD::SetJD(LPSYSTEMTIME)
  /// @param[in] nY: The year
  /// @param[in] nM: The month
  /// @param[in] nd: The day of the month
  /// @version 2015-06-01/GGB - Changed name to JD(....)
  /// @version 2011-07-05/GGB - Use two doubles to store the julian day in line with the SOFA library.
  /// @version 2009-11-11/GGB - Function created.

  void TJD::JD(unsigned int nY, unsigned int nM, unsigned int nD) noexcept
  {
    if (iauCal2jd(nY, nM, nD, &JD_[0], &JD_[1]) != 0)
    {
      JD_[0] = JD_[1] = 0;
    }
    else
    {
      normalise();
    }
  }

  /// @brief        Returns the JD referred to 0h UT. Must always be 0.5 ending on the JD.
  /// @returns      The JD referred to 0h UTC.
  /// @version      2011-07-05/GGB - Use two doubles to store julian day. See SOFA library documentaiton for more details.
  /// @version      2005-07-03/GGB - Function created.

  FP_t TJD::JD0()
  {
    FP_t IP, FP;

    normalise();

    FP = modf(JD_[1], &IP);
    if (FP == 0.5)
    {
      return (JD_[0] + JD_[1]);
    }
    else if (FP > 0.5)
    {
      return (JD_[0] + JD_[1] -FP + 0.5);
    }
    else
    {
      return (JD_[0] + JD_[1] - FP - .5);
    };
  }

  /// @brief        Addition operator (TJD + double) for the TJD class.
  /// @param[in]    rhs: The right hand value.
  /// @version      2011-07-09/GGB -  1) Argument changed to double
  ///                                 2) Changed to reflect two x double storage.
  /// @version      2010-06-08/GGB - Argument changed to const
  /// @version      2009-10-19/GGB - Function created.

  TJD TJD::operator+(FP_t rhs) const
  {
    TJD result(*this);
    FP_t d1;

    result.JD_[0] += modf(rhs, &d1);

    result.JD_[1] += d1;

    result.normalise();

    return (result);
  }

  /// @brief        Addition operator (TJD + TJD) for the TJD class.
  /// @param[in]    rhs: The right hand value.
  /// @version      2011-07-05/GGB - Changed to use 2 doubles for storage as per SOFA library.
  /// @version      2009-10-19/GGB - Function created.

  TJD TJD::operator +(const TJD &rhs) const
  {
    TJD result;

    result.JD_[0] = JD_[0] + rhs.JD_[0];
    result.JD_[1] = JD_[1] + rhs.JD_[1];
    result.normalise();

    return ( result );
  }


  /// @brief        Pre-increment operator for the class. (Increment the stored value by 1 day)
  /// @returns      Reference to the incremented instance.
  /// @version      2020-09-06/GGB - Change return value to reference.
  /// @version      2011-07-05/GGB -Uses two doubles as per SOFA library.
  /// @version      2009-10-19/GGB - Function created.

  TJD &TJD::operator++(void)
  {
    JD_[0]++;

    return ( *this );
  }

  /// @brief        Post-increment operator for the class. (Increment the stored value by 1 day).
  /// @returns      Copy of this (post incremented)
  /// @version      2011-07-05/GGB - Use two doubles as per SOFA library.
  /// @version      2009-10-19/GGB - Function created.

  TJD TJD::operator++(int)
  {
    JD_[0]++;

    return ( *this );
  }

  /// @brief        Assignment operator for a double.
  /// @param[in]    jday: The value to assign.
  /// @throws       None.
  /// @version      2011-07-05/GGB - Use two doubles as per SOFA library.
  /// @version      2009-10-11/GGB - Function created.

  TJD &TJD::operator=(FP_t jday)
  {
    JD_[0] = modf(jday, &JD_[1]);

    return (*this);
  }

  /// @brief        Assignemnt operator for the class.
  /// @param[in]    toCopy: The instance to copy.
  /// @version      2011-07-05/GGB - Uses two doubles as per SOFA library.
  /// @version      2009-10-11/GGB - Function created.

  TJD &TJD::operator=(TJD const &toCopy) noexcept
  {
    if (this == &toCopy)
    {
      return *this;
    }
    else
    {
      JD_[0] = toCopy.JD_[0];
      JD_[1] = toCopy.JD_[1];

      return (*this);
    };
  }

  /// @brief Operator to return a single double value.
  /// @returns The JD as a double value.
  /// @throws None.
  /// @version 2011-07-05/GGB - Function created.

  TJD::operator double() const
  {
    return (JD_[0] + JD_[1]);
  }

  /// @brief        Returns the julian day as an integer value.
  /// @returns      The Julian Day as an integer value.
  /// @throws       None.
  /// @version      2017-08-20/GGB - Function created.

  TJD::operator std::uint64_t() const
  {
    return static_cast<std::uint64_t>(std::floor(JD_[0] + JD_[1]));   // This will cause a problem some years in the future.
                                                                      // If this code is still current then I would be suprised!
  }

  /// @brief        Returns the relevant double from the value.
  /// @param[in]    val: The position [] of the double to return.
  /// @returns      The first or second stored value depending on the parameter.
  /// @throws
  /// @version      2011-07-09/GGB - Function created.

  double &TJD::operator ()(int val)
  {
    RUNTIME_ASSERT(((val >= 0) && (val <= 1)), boost::locale::translate("TJD::operator(): Parameter must be [0, 1]"));

    return JD_[val];
  }

  /// @brief        Returns the relevant double from the value.
  /// @param[in]    val: The index to access.
  /// @version      2011-07-09/GGB - Function created.

  FP_t const &TJD::operator ()(int val) const
  {
    RUNTIME_ASSERT(((val >= 0) && (val <= 1)), boost::locale::translate("TJD::operator(): Parameter must be [0, 1]"));

    return JD_[val];
  }

  /// @brief Convert the julian day to an integral Julian Day.
  /// @retval The integral Julian Day value (ie no decimal places) of the underlying Julian Day value.
  /// @throws None.
  /// @version 2011-07-13/GGB - Changed to have return type double. Also function is now a const function.
  /// @version 2011-07-05/GGB - Function created.

  FP_t TJD::floor() const
  {
    FP_t returnValue = ::floor(JD_[0]);

    returnValue += ::floor(JD_[1]);

    return returnValue;
  }

  /// @brief Returns the Julian day value as a Heliocentric Julian Day.
  /// @param[in] objectPosition: The position of the object to be corrected.
  /// @returns Heliocentric Julian Day (double)
  /// @throws None.
  /// @version 2010-07-07/GGB Calls function ACL::HJD to perform the conversion.
  /// @version 2009-09-17/GGB - Function created.

  FP_t TJD::HJD(CAstronomicalCoordinates const &objectPosition)
  {
    return ACL::JD2HJD(JD_[0] + JD_[1], objectPosition.RA().degrees(), objectPosition.DEC().degrees());
  }

  /// @brief Returns a JD value.
  /// @returns The JD as a FP_t value.
  /// @throws None.
  /// @version 2020-06-15/GGB - Added noexcept specifier.
  /// @version 2017-08-03/GGB - Function created.

  FP_t TJD::JD() const noexcept
  {
    return JD_[0] + JD_[1];
  }

  /// @brief Converts the JDay value to a formatted string.
  /// @param[out] szBuffer: The buffer to contain the formatted string.
  /// @param[in] nBuffer: The size of the buffer.
  /// @param[in] nSig: the number of decimal places.
  /// @returns The length of the resulting string.
  /// @version 2009-09-19/GGB - Function created.

  std::size_t TJD::JD(char *szBuffer, size_t nBuffer, int nSig)
  {
    return ( sprintf(szBuffer, "%.*f", nSig, JD_[0] + JD_[1]) );
  }

  /// @brief        Converts a JD to a Gregorian date in a structure tm.
  /// @param[out]   tmTimeDate: The time and date structure to contain the value.
  /// @returns      true - succesful
  /// @returns      false - not succesfull, value in the struct tm is not valid.
  /// @throws       None.
  /// @version      2019-12-15/GGB - Updated to use std::tm.
  /// @version      2015-05-18/GGB
  ///                 @li Changed return value to a bool.
  ///                 @li Changed function name to gregorianDate(...)
  /// @version      2015-01-01/GGB - Changed C style casts to static_cast.
  /// @version      2009-11-05/GGB - Use the IAU SOFA routines to perform the calculation.
  /// @version      2009-10-21/GGB - Function created.
  /// @todo         1. Change return type to optional and return a std::tm structure.

  bool TJD::gregorianDate(std::tm *tmTimeDate) const
  {
    int iY, iM, iD;
    FP_t dD;
    bool returnValue = false;

    if (iauJd2cal(JD_[0], JD_[1], &iY, &iM, &iD, &dD) == 0)
    {
      tmTimeDate->tm_year = iY - 1900;
      tmTimeDate->tm_mon = iM - 1;
      tmTimeDate->tm_mday = iD;
      dD *= 24;
      tmTimeDate->tm_hour = static_cast<int>(dD);
      dD -= std::floor(dD);
      dD *= 60;
      tmTimeDate->tm_min = static_cast<int>(dD);
      dD -= std::floor(dD);
      dD *= 60;
      tmTimeDate->tm_sec = static_cast<int>(dD);

      returnValue = true;
    };

    return returnValue;
  }

  /// @brief Returns the Epoch of the Julian Day.
  /// @returns The EPOCH as a floating point value.
  /// @note 1. Makes use of the SOFA iauEpj function.
  /// @version 2009-12-14/GGB - Function created.

  FP_t TJD::Epoch() const
  {
    return iauEpj(JD_[0], JD_[1]);
  }

  /// @brief        Initialiser function using a time_t value.
  /// @param[in]    time: The std::time_t value to initialise with.
  /// @throws       GCL::CError(ACL::0x3203) - Error while constructing JD from time_t
  /// @version      2015-06-01/GGB - Function created.

  void TJD::JD(std::time_t const &time)
  {
    struct tm *UTCTime;
    FP_t tod;

    if ( (UTCTime = gmtime(&time)) != nullptr )
    {
      if (iauCal2jd(UTCTime->tm_year + 1900, UTCTime->tm_mon + 1, UTCTime->tm_mday, &JD_[0], &JD_[1]) == 0)
      {
        tod = static_cast<FP_t>(UTCTime->tm_sec) / 86400 + static_cast<FP_t>(UTCTime->tm_min) / 1440 +
              static_cast<FP_t>(UTCTime->tm_hour) / 24;

        JD_[1] += tod;

        normalise();
      }
      else
      {
        RUNTIME_ERROR(boost::locale::translate("ASTROTIME: Error while constructing JD from time_t."), E_ASTROTIME_CONSTRUCTTIMET,
                      LIBRARYNAME);
      }
    }
    else
    {
      RUNTIME_ERROR(boost::locale::translate("ASTROTIME: Error while constructing JD from time_t."), E_ASTROTIME_CONSTRUCTTIMET,
                    LIBRARYNAME);
    }
  }

  /// @brief Returns the modified Julian Day.
  /// @returns The modified Julian day.
  /// @throws None.
  /// @version 2015-01-01/GGB - Changed C style cast to static_cast.
  /// @version 2011-07-09/GGB - Function created.

  unsigned long TJD::MJD() const
  {
    unsigned long retVal;
    FP_t d1, d2;

    d1 = JD_[0] - MJD0;
    d1 += JD_[1];

    modf(d1, &d2);

    retVal = static_cast<unsigned long>(d2);

    return retVal;
  }

  /// @brief        Normalises the JD. The normalised storage for the JD is the first element to have no fractional part and the
  ///               second element to have all the fractional part. This also ensures that there is no negative part.
  /// @throws       None.
  /// @version      2020-06-15/GGB - Added noexcept specifier.
  /// @version      2017-08-17/GGB - Simplified to support negative parts.
  /// @version      2011-07-05/GGB - Function created.

  void TJD::normalise() noexcept
  {
    JD_[1] = std::modf(JD_[0] + JD_[1], &JD_[0]);
  }

  /// @brief Converts the time of day to a string.
  /// @returns The time of day as a string. HH:MM:SS
  /// @version 2017-09-02/GGB - Function created.

  std::string TJD::timeOfDay()
  {
    normalise();

    // GGB - Not sure why, but if the following line is uncommented I receive a linker error that the function cannot be found.

    //return GCL::sprintfHMS(static_cast<std::uint32_t>(JD_[1] * SECONDS_PER_DAY));
  }

} // namespace
