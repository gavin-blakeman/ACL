//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								BTableHDB
// SUBSYSTEM:						Header/Data Blocks (HDB)
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL,  CFitsIO, boost::filesystem, boost::UUID
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2016 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images. The classes are designed to
//                      present a single interface to the other astro library classes while supporting several different file
//                      types.
//
// CLASSES INCLUDED:		CHDBPrimary - A primary HDB. (Without an image)
//
// CLASS HIERARCHY:     CHDB
//                        CHDBPrimary
//
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2013-09-30 GGB - AIRDAS 2013.09 release.
//                      2013-03-20 GGB - AIRDAS 2013.03 release.
//                      2012-12-30 GGB - Development of classes for AIRDAS
//
//*********************************************************************************************************************************

#ifndef ACL_HDBPRIMARY_H
#define ACL_HDBPRIMARY_H

  // ACL header files

#include "HDB.h"

  // Boost library

#include "boost/shared_ptr.hpp"

namespace ACL
{
  /// @brief The CHDBPrimary class corresponds to a primary HDU (without an image) in a  FITS file.
  /// @details The class provides a wrapper for a keyword only primary HDU.

  class CHDBPrimary : public CHDB
  {
  private:
    virtual bool loadFromRAW(LibRaw *) { CODE_ERROR(ACL); }

  protected:
  public:
    CHDBPrimary(CAstroFile *af) : CHDB(af, "PRIMARY") {}
    explicit CHDBPrimary(CHDBPrimary const &);

    virtual PHDB createCopy() const;

    virtual void loadFromRGBHP(SRGBHP_Ptr RGBData, EColour colour) {}

    virtual std::string XTENSION() const { CODE_ERROR(ACL); }

    virtual int BITPIX() const { return BYTE_IMG; }
    virtual void BITPIX(int) {}

    virtual FP_t imageExposure() const { ACL_CODE_ERROR; }

  };
  typedef boost::shared_ptr<CHDBPrimary> PHDBPrimary;

}

#endif // ACL_HDBPRIMARY_H
