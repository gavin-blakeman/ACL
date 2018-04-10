//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDBPhotometry
// SUBSYSTEM:						Class to support a photometry HDU
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, cfitsio, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2017 Gavin Blakeman.
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
// OVERVIEW:						Provides a class to "overlay" photometry information into a FITS file HDU. The class includes reading and
//                      writing routines to allow the information to be retrieved and stored in a FITS file.
//
// CLASSES INCLUDED:		CHDBPhotometry  - Class to encapsulate Photometry Data.
//
// CLASS HIERARCHY:     CHDB
//                        - CImageHDB
//                        - CHDBAsciiTable
//                          - CAstrometryHDB
//                          - CHDBPhotometry
//                        - CHDBBinTable
//
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-03-17 GGB - Moved class CHDBPhotometry into seperated file.
//                      2013-02-10 GGB - Moved class CHDBImage into seperate file.
//                      2013-02-01 GGB - Moved class CAstrometryHDB into seperate file.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2011-12-10 GGB - Move classed from AstroFile.h to HDB.h
//
//*********************************************************************************************************************************

#ifndef ACL_HDBPHOTOMETRY_H
#define ACL_HDBPHOTOMETRY_H

#include "PhotometryObservation.h"
#include "HDBBinTable.h"

  // Standard C++ header files.

#include <cstddef>
#include <memory>
#include <vector>

namespace ACL
{
  typedef std::vector<SPPhotometryObservation> DPhotometryObservationStore;

  /// @brief The CHDBPhotometry class is used to store photometry observation data within a FITS file structure.
  /// @details The photometry observation data related to the image is stored in a binary table. This class specialises the binary
  /// table HDB to represent photometry observational data.<br>
  /// When the FITS file is saved, the data is saved into a binary table. This allows the photometry data to be automatically
  /// reloaded when the FITS file is opened from disk again.

  class CHDBPhotometry : public CHDBBinTable
  {
  private:
    DPhotometryObservationStore photometryObservations;
    DPhotometryObservationStore::iterator photometryObservationsIterator;

  protected:
  public:
    CHDBPhotometry(CAstroFile *);
    explicit CHDBPhotometry(CHDBPhotometry const &);

    virtual PHDB createCopy() const;

    virtual EBlockType HDBType() const { return HDB_PHOTOMETRY;}

      // FITS file functions

    virtual void readFromFITS(fitsfile *);
    virtual void writeToFITS(fitsfile *);

      // Image manipulation functions

    virtual void imageFlip();
    virtual void imageFlop();
    virtual void imageRotate(FP_t);
    virtual void imageFloat(boost::tuple<AXIS_t, AXIS_t> const &, boost::tuple<AXIS_t, AXIS_t> const &);
    virtual void imageResample(long, long);
    virtual void binPixels(unsigned int);
    virtual void imageTransform(MCL::TPoint2D<FP_t> const &, MCL::TPoint2D<FP_t> const &, FP_t, FP_t, MCL::TPoint2D<FP_t> const &, std::unique_ptr<bool> &);
    virtual void imageCrop(MCL::TPoint2D<AXIS_t> origen, MCL::TPoint2D<AXIS_t> dims);
    virtual FP_t imageExposure() const { ACL_CODE_ERROR; }

      // Photometry functions

    virtual size_t photometryObjectCount() const;
    virtual bool photometryObjectAdd(SPPhotometryObservation);
    virtual bool photometryObjectRemove(SPPhotometryObservation);
    virtual bool photometryObjectRemove(std::string const &);
    void photometryObjectRemoveAll();

      // Iteration functions

    virtual SPPhotometryObservation photometryObjectFirst();
    virtual SPPhotometryObservation photometryObjectNext();
  };

  typedef std::shared_ptr<CHDBPhotometry> PHDBPhotometry;
}


#endif // ACL_HDBPHOTOMETRY
