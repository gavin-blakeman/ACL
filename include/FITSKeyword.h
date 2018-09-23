//*************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSKeyword
// SUBSYSTEM:						HDU Keyword Support
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost, SCL.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2018 Gavin Blakeman.
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
// CLASSES INCLUDED:		CFITSKeyword  - Virtual base class
//
// CLASS HIERARCHY:     CFITSKeyword
//                        CFITSKeywordBool
//                        CFITSKeywordDouble
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
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-06-24/GGB - Removed dependency on SCL and deriving from SCL::CPackage.
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2011-06-04 GGB - Development of classes for astroManager
//
// NOTE:                Prior to 2016-04 this heirarchy was implemented using a templated class approach. Faced with the need to
//                      specialise most of the functionality based in the information stored, it was decided to switch to a "proper"
//                      derived class approach. This allows full functionallity to be easier understood and fine-tuned. It also
//                      allows easier control of specialised keywords such as dates etc. This approach has also allowd a tightly
//                      coupled approach to the FITS keywords to be implemented.
//
//**********************************************************************************************************************************

#ifndef ACL_KEYWORD_H
#define ACL_KEYWORD_H

  // Standard C++ library header files

#include <memory>
#include <string>
#include <sstream>

  // ACL library header files

#include "error.h"
#include "FITSException.h"
#include "FITSStrings.h"

  // Miscellaneous library header files

#include "fitsio.h"
#include <GCL>

namespace ACL
{
  /// @brief Keyword Type

  enum KWType
  {
    KWT_NONE,       ///< Not defined
    KWT_DOUBLE,     ///< Double value
    KWT_FLOAT,      ///< Float value
    KWT_BOOL,       ///< boolean value
    KWT_STRING,     ///< String value
    KWT_INT08,      ///< 8 bit signed integer
    KWT_INT16,      ///< 16 bit signed integer
    KWT_INT32,      ///< 32 bit signed integer
    KWT_INT64,      ///< 64 bit signed integer
    KWT_UINT08,     ///< 8 bit unsigned integer
    KWT_UINT16,     ///< 16 bit unsigned integer
    KWT_UINT32,      ///< 32 bit unsigned integer
    KWT_DATE,
    KWT_DATETIME,
    KWT_COMPLEX,
    KWT_DOUBLECOMPLEX,
  };

  /// @brief CFITSKeyword is the virtual base class for storage of keywords.
  /// @details Implements all functionality except the actual value. All the individual keyword classes are derived from the
  /// CFITSKeyword class.

  class CFITSKeyword
  {
  private:
  protected:
    std::string keyword_;
    std::string comment_;

  public:
    CFITSKeyword(std::string const &);
    CFITSKeyword(std::string const &, std::string const &);

    virtual ~CFITSKeyword() {}

    virtual bool operator==(CFITSKeyword const &) const;
    virtual bool operator==(std::string const &) const;
    virtual bool operator!=(std::string const toTest) { return (!((*this) == toTest)) ;}

    virtual CFITSKeyword &operator=(double) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(float) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::int8_t) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::int16_t) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::int32_t) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::int64_t) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::string)  { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::uint8_t) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::uint16_t) { ACL_CODE_ERROR; }
    virtual CFITSKeyword &operator=(std::uint32_t) { ACL_CODE_ERROR; }

    virtual operator bool() const { ACL_CODE_ERROR; }
    virtual operator std::int8_t() const { ACL_CODE_ERROR; }
    virtual operator std::int16_t() const { ACL_CODE_ERROR; }
    virtual operator std::int32_t() const { ACL_CODE_ERROR; }
    virtual operator std::int64_t() const { ACL_CODE_ERROR; }
    virtual operator std::uint8_t() const { ACL_CODE_ERROR; }
    virtual operator std::uint16_t() const { ACL_CODE_ERROR; }
    virtual operator std::uint32_t() const { ACL_CODE_ERROR; }
    virtual operator float() const { ACL_CODE_ERROR; }
    virtual operator double() const { ACL_CODE_ERROR; }
    virtual operator std::string() const { ACL_CODE_ERROR; }

    inline virtual std::string keyword() const { return keyword_;}
    inline virtual std::string comment() const { return comment_;}

    virtual std::unique_ptr<CFITSKeyword> createCopy() const = 0;

    virtual KWType type() const { return KWT_NONE; }

    virtual void writeToFITS(fitsfile *) const = 0;
  };

}	// namespace

#endif // ACL_KEYWORD_H
