//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroImageFunctions.hpp
// SUBSYSTEM:						Templated stanalone coordinate transform functions for images
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2012-2017 Gavin Blakeman.
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
// OVERVIEW:						Standalone functions templated functions for coordinate manipulation.
//
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2013-09-30 GGB - AIRDAS 2013.09 release.
//                      2013-01-20 GGB - Release of AIRDAS 0000.00
//                      2012-01-14 GGB - Start Development of classes in file
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROIMAGEFUNCTIONS_HPP
#define ACL_ASTROIMAGEFUNCTIONS_HPP

#include "config.h"

  // Boost Library

#include "boost/optional.hpp"

  // Miscellaneous Libraries

#include <GCL>
#include <MCL>

/// @file
/// These functions are used for image coordinate manipulation when images are being manipulated.
/// This is necessary when an image is modified and the astrometry or photometry needs to be updated to track the image correctly.

namespace ACL
{

  /// @brief Calculates the black point.
  /// @param[in] mean - The image mean
  /// @param[in] stdev = The image stdev.
  /// @returns The black point.
  /// @throws None.
  /// @version 2017-08-28/GGB - Function created.

  template<typename T>
  T blackPoint(T const &mean, T const &stdev)
  {
    T returnValue;

    returnValue = mean;

    return returnValue;
  }

  /// @brief Calculates the white point.
  /// @param[in] mean - The image mean
  /// @param[in] stdev = The image stdev.
  /// @returns The white point.
  /// @throws None.
  /// @version 2017-08-28/GGB - Function created.

  template<typename T>
  T whitePoint(T const &mean, T const &stdev)
  {
    T returnValue;

    returnValue = mean + (3 * stdev);

    return returnValue;
  }

  //*******************************************************************************************************************************
  //
  // Image coordinate manipulation functions
  //
  //*******************************************************************************************************************************

  /// @brief Calculates the new coordinate value when binning pixels.
  /// @param[in] pt - Point to calculate new coordinates for
  /// @param[in] nsize - Binning factor.
  /// @returns New coordinate value.
  /// @throws CRuntimeAssert
  /// @note Templated function.
  /// @version 2015-07-30/GGB - Added runtime assertion for paramter nsize. (Bug 17)
  /// @version 2013-03-17/GGB - Function created.

  template<typename T>
  MCL::TPoint2D<T> imageBinPixels(MCL::TPoint2D<T> const &pt, int nsize)
  {
    RUNTIME_ASSERT(ACL, nsize >= 1, "Parameter nsize must be >= 1");
    RUNTIME_ASSERT(ACL, nsize <= 10, "Parameter nsize must be <= 10");

    MCL::TPoint2D<T> returnValue;

    returnValue.x() = pt.x() / nsize;
    returnValue.y() = pt.y() / nsize;

    return returnValue;
  }

  /// @brief Performs a crop on a pixel value.
  /// @param[in,out] pt - The point to crop.
  /// @param[in] o - The origen of the crop.
  /// @param[in] d - The dimensions of the cropped image.
  /// @returns true - The point falls within the new image.
  /// @returns false - The point does not fall within the new image.
  /// @throws None.
  /// @version 2015-09-20/GGB - Function created.

  template<typename T, typename U, typename V>
  boost::optional<MCL::TPoint2D<T> > imageCrop(MCL::TPoint2D<T> &pt, MCL::TPoint2D<U> const &o, MCL::TPoint2D<V> const &d)
  {
    boost::optional<MCL::TPoint2D<T> > returnValue;

    if ( (pt.x() >= o.x()) && (pt.y() >= o.y())  && (pt.x() <= (o.x() + d.x())) && (pt.y() <= (o.y() + d.y())))
    {

      returnValue = pt - o;;
    };

    return returnValue;
  }

  /// @brief Calculates the new value for the coordinate if the image is floated.
  /// @param[in] cp - Point to float.
  /// @param[in] oldDim - the old dimensions of the image (w, h)
  /// @param[in] newDim - The new dimensions of the image (w, h)
  /// @returns New value of the coordinate.
  /// @throws None.
  /// @version 2013-03-17/GGB - Function created.

  template<typename T>
  MCL::TPoint2D<T> imageFloat(MCL::TPoint2D<T> cp, boost::tuple<AXIS_t, AXIS_t> const &oldDim, boost::tuple<AXIS_t,
                              AXIS_t> const &newDim)
  {
    MCL::TPoint2D<T> returnValue;
    AXIS_t oldWidth = oldDim.get<0>();
    AXIS_t oldHeight = oldDim.get<1>();
    AXIS_t newWidth = newDim.get<0>();
    AXIS_t newHeight = newDim.get<1>();
    AXIS_t lox, hix, loy, hiy, xdiff, ydiff;

    xdiff = newWidth - oldWidth;
    ydiff = newHeight - oldHeight;

    lox = xdiff / 2;
    hix = xdiff - lox;

    loy = ydiff / 2;
    hiy = ydiff - loy;

    returnValue.x() = cp.x() + lox;
    returnValue.y() = cp.y() + loy;

    return returnValue;
  }

  /// @brief Flips the coordinate within the image.
  /// @param[in] pt - Point to flip (Mirror around X axis).
  /// @param[in] ymax - maximum Y coordinate.
  /// @returns New coordinates of pt.
  /// @throws None.
  /// @version 2012-01-21/GGB - Function created.

  template<typename T>
  MCL::TPoint2D<T> imageFlip(MCL::TPoint2D<T> const &pt, AXIS_t ymax)
  {
    MCL::TPoint2D<T> np;

    np.x() = pt.x();
    np.y() = ymax - pt.y();

    return np;
  }

  /// @brief Flops the coordinate within the image. (Mirror around the y axis)
  /// @param[in] pt - The point to flop.
  /// @param[in] xmax - The maximum x coordinate.
  /// @returns The new coordinates for pt.
  /// @throws None.
  /// @version 2012-01-21/GGB - Function created.

  template<typename T>
  MCL::TPoint2D<T> imageFlop(MCL::TPoint2D<T> const &pt, AXIS_t xmax)
  {
    MCL::TPoint2D<T> np;

    np.x() = xmax - pt.x();
    np.y() = pt.y();

    return np;
  }

  /// @brief Calculates the new image coordinates when resampling.
  /// @param[in] pt - The point to convert.
  /// @param[in] oldDim - The old image dimensions. (>= 0)
  /// @param[in] newDim - The new image dimensions.
  /// @returns The converted coordinates.
  /// @throws CRuntimeAssert.
  /// @version 2017-06-18/GGB - Added assertions to prevent divide-by-zero (Bug #22)
  ///                           Changed c-style casting to static_cast<>().
  /// @version 2013-05-15/GGB - Function created.

  template<typename T>
  MCL::TPoint2D<T> imageResample(MCL::TPoint2D<T> const &pt, boost::tuple<AXIS_t, AXIS_t> const &oldDim,
                                 boost::tuple<AXIS_t, AXIS_t> const &newDim)
  { 
    FP_t xVal, yVal;

    AXIS_t oldWidth = oldDim.get<0>();
    AXIS_t oldHeight = oldDim.get<1>();

    RUNTIME_ASSERT(ACL, oldWidth >= 0, "Parameter oldDim<0> must be >= 0");
    RUNTIME_ASSERT(ACL, oldHeight >= 0, "Parameter oldDim<1> must be >= 0");

    AXIS_t newWidth = newDim.get<0>();
    AXIS_t newHeight = newDim.get<1>();

    FP_t xRatio = static_cast<FP_t>(newWidth) / static_cast<FP_t>(oldWidth);
    FP_t yRatio = static_cast<FP_t>(newHeight) / static_cast<FP_t>(oldHeight);

    xVal = pt.x() * xRatio;
    yVal = pt.y() * yRatio;

    MCL::TPoint2D<T> returnValue(static_cast<T>(xVal), static_cast<T>(yVal));

    return returnValue;
  }

  /// @brief Rotates the coordinates around the origen by the angle theta.
  /// @param[in] c0 - Origin
  /// @param[in] cP - Coordinate to rotate.
  /// @param[in] theta - Angle to rotate (radians)
  /// @returns New Coordinates of cP.
  /// @todo Change order of c0 and cP in argument list.
  /// @version 2012-01-14/GGB - Function created.

  template<typename T, typename U>
  MCL::TPoint2D<U> imageRotate(MCL::TPoint2D<T> const &c0, MCL::TPoint2D<U> const &cP, FP_t theta)
  {
    MCL::TPoint2D<U> newPoint;

    newPoint.x() = (U) (c0.x() + (cP.x() - c0.x()) * cos(theta) + (cP.y() - c0.y()) * sin(theta));
    newPoint.y() = (U) (c0.y() - (cP.x() - c0.x()) * sin(theta) + (cP.y() - c0.y()) * cos(theta));

    return newPoint;
  }

  /// @brief Performs the forward transform of a point. (Original image coords -> transformed image coords)
  /// @param[in] pt - point to be transformed. (coordinates)
  /// @param[in] c0 - center of rotation
  /// @param[in] ct - translation amount
  /// @param[in] angle - angle of rotation (radians)
  /// @param[in] scale - amount the image should be scaled. (scale >= 0)
  /// @param[in] pixelSize - the size of the pixels. (pixelSize >= 0)
  /// @returns The forward transformed point coordinates.
  /// @throws CRuntimeAssert()
  /// @note 1. The translation value ct should be less than the size of the image. This cannnot however checked in the code and
  ///          could result in very large values being returned.
  /// @version 2017-06-18/GGB - Added parameter verification code. (Bug 24)
  ///                           Changed c-style casting to static_cast<>().
  /// @version 2013-08-03/GGB - Function created.

  template<typename T, typename U, typename V>
  MCL::TPoint2D<T> imageTransformForward(MCL::TPoint2D<T> const &pt, MCL::TPoint2D<U> const &c0, MCL::TPoint2D<U> const &ct,
                                         FP_t angle, FP_t scale, MCL::TPoint2D<V> const &pixelSize)
  {
    RUNTIME_ASSERT(ACL, scale >= 0, "Parameter scale must be >= 0");
    RUNTIME_ASSERT(ACL, pixelSize.x() >= 0, "Parameter pixelSize.x must be >= 0");
    RUNTIME_ASSERT(ACL, pixelSize.y() >= 0, "Parameter pixelSize.y must be >= 0");

    FP_t sina = sin(angle);
    FP_t cosa = cos(angle);
    FP_t x, y, xmm, ymm, xtemp, ytemp;

    xtemp = (pt.x() - c0.x()) * pixelSize.x();
    ytemp = (pt.y() - c0.y()) * pixelSize.y();
    xtemp *= scale;
    ytemp *= scale;

    xmm = xtemp * cosa + ytemp * sina;
    ymm = ytemp * cosa - xtemp * sina;
    x = xmm / pixelSize.x();
    y = ymm / pixelSize.y();
    x = x + ct.x() + c0.x();
    y = y + ct.y() + c0.y();

    return MCL::TPoint2D<T>(static_cast<T>(x), static_cast<T>(y));
  }

  /// @brief Performs the reverse transform of a point. (Transformed image coords -> original image coords)
  /// @param[in] pt - point to be transformed.
  /// @param[in] c0 - center of rotation
  /// @param[in] ct - translation amount
  /// @param[in] angle - angle of rotation (radians)
  /// @param[in] scale - amount the image should be scaled. (scale >= 0)
  /// @param[in] pixelSize - the size of the pixels. (pixelSize >= 0)
  /// @returns The forward transformed point.
  /// @throws CRuntimeAssert()
  /// @note 1. The translation value ct should be less than the size of the image. This cannnot however checked in the code and
  ///          could result in very large values being returned.
  /// @version 2017-06-18/GGB - Added parameter verification code. (Bug 25)
  ///                           Changed c-style casts to static_cast<>().
  /// @version 2013-08-03/GGB - Function created.

  template<typename T, typename U, typename V>
  MCL::TPoint2D<T> imageTransformReverse(MCL::TPoint2D<T> const &pt, MCL::TPoint2D<U> const &c0, MCL::TPoint2D<U> const &ct,
                                         FP_t angle, FP_t scale, MCL::TPoint2D<V> const &pixelSize)
  {
    RUNTIME_ASSERT(ACL, scale >= 0, "Parameter scale must be >= 0");
    RUNTIME_ASSERT(ACL, pixelSize.x() >= 0, "Parameter pixelSize.x must be >= 0");
    RUNTIME_ASSERT(ACL, pixelSize.y() >= 0, "Parameter pixelSize.y must be >= 0");

    FP_t sina = std::sin(angle);
    FP_t cosa = std::cos(angle);
    FP_t x, y, xmm, ymm, xtemp, ytemp;

    x = pt.x() - c0.x();
    y = pt.y() - c0.y();
    x = x - ct.x();
    y = y - ct.y();
    xmm = x * pixelSize.x();
    ymm = y * pixelSize.y();
    xtemp = xmm * cosa - ymm * sina;
    xtemp = xtemp / scale;
    ytemp = xmm * sina + ymm * cosa;
    ytemp = ytemp / scale;
    x = c0.x() + xtemp / scale;
    y = c0.y() + ytemp / scale;

    return MCL::TPoint2D<T>(static_cast<T>(x), static_cast<T>(y));
  }

} // namespace ACL

#endif  // ACL_ASTROIMAGEFUNCTIONS_HPP
