//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroImageMono
// SUBSYSTEM:						Monochrome astronomical image class.
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, CFitsIO.
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
//											These classes are tightly linked with the CFitsIO library. This linkage
//                      is required as these classes do ultimately service astronomical images and the standard format for
//                      astonomical images is FITS.
//
//											It is not intended that these classes support other formats at this time (2010), but support for other
//                      image formats may be added. (DNG, JPG, TIFF)
//
//											Support for statistical and other mathematics is derived from the gsl library.
//
//											The library is designed to be platform independant. IE, only standard C++ functionallity is used.
//
//											While this library has been designed to be used from Qt, it makes no reference to the Qt library.
//
//
// CLASSES INCLUDED:		CAstroImageMono			- A class that provides scientific functionality on an astronomical image.
//
// CLASS HIERARCHY:     CAstroImage
//												- CAstroImageMono
//												- CAstroImagePoly
//
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-08 GGB - Removed function profile(...)
//                      2013-05-25 GGB - Moved CAstroImageMono to it's own file
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../include/AstroImageMono.h"

  // Standard C++ library header files

#include <cstdint>
#include <limits>
#include <valarray>

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // CAstroImageMono
  //
  // The class does not read the data from a file when the instance is constructed.
  // The calling/owning application must make a call to readFile() to read the data from the file
  // into the calling application.
  //
  //*******************************************************************************************************************************

  /// @brief Class constructor
  /// @param[in] newX: The new X dimension.
  /// @param[in] newY: The new Y dimension.
  /// @throws std::bad_alloc
  /// @details Creates the image plane with valid data in the image plane.
  /// @version 2015-06-30/GGB - Removed the outputImage member. (Bug 6)
  /// @version 2013-03-13/GGB - Changed parameter list.
  /// @version 2011-05-04/GGB - Function created.

  CAstroImageMono::CAstroImageMono(AXIS_t newX, AXIS_t newY) : CAstroImage(newX, newY)
  {
      // Create the image plane

    //PImagePlane imagePlane(new CImagePlane(dimX, dimY));  Commented out 2015-08-02/GGB
    //imagePlaneStorage.push_back(imagePlane);
  }

  /// @brief Copy constructor for the class.
  /// @param[in] toCopy: The instance to make a copy of.
  /// @throws std::bad_alloc
  /// @version 2018-09-15/GGB - Updated to use std::unique_ptr
  /// @version 2015-06-30/GGB - Removed the outputImage member. (Bug 6)
  /// @version 2011-05-13/GGB - Function created.

  CAstroImageMono::CAstroImageMono(CAstroImageMono const &toCopy) : CAstroImage(toCopy.width(), toCopy.height())
  {
    imagePlaneStorage.emplace_back(std::make_unique<CImagePlane>(*(toCopy.imagePlaneStorage[0])));
  }

  /// @brief Simply add the two image planes together.
  /// @returns Reference to this.
  /// @throws 0x0004 - Inconsistent image sizes
  /// @throws 0x2300 - ASTROIMAGEMONO: Unable to cast instance to correct class.
  //
  // 2015-07-28/GGB - Updated to reflect new method of storing image planes.
  // 2011-05-12/GGB - Function created.

  CAstroImage &CAstroImageMono::operator+=(CAstroImage const &rhs)
  {
    if ( (dimX == rhs.width()) && (dimY == rhs.height()) )
    {
      try
      {
        (*imagePlaneStorage[0]) += *(dynamic_cast<CAstroImageMono const &>(rhs).imagePlaneStorage[0]);
      }
      catch(std::bad_cast)
      {
        ACL_ERROR(0x2300);
      }
    }
    else
    {
      ACL_ERROR(0x0004);    // Inconsistent image sizes.
    }

    return (*this);
  }

  /// @brief Simply subtract the two image planes together.
  /// @returns Reference to this.
  /// @throws 0x0004 - Inconsistent image sizes
  //
  // 2015-07-28/GGB - Updated to reflect new method of storing image planes.
  // 2011-05-12/GGB - Function created.

  CAstroImage &CAstroImageMono::operator-=(CAstroImage const &rhs)
  {
    if ( (dimX == rhs.width()) && (dimY == rhs.height()) )
    {
      try
      {
        (*imagePlaneStorage[0]) -= *(dynamic_cast<CAstroImageMono const &>(rhs).imagePlaneStorage[0]);
      }
      catch(std::bad_cast)
      {
        ACL_ERROR(0x2300);
      }
    }
    else
    {
      ACL_ERROR(0x0004);    // Inconsistent image sizes.
    };

    return (*this);
  }

  /// @brief Applies the passed dark frame to the image.
  /// @param[in] df - The dark frame to apply.
  /// @throws 0x2202 - ASTROIMAGE: No Image Plane available.
  /// @throws 0x2207 - ASTROIMAGE: applyDark requires dark image to be of same type (mono or poly)
  //
  // 2015-07-28/GGB - Updated function to reflect new method of storing the image plane.
  // 2012-01-28/GGB - Function created.

  void CAstroImageMono::applyDark(CAstroImage const &df)
  {
    try
    {
      CAstroImageMono const &darkFrame = dynamic_cast<CAstroImageMono const &>(df);

      if ( (imagePlaneStorage.empty()) || (darkFrame.imagePlaneStorage.empty()) )
      {
        ACL_ERROR(0x2202);    // ASTROIMAGE: No Image Plane available.
      }
      else
      {
        (*imagePlaneStorage[0]) -= (*darkFrame.imagePlaneStorage[0]);
      };
    }
    catch (std::bad_cast)
    {
      ACL_ERROR(0x2207);    // ASTROIMAGE: applyDark requires dark image to be of same type (mono or poly)
    };
  }

  /*void CAstroImageMono::applyFlat(CFlatFrame const &ff)
  {
  };*/

  /// @brief Gets the value of BITPIX.
  /// @returns The value of BITPIX for the image.
  /// @throws 0x2202 - ASTROIMAGE: No Image Plane available.
  //
  // 2012-11-27/GGB - Function created.

  int CAstroImageMono::BITPIX() const
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202); //ASTROIMAGE: No Image Plane available.
    }
    else
    {
      return imagePlaneStorage[0]->BITPIX();
    };
  }

  /// @brief Sets the value of BITPIX.
  /// @param[in] bp - The new value of BITPIX to use.
  /// @throws 0x2202 - ASTROIMAGE: No Image Plane available.
  //
  // 2012-11-27/GGB - Function created.

  void CAstroImageMono::BITPIX(int bp)
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);  // ASTROIMAGE: No Image Plane available.
    }
    else
    {
      imagePlaneStorage[0]->BITPIX(bp);
    };
  }

  /// Sets the value of BSCALE
  /// EXCEPTIONS: 0x2202 - ASTROIMAGE: No Image Plane available.
  //
  // 2012-11-30/GGB - Function created.

  void CAstroImageMono::BSCALE(FP_t bs)
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);  // ASTROIMAGE: No Image Plane available.
    }
    else
    {
      //imagePlaneStorage[0]->BSCALE(bs);
    }
  }

  /// @brief Gets the value of BZERO
  /// @returns The value of BZERO for the specified image plane.
  /// @throws CError ACL::0x2202 - ASTROIMAGE: No Image Plane available.
  //
  // 2012-11-30/GGB - Function created.

  FP_t CAstroImageMono::BZERO() const
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202); //ASTROIMAGE: No Image Plane available.
    }
    else
    {
      //return imagePlaneStorage[0]->BZERO();
    }
  }

  /// Sets the value of BZERO
  /// EXCEPTIONS: 0x2202 - ASTROIMAGE: No Image Plane available.
  //
  // 2012-11-30/GGB - Function created.

  void CAstroImageMono::BZERO(FP_t bz)
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);  // ASTROIMAGE: No Image Plane available.
    }
    else
    {
      //imagePlaneStorage[0]->BZERO(bz);
    };
  }

  /// @brief Function to determine the centroid of an object closest to the centroid point passed as the parameter.
  /// @param[in] c0 - Center point to search from.
  /// @param[in] radius - The maximum search radius (radius >= 1)
  /// @param[in] sensitivity - The search sensitivity
  /// @returns Centroid if found.
  /// @throws CError(ACL::0x2202) - Image plane not available
  /// @throws CRuntimeAssert(ACL)
  /// @version 2013-08-02/GGB - Changed return value to boost::optional<MCL::TPoint2D<FP_t>>
  /// @version 2012-11-10/GGB - Function created.

  std::optional<MCL::TPoint2D<FP_t> > CAstroImageMono::centroid(MCL::TPoint2D<AXIS_t> const &guess,
                                                                  AXIS_t radius, int sensitivity) const
  {
    RUNTIME_ASSERT(ACL, radius >= 1, "Parameter radius must be >= 1");

    std::optional<MCL::TPoint2D<FP_t> > guess2;

    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);  // Image plane not available.
    }
    else
    {
      try
      {
        guess2 = imagePlaneStorage[0]->brightWalk(guess, radius, sensitivity);
        if (guess2)
        {
          return imagePlaneStorage[0]->starCentroid((MCL::TPoint2D<AXIS_t>) *guess2);
        }
        else
        {
          return guess2;
        };
      }
      catch(...)
      {
        return guess2;
      };
    };
  }

  /// @brief Creates a copy of this.
  /// @returns A new copy of this.
  /// @throws std::bad_alloc
  /// @version 2018-08-14/GGB - Changed return value to std::unique_ptr
  /// @version 2011-05-13/GGB - Function created.

  std::unique_ptr<CAstroImage> CAstroImageMono::createCopy() const
  {
    std::unique_ptr returnValue = std::make_unique<CAstroImageMono>(*this);

    return returnValue;
  }

  /// @brief Identifies all the point sources (objects) in the image. These would typically be stars, planets or asteroids
  /// @throws CError ACL::0x2202 - No image plane available.
  /// @version 2014-02-01/GGB - Changed to function findStars. Use class CFindStars.
  /// @version 2013-10-29/GGB - Function created.

  void CAstroImageMono::findStars(TImageSourceContainer &imageObjectList, const SFindSources &sourceDefaults) const
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);
    }
    else
    {
      imagePlaneStorage[0]->findStars(imageObjectList, sourceDefaults);
    }
  }

  /// @brief Determine the FWHM of a star.
  /// @throws CError(ACL::0x2202) - ASTROIMAGE: No Image Plane available.
  /// @version 2013-04-14/GGB - Function created.

  std::optional<FP_t> CAstroImageMono::FWHM(MCL::TPoint2D<FP_t> const &star) const
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);    // ASTROIMAGE: No Image Plane available.
    }
    else
    {
      return imagePlaneStorage[0]->FWHM(star, 10);
    }
  }

  /// @brief Determines the object profile
  /// @param[in] centroid - The centroid of the object
  /// @param[in] radius - The radius to centroid to.
  /// @param[out] data - The output data.
  /// @throws GCL::CError(ACL::0x2202) - ASTROIMAGE: No Image Plane available.
  /// @version 2013-05-18/GGB - Function created.

  void CAstroImageMono::objectProfile(MCL::TPoint2D<FP_t> centroid, AXIS_t radius, std::vector<std::tuple<FP_t, FP_t> > &data) const
  {
    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);
    }
    else
    {
      imagePlaneStorage[0]->objectProfile(centroid, radius, data);
    };
  }

  /// @brief Looks for a centroid around the x,y coordinates passed.
  /// @param[in] center - The center point
  /// @param[in] radius - The radius to search within.
  /// @returns Optional value containing the centroid.
  /// @throws 0x2202 - ASTROIMAGE: No Image Plane available.
  /// @details The halfrect parameter gives the size of the box to check.<br>
  /// Finding a centroid is a multi-step process.
  /// @li	1. Find the mean
  /// @li Eliminate all pixels below the mean
  /// @li Eliminate all pixels that are not adjacent to 2 other pixels
  /// @li Check for one cluster of pixels
  /// @li Calculate the centroid.
  /// @version 2013-06-15/GGB - Changed to use valarry as the array type.
  /// @version 2010-11-05/GGB - Function created.

  std::optional<MCL::TPoint2D<FP_t> > CAstroImageMono::findCentroid(const MCL::TPoint2D<AXIS_t> &center, AXIS_t halfRect) const
  {
    AXIS_t xIndex, yIndex;
    INDEX_t lIndex = 0;
    INDEX_t numElements = MCL::pow2(halfRect) * 4;
    AXIS_t sx = halfRect * 2;
    int nAdjacent;
    FP_t xbar = 0, ybar = 0;
    FP_t skyMean;
    FP_t xden = 0, yden = 0;
    std::optional<MCL::TPoint2D<FP_t> > returnValue;
    MCL::TPoint2D<FP_t> centroid;

    if (imagePlaneStorage.empty())
    {
      ACL_ERROR(0x2202);      // ASTROIMAGE: No Image Plane available.
    };

      // Cannot look for centroids closer than rect/2 pixels from the edge of the image.

    if ( (center.x() >= halfRect) && (center.y() >= halfRect) &&
         (center.x() < dimX - halfRect) && (center.y() < dimY-halfRect))
    {
      FP_t imageCopy1[numElements];		// Allocate memory for the sub-array
      FP_t imageCopy2[numElements];

        // Copy to the sub array.

      for(xIndex = (center.x() - halfRect); xIndex < (center.x() + halfRect); xIndex++)
      {
        for(yIndex = (center.y() - halfRect); yIndex < (center.y() + halfRect); yIndex++)
        {
          imageCopy1[lIndex] = imagePlaneStorage[0]->getValue(xIndex, yIndex);
          imageCopy2[lIndex++] = imagePlaneStorage[0]->getValue(xIndex, yIndex);
        };
      };

        // Calculate the mean (Step 1)

      FP_t dMean = *MCL::mean(imageCopy1, numElements);

        // Zero all the values below the mean value. (Step 2)

      for (lIndex = 0; lIndex < numElements; lIndex++)
      {
        if (imageCopy1[lIndex] <= dMean)
        {
          imageCopy1[lIndex] = 0;
        }
        else
        {
            // Check that the pixel is adjacent to 2 other pixels. (Step 3)

          nAdjacent = 0;

            // Only 8 possible pixels around... Check all 8

          yIndex = lIndex / sx;
          xIndex = lIndex % sx;

          yIndex--;
          xIndex--;			// Move to top left corner

          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };
          xIndex++;
          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };
          xIndex++;
          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };
          xIndex -= 2;
          yIndex++;
          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };
          xIndex += 2;
          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };
          xIndex -= 2;
          yIndex++;
          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };
          xIndex++;
          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };
          xIndex++;
          if ( (yIndex >= 0) && (xIndex >= 0) && (yIndex < sx) && (xIndex < sx) )
          {
            if (imageCopy1[yIndex * sx + xIndex] > dMean)
              nAdjacent++;
          };

          if (nAdjacent <= 2)
          {
            imageCopy1[lIndex] = 0;
          }
          else
          {
            imageCopy2[lIndex] = 0;
          };
        };
      };

        // Array has been iterated for step 2 & 3

        // Step 4 - not sure how to do this yet

        // Step 5 - Calculate the centroid.

      skyMean = *MCL::mean(imageCopy2, numElements);

      for(xIndex = 0; xIndex < sx; xIndex++)
        for(yIndex = 0; yIndex < sx; yIndex++)
        {
          lIndex = yIndex * sx + xIndex;
          if (imageCopy1[lIndex] != 0)
          {
            xbar += xIndex * (imageCopy1[lIndex] - skyMean);
            ybar += yIndex * (imageCopy1[lIndex] - skyMean);
            xden += imageCopy1[lIndex] - skyMean;
            yden += imageCopy1[lIndex] - skyMean;
          };
        };

      if (xden == 0)
      {
        centroid.x() = 0;
      }
      else
      {
        centroid.x() = (FP_t) (xbar / xden);
      };

      if (yden == 0)
        centroid.y() = 0;
      else
        centroid.y() = (FP_t) (ybar / yden);

      centroid.x() += center.x() - halfRect;
      centroid.y() += center.y() - halfRect;

      returnValue = centroid;
    };
    return returnValue;
  }

  /// @brief Returns the PEDESTAL value.
  /// @throws GCL::CError(ACL::0x2202) - ASTROIMAGE: No Image Plane available.
  /// @version 2012-12-01/GGB - Function created.

  int CAstroImageMono::PEDESTAL() const
  {
//    if (imagePlaneStorage.empty())
//      ACL_ERROR(0x2202); //ASTROIMAGE: No Image Plane available.
//    else
      //return imagePlaneStorage[0]->PEDESTAL();
  }

  /// @brief Performs photometry on the image plane
  /// @param[in] po - THe photometry object.
  /// @throws 0x0003 - Overlaps edge
  /// @version 2013-05-08/GGB - Rewrite to use PPhotometryObservation as the parameter.
  /// @version 2012-11-10/GGB - Changed function arguments to align with changes to argument lists globally.
  /// @version 2010-11-07/GGB - Function created.

  void CAstroImageMono::photometry(SPPhotometryObservation po) const
  {
    AXIS_t xIndex, yIndex;
    AXIS_t xStart = static_cast<AXIS_t>(po->CCDCoordinates().x() - po->photometryAperture()->halfWidth() - 1);
    AXIS_t xFinish = static_cast<AXIS_t>(po->CCDCoordinates().x() + po->photometryAperture()->halfWidth() + 1);
    AXIS_t yStart = static_cast<AXIS_t>(po->CCDCoordinates().y() - po->photometryAperture()->halfHeight() - 1);
    AXIS_t yFinish = (AXIS_t) (po->CCDCoordinates().y() + po->photometryAperture()->halfHeight() + 1);
    MCL::TPoint2D<AXIS_t> center( (MCL::TPoint2D<AXIS_t>) po->CCDCoordinates() );

    po->sourceADU() = 0;
    po->sourceArea() = 0;
    po->skyADU() = 0;
    po->skyArea() = 0;

    if ( (xStart < 0) || (yStart < 0) || (xFinish >= dimX) || (yFinish >= dimY) )
    {
      ACL_ERROR(0x0003);    // Overlaps edge
    }
    else
    {
      for (xIndex = xStart; xIndex <= xFinish; xIndex++)
      {
        for (yIndex = yStart; yIndex <= yFinish; yIndex++)
        {
          if ( po->photometryAperture()->isSource(center, MCL::TPoint2D<INDEX_t>(xIndex, yIndex)) ) // Is this part of the aperture?
          {
            po->sourceADU() += imagePlaneStorage[0]->getValue(xIndex, yIndex);
            ++(po->sourceArea());
          }
          else if ( po->photometryAperture()->isSky(center, MCL::TPoint2D<INDEX_t>(xIndex, yIndex)) ) // Is this part of the annulus?
          {
            po->skyADU() += imagePlaneStorage[0]->getValue(xIndex, yIndex);
            ++(po->skyArea());
          };
        };
      };
    };
  }

  /// @brief Reads data from a FITS file into the mono image.
  /// @param[in] file - The FITS file to read the data from.
  /// @throws std::bad_alloc
  /// @version 2015-08-13/GGB - Updated to use cfitsio.
  /// @version 2013-03-13/GGB - Function created.

  void CAstroImageMono::readFromFITS(fitsfile *file)
  {
    RUNTIME_ASSERT(ACL, file != nullptr, "parameter file cannot be equal to nullptr");

    imagePlaneStorage.emplace_back(new CImagePlane(file, 1));
  }

}  // namespace ACL
