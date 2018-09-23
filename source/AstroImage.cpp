//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroImage
// SUBSYSTEM:						Astronomical Image Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	cfitsio, boost, MCL.
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
// OVERVIEW:						A number of classes supporting astronomical images. The focus is on the
//											manipulation of the images and on providing displayable data. No image
//											display is done by these classes, although they can provide raw data for
//											display.
//
//											These classes are tightly linked with the CFitsIO library. This linkage is required as these classes do
//											ultimately service astronomical images and the standard format for astonomical images is FITS.
//
//											It is not intended that these classes support other formats at this time (2010), but support for other image
//                      formats may be added. (DNG, JPG, TIFF)
//
// CLASSES INCLUDED:    CAstroImage       - Virtual base class for an astronomical image.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-05-25 GGB - Moved CAstroImageMono to it's own file
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../include/AstroImage.h"

  // ACL include files.

#include "../include/AstroImageMono.h"
#include "../include/AstroImagePoly.h"
#include "../include/config.h"
#include "../include/AstroImageFunctions.hpp"

  // Standard libraries

#include <cmath>
#include <cstdio>

  // Miscellaneous Libraries

#include "boost/thread/thread.hpp"
//#include "boost/scoped_array.hpp"
#include <GCL>
#include "sofam.h"

namespace ACL
{
  //********************************************************************************************************************************
  //
  // CAstroImage
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor for the class.
  /// @throws None
  /// @version 2015-06-30/GGB - Removed the outputImage member. (Bug 6)
  /// @version 2011-12-27/GGB - Function created.

  CAstroImage::CAstroImage() : dimX(0), dimY(0), colourPlanes(0)
  {
  }

  /// @brief Simple constructor.
  /// @param[in] newx - x dimension.
  /// @param[in] newy - y dimension.
  /// @param[in] newColour - Number of colour planes.
  /// @throws None.
  /// @version 2015-06-30/GGB - Removed the outputImage member. (Bug 6)
  /// @version 2011-12-27/GGB - Function created.

  CAstroImage::CAstroImage(AXIS_t newx, AXIS_t newy, AXIS_t newColour) : dimX(newx), dimY(newy), colourPlanes(newColour),
    renderedImage(nullptr)
  {
  }

  /// @brief Ensures that the dynamically allocated memory is released.
  /// @details Deletes the output image.
  /// @version 2011-12-27/GGB - Function created.

  CAstroImage::~CAstroImage()
  {
    if (renderedImage)
    {
      delete [] renderedImage;
      renderedImage = nullptr;
    };
  }

  /// @brief Multiplies the image by a fixed value.
  /// @returns Reference to this.
  /// @throws None.
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-07-28/GGB - Updated to reflect new method of storing image planes.
  /// @version 2011-05-12/GGB - Function created.

  CAstroImage &CAstroImage::operator*=(FP_t dbl)
  {
    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &ip) { *ip *= dbl; });

    return (*this);
  }

  /// @brief Division by a fixed value.
  /// @param[in] dbl: Factor to divide each point by.
  /// @returns Reference to this.
  /// @throws 0x2200 - Division by zero.
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-07-28/GGB - Updated to reflect new method of storing image planes.
  /// @version 2011-05-12/GGB - Function created.

  CAstroImage &CAstroImage::operator/=(FP_t dbl)
  {
    if (dbl == 0)
    {
      ACL_ERROR(0x2200);
    }
    else
    {
      std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                    [&] (std::unique_ptr<CImagePlane> const &ip) { *ip /= dbl; });
    };
    return (*this);
  }

  /// @brief Returns the value of the specified point
  /// @throws 0x2201 - ASTROIMAGE: Invalid image plane.
  /// @version 2011-12-27/GGB - Function created.

  FP_t CAstroImage::operator()(AXIS_t lx, AXIS_t ly, AXIS_t lp)
  {
    if ( (lp > 0) && (static_cast<size_t>(lp) > imagePlaneStorage.size()) )
    {
      return (*(imagePlaneStorage[lp]))(lx, ly);
    }
    else
    {
      ACL_ERROR(0x2201);      // ASTROIMAGE: Invalid image plane.
    };
  }

  /// @brief Returns the value of the specified point
  /// @returns The value at the specifid point
  /// @throws 0x2201 - ASTROIMAGE: Invalid image plane.
  /// @version 2011-12-27/GGB - Function created.

  FP_t CAstroImage::operator()(AXIS_t lx, AXIS_t ly, AXIS_t lp) const
  {
    if ( (lp > 0) && (static_cast<size_t>(lp) < imagePlaneStorage.size()) )
    {
      return (*(imagePlaneStorage[lp]))(lx, ly);
    }
    else
    {
      ACL_ERROR(0x2201);      // ASTROIMAGE: Invalid image plane.
    };
  }

  /// @brief Function to bin pixels in the required range.
  /// @details All the imagePlanes in the storage are iterated and binned. The statistical values are also recalculated.
  /// @param[in] nsize: The binning parameter size. (1 <= nsize <= 10)
  /// @throws GCL::CRuntimeAssert
  /// @throws std::bad_alloc
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2013-06-23/GGB - Added code to recalculate the mean and standard deviation.. (Bug # 1193740)
  /// @version 2011-06-10/GGB - Function created.

  void CAstroImage::binPixels(unsigned int nsize)
  {
    RUNTIME_ASSERT(ACL, nsize != 0, "Bin Pixels is only valid with a non-zero integer.");
    RUNTIME_ASSERT(ACL, nsize <= 10, "Bin Pixels is only valid with a binning value of <= 10.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &p) { p->binPixels(nsize); p->getMeanValue(); p->getStDevValue(); } );

      // Calculate the image dimensions.

    MCL::TPoint2D<AXIS_t> dims = imageBinPixels(MCL::TPoint2D<AXIS_t>(dimX, dimY), nsize);
    dimX = dims.x();
    dimY = dims.y();

      // Update the renderedImage

    deleteRenderImageArray();
  }

  /// @brief Gets the value of BSCALE
  /// @throws 0x2202 - ASTROIMAGE: No Image Plane available.
  /// @version 2012-11-30/GGB - Function created.

  FP_t CAstroImage::BSCALE() const
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202); //ASTROIMAGE: No Image Plane available.
    }
    else
    {
      //return imagePlaneStorage[0]->BSCALE();
    }
  }

  /// @brief Factory function to create a CAstroImage object.
  /// @param[in] naxisn: dimension of axis[n]
  /// @returns Unique pointer to a new AstroImage of the relevant class.
  /// @details Calls the relevant creation function for a poly image or a mono image. A number of axises greater that 3 is not
  ///          supported. (Will throw an error)
  /// @throws std::bad_alloc
  /// @throws 0x0008 - Naxis == 1 not allowed.
  /// @throws 0x0007 - Naxis > 3 not supported.
  /// @version 2018-09-14/GGB - Changed return type to unique_ptr.
  /// @version 2015-08-13/GGB - removed parameter for number of axes.
  /// @version 2013-03-13/GGB - Changed parameters and simplified function to work with readFromFITS() functions better.
  /// @version 2011-03-20/GGB - Function created.

  std::unique_ptr<CAstroImage> CAstroImage::CreateAstroImage(std::vector<AXIS_t> const &naxisn)
  {
    std::unique_ptr<CAstroImage> returnValue;

    if (naxisn.size() == 1)
    {
      ACL_ERROR(0x0008);
    }
    else if (naxisn.size() == 2)
    {
      returnValue = std::make_unique<CAstroImageMono>(naxisn[0], naxisn[1]);
    }
    else if (naxisn.size() == 3)
    {
      returnValue = std::make_unique<CAstroImagePoly>(naxisn[0], naxisn[1], naxisn[2]);
    }
    else
    {
      ACL_ERROR(0x0007);
    };

    return returnValue;
  }

  /// @brief Function to crop an image.
  /// @param[in] o: Crop Origin
  /// @param[in] d: Crop dimensions.
  /// @throws std::bad_alloc
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @throws GCL::CCodeError(ACL)
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-08-02/GGB - Removed check for no image planes.
  /// @version 2013-03-08/GGB - Changed TPoint from <float> to <double>
  /// @version 2011-08-15/GGB - Function created.

  void CAstroImage::crop(const MCL::TPoint2D<AXIS_t> &o, const MCL::TPoint2D<AXIS_t> &d)
  {
    RUNTIME_ASSERT(ACL, o.x() + d.x() <= dimX, "Cropped image extents will exceed current image extents.");
    RUNTIME_ASSERT(ACL, o.y() + d.y() <= dimY, "Cropped image extents will exceed current image extents.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &p) { p->crop(o, d); p->getMeanValue(); p->getStDevValue(); });

    dimX = d.x();
    dimY = d.y();

    deleteRenderImageArray();
  }

  /// @brief Delete the renderImage array for the required image type.
  /// @throws std::bad_alloc
  /// @version 2015-08-02/GGB - Function created.

  void CAstroImage::deleteRenderImageArray()
  {
    if (renderedImage)
    {
      delete [] renderedImage;
      renderedImage = nullptr;
    };
  }

  /// @brief Flips the image plane
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-08-02/GGB - Changed check for no image planes to a runtime assert.
  /// @version 2011-05-29/GGB - Function created.

  void CAstroImage::flip()
  {
    RUNTIME_ASSERT(ACL, !imagePlaneStorage.empty(), "No Image planes available.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [] (std::unique_ptr<CImagePlane> const &p) { p->mirrorAxisY(); } );
  }

  /// @brief Floats the image onto a larger image plane.
  /// @param[in] newWidth: The new width of the image.
  /// @param[in] newHeight: The new height of the image.
  /// @param[in] newBkgnd: The new background colour to use.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-08-02/GGB - Changed check for no image planes to a runtime assert.
  /// @version 2011-06-04/GGB - Function created.

  void CAstroImage::floatImage(AXIS_t newWidth, AXIS_t newHeight, long newBkgnd)
  {
    RUNTIME_ASSERT(ACL, !imagePlaneStorage.empty(), "No Image planes available.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &p) { p->floatImage(newWidth, newHeight, newBkgnd); });

      // Get the new image dimensions

    dimX = newWidth;
    dimY = newHeight;

    deleteRenderImageArray();
  }

  /// @brief Flops the image plane.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-08-02/GGB - Changed check for no image planes to a runtime assert.
  /// @version 2011-05-29/GGB - Function created.

  void CAstroImage::flop()
  {
    RUNTIME_ASSERT(ACL, !imagePlaneStorage.empty(), "No Image planes available.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [] (std::unique_ptr<CImagePlane> const &p) { p->mirrorAxisX(); } );
  }

  /// @brief Returns the maximum value from the indicated image plane.
  /// @param[in] ip: The image plane to return the maximum value for. (zero = first image plane)
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-07-30/GGB - Added runtime assertions and changed to new style image planes.
  /// @version 2010-10-30/GGB - Function created.

  FP_t CAstroImage::getMax(size_t ip) const
  {
    RUNTIME_ASSERT(ACL, ip >= 0, "Parameter ip must be >= 0");
    RUNTIME_ASSERT(ACL, ip <= imagePlaneStorage.size(), "Parameter ip must be < number of image planes.");

    return imagePlaneStorage[ip]->getMaxValue();
  }

  /// @brief Returns the Median value for the image.
  /// @throws CRuntimeAssert
  /// @version 2015-07-30/GGB - Function created.

  FP_t CAstroImage::getMedian(size_t ip) const
  {
    RUNTIME_ASSERT(ACL, ip >= 0, "Parameter ip must be >= 0");
    RUNTIME_ASSERT(ACL, ip <= imagePlaneStorage.size(), "Parameter ip must be < number of image planes.");

    return imagePlaneStorage[ip]->getMaxValue();
  }

  /// @brief Returns the minimum value from the indicated image plane.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-07-30/GGB - Added runtime assertions and changed to new style image planes.
  /// @version 2010-10-30/GGB - Function created.

  FP_t CAstroImage::getMin(size_t ip) const
  {
    RUNTIME_ASSERT(ACL, ip >= 0, "Parameter ip must be >= 0");
    RUNTIME_ASSERT(ACL, ip <= imagePlaneStorage.size(), "Parameter ip must be < number of image planes.");

    return imagePlaneStorage[ip]->getMinValue();
  }

  /// @brief Returns the mean from the indicated image plane.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-07-30/GGB - Added runtime assertions and changed to new style image planes.
  /// @version 2010-10-30/GGB - Function created.

  FP_t CAstroImage::getMean(size_t ip) const
  {
    RUNTIME_ASSERT(ACL, ip >= 0, "Parameter ip must be >= 0");
    RUNTIME_ASSERT(ACL, ip <= imagePlaneStorage.size(), "Parameter ip must be < number of image planes.");

    return imagePlaneStorage[ip]->getMeanValue();
  }

  /// @brief Returns the pointer to the output image array
  /// @returns A pointer to the output image array
  /// @throws None.
  /// @version 2010-10-16/GGB - Function created.

  renderImage_t *CAstroImage::getRenderedImage() const
  {
    return renderedImage;
  }

  /// @brief Returns the standard deviation of the image plane.
  /// @throws CRuntimeAssert
  /// @version 2015-07-30/GGB - Added runtime assertions and changed to new style image planes.
  /// @version 2011-08-26/GGB - Function created.

  FP_t CAstroImage::getStDev(size_t ip) const
  {
    RUNTIME_ASSERT(ACL, ip >= 0, "Parameter ip must be >= 0");
    RUNTIME_ASSERT(ACL, ip < imagePlaneStorage.size(), "Parameter ip must be < number of image planes.");

    return imagePlaneStorage[ip]->getStDevValue();
  }

  /// @brief Returns the image value from the (lx, ly) coordinates.
  /// @param[in] index - The index to get.
  /// @param[in] ip - 1 based image plane number.
  /// @returns The value of the specified coordinate.
  /// @throws GCL::CRuntimeAssert (ACL)
  /// @version 2015-08-01/GGB - Function created.

  FP_t CAstroImage::getValue(INDEX_t index, AXIS_t ip) const
  {
    RUNTIME_ASSERT(ACL, ip >= 0, "Parameter ip must be >= 0");
    RUNTIME_ASSERT(ACL, ip < imagePlaneStorage.size(), "Parameter ip must be < number of image planes.");

    return imagePlaneStorage[ip]->getValue(index);
  }

  /// @brief Returns the image value from the (lx, ly) coordinates.
  /// @param[in] lp - 1 based image plane number.
  /// @returns The value of the specified coordinate.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-07-29/GGB - Moved to CAstroImage and updated to reflect new members in CAstroImage.
  /// @version 2013-03-07/GGB - Function renamed from imageValue() to getValue()
  /// @version 2011-05-05/GGB - Function Created

  FP_t CAstroImage::getValue(AXIS_t lx, AXIS_t ly, AXIS_t ip) const
  {
    RUNTIME_ASSERT(ACL, ip >= 0, "Parameter ip must be >= 0");
    RUNTIME_ASSERT(ACL, ip < imagePlaneStorage.size(), "Parameter ip must be < number of image planes.");

    return imagePlaneStorage[ip]->getValue(lx, ly);
  }

  /// @brief Inserts the image plane into the image plane list.
  /// @param[in] newPlane: The imageplane to insert at the back of the list.
  /// @throws CRuntimeAssert
  /// @version 2018-09-15/GGB - Moved function from CAstroImagePoly and changed parameter to unique_ptr
  /// @version 2011-12-26/GGB - Function created.

  void CAstroImage::insertImagePlane(std::unique_ptr<CImagePlane> &newPlane)
  {
    RUNTIME_ASSERT(ACL, newPlane != nullptr, "A valid image plane must be passed in newPlane.");

    imagePlaneStorage.push_back(std::move(newPlane));
  }

  /// @brief Returns the size of NAXISn(NAXIS)
  /// @note According to the FITS standard, the first axis is 1. This convention is followed in the Astronomy Class Library. The
  /// normal C use of the first axis being zero (0) is not followed for naxisn.
  /// @param[in] naxis - The axis to get the extents of. (1 <= naxis <= 999)
  /// @returns The extents of the axis specified.
  /// @throws CError(ACL::0x2201) - ASTROIMAGE: Invalid image plane.
  /// @throws CError(ACL::0x2202) - ASTROIMAGE: No Image Plane available.
  /// @throws CRuntimeAssert
  /// @version 2013-07-19/GGB - Function created.

  AXIS_t CAstroImage::NAXISn(NAXIS_t naxis) const
  {
    RUNTIME_ASSERT(ACL, naxis >= 1, "Parameter naxis (1 <= naxis <= 999");
    RUNTIME_ASSERT(ACL, naxis <= 999, "Parameter naxis (1 <= naxis <= 999");

    if (!imagePlaneStorage.empty())
    {
      if (naxis == 1)
      {
        return dimX;
      }
      else if (naxis == 2)
      {
        return dimY;
      }
      else if (naxis == 3)
      {
        return imagePlaneStorage.size();
      }
      else
      {
        ACL_ERROR(0x2201);      // "ASTROIMAGE: Invalid image plane."
      }
    }
    else
      ACL_ERROR(0x2202); //ASTROIMAGE: No Image Plane available.
  }

  /// @brief Renders an image in the specified format.
  /// @param[in] rm - Render Mode
  /// @throws GCL::CCodeError(ACL)
  /// @note The outputImage is not tested for size.
  /// @version 2015-07-04/GGB - Function created.

  void CAstroImage::renderImage(ERenderMode rm)
  {
    switch (rm)
    {
      case RM_GREY8:
      {
        renderImageGrey8();
        break;
      };
      case RM_RGB32:
      {
        renderImageRGB32();
        break;
      };
      case RM_RGBD:
      {
        renderImageRGBD();
        break;
      }
      case RM_NONE:
      default:
      {
        CODE_ERROR(ACL);
      };
    };
  }

  /// @brief Renders the image as an 8 bit greyscale image.
  /// @throws None.
  /// @note As CImagePlane uses multi-threading, this must not, otherwise there can be clashes when accessing the output array.
  /// @note The function assumes that the output array has been allocated and is the correct size.
  /// @details Each image plane is called in turn to render as a grey8 output. Grey8 uses a single byte per pixel and stores a value
  /// of zero (0) for black and 255 for white.<br>
  /// This type of image should be used for a single image plane only. (IE a monochrome image) It can also be used to render a
  /// colour image as a greyscale image if required.
  /// @version 2015-07-04/GGB - Moved to CAstroImage from CAstroImageMono
  /// @version 2013-05-26/GGB - Added functionality for using different transfer functions.
  /// @version 2011-08-28/GGB - This function now serves as the kick-off routine for the multi-threading. All threads are started
  /// from this routine.
  /// @version 2011-05-29/GGB @li Store values for the blackPoint and whitePoint in the class.<br>
  /// @li If blackPoint == whitePoint, the output value is set to zero.<br>
  /// @li Store the invert value<br>
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2011-03-07/GGB - Added parameter bInvert to header.
  /// @version 2010-10-17/GGB - Function created.

  void CAstroImage::renderImageGrey8()
  {
    if (renderedImage)
    {
      delete [] renderedImage;
      renderedImage = nullptr;
    };

    renderedImage = new renderImage_t[dimX * dimY];

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &p) {p->renderImageGrey8(renderedImage);});
  }

  void CAstroImage::renderImageRGB32()
  {

  }

  void CAstroImage::renderImageRGBD()
  {

  }

  /// @brief Rotates the image around the origin.
  /// @param[in] angle: The angle to rotate (radians)
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-09-20/GGB - (Bug 85) Corrected incorrect image sizeing code.
  /// @version 2015-07-29/GGB - Corrected bug where this function was rotating about the center, not the origin.
  /// @version 2015-06-30/GGB - Removed the outputImage member. (Bug 6)
  /// @version 2011-05-30/GGB - Function created.

  void CAstroImage::rotate(FP_t angle)
  {
    RUNTIME_ASSERT(ACL, !imagePlaneStorage.empty(), "No Image planes available.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &p) { p->rotate(0, 0, angle); p->getMeanValue(); p->getStDevValue(); });

      // Update the image dimensions

    dimX = imagePlaneStorage[0]->width();
    dimY = imagePlaneStorage[0]->height();

    deleteRenderImageArray();
  }

  /// @brief Function to set the colour paramters for the image plane.
  /// @details The colour parameters are only used when rendering the image in colour. If rendering in greyscale, then the
  /// parameters are not used.
  /// @param[in] ip - The image plane to set the values for.
  /// @param[in] colour - The colour parameters for the image plane.
  /// @param[in] transparency - The transparency of the image plane.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-07-04/GGB - Function created.

  void CAstroImage::setImagePlaneColourValues(size_t ip, SColourRGB colour, FP_t transparency)
  {
    RUNTIME_ASSERT(ACL, ip < imagePlaneStorage.size(), "Invalid Image Plane referenced");

    imagePlaneStorage[ip]->setImagePlaneColourValues(colour, transparency);
  }

  /// @brief Resamples the image to a new percentage size.
  /// @param[in] percent - The percentage that the image size should be resampled to. (0 < percent < 100)
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-08-02/GGB - Changed check for no image planes to a runtime assert.
  /// @version 2013-03-07/GGB - Function created.

  void CAstroImage::resampleImage(FP_t percent)
  {
    RUNTIME_ASSERT(ACL, percent = 0, "Parameter percent == 0");
    RUNTIME_ASSERT(ACL, percent < 100, "Parameter percent > 100");

    FP_t newX, newY;

    percent /= 100;
    newX = static_cast<FP_t>(dimX) * percent;
    newY = static_cast<FP_t>(dimY) * percent;

    resampleImage(static_cast<AXIS_t>(newX), static_cast<AXIS_t>(newY));
  }

  /// @brief Function to resample the image.
  /// @param[in] w: The new width.
  /// @param[in] h: The new height.
  /// @throws CRuntimeAssert
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-08-02/GGB - Use runtime assert to check for no image planes.
  /// @version 2011-06-07/GGB - Function created.

  void CAstroImage::resampleImage(AXIS_t w, AXIS_t h)
  {
    RUNTIME_ASSERT(ACL, !imagePlaneStorage.empty(), "No Image planes available.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &p) { p->resample(w, h); p->getMeanValue(); p->getStDevValue(); });

      // Update the image dimensions

    dimX = w;
    dimY = h;

    deleteRenderImageArray();
  }

  /// @brief Sets rendering function parameters.
  /// @param[in] ip - Image Plane to set parameters for
  /// @param[in] bp - Black Point.
  /// @param[in] wp - White Point
  /// @param[in] invert - Invert image
  /// @param[in] tf - Transfer function
  /// @param[in] tfp - Transfer function parameter.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-07-04/GGB - Function created.

  void CAstroImage::setImagePlaneRenderFunction(size_t ip, FP_t bp, FP_t wp, bool invert, ETransferFunction tf, FP_t tfp)
  {
    RUNTIME_ASSERT(ACL, ip < imagePlaneStorage.size(), "Invalid Image Plane referenced");

    imagePlaneStorage[ip]->setImagePlaneRenderFunction(bp, wp, invert, tf, tfp);
  }

  /// @brief Sets the image value at the (lx, ly) coordinates.
  /// @param[in] lp - Index of the image plane.
  /// @throws CError(ACL::0x0001) - Invalid Coordinates
  /// @throws CError(ACL::0x0005) - Invalid image Plane
  /// @version 2013-03-07/GGB - Function Created

  void CAstroImage::setValue(AXIS_t lx, AXIS_t ly, AXIS_t lp, FP_t value)
  {
    RUNTIME_ASSERT(ACL, lp >= 0, "Parameter lp should be >= 0");

    if ( (lx < 0) || (ly < 0) || (lx >= dimX) || (ly >= dimY) )
    {
      ACL_ERROR(0x0001);      // invalid coordinates
    }
    else
    {
      imagePlaneStorage[lp]->setValue(lx, ly, value);
    }
  }

  /// @brief Function to transform an image.
  /// @param[in] c0: Rotation center.
  /// @param[in] ct:
  /// @param[in] th: Rotation angle (radians)
  /// @param[in] sc: Scaling parameter
  /// @param[in] pix: Pixel size.
  /// @param[in] maskPlane: The mask plane (may be nullptr)
  /// @note The image dimenions do not change when the image is transformed.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2017-09-02/GGB - Updated to support image masking.
  /// @version 2016-03-28/GGB - Removed code to resize the image as the image does not resize.
  /// @version 2011-08-08/GGB - Function created.

  void CAstroImage::transform(MCL::TPoint2D<FP_t> const &c0, MCL::TPoint2D<FP_t> const  &ct, FP_t th, FP_t sc,
    MCL::TPoint2D<FP_t> const &pix, std::unique_ptr<bool[]> &maskPlane)
  {
    RUNTIME_ASSERT(ACL, !imagePlaneStorage.empty(), "No Image planes available.");

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end
                  (), [&] (std::unique_ptr<CImagePlane> const &p)
                  { p->transform(c0, ct, th, sc, pix, maskPlane); p->getMeanValue(); p->getStDevValue(); });
  }

  /// @brief Pass through function to save the image plane to a FITS file.
  /// @param[in] file: The FITS file to read the data from.
  /// @throws CError - ACL::0x2202 -No Image Plane available.
  /// @throws CRuntimeAssert (directly and propogates???)
  /// @throws CFITSException (propogates)
  /// @version 2018-08-15/GGB - Refactor to use std::unique_ptr
  /// @version 2015-08-13/GGB - Updated to use cfitsio.
  /// @version 2015-07-28/GGB - Changed imagePlane storage method.
  /// @version 2013-03-10/GGB - Function created.

  void CAstroImage::writeToFITS(fitsfile *file)
  {
    RUNTIME_ASSERT(ACL, file != nullptr, "parameter file cannot be equal to nullptr");

    AXIS_t imagePlane = 1;

    std::for_each(imagePlaneStorage.begin(), imagePlaneStorage.end(),
                  [&] (std::unique_ptr<CImagePlane> const &p) { p->writeToFITS(file, imagePlane++);});
  }


}	// namespace ACL
