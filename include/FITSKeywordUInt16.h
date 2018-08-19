//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeywordUInt16
// SUBSYSTEM:						FITS Keywords Functions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2016-2018 Gavin Blakeman.
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
//                        CFITSKeywordDouble
//                        CFITSKeywordFloat
//                        CFITSKeywordLong
//                        CFITSKeywordLongLong
//                        CFITSKeywordSByte
//                        CFITSKeywordShort
//                        CFITSKeywordString
//                        CFITSKeywordULong
//                        CFITSKeywordUInt16
//
// HISTORY:             2016-04-10 - Classes rewritten and moved into seperate files.
//
//**********************************************************************************************************************************


#ifndef ACL_FITSKEYWORDUSHORT_H
#define ACL_FITSKEYWORDUSHORT_H

  // ACL Library

#include "FITSKeyword.h"

  // Standard library

#include <cstdint>


namespace ACL
{
  class CFITSKeywordUInt16 : public CFITSKeyword
  {
  private:
    std::uint16_t value_;

    CFITSKeywordUInt16() = delete;

  protected:
  public:
    CFITSKeywordUInt16(CFITSKeywordUInt16 const &);
    CFITSKeywordUInt16(std::string const &keyword) : CFITSKeyword(keyword), value_() {}
    CFITSKeywordUInt16(std::string const &keyword, std::uint16_t const &value) : CFITSKeyword(keyword), value_(value) {}
    CFITSKeywordUInt16(std::string const &keyword, std::uint16_t const &value, std::string const &comment): CFITSKeyword(keyword, comment), value_(value) {}

    virtual CFITSKeywordUInt16 &operator =(const CFITSKeywordUInt16 &);

    virtual operator std::int16_t() const;
    virtual operator std::int32_t() const;
    virtual operator std::int64_t() const;
    virtual operator std::uint16_t() const;
    virtual operator std::uint32_t() const;
    virtual operator float() const;
    virtual operator double() const;
    virtual operator std::string() const;

    virtual CFITSKeyword *createCopy() const;

    virtual KWType type() const;

    virtual void writeToFITS(fitsfile *) const;
  };
}

#endif // ACL_FITSKEYWORDUSHORT_H
