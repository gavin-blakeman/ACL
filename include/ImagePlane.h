//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								ImagePlane
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
// OVERVIEW:						A number of classes supporting astronomical images. The focus is on the	manipulation of the images and on
//                      providing displayable data. No image display is done by these classes, although they can provide raw data
//                      for display.
//
//											These classes are tightly linked with the CFitsIO library. This linkage
//                      is required as these classes do ultimately service astronomical images and the standard format for
//                      astonomical images is FITS.
//
//											The library is designed to be platform independant. IE, only standard C++ functionality is used.
//
//											While this library has been designed to be used from Qt, it makes no reference to the Qt library.
//
// CLASSES INCLUDED:		CImagePlane					- A single plane image.
//
// CLASS HIERARCHY:     CImagePlane
//
// HISTORY:             2018-08-25 GGB - Changed imagePointer types to std::unique_ptr<>()
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-04-14 GGB - Changed filename from CImagePlane.h to ImagePlane.h
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_CIMAGEPLANE_H
#define ACL_CIMAGEPLANE_H

  // Standard C++ library header files

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <tuple>

  // ACL Library header file.

#include "common.h"
#include "config.h"
#include "findstar.h"
#include "SourceExtraction.h"

  // Miscellaneous Library header files.

#include "boost/shared_array.hpp"
#include "fitsio.h"
#include <GCL>
#include <MCL>
#include <SCL>

namespace ACL
{
  /// @brief Function to calculate an array offset.
  /// @param[in] x - x-coordinate
  /// @param[in] y - y-coordinate
  /// @param[in] xd - The x dimension
  /// @returns The index in the array.
  /// @throws None.
  /// @version 2013-04-21/GGB - Moved out of class CImagePlane

  inline INDEX_t arrayIndex(AXIS_t x, AXIS_t y, AXIS_t xd)
  {
    return (x + y * xd);
  }

  /// @brief Returns the index of a point in the array.
  /// @param[in] x - x-coordinate
  /// @param[in] y - y-coordinate
  /// @param[in] xm - minimum x value
  /// @param[in] ym - minimum y value
  /// @param[in] xd - dimension of x axis
  /// @returns The index in the array.
  /// @throws None.
  /// @version 2013-04-21/GGB - Moved out of class CImagePlane
  /// @version 2011-05-31/GGB - Function created.

  inline INDEX_t arrayIndex(AXIS_t x, AXIS_t y, AXIS_t xm, AXIS_t ym, const AXIS_t xd)
  {
    return ( (x - xm) + ((y - ym) * xd) );
  }

  /// The structure SThreadData is used by the transform function for passing data.

  struct SThreadData
  {
    MCL::TPoint2D<FP_t> ct;                       ///< The translation amount of the transform.
    MCL::TPoint2D<FP_t> c0;                       ///< The zero point of the transform.
    FP_t th;                                      ///< The angle of the transform (radians)
    FP_t sc;                                      ///< The scale of the transform
    MCL::TPoint2D<FP_t> pix;                      ///< The pixel size
    std::unique_ptr<double[]> newImagePlane;      ///< The new image plane to write the data to.
    std::unique_ptr<bool[]> &maskPlane;           ///< If valid, the mask plane.

    SThreadData(std::unique_ptr<bool[]> &maskPlane_) : maskPlane(maskPlane_){}
  };

  /// @brief The CImagePlane class stores image plane data from an image.
  ///
  /// All images need to store the data in some format. In the Astronomy Class Library, images are stored as a number of planes
  /// where each 2D plane can store the information for a single colour. In a black and white image, there will be only one image
  /// plane, while a colour image will have multiple image planes.<br>
  /// It may be thought that a colour image will have three image planes (RGB), but in astronomy it is possible to have non-visible
  /// image planes (think UV or radio images). By storing each set of data as a seperate image plane, it is possible to build up
  /// images having multiple data sources and to then render the image as a visible image using false colour techniques.<br>
  /// As an example, it is possible to build an image from a simple observatory using the standard photometric (UBVRI) filters that
  /// has five image planes. (See also the CAstoImagePoly class)<br>
  /// This storage method aligns well with the storage method used in FITS files.

  class CImagePlane
  {
  private:
      // Basic information

    AXIS_t dimX;                                ///< x dimension of image (width)
    AXIS_t dimY;                                ///< y dimension of image (height)

      // Statistical information

    mutable FP_t fMin;                          ///< The minimum value in the image.
    mutable FP_t fMax;                          ///< The maximum value in the image.
    mutable FP_t fMean;                         ///< The mean value of the image.
    mutable FP_t fStDev;                        ///< The standard deviation of the image.

    mutable bool bMinMax;                       ///< Variable to track if the min/max values are valid or need to be recalculated.
    mutable bool bMean;                         ///< Track if the average value is valid or needs to be recalculated.

    int bitpix_;                                ///< same as BITPIX
    FP_t bscale_;                               ///< Same as BSCALE
    FP_t bzero_;                                ///< Same as BZERO
    int pedestal_;                              ///< zero offset in ADU.

      // The following members are used for rendering the image.

    SColourRGB planeColour;                     ///< The colour to render this plane to
    FP_t transparency;                          ///< The transparency of this plane.
    FP_t blackPoint;                            ///< THe black point for this plane.
    FP_t whitePoint;                            ///< The white point for this plane.
    bool invert;                                ///< Invert the image plane.
    ETransferFunction transferFunction;         ///< Transfer function to use when rendering.
    FP_t transferFunctionParameter;             ///< Transfer function parameter.

      // The following arrays are used for storing the image. Note that std::valarray was used at one time for storing the image,
      // the reason for moving from std::valarray is not recorded, but it must have been good.
      // It is possibly a good idea to write a specialised valarray template that supports multi-threading and also c-style array
      // access specifically for storing the image plane data.

    std::unique_ptr<std::uint8_t[]> imagePlane8;
    std::unique_ptr<std::int8_t[]> imagePlaneS8;
    std::unique_ptr<std::uint16_t[]> imagePlaneU16;
    std::unique_ptr<std::int16_t[]> imagePlane16;
    std::unique_ptr<std::uint32_t[]> imagePlaneU32;
    std::unique_ptr<std::int32_t[]> imagePlane32;
    std::unique_ptr<std::int64_t[]> imagePlane64;
    std::unique_ptr<float[]> imagePlaneF;
    std::unique_ptr<double[]> imagePlaneD;

    template<typename T>
    void copyImagePlane(T const *, T*);

    template<typename T, typename U>
    void convertImagePlane(T const *, U *);
    template<typename T, typename U>
    void convertImagePlaneThread(T const *, U *, INDEX_t, INDEX_t);

    template<typename T>
    void crop(T const *, T *, MCL::TPoint2D<AXIS_t> const &, MCL::TPoint2D<AXIS_t> const &);

    template<typename T>
    void floatImage(std::unique_ptr<T[]> &imagePlane, AXIS_t newWidth, AXIS_t newHeight, long newBkgnd);

    template<typename T>
    void mirrorAxisX(std::unique_ptr<T[]> &);
    template<typename T>
    void mirrorAxisXThread(AXIS_t rowBegin, AXIS_t rowEnd, T *, T *);

    template<typename T>
    void mirrorAxisY(std::unique_ptr<T[]> &);
    template<typename T>
    void mirrorAxisYThread(AXIS_t rowBegin, AXIS_t rowEnd, T *imagePlane, T *newImagePlane);

    template<typename T>
    void rotate(std::unique_ptr<T[]> &imagePlane, std::tuple<AXIS_t, AXIS_t, AXIS_t, AXIS_t>, FP_t, FP_t, FP_t, FP_t);
    template<typename T>
    void rotateThread(std::tuple<AXIS_t, AXIS_t>, T *, T *, std::tuple<AXIS_t , AXIS_t, AXIS_t, AXIS_t>, FP_t, FP_t, FP_t, FP_t);

      // Thread functions for doing the heavy lifting. Called from the public functions as required.

    void plusThread(CImagePlane const &, std::tuple<AXIS_t, AXIS_t> const &);
    void minusThread(CImagePlane const &, std::tuple<AXIS_t, AXIS_t> const &);
    void transformThread(AXIS_t yStart, AXIS_t yEnd, SThreadData const &);
    void resampleThread(double *newImagePlane, boost::tuple<AXIS_t, AXIS_t> yVals, AXIS_t newWidth, boost::tuple<FP_t, FP_t>);

    template<typename T>
    void binPixelsThread(T *newImagePlane, std::tuple<AXIS_t, AXIS_t>, AXIS_t, unsigned int nsize);

    void deleteImagePlane();

  protected:

      // Interpolation functions ... Called from functions that require interpolation

    FP_t bilinear(FP_t const &, FP_t const &) const;
    FP_t bicubic(FP_t const &, FP_t const &) const;

      // Rendering functions

    void renderImageGrey8CbrtThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd);
    void renderImageGrey8LinearThread(renderImage_t *outputImage, std::tuple<INDEX_t, INDEX_t> const &startEnd);
    void renderImageGrey8GammaThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd);
    void renderImageGrey8GammaLogThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd);
    void renderImageGrey8SigmoidThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd);
    void renderImageGrey8SqrtThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd);
    void renderImageGrey8SquareThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd);
    void renderImageGrey8LogThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd);

  public:
    explicit CImagePlane();                                     // Default constructor.
    CImagePlane(CImagePlane const &);                           // Copy constructor.
    CImagePlane(CImagePlane const *, AXIS_t, AXIS_t, AXIS_t, AXIS_t);   // Create a sub-image from an image.
    CImagePlane(AXIS_t, AXIS_t);	// Constructor with a size
    CImagePlane(AXIS_t, AXIS_t, AXIS_t, AXIS_t); // Constructor with an origen and size.
    CImagePlane(fitsfile *, AXIS_t);         // Constructor from a FITS HDU with slicing.

    /// @brief Data constructor for class.
    /// @param[in] bitpix: The value for BITPIX
    /// @throws 0x1200 - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
    /// @throws 0x1206 - CIMAGEPLANE: Invalid BITPIX value.
    /// @version 2012-12-01/GGB - Added support for FITS native data types
    /// @version 2011-05-04/GGB - Function created.

    template<typename T>
    CImagePlane(int bitpix, AXIS_t x, AXIS_t y, std::vector<T> const &data, FP_t bscale, FP_t bzero, int pedestal) :
      bitpix_(bitpix), bscale_(bscale), bzero_(bzero), pedestal_(pedestal)
    {
      INDEX_t const dim = x * y;
      INDEX_t lIndex;

      dimX = x;
      dimY = y;

      fMin = fMax = data[0];

      T *newImagePlane = new T[dim];

      for(lIndex = 0; lIndex < dim; lIndex++)
      {
        newImagePlane[lIndex] = data[lIndex];
        fMax = std::max(fMax, newImagePlane[lIndex]);
        fMin = std::min(fMin, newImagePlane[lIndex]);
      };

      bMinMax = true;
      bMean = false;

      switch (bitpix)
      {
      case BYTE_IMG:
        imagePlane8 = reinterpret_cast<std::uint8_t *>(newImagePlane);
        break;
      case SHORT_IMG:    // (16)
        imagePlane16 = reinterpret_cast<std::int16_t *>(newImagePlane);
        break;
      case LONG_IMG:
        imagePlane32 = reinterpret_cast<std::int32_t *>(newImagePlane);
        break;
      case LONGLONG_IMG:
        imagePlane64 = reinterpret_cast<std::uint64_t *>(newImagePlane);
        break;
      case FLOAT_IMG:
        imagePlaneF = reinterpret_cast<float *>(newImagePlane);
        break;
      case DOUBLE_IMG:
        imagePlaneD = reinterpret_cast<double *>(newImagePlane);
        break;
      default:
        ERROR(ACL, 0x1203); // CIMAGEPLANE: Invalid BITPIX value.
        break;
      };
    }

    CImagePlane(AXIS_t, AXIS_t, unsigned short *);

    virtual ~CImagePlane();

      // Basic arithmetic operations

    CImagePlane &operator=(const CImagePlane &);
    CImagePlane &operator+=(const CImagePlane &);
    CImagePlane &operator-=(const CImagePlane &);
    CImagePlane &operator*=(CImagePlane const &);
    CImagePlane &operator*=(double);
    CImagePlane &operator/=(const CImagePlane &);
    CImagePlane &operator/=(double);
    CImagePlane &operator%=(CImagePlane const &);			// Used for absolute difference
    const CImagePlane operator+(const CImagePlane &);
    const CImagePlane operator-(const CImagePlane &);
    const CImagePlane operator*(CImagePlane const &);
    const CImagePlane operator/(const CImagePlane &);
    const CImagePlane operator%(CImagePlane const &);	// Used for absolute difference
    FP_t operator[](INDEX_t) const;
    FP_t operator()(AXIS_t, AXIS_t) const;

      // File functions

    void readFromFITS(fitsfile *, AXIS_t);
    void writeToFITS(fitsfile *, AXIS_t);

    int BITPIX() const;
    void BITPIX(int);

    double BSCALE() const;
    void BSCALE(double);

    double BZERO() const;
    void BZERO(double);

    inline int PEDESTAL() const noexcept { return pedestal_; }

      // Basic image information

    inline AXIS_t width() const noexcept { return dimX; }
    inline AXIS_t height() const noexcept { return dimY; }

    FP_t getValue(INDEX_t) const;
    FP_t getValue(AXIS_t, AXIS_t) const;
    double getValue(MCL::TPoint2D<AXIS_t> const &) const;
    void getRow(AXIS_t x1, AXIS_t x2, AXIS_t y, double *);

    /// @brief Sets the value of a particular pixel.
    /// @param[in] index: The index of the pixel.
    /// @param[in] value: The value to set.
    /// @throws 0x1200 - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
    /// @throws 0x1201 - CIMAGEPLANE: setValue(index), getValue(index). index is beyond end of array.
    /// @version 2017-08-27/GGB - Ensure min/max are recalculated as required.
    /// @version 2013-01-27/GGB - Function created.

    template<typename T>
    void setValue(INDEX_t index, T value)
    {
      if ( (index >= (INDEX_t) (dimX * dimY)) )
      {
        ERROR(ACL, 0x1202);    // CIMAGEPLANE: setValue(index), getValue(index). index is beyond end of array.
      }
      else
      {
        switch (bitpix_)
        {
          case BYTE_IMG:
          {
            imagePlane8[index] = static_cast<std::uint8_t>(value);
            break;
          };
          case SBYTE_IMG:
          {
            imagePlaneS8[index] = static_cast<std::int8_t>(value);
            break;
          };
          case USHORT_IMG:
          {
            imagePlaneU16[index] = static_cast<std::uint16_t>(value);
            break;
          };
          case SHORT_IMG:
          {
            imagePlane16[index] = static_cast<std::int16_t>(value);
            break;
          };
          case ULONG_IMG:
          {
            imagePlaneU32[index] = static_cast<std::uint32_t>(value);
            break;
          };
          case LONG_IMG:
          {
            imagePlane32[index] = static_cast<std::uint32_t>(value);
            break;
          };
          case LONGLONG_IMG:
          {
            imagePlane64[index] = static_cast<boost::uint64_t>(value);
            break;
          };
          case FLOAT_IMG:
          {
            imagePlaneF[index] = static_cast<float>(value);
            break;
          };
          case DOUBLE_IMG:
          {
            imagePlaneD[index] = static_cast<double>(value);
            break;
          };
          default:
          {
            ERROR(ACL, 0x1203);  // CIMAGEPLANE: Invalid BITPIX value.
            break;
          };
        };
      };
      bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
    }

    /// @brief Sets the value of the image at the relevant point to the passed value.
    /// @param[in] x: The x coordinate
    /// @param[in] y:  The y coordinate
    /// @param[in] value: The value to set.
    /// @throws GCL::CError(ACL, 0x1202)
    /// @version 2010-10-16/GGB - Function created.

    template<typename T>
    void setValue(AXIS_t x, AXIS_t y, T value)
    {
      if ( (x >= 0) && (y >= 0) && (x < dimX) && (y < dimY))
      {
        setValue(arrayIndex(x,y, dimX), value);
      }
      else
      {
        ERROR(ACL, 0x1202);
      };
    }

      // Basic image statistics

    FP_t getMaxValue() const;
    FP_t getMeanValue() const;
    FP_t getMedianValue() const;
    FP_t getStDevValue() const;
    FP_t getMinValue() const;

      // Image Rendering Functions

    virtual void setImagePlaneRenderFunction(FP_t, FP_t, bool, ETransferFunction, FP_t);
    virtual void setImagePlaneColourValues(SColourRGB, FP_t);
    virtual void renderImageGrey8(renderImage_t *);
    virtual void renderImageRGB32(renderImage_t *) const {}

      // Basic Image transformation

    void crop(AXIS_t, AXIS_t, AXIS_t, AXIS_t);
    void crop(const MCL::TPoint2D<AXIS_t> &, const MCL::TPoint2D<AXIS_t> &);
    void floatImage(AXIS_t newWidth, AXIS_t newHeight, long newBkgnd);
    void mirrorAxisX();
    void mirrorAxisY();
    void rotate(FP_t);
    void rotate(FP_t, FP_t, FP_t);
    void translate(AXIS_t const, AXIS_t const);
    void translate(FP_t, FP_t);
    void scale(FP_t, const MCL::TPoint2D<FP_t> &);
    void transform(MCL::TPoint2D<FP_t> const &co, MCL::TPoint2D<FP_t> const &ct, FP_t, FP_t, MCL::TPoint2D<FP_t> const &, std::unique_ptr<bool[]> &);
    void resample(AXIS_t const &, AXIS_t const &);
    void binPixels(unsigned int);

      // Image information functions

    MCL::TPoint2D<FP_t> starCentroid(MCL::TPoint2D<AXIS_t> const &) const;
    std::optional<MCL::TPoint2D<AXIS_t> > brightWalk(MCL::TPoint2D<AXIS_t> const &guess, AXIS_t rmax, int sensitivity) const;
    std::optional<FP_t> FWHM(MCL::TPoint2D<AXIS_t> const &, AXIS_t radius) const;
    void objectProfile(MCL::TPoint2D<FP_t> centroid, AXIS_t radius, std::vector<boost::tuple<FP_t, FP_t> > &data) const;

      // Image Analysis functions

    void findStars(TImageSourceContainer &sourceList, SFindSources const &sourceDefaults);

      // Object special functionality

    virtual boost::shared_array<FP_t> objectProfile2D(const MCL::TPoint2D<AXIS_t> &center, AXIS_t halfLength, FP_t angle);
  };

  typedef boost::shared_ptr<CImagePlane> PImagePlane;

}  // namespace ACL

#endif // ACL_CIMAGEPLANE_H
