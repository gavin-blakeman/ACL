//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDBPhotometry
// SUBSYSTEM:						Class to support a photometry HDU
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, cfitsio.
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
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-03-17 GGB - Moved class CHDBPhotometry into seperated file.
//                      2013-02-10 GGB - Moved class CHDBImage into seperate file.
//                      2013-02-01 GGB - Moved class CAstrometryHDB into seperate file.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2011-12-10 GGB - Move classed from AstroFile.h to HDB.h
//
//*********************************************************************************************************************************

#ifndef ACL_HDBPHOTOMETRY_H
#define ACL_HDBPHOTOMETRY_H

  // Standard C++ library header files.

#include <cstddef>
#include <memory>
#include <tuple>
#include <vector>

  // ACL library header files.

#include "PhotometryObservation.h"
#include "HDBBinaryTable.h"

namespace ACL
{
  /// @brief The CHDBPhotometry class is used to store photometry observation data within a FITS file structure.
  /// @details The photometry observation data related to the image is stored in a binary table. This class specialises the binary
  ///          table HDB to represent photometry observational data.<br>
  ///          When the FITS file is saved, the data is saved into a binary table. This allows the photometry data to be
  ///          automatically reloaded when the FITS file is opened from disk again.

  class CHDBPhotometry : public CHDBBinaryTable
  {
  private:

    typedef std::vector<std::shared_ptr<CPhotometryObservation>> DPhotometryObservationStore;

      // Use a shared_ptr specifically as the calling application may wish to share ownership.

    DPhotometryObservationStore photometryObservations;
    DPhotometryObservationStore::iterator photometryObservationsIterator;

  protected:
  public:
    CHDBPhotometry(CAstroFile *);
    explicit CHDBPhotometry(CHDBPhotometry const &);

    virtual std::unique_ptr<CHDB> createCopy() const override;

    virtual EBlockType HDBType() const override { return HDB_PHOTOMETRY;}

      // FITS file functions

    virtual void readFromFITS(fitsfile *) override;
    virtual void writeToFITS(fitsfile *) override;

      // Image manipulation functions

    virtual void imageFlip() override;
    virtual void imageFlop() override;
    virtual void imageRotate(FP_t) override;
    virtual void imageFloat(std::tuple<AXIS_t, AXIS_t> const &, std::tuple<AXIS_t, AXIS_t> const &);
    virtual void imageResample(AXIS_t, AXIS_t) override;
    virtual void binPixels(unsigned int);
    virtual void imageTransform(MCL::TPoint2D<FP_t> const &, MCL::TPoint2D<FP_t> const &, FP_t, FP_t, MCL::TPoint2D<FP_t> const &, std::unique_ptr<bool> &);
    virtual void imageCrop(MCL::TPoint2D<AXIS_t> origen, MCL::TPoint2D<AXIS_t> dims);
    virtual FP_t imageExposure() const { ACL_CODE_ERROR; }

      // Photometry functions

    virtual size_t photometryObjectCount() const;
    virtual void photometryObjectAdd(std::shared_ptr<CPhotometryObservation>);
    //virtual bool photometryObjectRemove(CPhotometryObservation &);
    virtual bool photometryObjectRemove(std::string const &);
    void photometryObjectRemoveAll();

      // Iteration functions

    virtual CPhotometryObservation *photometryObjectFirst();
    virtual CPhotometryObservation *photometryObjectNext();
  };
}


#endif // ACL_HDBPHOTOMETRY
