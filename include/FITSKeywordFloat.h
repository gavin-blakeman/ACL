//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeywordFloat
// SUBSYSTEM:						Templated functions for keywords
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
//                        CFITSKeywordBool
//                        CFITSKeywordComplex
//                        CFITSKeywordDate
//                        CFITSKeywordDateTime
//                        CFITSKeywordDouble
//                        CFITSKeywordDoubleComplex
//                        CFITSKeywordFloat
//                        CFITSKeywordInt08
//                        CFITSKeywordInt16
//                        CFITSKeywordInt32
//                        CFITSKeywordInt64
//                        CFITSKeywordString
//                        CFITSKeywordUInt08
//                        CFITSKeywordUInt16
//                        CFITSKeywordUInt32
//
// HISTORY:             2016-04-10 - Classes rewritten and moved into seperate files.
//
//**********************************************************************************************************************************

#ifndef ACL_FITSKEYWORDFLOAT_H
#define ACL_FITSKEYWORDFLOAT_H

  // ACL Library

#include "FITSKeyword.h"

namespace ACL
{
  class CFITSKeywordFloat : public CFITSKeyword
  {
  private:
    float value_;

    CFITSKeywordFloat() = delete;

  protected:
  public:
    CFITSKeywordFloat(CFITSKeywordFloat const &);
    CFITSKeywordFloat(std::string const &keyword) : CFITSKeyword(keyword), value_() {}
    CFITSKeywordFloat(std::string const &keyword, float const &value) : CFITSKeyword(keyword), value_(value) {}
    CFITSKeywordFloat(std::string const &keyword, float const &value, std::string const &comment): CFITSKeyword(keyword, comment), value_(value) {}

    virtual CFITSKeyword &operator =(const CFITSKeywordFloat &);
    virtual bool operator==(const CFITSKeyword &) const;

    virtual operator std::int16_t() const;
    virtual operator double() const;
    virtual operator float() const;

    virtual std::unique_ptr<CFITSKeyword> createCopy() const override;

    virtual KWType type() const;

    virtual void writeToFITS(fitsfile *) const;
  };
}

#endif // FITSKEYWORDFLOAT_H
