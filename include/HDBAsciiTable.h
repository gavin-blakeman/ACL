//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDBAsciiTable
// SUBSYSTEM:						Astronomical HDb classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, CFitsIO, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images.
//											The classes are designed to present a single interface to the other astro library
//											classes while supporting several different file types.
//
// CLASSES INCLUDED:		CHDBAsciiTable      - Class to encapsulate Ascii Table HDB data.
//
// CLASS HIERARCHY:
//                      CHDB
//                        - CImageHDB
//                        - CHDBAsciiTable
//                          - CAstrometryHDB
//                          - CHDBPhotometry
//                        - CHDBBinTable
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-06-08/GGB - Move class CHDBAsciiTable into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 released.
//                      2013-03-17 GGB - Moved class CHDBPhotometry into seperated file.
//                      2013-02-10 GGB - Moved class CHDBImage into seperate file.
//                      2013-02-01 GGB - Moved class CAstrometryHDB into seperate file.
//                      2013-01-20 GGB - astroManager 0000.00 released.
//                      2011-12-10 GGB - Move classed from AstroFile.h to HDB.h
//
//*********************************************************************************************************************************

#ifndef ACL_HDBASCIITABLE_H
#define ACL_HDBASCIITABLE_H

// Standard C++ library header files

#include <cstddef>
#include <memory>
#include <vector>

  // ACL library header files.

#include "HDB.h"
#include "error.h"

namespace ACL
{
  class CHDBAsciiTable : public CHDB
  {
  private:
    int TFields;                        // The number of fields/Row
    std::vector<long> TBColn;           // Start value of each field.
    std::vector<std::string> TFormn;    // The format of each column.
    int bitpix_;

    virtual void setImagePlaneRenderFunction(size_t, FP_t, FP_t, bool, ETransferFunction, FP_t) { CODE_ERROR(ACL); }
    virtual void setImagePlaneColourValues(size_t, SColourRGB, FP_t) { CODE_ERROR(ACL); }
    virtual void renderImage(renderImage_t *, ERenderMode) { CODE_ERROR(ACL); }

  protected:
  public:
    CHDBAsciiTable(CAstroFile *, std::string const &);
    CHDBAsciiTable(CHDBAsciiTable const &);

    virtual std::unique_ptr<CHDB> createCopy() const override;

    virtual void loadFromRGBHP(SRGBHP_Ptr RGBData, EColour colour) {}

      // FITS functions

    virtual void readFromFITS(fitsfile *) override;
    virtual void writeToFITS(fitsfile *) override;

      // Information functions

    virtual EBlockType HDBType() const { return BT_ATABLE;}
    virtual std::string XTENSION() const {return FITS_XTENSION_TABLE;}

    virtual int BITPIX() const { return bitpix_;}
    virtual void BITPIX(int bp) { bitpix_ = bp;}

    virtual FP_t imageExposure() const { ACL_CODE_ERROR; }

  };
}

#endif // ACL_HDBASCIITABLE_H
