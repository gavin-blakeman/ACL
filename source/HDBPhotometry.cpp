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

#include "../include/HDBPhotometry.h"

  // ACL header files

#include "../include/AstroFile.h"
#include "../include/AstroImageFunctions.hpp"

  // Boost library

#include "boost/assign.hpp"

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // class CHDBPhotometry
  //
  //*******************************************************************************************************************************

  /// @brief Class constructor
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  CHDBPhotometry::CHDBPhotometry(CAstroFile *np) : CHDBBinTable(np, astroManager_HDB_PHOTOMETRY), photometryObservations()
  {
  }

  /// @brief Copy constructor for the class.
  /// @throws None.
  /// @brief 2013-06-08/GGB - Function created.

  CHDBPhotometry::CHDBPhotometry(CHDBPhotometry const &toCopy) : CHDBBinTable(toCopy)
  {
    DPhotometryObservationStore::const_iterator iterator;

    for (iterator = toCopy.photometryObservations.begin(); iterator != toCopy.photometryObservations.end(); ++iterator)
    {
      SPPhotometryObservation newObservation( std::dynamic_pointer_cast<CPhotometryObservation>((*iterator)->createCopy()) );

      photometryObservations.push_back(newObservation);
    };
  }

  /// @brief Function to create a copy of *this.
  /// @returns A copy of this.
  /// @throws std::bad_alloc
  /// @version 2013-06-08/GGB - Function created.

  PHDB CHDBPhotometry::createCopy() const
  {
    PHDB returnValue(new CHDBPhotometry(*this));

    return returnValue;
  }

  /// Processes the flipping of the image. All the coordinates need to be flipped.
  //
  // 2013-05-13/GGB - Function created.

  void CHDBPhotometry::imageFlip()
  {
    DPhotometryObservationStore::iterator iter;

    for (iter =  photometryObservations.begin(); iter != photometryObservations.end(); iter++)
    {
      (*iter)->CCDCoordinates() = ACL::imageFlip( (*iter)->CCDCoordinates(), parent_->NAXISn(2));
    };
  }

  /// Processes the flopping of the image. All the coordinates need to be flopped.
  //
  // 2013-05-13/GGB - Function created.

  void CHDBPhotometry::imageFlop()
  {
    DPhotometryObservationStore::iterator iter;

    for (iter =  photometryObservations.begin(); iter != photometryObservations.end(); iter++)
    {
      (*iter)->CCDCoordinates() = ACL::imageFlop( (*iter)->CCDCoordinates(), parent_->NAXISn(2));
    };
  }

  /// Processes rotating of the image.
  //
  // 2013-05-13/GGB - Function created.

  void CHDBPhotometry::imageRotate(FP_t angle)
  {
    DPhotometryObservationStore::iterator iter;

    for (iter =  photometryObservations.begin(); iter != photometryObservations.end(); iter++)
    {
      (*iter)->CCDCoordinates() = ACL::imageRotate( MCL::TPoint2D<long>(0, 0), (*iter)->CCDCoordinates(), angle);
    };
  }

  /// Processes floating of the image.
  //
  // 2013-05-14/GGB - Function created.

  void CHDBPhotometry::imageFloat(const boost::tuple<AXIS_t, AXIS_t> &oldDim, const boost::tuple<AXIS_t, AXIS_t> &newDim)
  {

    DPhotometryObservationStore::iterator iter;

    for (iter =  photometryObservations.begin(); iter != photometryObservations.end(); iter++)
    {
      (*iter)->CCDCoordinates() = ACL::imageFloat((*iter)->CCDCoordinates(), oldDim, newDim);
    };
  }

  /// Function called when the image is resampled.
  //
  // 2013-05-14/GGB - Function created.

  void CHDBPhotometry::imageResample(long rx, long ry)
  {
    DPhotometryObservationStore::iterator iterator;

    for (iterator = photometryObservations.begin(); iterator != photometryObservations.end(); iterator++)
      (*iterator)->CCDCoordinates(ACL::imageResample((*iterator)->CCDCoordinates(), rx, ry));
  }

  /// @brief Processes the binning of pixels.
  /// @param[in] nsize - The binning factor.
  /// @throws None.
  /// @version 2015-09-03/GGB - (Bug 48) Updated to use lambda functions.
  /// @version 2013-05-13/GGB - Function created.

  void CHDBPhotometry::binPixels(unsigned int nsize)
  {
    std::for_each(photometryObservations.begin(), photometryObservations.end(),
                  [&] (SPPhotometryObservation po) { po->CCDCoordinates() = ACL::imageBinPixels( po->CCDCoordinates(), nsize); });
  }

  /// @brief Performs the transform (TRS) on the point.
  /// @param[in] center - The center of the transformation.
  /// @param[in] ct - The amount to translate the image.
  /// @param[in] angle - The angle to rotate the image.
  /// @param[in] scale - The scaling parameter.
  /// @param[in] pixelSize - The size of the pixels.
  /// @version 2013-05-15/GGB - Function created.

  void CHDBPhotometry::imageTransform(MCL::TPoint2D<FP_t> const &center, MCL::TPoint2D<FP_t> const &ct, FP_t angle, FP_t scale,
                                      MCL::TPoint2D<FP_t> const &pixelSize, std::unique_ptr<bool> &)
  {
    DPhotometryObservationStore::iterator iter;

    for (iter =  photometryObservations.begin(); iter != photometryObservations.end(); iter++)
    {
      (*iter)->CCDCoordinates() = ACL::imageTransformForward( (*iter)->CCDCoordinates(), center, ct, angle, scale, pixelSize);
    };
  }

  /// @brief Performs cropping on the photometry HDB.
  /// @param[in] origin - The origin of the crop
  /// @param[in] dims - The dimensions of the crop.
  /// @throws None.
  /// @details Updates the coordinates of photometry objects that are stored in the HDB after a crop of the image. Any
  /// objects that are not part of the new image are removed from the list and deleted.
  /// @version 2013-05-15/GGB - Function created.

  void CHDBPhotometry::imageCrop(MCL::TPoint2D<AXIS_t> origin, MCL::TPoint2D<AXIS_t> dims)
  {
    DPhotometryObservationStore::iterator iter;

    for (iter =  photometryObservations.begin(); iter != photometryObservations.end(); iter++)
    {
      boost::optional<MCL::TPoint2D<FP_t> > temp = ACL::imageCrop( (*iter)->CCDCoordinates(), origin, dims);
      if (temp)
      {
        (*iter)->CCDCoordinates() = *temp;
      }
      else
      {
          // Delete the observation as it falls outside of the new image.

        photometryObservations.erase(iter);
      }
    };
  }

  /// @brief Adds a photometry object to the list of photometry objects.
  //
  // 2013-08-29/GGB - Added code to update value of NAXISn(2) (Bug #1213104)
  // 2012-11-11/GGB - Function created.

  bool CHDBPhotometry::photometryObjectAdd(SPPhotometryObservation toAdd)
  {
    photometryObservations.push_back(toAdd);
    NAXISn(2, photometryObservations.size() );
    return true;
  }

  /// Returns the count of objects in the photometry list.
  //
  // 2013-08-03/GGB - Function created.

  size_t CHDBPhotometry::photometryObjectCount() const
  {
    return photometryObservations.size();
  }

  /// Deletes the photometry observation with the name passed.
  //
  // 2013-08-29/GGB - Added code to update value of NAXISn(2) (Bug #1213104)
  // 2013-08-11/GGB - Function created.

  bool CHDBPhotometry::photometryObjectRemove(std::string const &toRemove)
  {
    assert(toRemove.size() != 0);

    DPhotometryObservationStore::iterator iter = photometryObservations.begin();

    while ( (iter != photometryObservations.end()) && (*(*iter) != toRemove) )
      ++iter;

    if (iter != photometryObservations.end())
    {
      photometryObservations.erase(iter);
      NAXISn(2, photometryObservations.size() );
      return true;
    }
    else
      return false;
  }

  /// @brief Removes the specified photometry object from the list of photometry objects.
  /// @param[in] toRemove - The photometry object to remove.
  /// @returns true - object found and removed.
  /// @returns false - object not found.
  /// @throws None.
  /// @version 2015-08-17/GGB - Updated to use C++11 functions.
  /// @version 2013-08-29/GGB - Added code to update value of NAXISn(2) (Bug #1213104)
  /// @version 2012-11-11/GGB - Function created

  bool CHDBPhotometry::photometryObjectRemove(SPPhotometryObservation toRemove)
  {
    DPhotometryObservationStore::iterator iter = std::find(photometryObservations.begin(), photometryObservations.end(), toRemove);

    if ( iter != photometryObservations.end())
    {
      photometryObservations.erase(iter);
      NAXISn(2, photometryObservations.size() );
      return true;
    }
    else
    {
      return false;
    };
  }

  /// @brief Removes all the elements from the list.
  /// @throws None.
  /// @version 2013-08-29/GGB - Added code to update value of NAXISn(2) (Bug #1213104)
  /// @version 2013-05-10/GGB - Function created.

  void CHDBPhotometry::photometryObjectRemoveAll()
  {
    photometryObservations.clear();
    NAXISn(2, photometryObservations.size() );
  }

  /// @brief Returns the first item in the photometry object list.
  /// @returns The first item in the photometryObject list.
  /// @throws None.
  /// @version 2012-11-11/GGB - Function created.

  SPPhotometryObservation CHDBPhotometry::photometryObjectFirst()
  {
    photometryObservationsIterator = photometryObservations.begin();

    if (photometryObservationsIterator == photometryObservations.end())
    {
      return SPPhotometryObservation();
    }
    else
    {
      return (*photometryObservationsIterator);
    }

  }

  /// @brief Returns the next item in the photometry object list.
  /// @returns The next item in the photometry list.
  /// @throws None.
  /// @version 2012-11-11/GGB - Function created.

  SPPhotometryObservation CHDBPhotometry::photometryObjectNext()
  {
    if (photometryObservationsIterator == photometryObservations.end())
    {
      return SPPhotometryObservation();
    }
    else if (++photometryObservationsIterator == photometryObservations.end())
    {
      return SPPhotometryObservation();
    }
    else
    {
      return (*photometryObservationsIterator);
    };
  }

  /// @brief Loads the HDB from the FITS HDB. All the keywords and data are loaded.
  /// @param[in] file - Pointer to FITS file to open from.
  /// @note The magnitude and magnitude error do not need to be read from the HDU as they are calculated values.
  /// @note This function overrides the CHDBBinTable::readFromFits function. Do not call the CHDBBinTable function.
  /// @throws CError(ACL::0x190C) - HDB: Cannot dynamic_cast to BinTable.
  /// @throws CRuntimeAssert
  /// @version 2015-08-11/GGB - Converted to cfitsio.
  /// @version 2013-08-31/GGB - Added code for case when there are no objects in the HDB. (Bug #1213104)
  /// @version 2013-08-12/GGB - Added egain value support.
  /// @version 2013-05-09/GGB - Added support for aperture serialisation.
  /// @version 2013-04-07/GGB - Function created.

  void CHDBPhotometry::readFromFITS(fitsfile *file)
  {
    size_t const nameLength = 40;
    int status = 0;

    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr");

    CHDB::readFromFITS(file);   // Call the parent to load all the common stuff as well as the keywords.

    std::vector<std::string> objectNames;
    std::vector<FP_t> RA(NAXISn(2));
    std::vector<FP_t> DEC(NAXISn(2));
    std::vector<FP_t> CCDX(NAXISn(2));
    std::vector<FP_t> CCDY(NAXISn(2));
    std::vector<FP_t> sourceADU(NAXISn(2));
    std::vector<FP_t> sourceArea(NAXISn(2));
    std::vector<FP_t> skyADU(NAXISn(2));
    std::vector<FP_t> skyArea(NAXISn(2));
    std::vector<FP_t> FWHM(NAXISn(2));
    std::vector<std::valarray<std::uint8_t> > aperture(NAXISn(2));

    std::vector<std::string>::size_type columnIndex = 0;
    DPhotometryObservationStore::size_type index;
    SPPhotometryObservation newRecord;
    FP_t exposure = 0;
    FP_t egain = 1;

    if (NAXISn(2) > 0)
    {
      try
      {
        if (keywordExists(SBIG_EGAIN))
        {
          egain = static_cast<FP_t>(keywordData(SBIG_EGAIN));
        }
        else if (parent_->keywordExists(0, SBIG_EGAIN))
        {
          egain = static_cast<FP_t>(parent_->keywordData(0, SBIG_EGAIN));
        };

        exposure = EXPOSURE();
      }
      catch(...)
      {
        try
        {
          exposure = parent_->getHDB(0)->EXPOSURE();
          keywordWrite(NOAO_EXPTIME, exposure, NOAO_COMMENT_EXPTIME);
        }
        catch(...)
        {
          exposure = 1;
        };
      };

        // Load the table data now.

      char objectName[nameLength+1];

      CFITSIO_TEST(fits_read_col, file, TSTRING, columnIndex++, 1, 1, 1, 0, objectName, 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, RA.data(), 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, DEC.data(), 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, CCDX.data(), 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, CCDY.data(), 0);
      columnIndex++;   // Don't read magnitude
      columnIndex++;   // Don't read magErr
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, sourceADU.data(), 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, sourceArea.data(), 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, skyADU.data(), 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, skyArea.data(), 0);
      CFITSIO_TEST(fits_read_col, file, TFLOAT, columnIndex++, 1, 1, NAXISn(2), 0, FWHM.data(), 0);

        // Now construct the objects.

      for (index = 0; index < static_cast<DPhotometryObservationStore::size_type>(NAXISn(2)); index++)
      {
        if (aperture[index].size() != 0)
        {
          newRecord.reset(new CPhotometryObservation(objectNames[index]));
          if ( RA[index] != DOUBLENULLVALUE)
          {
            newRecord->observedCoordinates(CAstronomicalCoordinates(MCL::angle_t(RA[index], MCL::AF_Dd), MCL::angle_t(DEC[index], MCL::AF_Dd)));
          };
          newRecord->CCDCoordinates(MCL::TPoint2D<FP_t>(CCDX[index], CCDY[index]));
          newRecord->sourceADU(sourceADU[index]);
          newRecord->sourceArea(sourceArea[index]);
          newRecord->skyADU(skyADU[index]);
          newRecord->skyArea(skyArea[index]);
          newRecord->exposure(exposure);
          newRecord->gain(egain);
          if (FWHM[index] != DOUBLENULLVALUE)
          {
            newRecord->FWHM(boost::optional<FP_t>(FWHM[index]));
          };
          PPhotometryAperture photometryAperture(CPhotometryAperture::createAperture(aperture[index]));
          photometryAperture->serialiseIn(aperture[index]);
          newRecord->photometryAperture(photometryAperture);

          photometryObservations.push_back(newRecord);
        };
      };
    };
  }

  /// @brief Function to write data to the FITS file.
  /// @param[in] file - Pointer to the fits file to be written to.
  /// @throws 0x190C - HDB: Cannot dynamic_cast to BinaryTable.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @details The data relevant to the photometry HDB must be written. The CHDB::writeToFITS
  /// function must also be called to ensure that the keywords are correctly written to the FITS file.
  /// @version 2015-08-11/GGB - Converted to cfitsio.
  /// @version 2013-05-08/GGB - Changed to use binary table for the information and added support for the aperture.
  /// @version 2013-04-07/GGB - Function created.

  void CHDBPhotometry::writeToFITS(fitsfile *file)
  {
      // Column names and definitions for the photometry HDU
      // Note that the last column is defined as a variable length column of 256 bytes. This is used for storing the aperture
      // information.

    static char *ttype[] = { "OBJECT NAME", "CENTROID-RA", "CENTROID-DEC", "CENTROID-X", "CENTROID-Y", "MAGNITUDE", "UNCERTANTY",
                             "SOURCE-ADU", "SOURCE-AREA", "SKY-ADU", "SKY-AREA", "FWHM", "APERTURE" };
    static char *tform[] = { "40A", "1D", "1D", "1D", "1D", "1D", "1D", "1D", "1D", "1D", "1D", "1D", "-1B"};
    static char *tunit[] = { "", "dd.dd", "dd.dd", "Pixels", "pixels", "MAG", "MAG", "ADU", "PIXELS", "ADU", "PIXELS", "PIXELS", ""};
    static int tfields = 13;


    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr.");

    std::vector<char> objectNames;
    std::vector<FP_t> RA(NAXISn(2));
    std::vector<FP_t> DEC(NAXISn(2));
    std::vector<FP_t> CCDX(NAXISn(2));
    std::vector<FP_t> CCDY(NAXISn(2));
    std::vector<FP_t> magnitude(NAXISn(2));
    std::vector<FP_t> magnitudeError(NAXISn(2));
    std::vector<FP_t> sourceADU(NAXISn(2));
    std::vector<FP_t> sourceArea(NAXISn(2));
    std::vector<FP_t> skyADU(NAXISn(2));
    std::vector<FP_t> skyArea(NAXISn(2));
    std::vector<FP_t> FWHM(NAXISn(2));
    std::vector<std::vector<std::uint8_t> > aperture(NAXISn(2));
    std::vector<std::string>::size_type columnIndex = 0;
    DPhotometryObservationStore::iterator iterator;
    std::vector<std::uint8_t> aps;
    int status;

      // Copy all the objects into the vectors in preperation for writing to the FITS file.

    for (iterator = photometryObservations.begin(); iterator != photometryObservations.end(); iterator++)
    {
      std::for_each((*iterator)->objectName().begin(), (*iterator)->objectName().end(),
                    [&] (char &i) { objectNames.push_back(i);});
      objectNames.push_back('\0');

        // If the observed coordinates are not defined, then write the nullValue.

      if ( (*iterator)->observedCoordinates())
      {
        RA.push_back((*(*iterator)->observedCoordinates()).RA().degrees());
        DEC.push_back((*(*iterator)->observedCoordinates()).DEC().degrees());
      }
      else
      {
        RA.push_back(DOUBLENULLVALUE);
        DEC.push_back(DOUBLENULLVALUE);
      };
      CCDX.push_back((*iterator)->CCDCoordinates().x());
      CCDY.push_back((*iterator)->CCDCoordinates().y());
      magnitude.push_back(*((*iterator)->instrumentMagnitude()));
      magnitudeError.push_back((*iterator)->magnitudeError());
      sourceADU.push_back((*iterator)->sourceADU());
      sourceArea.push_back((*iterator)->sourceArea());
      skyADU.push_back((*iterator)->skyADU());
      skyArea.push_back((*iterator)->skyArea());
      if ( (*iterator)->FWHM() )
      {
        FWHM.push_back(*(*iterator)->FWHM());
      }
      else
      {
        FWHM.push_back(DOUBLENULLVALUE);
      }
      //(*iterator)->photometryAperture()->serialiseOut(aps);
      //aperture.push_back(aps);        // Creates a copy of aps.
    };

    fits_create_tbl(file, BINARY_TBL, NAXISn(2), tfields, ttype, tform, tunit, const_cast<char *>(astroManager_HDB_PHOTOMETRY.c_str()),
                    &status);

    CHDB::writeToFITS(file);

    //CFITSIO_TEST(fits_write_col(bhdu->column(photometryColNames[columnIndex++]).write(objectNames, 1));
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), RA.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), DEC.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), CCDX.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), CCDY.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), magnitude.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), magnitudeError.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), sourceADU.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), sourceArea.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), skyADU.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), skyArea.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), FWHM.data());
    CFITSIO_TEST(fits_write_col, file, TDOUBLE, columnIndex++, 1, 1, NAXISn(2), aperture.data());
  }

}  // namespace ACL
