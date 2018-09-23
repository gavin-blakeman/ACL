//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDBBinTable
// SUBSYSTEM:						Header/Data Blocks (HDB)
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL,  CFitsIO, boost::filesystem, boost::UUID
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images. The classes are designed to
//                      present a single interface to the other astro library classes while supporting several different file
//                      types.
//
// CLASSES INCLUDED:		HDBBinTable - Binary table HDB.
//
// CLASS HIERARCHY:     CHDB
//                        - CHDBBinTable
//                          - CPhotometryHDB
//                        - CHDBAsciiTable
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-12-30 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_HDBBINTABLE_H
#define ACL_HDBBINTABLE_H

  // Standard C++ library header files

#include <memory>

  // ACL library header files

#include "HDB.h"
#include "error.h"

namespace ACL
{
  class CHDBBinaryTable : public CHDB
  {
  private:
    struct SColumnType
    {
      std::string name;
      int type;
      LONGLONG repeat;
      LONGLONG width;
      void *data;

      SColumnType(std::string nn, int nt, LONGLONG nr, LONGLONG nw) : name(nn), type(nt), repeat(nr), width(nw) {}
    };

    int bitpix_;
    int tfields_;
    std::vector<SColumnType> columnData_;

    virtual bool loadFromRAW(LibRaw *) { ACL_CODE_ERROR; }

  protected:
  public:
    CHDBBinaryTable(CAstroFile *, std::string const &);
    explicit CHDBBinaryTable(CHDBBinaryTable const &);
    virtual ~CHDBBinaryTable() {}

    virtual std::unique_ptr<CHDB> createCopy() const override;

      // FITS file functions.

    virtual void readFromFITS(fitsfile *) override;
    virtual void writeToFITS(fitsfile *) override;

    virtual EBlockType HDBType() const { return BT_BTABLE;}

    virtual void loadFromRGBHP(SRGBHP_Ptr RGBData, EColour colour) {}

    virtual int BITPIX() const override { return bitpix_; }
    virtual void BITPIX(int bp) override { bitpix_ = bp;}
    virtual std::string XTENSION() const override {return FITS_XTENSION_TABLE;}

    virtual FP_t imageExposure() const override { ACL_CODE_ERROR; }
  };
}  // namespace ACL

#endif // ACL_HDBBINTABLE_H
