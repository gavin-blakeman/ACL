//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								gregorianDate
// SUBSYSTEM:						Civil Time/Date Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2020 Gavin Blakeman.
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
// OVERVIEW:						Contains a class for working with gregorian dates.
//
// CLASSES INCLUDED:		gregorianDate_t;
//
// CLASS HIERARCHY:
//
// HISTORY:             2020-06-15 GGB - File created.
//
//*********************************************************************************************************************************

#include "include/gregorianDate.h"

  // Miscellaneous library header files

#include <GCL>

namespace ACL
{
  /// @brief Default class constructor. Constructs a date with the gregorian date 2000-01-01
  /// @throws None.
  /// @version 2020-06-15/GGB - Function created.

  gregorianDate_t::gregorianDate_t() noexcept : year_(2020), month_(1), day_(1)
  {
  }

  /// @brief Constructs a date object from a std::tm object.
  /// @param[in] date: The initialiser to use.
  /// @throws None.
  /// @version 2020-06-15/GGB - Function created.

  gregorianDate_t::gregorianDate_t(std::tm &date) noexcept
    : year_(date.tm_year + 1900), month_(date.tm_mon + 1), day_(date.tm_mday)
  {

  }

  /// @brief Returns a date for the end of the specified number of months in the past or future.
  /// @param[in] months: The number of months (future or past). 0 - gives end of current month.
  /// @throws None.
  /// @version 2020-06-15/GGB - Function created.

  gregorianDate_t gregorianDate_t::EOMonth(std::int32_t months) const
  {
    RUNTIME_ASSERT(std::abs(months) <= 393,204, "Parameter months cannot exceed 32767 years.");

    gregorianDate_t returnValue;

    std::int32_t years = months / 12;
    double year = year_ + years;

    if (months < 0)
    {
      months += years * 12;
    }
    else
    {
      months -= years * 12;
    };

    if (month_ + months > 12)
    {
      years++;
    }
    else if (month_ + months < 1)
    {
      years--;
    };

    RUNTIME_ASSERT("ACL",
                   (year <= std::numeric_limits<year_t>::max()) &&
                   (year >= std::numeric_limits<year_t>::min()), "Year value will exceed limits of storage type");

    returnValue.year_ = static_cast<year_t>(year);
    returnValue.month_ = std::min(std::max(month_ + months, 12), 1);
    returnValue.day_ = daysInMonth_.at(returnValue.month_);

      // Correct for leap year.

    if ((returnValue.month_ == 2) && (returnValue.leapYear()))
    {
      returnValue.day_++;
    };

    return returnValue;
  }


}
