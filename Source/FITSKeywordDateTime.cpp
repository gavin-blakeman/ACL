//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeywordDateTime
// SUBSYSTEM:						Templated functions for keywords
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2016 Gavin Blakeman.
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
// OVERVIEW:            Classes to support the keywords in a FITS file. The class heirarchy enables the keywords to be stored in the
//                      STL container classes using a virtual base class.
//
// CLASSES INCLUDED:		CFITSKeywordSByte  - Handle 8 bit signed integers.
//
// CLASS HIERARCHY:     CFITSKeyword
//                        CFITSKeywordByte
//                        CFITSKeywordSByte
//
// HISTORY:             2016-04-10 - Classes rewritten and moved into seperate files.
//
//**********************************************************************************************************************************

#include "../Include/FITSKeywordDateTime.h"

  // ACL Library

#include "../Include/FITSStrings.h"

  // cfitsio

#include "cfitsio/fitsio.h"

  // Standard Libraries

#include <ctime>
#include <string>

namespace ACL
{
  CFITSKeywordDateTime::CFITSKeywordDateTime(CFITSKeywordDateTime const &toCopy) : CFITSKeyword(toCopy.keyword_, toCopy.comment_),
    value_(toCopy.value_)
  {

  }


  CFITSKeywordDateTime::CFITSKeywordDateTime(std::string const &nk, PAstroTime const &nv, std::string const &nc) :
    CFITSKeyword(nk, nc), value_(*nv)
  {

  }

  CFITSKeywordDateTime &CFITSKeywordDateTime::operator=(CFITSKeywordDateTime const &toCopy)
  {
    keyword_ = toCopy.keyword_;
    value_ = toCopy.value_;
    comment_ = toCopy.comment_;

    return (*this);
  }

  /// @brief Creates a Date/Time as a string.
  /// @returns A string containing the date and time.
  /// @throws None.
  /// @version 2016-04-15/GGB - Function created.

  CFITSKeywordDateTime::operator std::string() const
  {
    char dateTime[FITSLEN_DATETIME];
    struct tm dt;
    int const decimals = 0;
    int status = 0;

    value_.UTC().gregorianDate(&dt);
    CFITSIO_TEST(fits_time2str(dt.tm_year , dt.tm_mon, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec, decimals, dateTime, &status));

    return std::string(dateTime);
  }

  CFITSKeyword *CFITSKeywordDateTime::createCopy() const
  {
    return (new CFITSKeywordDateTime(*this));
  }

  KWType CFITSKeywordDateTime::type() const
  {
    return KWT_DATETIME;
  }

  void CFITSKeywordDateTime::writeToFITS(fitsfile *file) const
  {
    int status = 0;
    std::string szString = static_cast<std::string>(*this);

    CFITSIO_TEST(fits_write_key(file, TSTRING, keyword_.c_str(), const_cast<char *>(szString.c_str()), comment_.c_str(), &status));
  }
}
