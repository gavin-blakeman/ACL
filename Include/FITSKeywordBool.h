//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeywordByte
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
// CLASSES INCLUDED:		CFITSKeywordByte  - Handle 8 bit unsigned integers.
//
// CLASS HIERARCHY:     CFITSKeyword
//                        CFITSKeywordBool
//                        CFITSKeywordByte
//
// HISTORY:             2016-04-10 - Classes rewritten and moved into seperate files.
//
//**********************************************************************************************************************************

#ifndef ACL_FITSKEYWORDBOOL_H
#define ACL_FITSKEYWORDBOOL_H

  // ACL Library

#include "FITSKeyword.h"

namespace ACL
{
  class CFITSKeywordBool : public CFITSKeyword
  {
  private:
    bool value_;

    CFITSKeywordBool() = delete;

  protected:
  public:
    CFITSKeywordBool(CFITSKeywordBool const &toCopy) : CFITSKeyword(toCopy.keyword_, toCopy.comment()), value_(toCopy.value_) {}
    CFITSKeywordBool(std::string const &keyword) : CFITSKeyword(keyword), value_() {}
    CFITSKeywordBool(std::string const &keyword, std::uint8_t const &value) : CFITSKeyword(keyword), value_(value) {}
    CFITSKeywordBool(std::string const &keyword, std::uint8_t const &value, std::string const &comment):
      CFITSKeyword(keyword, comment), value_(value) {}

    virtual CFITSKeywordBool &operator=(const CFITSKeywordBool &);

    virtual operator bool() const { return value_;}
    virtual operator std::string() const;

    virtual CFITSKeyword *createCopy() const;

    virtual KWType type() const;

    virtual void writeToFITS(fitsfile *) const;
  };
}

#endif // ACL_FITSKEYWORDBOOL_H
