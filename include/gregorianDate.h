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

#ifndef ACL_GREGORIANDATE_H
#define ACL_GREGORIANDATE_H

  // Standard C++ library header files

#include <cstdint>
#include <ctime>
#include <map>

  // ACL Library header files

#include "include/julianDay.h"

namespace ACL
{
  class gregorianDate_t
  {
  private:
    using year_t = std::int16_t;
    using month_t = std::int8_t;
    using day_t = std::int8_t;

    std::map<std::int8_t, std::int8_t> const daysInMonth_ = { {1, 31}, {2, 28}, {3, 31}, {4, 30}, {5, 31}, {6, 30},
                                                              {7, 31}, {8, 31}, {9, 30}, {10, 31}, {11, 30}, {12, 31}};

    year_t year_;       ///< Year in the gregorian calendar.
    month_t month_;      ///< Month of the year. 1 = January
    day_t day_;        ///< Day of the month. 1 = 1st Day of the month.

  protected:
  public:
    gregorianDate_t() noexcept;
    gregorianDate_t(std::tm &) noexcept;

    gregorianDate_t EOMonth(std::int32_t) const;
    bool leapYear() const noexcept;

  };
}

#endif // GREGORIANDATE_H
