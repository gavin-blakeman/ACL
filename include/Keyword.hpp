//*************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Keyword
// SUBSYSTEM:						Templated functions for keywords
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost, SCL.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2015 Gavin Blakeman.
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
// CLASSES INCLUDED:		CKeyword  - Virtual base class
//                      TKeyword  - Templated keyword class.
//
// CLASS HIERARCHY:     SCL::CPackage
//                        - CKeyword
//                          - TKeyword
//
// HISTORY:             2015-06-24/GGB - Removed dependency on SCL and deriving from SCL::CPackage.
//                      2013-09-30 GGB - AIRDAS 2013.09 release.
//                      2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2011-06-04 GGB - Development of classes for AIRDAS
//
// TO DO:               1. Consider changing the templated approach to using a single class with polymorphic (union, boost::any)
//                        implmentation.
//
//**********************************************************************************************************************************

#ifndef ACL_KEYWORD_H
#define ACL_KEYWORD_H

  // Standard library files

#include <string>
#include <sstream>

  // ACL Files

#include "../Include/FITSException.h"

  // Boost files

#include "boost/shared_ptr.hpp"

  // cfitsio

#include "cfitsio/fitsio.h"


  // GCL Library

#include <GCL>

namespace ACL
{
  ///@ brief Keyword Type
  enum KWType
  {
    KWT_NONE,       ///< Not defined
    KWT_DOUBLE,     ///< Double value
    KWT_FLOAT,      ///< Float value
    KWT_BOOL,       ///< boolean value
    KWT_STRING,     ///< String value
    KWT_LONG,       ///< Long integer value
    KWT_INT,        ///< Integer value
    KWT_UINT,       ///< Unsigned integer value.
    KWT_SHORT,
    KWT_UINT8,
    KWT_INT8,
    KWT_UINT16
  };

  /// @brief Base class for storage of keywords.
  /// @details Implements all functionality except the actual value.

  class CKeyword
  {
  private:
  protected:
    std::string keyword_;
    std::string comment_;

  public:
     CKeyword(std::string const &keyword);
     CKeyword(std::string const &keyword, std::string const &comment);

    //virtual bool operator==(CKeyword const &) const;
    virtual bool operator==(std::string const &toTest) const;
    virtual bool operator!=(std::string const toTest) { return (!((*this) == toTest)) ;}

    virtual CKeyword &operator=(const CKeyword &) = 0;

    virtual operator double() const = 0;
    virtual operator int() const = 0;
    virtual operator float() const = 0;
    virtual operator std::string() = 0;

    inline virtual std::string keyword() const { return keyword_;}
    inline virtual std::string value() const = 0;
    inline virtual std::string comment() const { return comment_;}
    inline virtual std::string comment() { return comment_;}

    virtual CKeyword *createCopy() const = 0;

    virtual void getString(std::string &, std::string &) const = 0;

    virtual KWType type() const { return KWT_NONE;}

    virtual void writeToFITS(fitsfile *) const = 0;
  };

  typedef boost::shared_ptr<CKeyword> PKeyword;

  template<typename T>
  class TKeyword : public CKeyword
  {
  private:
    T data_;

  protected:
  public:
    TKeyword() {}
    TKeyword(std::string const &keyword) : CKeyword(keyword), data_() {}
    TKeyword(std::string const &keyword, T const &data) : CKeyword(keyword), data_(data) {}
    TKeyword(std::string const &keyword, T const &data, std::string const &comment): CKeyword(keyword, comment), data_(data) {}

    T const &data() const { return data_;}

    virtual TKeyword &operator =(const CKeyword &);
    virtual bool operator==(const CKeyword &) const;

    virtual operator double() const;
    virtual operator int() const;
    virtual operator float() const;
    virtual operator std::string();

    virtual std::string value() const;

    virtual CKeyword *createCopy() const;

    virtual void getString(std::string &value, std::string &comment) const;

    virtual KWType type() const;

    virtual void writeToFITS(fitsfile *) const;
  };

  /// @brief Copy operator
  //
  // 2011-03-12/GGB - Function created.

  template<typename T>
  inline TKeyword<T> &TKeyword<T>::operator =(CKeyword const &rhs)
  {
    keyword_ = rhs.keyword();
    data_ = dynamic_cast<TKeyword const *>(&rhs)->data_;
    comment_ = rhs.comment();
    return (*this);
  }

  /// @brief Equality check operator
  //
  // 2011-03-12/GGB - Function created.

  template<typename T>
  inline bool TKeyword<T>::operator ==(const CKeyword &other) const
  {
    return (dynamic_cast<TKeyword<T> const *>(&other)->data_ == data_);
  }

  /// @brief Float operator. Just take the double value and make it a float.
  //
  // 2011-07-16/GGB - Function created.

  template<typename T>
  inline TKeyword<T>::operator float() const
  {
    return (float) (double) (*this);
  }

  template<typename T>
  inline void TKeyword<T>::getString(std::string &value, std::string &comment) const
  {
    comment = comment_;
    std::ostringstream out;
    out << data_;
    value = out.str();
  }

  template<>
  inline void TKeyword<bool>::getString(std::string &value, std::string &comment) const
  {
    if (data_)
    {
      value = std::string("T");
    }
    else
    {
      value = std::string("F");
    };
    comment = comment_;
  }

  /// @brief Returns the value as a string.
  /// Makes use of the std::string operator.
  //
  // 2013-06-20/GGB - Function created.

  template<>
  inline std::string TKeyword<bool>::value() const
  {
    std::string returnValue;

    if (data_)
      returnValue = std::string("T");
    else
      returnValue = std::string("F");

    return returnValue;
  }

  /// @brief std::string operator for boolean.
  //
  // 2011-09-01/GGB - Function created.

  template<>
  inline TKeyword<bool>::operator std::string()
  {
    if (data_)
    {
      return std::string("T");
    }
    else
    {
      return std::string("F");
    }
  }

  /// @brief std::string operator.
  //
  // 2011-09-01/GGB - Function created.

  template<typename T>
  inline TKeyword<T>::operator std::string()
  {
    std::ostringstream out;
    out << data_;
    return out.str();
  }

  /// @brief Returns the value as a string.
  /// @returns std::string representation of the value.
  /// @throws None.
  /// @version 2013-06-20/GGB - Function created.

  template<typename T>
  inline std::string TKeyword<T>::value() const
  {
    std::ostringstream out;
    out << data_;
    return out.str();
  }

  /// @brief Creates a copy of this object.
  /// @returns Pointer to the copy.
  /// @throws std::bad_alloc
  /// @version 2011-07-18/GGB - FUnction created.

  template<typename T>
  inline CKeyword *TKeyword<T>::createCopy() const
  {
    return (new TKeyword<T>(*this));
  }

  /// @brief Cast operator to convert to an integer value.
  /// @returns The value as an integer value.
  /// @throws GCL::CError(ACL, 0x0101) - KEYWORD: Unable to cast value... Out of range.
  /// @version 2015-08-23/GGB - Function created.

  template<typename T>
  inline TKeyword<T>::operator int() const
  {
    if ( (data_ >= std::numeric_limits<int>::min()) &&
         (data_ <= std::numeric_limits<int>::max()) )
    {
      return static_cast<int>(data_);   // Safe to cast.
    }
    else
    {
      ERROR(ACL, 0x0101);
    };
  }

  template<>
  inline TKeyword<std::string>::operator int() const
  {
    ERROR(ACL, 0x0100);    // Invalid typecast.
  }

  /// @brief Casting operator to a double.
  /// @version 2015-08-23/GGB - Function created.
  /// @note Casting any number to a double is always succesfull.
  /// @version 2015-09-04/GGB - Function created.

  template<typename T>
  inline TKeyword<T>::operator double() const
  {
    return static_cast<double>(data_);
  }

  template<>
  inline TKeyword<std::string>::operator double() const
  {
    ERROR(ACL, 0x0100);    // Invalid typecast.
  }

  /// @brief Writes the keyword as a native type to the FITS header.
  /// @param[in] file - The FITS file to write to.
  /// @throws GCL::CRuntimeAssert
  /// @version 2015-08-12/GGB - Converted to use cfitsio
  /// @version 2013-03-14/GGB - Function created.

  template<typename T>
  inline void TKeyword<T>::writeToFITS(fitsfile *file) const
  {
    int status = 0;
    char value[FLEN_VALUE];

    std::to_string(data_).copy(value, FLEN_VALUE-1);
    value[FLEN_VALUE-1] = 0;

    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr");

    CFITSIO_TEST(fits_update_key(file, TSTRING, keyword_.c_str(), value, comment_.c_str(), &status));
  }

  /// @brief Writes the keyword as a native type to the FITS header.
  /// @param[in] file - The FITS file to write to.
  /// @throws GCL::CRuntimeAssert
  /// @version 2015-08-12/GGB - Converted to use cfitsio
  /// @version 2013-03-14/GGB - Function created.

  template<>
  inline void TKeyword<std::string>::writeToFITS(fitsfile *file) const
  {
    int status = 0;
    char value[FLEN_VALUE];

    data_.copy(value, FLEN_VALUE-1);
    value[FLEN_VALUE-1] = 0;

    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr");

    fits_update_key(file, TSTRING, keyword_.c_str(), value, comment_.c_str(), &status);
  }

}	// namespace

#endif // ACL_KEYWORD_H
