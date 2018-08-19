//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDB.h
// SUBSYSTEM:						Astronomical HDb classes
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images.
//											The classes are designed to present a single interface to the other astro library
//											classes while supporting several different file types.
//
//											File types supported for input are:   FITS
//																								            SBIG image files
//                                                            DNG Files
//                                                            RAW Files
//                      File types supported for output are": FITS
//
// CLASSES INCLUDED:		CHDB            - Class to encapsulate header and data information.
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
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 released.
//                      2013-03-17 GGB - Moved class CHDBPhotometry into seperated file.
//                      2013-02-10 GGB - Moved class CHDBImage into seperate file.
//                      2013-02-01 GGB - Moved class CAstrometryHDB into seperate file.
//                      2013-01-20 GGB - astroManager 0000.00 released.
//                      2011-12-10 GGB - Move classed from AstroFile.h to HDB.h
//
//*********************************************************************************************************************************

#ifndef ACL_HDB_H
#define ACL_HDB_H

  // ACL Library

#include "AstroCatalogue.h"
#include "AstroImage.h"
#include "Astrometry.h"
#include "FITSKeyword.h"
#include "FITSStrings.h"

  // Standard C++ libraries

#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <vector>

  // Boost

#include "boost/optional.hpp"

  // cfitsio

#include "fitsio.h"

  // MIscellaneous libraries

#include <MCL>
#include <SCL>

  // The following are defined in cfitsio and need to be undefined to be able to include libraw (winnt.h).
  // As cfitsio is never accessed directly by the code in this library, this will not be a concern.

#ifdef WIN32
#undef TBYTE
#endif

  // LibRaw

#include "../../LibRaw/libraw/libraw.h"

namespace ACL
{

    /// @brief The type of HDB (Header and Data Block).

  enum EBlockType		// Type of the data block.
  {
    BT_NONE,          ///< No block stored.
    BT_IMAGE,         ///< An image
    BT_BTABLE,        ///< A binary table
    BT_ATABLE,        ///< An ascii table
    HDB_ASTROMETRY,   ///< Astrometry data
    HDB_PHOTOMETRY    ///< Photometry data
  };

    // Forward Definitions

  class CAstroFile;

  typedef std::list<PFITSKeyword> DKeywordStore;
  class CHDB;
  typedef std::shared_ptr<CHDB> PHDB;

  /// @brief The CHDB class corresponds to an HDU in a  FITS file.
  /// @details As the class is used to store all the information that can be found in an HDU of a FITS file, there are a large
  /// number of pass-through functions that are used to manipulate data. This is the facade pattern This encapsulation is preferred
  /// to the idiom of requesting the contained objects and then operating on the contained objects.<br>

  class CHDB		// Structure to store a data block
  {
  private:
    bool bPrimary_;           ///< true - This HDB is the primary HDB. False - This HDB is not the primary HDB.
    bool bSimple;             ///< @todo Is this needed
    NAXIS_t naxis_;           ///< Number of axes (1 <= naxis <= 999). This limit is imposed by FITS standard.
    int pcount_;
    int gcount_;
    bool firstEdit_;          ///< true = HDB has not yet been edited. false = HDB has been edited.

    virtual void readKeywords(fitsfile *file);
    //virtual bool processHistory(PFITSKeyword);
    //virtual bool processComment(PFITSKeyword);

    virtual void writeKeywordsToFITS(fitsfile *) const;
    virtual void writeHistoryToFITS(fitsfile *) const;
    virtual void writeCommentsToFITS(fitsfile *) const;

  protected:
    std::vector<AXIS_t> naxisn_;                ///< Size of each axis
    CAstroFile *parent_;                        ///< The parent instance.
    std::string extname_;                       ///< Name of the HDB
    DKeywordStore keywords_;	                  ///< Keywords for the HDB
    DKeywordStore::iterator keywordIterator_;   ///< Iterator for the keywords.
    std::vector<std::string> comments_;         ///< Comments
    std::vector<std::string> history_;          ///< History

    virtual void firstEdit();

      // Special keyword functions

    virtual bool specialKeyword(PFITSKeyword);

  public:
    CHDB(CAstroFile *, std::string const &);
    CHDB(CHDB const &);

      // Operators

    virtual bool operator==(std::string const &) const;

    virtual PHDB createCopy() const = 0;

      // FITS file functions.

    virtual void readFromFITS(fitsfile *);
    virtual void writeToFITS(fitsfile *);

       // RAW functions

#ifdef USE_LIBRAW
    virtual bool loadFromRAW(LibRaw *) = 0; //{ ACL_CODE_ERROR; }
#endif
    virtual void loadFromRGBHP(SRGBHP_Ptr RGBData, EColour colour) = 0;
      // Information functions

    virtual std::string HDBName() const { return extname_;}
    virtual EBlockType HDBType() const { return BT_NONE; }

    virtual bool PRIMARY() const { return bPrimary_; }
    virtual void PRIMARY(bool p) { bPrimary_ = p; }

      // FITS information functions

    virtual bool SIMPLE() const;
    virtual std::string XTENSION() const = 0;

    virtual int BITPIX() const = 0;
    virtual void BITPIX(int) = 0;

    virtual NAXIS_t NAXIS() const { return naxis_; }
    virtual void NAXIS(NAXIS_t);

    virtual AXIS_t NAXISn(NAXIS_t) const;
    virtual void NAXISn(std::vector<AXIS_t>::size_type, AXIS_t);
    virtual std::vector<AXIS_t> const &NAXISn() const { return naxisn_;}

    virtual int PCOUNT() const { return pcount_;}
    virtual void PCOUNT(int);

    virtual int GCOUNT() const { return gcount_;}
    virtual void GCOUNT(int);

    virtual FP_t BSCALE() const { CODE_ERROR(ACL); }
    virtual void BSCALE(FP_t) { CODE_ERROR(ACL); }

    virtual FP_t BZERO() const { CODE_ERROR(ACL); }
    virtual void BZERO(FP_t) { CODE_ERROR(ACL); }

    virtual FP_t EXPOSURE();

    virtual int PEDESTAL() const { CODE_ERROR(ACL); }

    CAstroTime const &getObservationTime() const;
    std::unique_ptr<CWeather> &getObservationWeather() const;

      // Keyword functions

    virtual size_t keywordCount() const;
    virtual bool keywordDelete(std::string const &);
    virtual bool keywordExists(std::string const &) const;
    virtual KWType keywordType(std::string const &) const;
    virtual DKeywordStore &keywordStore();
    virtual CFITSKeyword const &keywordData(std::string const &) const;
    virtual void copyKeywords(PHDB const &);

    void keywordWrite(std::string const &, std::int8_t const &, std::string const &);
    void keywordWrite(std::string const &, std::int16_t const &, std::string const &);
    void keywordWrite(std::string const &, std::int32_t const &, std::string const &);
    void keywordWrite(std::string const &, std::int64_t const &, std::string const &);
    void keywordWrite(std::string const &, std::uint8_t const &, std::string const &);
    void keywordWrite(std::string const &, std::uint16_t const &, std::string const &);
    void keywordWrite(std::string const &, std::uint32_t const &, std::string const &);
    void keywordWrite(std::string const &, float const &, std::string const &);
    void keywordWrite(std::string const &, double const &, std::string const &);
    void keywordWrite(std::string const &, std::string const &, std::string const &);
    virtual void keywordWrite(PFITSKeyword const &);

      // Keyword iteration functions

    virtual PFITSKeyword keywordIteratorFirst();
    virtual PFITSKeyword keywordIteratorNext();
    virtual PFITSKeyword keywordIteratorPrev();
    virtual PFITSKeyword keywordIteratorLast();

      // Comment functions

    virtual void commentWrite(std::string const &);
    virtual std::string commentGet() const;

      // History functions

    virtual void historyWrite(std::string const &);
    virtual std::string historyGet() const;

      // Image functions

    virtual AXIS_t width() const { CODE_ERROR(ACL); }
    virtual AXIS_t height() const { CODE_ERROR(ACL); }
    virtual bool isMonoImage() const { CODE_ERROR(ACL); }
    virtual bool isPolyImage() const { CODE_ERROR(ACL); }
    virtual void imageSet(CAstroImage *) { CODE_ERROR(ACL); }
    virtual CAstroImage *imageGet() { CODE_ERROR(ACL); }
    virtual MCL::TPoint2D<FP_t> &getPixelSize() { CODE_ERROR(ACL); }
    virtual FP_t imageExposure() const = 0;

      // Image rendering functions

    virtual renderImage_t *getRenderedImage() const { CODE_ERROR(ACL); }
    virtual void setImagePlaneRenderFunction(size_t, FP_t, FP_t, bool, ETransferFunction, FP_t) { CODE_ERROR(ACL); }
    virtual void setImagePlaneColourValues(size_t, SColourRGB, FP_t) { CODE_ERROR(ACL); }
    virtual void renderImage(ERenderMode) { CODE_ERROR(ACL); }
    virtual FP_t blackPoint() { CODE_ERROR(ACL); }
    virtual FP_t whitePoint() { CODE_ERROR(ACL); }
    virtual FP_t getMaxValue() const { CODE_ERROR(ACL); }
    virtual FP_t getMeanValue() const { CODE_ERROR(ACL); }
    virtual FP_t getStDevValue() const { CODE_ERROR(ACL); }
    virtual FP_t getMinValue() const { CODE_ERROR(ACL); }
    virtual void objectProfile(MCL::TPoint2D<FP_t> /*centroid*/, long /*radius*/, std::vector<boost::tuple<FP_t, FP_t> > &/*data*/) const {CODE_ERROR(ACL); }

      // Image manipulation functions

    virtual void imageFlip() { CODE_ERROR(ACL); }
    virtual void imageFlop() { CODE_ERROR(ACL); }
    virtual void imageRotate(FP_t) { CODE_ERROR(ACL); }
    virtual void imageFloat(AXIS_t, AXIS_t, long) { CODE_ERROR(ACL); }
    virtual void imageResample(long, long) { CODE_ERROR(ACL); }
    virtual void binPixels(unsigned int) { CODE_ERROR(ACL); }
    virtual void imageTransform(MCL::TPoint2D<FP_t> const &, MCL::TPoint2D<FP_t> const &, FP_t const &, FP_t const &, MCL::TPoint2D<FP_t> const &, std::unique_ptr<bool> &) { CODE_ERROR(ACL); }
    virtual void imageCrop(MCL::TPoint2D<AXIS_t>, MCL::TPoint2D<AXIS_t> ) { CODE_ERROR(ACL); }

      // Calibration functions

    virtual void calibrationApplyDark(CAstroImage const &) { CODE_ERROR(ACL); }
    virtual void calibrationApplyFlat(CAstroImage const &) { CODE_ERROR(ACL); }

      // Image Analysis functions

    virtual boost::optional<MCL::TPoint2D<FP_t> > centroid(MCL::TPoint2D<AXIS_t> const &, AXIS_t, int) const { CODE_ERROR(ACL); }
    virtual void findStars(TImageSourceContainer &, SFindSources const &) const { CODE_ERROR(ACL); }
    //virtual bool starMatch(SAstroCatalogueContainer &referenceStars, double tol, WorldCoor &wcs) {CODE_ERROR(ACL); };
    virtual bool plateSolve(AXIS_t) { CODE_ERROR(ACL); }

      // Photometry functions

    virtual void pointPhotometry(SPPhotometryObservation) { CODE_ERROR(ACL); }
    virtual boost::optional<FP_t> FWHM(MCL::TPoint2D<FP_t> const &) const { CODE_ERROR(ACL); }

      // WCS functions

    virtual bool hasWCSData() const { CODE_ERROR(ACL); }
    virtual boost::optional<CAstronomicalCoordinates> pix2wcs(MCL::TPoint2D<FP_t> const &) const { CODE_ERROR(ACL); }
    virtual boost::optional<MCL::TPoint2D<FP_t>> wcs2pix(CAstronomicalCoordinates const &) const { CODE_ERROR(ACL); }

  };

}  // namespace ACL

#endif  // ACL_HDB_H
