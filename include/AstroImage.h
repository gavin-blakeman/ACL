//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroImage
// SUBSYSTEM:						Astronomical Image Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, cfitsio, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2018 Gavin Blakeman.
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
//
// OVERVIEW:						A number of classes supporting astronomical images. The focus is on the	manipulation of the images and on
//                      providing displayable data. No image display is done by these classes, although they can provide raw data
//                      for display.
//
//											These classes are tightly linked with the cfitsio library. This linkage is required as these classes do
//                      ultimately service astronomical images and the standard format for astonomical images is FITS.
//
//											It is not intended that these classes support other formats at this time (2010), but support for other
//                      image formats may be added. (DNG, JPG, TIFF)
//
//											The library is designed to be platform independant. IE, only standard C++ functionallity is used.
//
//											While this library has been designed to be used from Qt, it makes no reference to the Qt library.
//
//
// CLASSES INCLUDED:		CAstroImage					- Virtual class
//
// CLASS HIERARCHY:     CAstroImage
//												- CAstroImageMono
//												- CAstroImagePoly
//
// FITS FILE INFORMATION:
//											In order to simplify the reading process for FITS files, the following assumptions are made.
//											  1) The image is contained in the primary extension.
//											  2) If there is an EXTENSION with the name DARKFRAME it is loaded as the dark frame.
//											  3) If there is an EXTENSION with the name FLATFRAME it is loaded as the flat frame.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-05-25 GGB - Moved CAstroImageMono & CAstroImagePoly to their own files.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROIMAGE_H
#define ACL_ASTROIMAGE_H

  // Standard C++ library header files.

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <valarray>
#include <vector>

  // ACL library header files.

#include "common.h"
#include "config.h"
#include "ImagePlane.h"
#include "PhotometryObservation.h"

  // Miscellaneous library header files.

#include "fitsio.h"
#include <MCL>

#ifdef _MSC_VER
#pragma warning( disable : 4290 )  /* Disable the warning about C++ exception specifications. */
#endif

namespace ACL
{

typedef std::vector<std::unique_ptr<CImagePlane>> DImagePlaneStorage;

  class CAstroImageMono;
  class CAstroImagePoly;

  /// @brief The CAstroImage class stores image data for an image.
  /// @details All images need to store the data in some format. In the Astronomy Class Library, images are stored as a number of
  /// planes where each 2D plane can store the information for a single colour. In a black and white image, there will be only one
  /// image plane, while a colour image will have multiple image planes.<br>
  /// It may be thought that a colour image will have three image planes (RGB), but in astronomy it is possible to have non-visible
  /// image planes (think UV or radio images). By storing each set of data as a seperate image plane, it is possible to build up
  /// images having multiple data sources and to then render the image as a visible image using false colour techniques.<br>
  /// As an example, it is possible to build an image from a simple observatory using the standard photometric (UBVRI) filters that
  /// has five image planes. (See also the CAstoImagePoly class)<br>
  /// This storage method aligns well with the storage method used in FITS files.
  ///
  /// @note The operator[] is specifically not implemented as it is not compatible with the three levels of indexing that are
  /// required for colour images.

  class CAstroImage
  {
  private:
    CAstroImage(CAstroImage const &) = delete;
    CAstroImage & operator=(CAstroImage const&) = delete;

    void deleteRenderImageArray();

  protected:
    AXIS_t dimX;                            ///< x-dimension of the image (width)
    AXIS_t dimY;                            ///< y-dimension of the image (height)
    AXIS_t colourPlanes;                    ///< Number of colour planes in the image.
    DImagePlaneStorage imagePlaneStorage;   ///< Container for the image planes.
    renderImage_t *renderedImage;           ///< Output image. Typecast to required format.

      // Constructors and Destructors

    CAstroImage();
    CAstroImage(AXIS_t newX, AXIS_t newY, AXIS_t newColour = 1);

      // Rendering functions

    void renderImageGrey8();
    void renderImageRGB32();
    void renderImageRGBD();

  public:
    virtual ~CAstroImage();

      // Factory functions

    virtual std::unique_ptr<CAstroImage> createCopy() const = 0;
    static std::unique_ptr<CAstroImage> CreateAstroImage(std::vector<AXIS_t> const &naxisn);

    virtual void loadFromRGBHP(SRGBHP_Ptr RGBData, EColour colour) = 0;

      // File functions

    virtual void readFromFITS(fitsfile *) = 0;
    virtual void writeToFITS(fitsfile *);

      // imagePlane functions

    virtual int BITPIX() const = 0;
    virtual void BITPIX(int) = 0;

    virtual NAXIS_t NAXIS() const = 0;
    virtual AXIS_t NAXISn(NAXIS_t) const;

    virtual double BSCALE() const;
    virtual void BSCALE(double) = 0;

    virtual double BZERO() const =0;
    virtual void BZERO(double) = 0;

    virtual int PEDESTAL() const = 0;

    virtual void insertImagePlane(std::unique_ptr<CImagePlane> &);

    virtual std::optional<MCL::TPoint2D<FP_t> > findCentroid(MCL::TPoint2D<AXIS_t> const &, AXIS_t) const = 0;
    virtual void objectProfile(MCL::TPoint2D<FP_t> centroid, AXIS_t radius, std::vector<boost::tuple<FP_t, FP_t> > &data) const = 0;

      // Image information functions

    virtual FP_t getMin(size_t = 0) const;
    virtual FP_t getMax(size_t = 0) const;
    virtual FP_t getMean(size_t = 0) const;
    virtual FP_t getMedian(size_t = 0) const;
    virtual FP_t getStDev(size_t = 0) const;
    inline virtual AXIS_t width() const { return dimX;}   /// @brief Returns the width (naxis1) of the image.
                                                          /// @returns The width of the image.
                                                          /// @throws Nothing.

    inline virtual AXIS_t height() const { return dimY;}  /// @brief Returns the height (naxis2) of the image.
                                                          /// @returns The height of the image.
                                                          /// @throws Nothing.

    virtual bool isMonoImage() const {return false;}      ///< @brief Checks for a monochrome image.
                                                          ///< @details A monochrome image has only one colour plane.
                                                          ///< @returns true = monochrome image
                                                          ///< @returns false = not a monochrome image.
                                                          ///< @throws Nothing.
                                                          ///< @note !isMonoImage() is not equivalent to isPolyImage().

    virtual bool isPolyImage() const {return false;}      ///< @brief Checks for a polychrome image.
                                                          ///< @details A polychrome image has more than colour plane.
                                                          ///< @returns true = polychrome image
                                                          ///< @returns false = not a polychrome image.
                                                          ///< @throws Nothing.
                                                          ///< @note !isPolyImage() is not equivalent to isMonoImage().

      // Element access functions.

    virtual FP_t getValue(AXIS_t, AXIS_t, AXIS_t = 0) const;
    virtual FP_t getValue(INDEX_t, AXIS_t = 0) const;
    virtual void setValue(AXIS_t, AXIS_t, AXIS_t, FP_t);

    template<typename T>
    void setValue(INDEX_t index, T value)
    {
      for (std::unique_ptr<CImagePlane> const &imagePlane : imagePlaneStorage)
      {
        imagePlane->setValue(index, value);
      }
    }

    virtual INDEX_t imageArraySize() const { return (dimX * dimY * colourPlanes); }

      // Operators

    //virtual const CAstroImage operator+(CAstroImage const &) = 0
    virtual CAstroImage &operator+=(CAstroImage const &) = 0;
    //virtual const CAstroImage operator-(CAstroImage const &)  = 0;
    virtual CAstroImage &operator-=(CAstroImage const &) = 0;
    //virtual CAstroImage &operator*(double) = 0;
    virtual CAstroImage &operator*=(FP_t);
    virtual CAstroImage &operator/=(FP_t);
    virtual FP_t operator()(AXIS_t, AXIS_t, AXIS_t=0);
    virtual FP_t operator()(AXIS_t, AXIS_t, AXIS_t=0) const;

      // Image Rendering Functions

    virtual void setImagePlaneRenderFunction(size_t, FP_t, FP_t, bool, ETransferFunction, FP_t);
    virtual void setImagePlaneColourValues(size_t, SColourRGB, FP_t);
    virtual void renderImage(ERenderMode);
    virtual renderImage_t *getRenderedImage() const;

      // Image Manipulation functions

    virtual void rotate(FP_t);
    virtual void flip();
    virtual void flop();
    virtual void floatImage(AXIS_t, AXIS_t, long);
    virtual void resampleImage(AXIS_t, AXIS_t);
    virtual void resampleImage(FP_t );
    virtual void binPixels(unsigned int);
    virtual void transform(MCL::TPoint2D<FP_t> const &, MCL::TPoint2D<FP_t> const &, FP_t, FP_t, MCL::TPoint2D<FP_t> const  &, std::unique_ptr<bool[]> &);
    virtual void crop(MCL::TPoint2D<AXIS_t> const &, MCL::TPoint2D<AXIS_t> const &);

      // Image Calibration functions

    //virtual void applyCalibration(CMasterDarkFrame const &, CFlatFrame const &) = 0;
    virtual void applyDark(CAstroImage const &) = 0;
    //virtual void applyFlat(CFlatFrame const &) = 0;

      // Image Analysis functions

    virtual void findStars(TImageSourceContainer &, SFindSources const &) const = 0;
    virtual std::optional<MCL::TPoint2D<FP_t> > centroid(MCL::TPoint2D<AXIS_t> const &, AXIS_t, int) const = 0;

      // Photometry functions

    virtual void photometry(SPPhotometryObservation) const = 0;
    virtual std::optional<FP_t> FWHM(MCL::TPoint2D<FP_t> const &) const = 0;
  };

}	// namespace ACL

#endif // ACL_ASTROIMAGING_H
