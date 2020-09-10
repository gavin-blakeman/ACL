//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								ImagePlane
// SUBSYSTEM:						Astronomical Image Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, cfitsio, boost.
// NAMESPACE:						ACL
// AUTHORS:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2020 Gavin Blakeman.
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
//											It is not intended that these classes support other formats at this time (2010), but support for other
//                      image formats may be added. (DNG, JPG, TIFF)
//
//											The library is designed to be platform independant. IE, only standard C++ functionallity is used.
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
//                      2013-04-14 GGB - Changed filename from CImagePlane.cpp to ImagePlane.cpp
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../include/ImagePlane.h"

  // Standard C++ Library header files

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

  // Miscellaneous header files

#include "boost/locale.hpp"
#include "boost/thread/thread.hpp"

  // ACL library header files

#include "include/constants.h"
#include "include/error.h"
#include "include/findstar.h"
#include "include/FITSException.h"
#include "include/FITSStrings.h"
#include "include/FWHM.h"

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // General function - only used by this file, but not part of the class.
  //
  //*******************************************************************************************************************************

  /// @brief Thread function to perform the copy of the data.
  /// @param[in] srcData<0>: sourceImage
  /// @param[in] srcData<1>: startX
  /// @param[in] srcData<2>: startY
  /// @param[in] srcData<3>: dimX
  /// @param[in] destData<0>: destination image
  /// @param[in] destData<1>: dimX
  /// @param[in] destData<2>: rowBegin
  /// @param[in] destData<3>: rowEnd
  /// @throws None.
  /// @version 2013-04-21/GGB - Function created.

  template <typename T>
  void copyImagePlaneThread(std::tuple<T const *, AXIS_t, AXIS_t, AXIS_t> & srcData, std::tuple<T *, AXIS_t, AXIS_t, AXIS_t> &destData)
  {
    AXIS_t indexX, indexY;
    AXIS_t destX = std::get<1>(destData);
    T const *srcImage = std::get<0>(srcData);
    T *destImage = std::get<0>(destData);

    for (indexY = std::get<2>(destData); indexY < std::get<3>(destData); indexY ++)
    {
      for (indexX = 0; indexX < std::get<1>(destData); indexX++)
      {
        destImage[arrayIndex(indexX, indexY, destX)] =
          srcImage[arrayIndex(indexX + std::get<1>(srcData), indexY + std::get<2>(srcData), std::get<3>(srcData))];
      };
    };
  }

  /// @brief        Copies part of an image plane from src to dest.
  /// @details      This is a templated function that can copy any type of image plane. This function starts the thread functions
  ///               that perform the actual copy. The work is divided evenly over the threads.
  /// @param[in]    srcData<0>:  imagePlane
  /// @param[in]    srcData<1>:  startX
  /// @param[in]    srcData<2>:  startY
  /// @param[in]    srcData<3>:  dimX
  /// @param[in,out] destData<0>: imagePlane
  /// @param[in]    destData<1>: dimX
  /// @param[in]    destData<2>: dimY
  /// @throws       None.
  /// @note         The types of the source and destination image planes must be the same.
  /// @version      2013-04-21/GGB - Function created.

  template<typename T>
  void copyImagePlane(std::tuple<T const *, AXIS_t, AXIS_t, AXIS_t> srcData, std::tuple<T *, AXIS_t, AXIS_t> destData)
  {
    size_t numberOfThreads;
    size_t threadNumber;
    AXIS_t rowStep;
    AXIS_t rowBegin = 0, rowEnd = 0;
    boost::thread_group threadGroup;
    boost::thread *thread;
    AXIS_t rows = std::get<2>(destData);

      // Ensure that we are using a reasonable number of threads.

    if ( (rows >= 0) && (static_cast<size_t>(rows) < maxThreads) )
    {
      numberOfThreads = static_cast<size_t>(rows);
    }
    else
    {
      numberOfThreads = maxThreads;
    };

    rowStep = rows / numberOfThreads;

      // Spawn the threads.

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      rowBegin = rowEnd;
      if (threadNumber == (numberOfThreads -1) )
        rowEnd = rows;
      else
        rowEnd += rowStep;

      thread = new boost::thread(&copyImagePlaneThread<T>, srcData, std::tuple<T *, AXIS_t, AXIS_t, AXIS_t>(std::get<0>(destData),
                                                                                                              std::get<1>(destData),
                                                                                                              rowBegin, rowEnd));
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.
  }

  //*******************************************************************************************************************************
  //
  // CImagePlane
  //
  //*******************************************************************************************************************************

  /// @brief        Default constructor for the class.
  /// @throws       None.
  /// @version      2015-09-02/GGB
  /// @li             Use C-style arrays as storage type.
  /// @li             Use cfitsio rather than CCfits for accessing FITS files.
  /// @li             Add additional data types as supported by cfitsio V3.3
  /// @version      2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version      2012-11-27/GGB - Added support for native image planes and bitpix_
  /// @version      2010-10-16/GGB - Function created.

  CImagePlane::CImagePlane() : dimX(0), dimY(0), fMin(0), fMax(0), fMean(0), fStDev(0), bMinMax(false), bMean(false),
    bitpix_(0), bscale_(1), bzero_(0), pedestal_(0), imagePlane8(nullptr), imagePlaneS8(nullptr), imagePlaneU16(nullptr),
    imagePlane16(nullptr), imagePlaneU32(nullptr), imagePlane32(nullptr), imagePlane64(nullptr), imagePlaneF(nullptr),
    imagePlaneD(nullptr)
  {
  }

  /// @brief Copy constructor for the class.
  /// @details Uses the operator= function to perform the copy.
  /// @param[in] toCopy: The imagePlane to copy to this.
  /// @throws None.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2011-05-01/GGB - Function created.

  CImagePlane::CImagePlane(const CImagePlane &toCopy) : dimX(toCopy.dimX), dimY(toCopy.dimY), imagePlane8(nullptr),
    imagePlaneS8(nullptr), imagePlaneU16(nullptr), imagePlane16(nullptr), imagePlaneU32(nullptr), imagePlane32(nullptr),
    imagePlane64(nullptr), imagePlaneF(nullptr), imagePlaneD(nullptr), bMinMax(false), bMean(false),  bitpix_(toCopy.bitpix_),
    bscale_(toCopy.bscale_), bzero_(toCopy.bzero_), pedestal_(toCopy.pedestal_)
  {
    (*this) = toCopy;
  }

  /// @brief Creates a sub-image from an existing image.
  /// @param[in] oldPlane: The image plane to copy the data from.
  /// @param[in] xStart: The starting x-value fo the sub image.
  /// @param[in] xEnd: The ending x-value of the sub image.
  /// @param[in] yStart: The starting y-value for the sub image.
  /// @param[in] yEnd: The ending y value for the sub image.
  /// @throws CError(ACL::0x1002) - CCFITS: Invalid BITPIX value.
  /// @throws std::bad_alloc
  /// @throws CRuntimeAssert.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  ///                         @li Use C-style arrays as storage type.
  ///                         @li Use cfitsio rather than CCfits for accessing FITS files.
  ///                         @li Add additional data types as supported by cfitsio V3.3
  /// @version 2015-07-31/GGB - Addded runtime assertations for parameters.
  /// @version 2013-04-20/GGB - Function created.

  CImagePlane::CImagePlane(CImagePlane const *oldPlane, AXIS_t xStart, AXIS_t xEnd, AXIS_t yStart, AXIS_t yEnd) : dimX(0), dimY(0),
    imagePlane8(nullptr), imagePlaneS8(nullptr), imagePlaneU16(nullptr), imagePlane16(nullptr), imagePlaneU32(nullptr),
    imagePlane32(nullptr), imagePlane64(nullptr), imagePlaneF(nullptr), imagePlaneD(nullptr), bMinMax(false), bMean(false),
    bitpix_(0), bscale_(1), bzero_(0), pedestal_(0)
  {
      // Check paramter values are valid

    RUNTIME_ASSERT(oldPlane, "Parameter oldPlane == nullptr");
    RUNTIME_ASSERT(xStart >= 0, "Parameter xStart < 0");
    RUNTIME_ASSERT(xEnd >= 0, "Parameter xEnd < 0");
    RUNTIME_ASSERT(xStart < xEnd, "Paramter xStart >= xEnd");
    RUNTIME_ASSERT(yStart >= 0, "Parameter yStart < 0");
    RUNTIME_ASSERT(yEnd >= 0, "Parameter yEnd < 0");
    RUNTIME_ASSERT(yStart < yEnd, "Paramter yStart >= yEnd");

    bitpix_ = oldPlane->BITPIX();
    dimX = xEnd - xStart;
    dimY = yEnd - yStart;

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        imagePlane8 = std::make_unique<std::uint8_t[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlane8, imagePlane8);
        break;
      };
      case SBYTE_IMG:
      {
        imagePlaneS8 = std::make_unique<std::int8_t[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlaneS8, imagePlaneS8);
        break;
      };
      case USHORT_IMG:
      {
        imagePlaneU16 = std::make_unique<std::uint16_t[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlaneU16, imagePlaneU16);
        break;
      };
      case SHORT_IMG:
      {
        imagePlane16 = std::make_unique<std::int16_t[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlane16, imagePlane16);
        break;
      };
      case ULONG_IMG:
      {
        imagePlaneU32 = std::make_unique<std::uint32_t[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlaneU32, imagePlaneU32);
        break;
      };
      case LONG_IMG:
      {
        imagePlane32 = std::make_unique<std::int32_t[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlane32, imagePlane32);
        break;
      };
      case LONGLONG_IMG:
      {
        imagePlane64 = std::make_unique<std::int64_t[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlane64, imagePlane64);
        break;
      };
      case FLOAT_IMG:
      {
        imagePlaneF = std::make_unique<float[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlaneF, imagePlaneF);
        break;
      };
      case DOUBLE_IMG:
      {
        imagePlaneD = std::make_unique<double[]>(dimX * dimY);
        //copyImagePlane(oldPlane->imagePlaneD, imagePlaneD);
        break;
      };
      default:
      {
        ERROR(ACL, 0x1002);  // CCFITS: Invalid BITPIX value.
        break;
      };
    };
  }

  /// @brief Constructor for CImagePlane class.
  /// @param[in] newx: New x-dimension
  /// @param[in] newy: New y-dimension.
  /// @throws None.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2010-10-16/GGB - Function created.

  CImagePlane::CImagePlane(AXIS_t newx, AXIS_t newy) : dimX(newx), dimY(newy), fMin(0), fMax(0), fMean(0), fStDev(0),
    bMinMax(false), bMean(false), bitpix_(0), bscale_(1), bzero_(0), pedestal_(0), imagePlane8(nullptr), imagePlaneS8(nullptr),
    imagePlaneU16(nullptr), imagePlane16(nullptr), imagePlaneU32(nullptr), imagePlane32(nullptr), imagePlane64(nullptr),
    imagePlaneF(nullptr), imagePlaneD(nullptr)
  {
    fMin = fMax = fMean = 0;
    bMinMax = bMean = false;
  }

  /// @brief Constructs an image plane from an FITS HDU.
  /// @param[in] hdu - Pointer to the HDU to load from.
  /// @param[in] axis - The axis number to load. (One based)
  /// @throws CError(ACL::0x1002) - CCFITS: Invalid BITPIX value.
  /// @throws CRuntimeAssert
  /// @throws CCodeError
  /// @throws std::bad_alloc
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-08-26/GGB - Changed function to allow to be used with polychromatic images
  /// @version 2013-03-13/GGB - Function created.

  CImagePlane::CImagePlane(fitsfile *file, AXIS_t axis) : dimX(0), dimY(0), fMin(0), fMax(0), fMean(0), fStDev(0), bMinMax(false),
    bMean(false), bitpix_(0), bscale_(1), bzero_(0), pedestal_(0), imagePlane8(nullptr), imagePlaneS8(nullptr),
    imagePlaneU16(nullptr), imagePlane16(nullptr), imagePlaneU32(nullptr), imagePlane32(nullptr), imagePlane64(nullptr),
    imagePlaneF(nullptr), imagePlaneD(nullptr)
  {
    readFromFITS(file, axis);
  }

  /// @brief Destructor for CImagePlane. Ensures all dynamically allocated memory is cleared.
  /// @throws None.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Changed storage type of imagePlanes to std::vector<>
  /// @version 2012-11-27/GGB - Added support for native image planes.
  /// @version 2010-10-16/GGB - Function created.

  CImagePlane::~CImagePlane()
  {
  }

  /// @brief Assignment operator
  /// @param[in] rhs: The image plane to copy to this.
  /// @throws std::bad_alloc
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  ///           @li Use C-style arrays as storage type.
  ///           @li Use cfitsio rather than CCfits for accessing FITS files.
  ///           @li Add additional data types as supported by cfitsio V3.3
  /// @version 2015-07-06/GGB - Added the additional fields for rendering.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-03-09/GGB - Converted to reflect native data types.
  /// @version 2011-05-01/GGB - Function created.

  CImagePlane &CImagePlane::operator=(const CImagePlane &rhs)
  {
    if (this != &rhs)		// Self assignment
    {

      deleteImagePlane();

        // Copy all the details.

      dimX = rhs.dimX;
      dimY = rhs.dimY;
      bMinMax = rhs.bMinMax;
      bMean = rhs.bMean;
      fMin = rhs.fMin;
      fMax = rhs.fMax;
      fMean = rhs.fMean;
      bitpix_ = rhs.bitpix_;
      bscale_ = rhs.bscale_;
      bzero_ = rhs.bzero_;
      planeColour = rhs.planeColour;
      transparency = rhs.transparency;
      blackPoint = rhs.blackPoint;
      whitePoint = rhs.whitePoint;
      invert = rhs.invert;
      transferFunction = rhs.transferFunction;
      transferFunctionParameter = rhs.transferFunctionParameter;

      switch (bitpix_)
      {
        case BYTE_IMG:
        {
          imagePlane8 = std::make_unique<std::uint8_t[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlane8.get(), imagePlane8.get());
          break;
        };
        case SBYTE_IMG:
        {
          imagePlaneS8 = std::make_unique<std::int8_t[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlaneS8.get(), imagePlaneS8.get());
          break;
        };
        case USHORT_IMG:
        {
          imagePlaneU16 = std::make_unique<std::uint16_t[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlaneU16.get(), imagePlaneU16.get());
          break;
        };
        case SHORT_IMG:
        {
          imagePlane16 = std::make_unique<std::int16_t[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlane16.get(), imagePlane16.get());
          break;
        };
        case ULONG_IMG:
        {
          imagePlaneU32 = std::make_unique<std::uint32_t[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlaneU32.get(), imagePlaneU32.get());
          break;
        };
        case LONG_IMG:
        {
          imagePlane32 = std::make_unique<std::int32_t[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlane32.get(), imagePlane32.get());
          break;
        };
        case LONGLONG_IMG:
        {
          imagePlane64 = std::make_unique<std::int64_t[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlane64.get(), imagePlane64.get());
          break;
        };
        case FLOAT_IMG:
        {
          imagePlaneF = std::make_unique<float[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlaneF.get(), imagePlaneF.get());
          break;
        };
        case DOUBLE_IMG:
        {
          imagePlaneD = std::make_unique<double[]>(dimX * dimY);
          copyImagePlane(rhs.imagePlaneD.get(), imagePlaneD.get());
          break;
        };
      };
    };
    return (*this);
  }

  /// @brief Thread function for adding two image planes together.
  /// @param[in] rhs - The right hand side image plane.
  /// @param[in] rows - <0> the Start row and <1> the end row.
  /// @details The function acts on entire rows (x values)
  /// @throws None.
  /// @version 2013-04-26/GGB - Function created.

  void CImagePlane::plusThread(CImagePlane const &rhs, std::tuple<AXIS_t, AXIS_t> const &rows)
  {
    AXIS_t yIndexStart = std::get<0>(rows);
    AXIS_t yIndexEnd = std::get<1>(rows);
    AXIS_t xIndex, yIndex;

    for (yIndex = yIndexStart; yIndex < yIndexEnd; ++yIndex)
    {
      for (xIndex = 0; xIndex < dimX; ++xIndex)
      {
        setValue(xIndex, yIndex, getValue(xIndex, yIndex) + rhs.getValue(xIndex, yIndex));
      };
    };
  }

  /// @brief += operator
  /// @param[in] rhs - The right hand side value.
  /// @returns this object, with the sum of the two arrays.
  /// @throws GCL::CError(ACL, 0x1002)
  /// @throws GCL::CError(ACL, 0x0004)
  /// @version 2013-08-31/GGB - Added code to ensure that maxThreads cannot be zero.
  /// @version 2013-04-26/GGB - Converted to a multi-threaded function, the BITPIX() value is also changed to allow for the addition.
  /// @version 2010-12-10/GGB - Function created.

  CImagePlane &CImagePlane::operator +=(CImagePlane const &rhs)
  {
    AXIS_t rowStep, rowBegin, rowEnd;
    size_t numberOfThreads;
    size_t threadNumber;
    boost::thread_group threadGroup;
    boost::thread *thread;

      // Adding two images together, we should increase the dyanmic range by changing the BITPIX value.

    switch (rhs.BITPIX())
    {
      case BYTE_IMG:
      {
        switch (BITPIX())
        {
          case BYTE_IMG:
          {
            BITPIX(SHORT_IMG);
            break;
          };
          case USHORT_IMG:
          {
            BITPIX(USHORT_IMG);
            break;
          };
          case SHORT_IMG:
          {
            BITPIX(LONG_IMG);
            break;
          };
          case LONG_IMG:
          {
            BITPIX(LONGLONG_IMG);
            break;
          };
          case LONGLONG_IMG:
          {
            BITPIX(DOUBLE_IMG);
            break;
          };
          case FLOAT_IMG:
          case DOUBLE_IMG:
            break;
          default:
            ACL_ERROR(0x1002);  // CCFITS: Invalid BITPIX value.
        };
        break;
      };
    case SHORT_IMG:
      switch (BITPIX())
      {
      case BYTE_IMG:
      case SHORT_IMG:
        BITPIX(LONG_IMG);
        break;
      case LONG_IMG:
        BITPIX(LONGLONG_IMG);
        break;
      case LONGLONG_IMG:
        BITPIX(DOUBLE_IMG);
        break;
      case FLOAT_IMG:
      case DOUBLE_IMG:
        break;
      default:
        ACL_ERROR(0x1002);  // CCFITS: Invalid BITPIX value.
      };
      break;
    case LONG_IMG:
      switch (BITPIX())
      {
      case BYTE_IMG:
      case SHORT_IMG:
      case LONG_IMG:
        BITPIX(LONGLONG_IMG);
        break;
      case LONGLONG_IMG:
        BITPIX(DOUBLE_IMG);
        break;
      case FLOAT_IMG:
      case DOUBLE_IMG:
        break;
      default:
        ACL_ERROR(0x1002);  // CCFITS: Invalid BITPIX value.
      };
      break;
    case LONGLONG_IMG:
      switch (BITPIX())
      {
      case BYTE_IMG:
      case SHORT_IMG:
      case LONG_IMG:
      case LONGLONG_IMG:
        BITPIX(DOUBLE_IMG);
        break;
      case FLOAT_IMG:
      case DOUBLE_IMG:
        break;
      default:
        ACL_ERROR(0x1002);  // CCFITS: Invalid BITPIX value.
      };
      break;
    case FLOAT_IMG:
    case DOUBLE_IMG:
      BITPIX(DOUBLE_IMG);
      break;
    default:
      ACL_ERROR(0x1002);  // CCFITS: Invalid BITPIX value.
    };

    if ( (dimX == rhs.dimX) && (dimY == rhs.dimY) )
    {
      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows
      // in the image.

      maxThreads = std::max(static_cast<size_t>(1), maxThreads);

      if ( (dimY >= 0) && (static_cast<size_t>(dimY) < maxThreads))
      {
        numberOfThreads = static_cast<size_t>(dimY);
      }
      else
      {
        numberOfThreads = maxThreads;
      };

      rowStep = dimY / numberOfThreads;  // Size of each portion for threading.

        // Spawn the threads.

      rowEnd = 0;

      for (threadNumber = 0; threadNumber < numberOfThreads; ++threadNumber)
      {
        rowBegin = rowEnd;
        if (threadNumber == (numberOfThreads -1) )
        {
          rowEnd = dimY;
        }
        else
        {
          rowEnd += rowStep;
        };

        thread = new boost::thread(&CImagePlane::plusThread, this, rhs, std::make_tuple(rowBegin, rowEnd));
        threadGroup.add_thread(thread);
        thread = nullptr;
      };

      threadGroup.join_all();     // Wait for all the threads to finish.
    }
    else
    {
      RUNTIME_ERROR(boost::locale::translate("Inconsistent image sizes."));
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.

    return (*this);
  }

  /// @brief Thread function for subtracting two image planes.
  //
  // 2013-04-26/GGB - Function created.

  void CImagePlane::minusThread(CImagePlane const &rhs, std::tuple<AXIS_t, AXIS_t> const &rows)
  {
    AXIS_t yIndexStart = std::get<0>(rows);
    AXIS_t yIndexEnd = std::get<1>(rows);
    AXIS_t xIndex, yIndex;

    for (yIndex = yIndexStart; yIndex < yIndexEnd; ++yIndex)
    {
      for (xIndex = 0; xIndex < dimX; ++xIndex)
      {
        setValue(xIndex, yIndex, getValue(xIndex, yIndex) - rhs.getValue(xIndex, yIndex));
      };
    };
  }

  /// @brief operator -=
  //
  // 2013-08-31/GGB - Added code to ensure that maxThreads cannot be zero.
  // 2010-12-10/GGB - Function created.

  CImagePlane &CImagePlane::operator-=(const CImagePlane &rhs)
  {
    AXIS_t rowStep, rowBegin, rowEnd;
    size_t numberOfThreads;
    size_t threadNumber;
    boost::thread_group threadGroup;
    boost::thread *thread;

    if ( (dimX == rhs.dimX) && (dimY == rhs.dimY) )
    {
      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows
      // in the image.

      maxThreads = std::max(static_cast<size_t>(1), maxThreads);

      if ( (dimY >= 0) && (static_cast<size_t>(dimY) < maxThreads))
      {
        numberOfThreads = static_cast<size_t>(dimY);
      }
      else
      {
        numberOfThreads = maxThreads;
      };

      rowStep = dimY / numberOfThreads;  // Size of each portion for threading.

        // Spawn the threads.

      rowEnd = 0;

      for (threadNumber = 0; threadNumber < numberOfThreads; ++threadNumber)
      {
        rowBegin = rowEnd;
        if (threadNumber == (numberOfThreads -1) )
        {
          rowEnd = dimY;
        }
        else
        {
          rowEnd += rowStep;
        }

        thread = new boost::thread(&CImagePlane::minusThread, this, rhs, std::make_tuple(rowBegin, rowEnd));
        threadGroup.add_thread(thread);
        thread = nullptr;
      };

      threadGroup.join_all();     // Wait for all the threads to finish.
    }
    else
    {
      RUNTIME_ERROR(boost::locale::translate("Inconsistent image sizes."));
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.

    return (*this);
  }

  /// @brief Multiplication operator
  /// @param[in] rhs - The right hand side image plane.
  /// @throws GCL::CRuntimeAssert
  //
  // 2010-12-28/GGB - Function created.

  CImagePlane &CImagePlane::operator*=(CImagePlane const &rhs)
  {
    RUNTIME_ASSERT(rhs.dimX != dimX, "this->dimX != rhs.dimX");
    RUNTIME_ASSERT(rhs.dimY != dimY, "this->dimY != rhs.dimY");

    INDEX_t index;
    INDEX_t const count = dimX * dimY;

    if (this == &rhs)
    {
        // Self multiplication

      for(index = 0; index < count; index++)
      {
        imagePlaneD[index] *= imagePlaneD[index];
      }
    }
    else
    {
      for(index = 0; index < count; index++)
      {
        imagePlaneD[index] *= rhs.imagePlaneD[index];
      }
    };

    bMinMax = false;
    bMean = false;

    return (*this);
  }

  /// @brief Multiplication by a constant value operator
  //
  // 2011-05-12/GGB - Function created.

  CImagePlane &CImagePlane::operator*=(double dbl)
  {
    INDEX_t index;
    INDEX_t const count = dimX * dimY;

    for (index = 0; index < count; index++)
    {
      imagePlaneD[index] *= dbl;
    };

    bMinMax = false;
    bMean = false;

    return (*this);
  }

  // /= operator
  //
  // 2010-12-11/GGB - Function created.

  CImagePlane &CImagePlane::operator/=(const CImagePlane &rhs)
  {
    INDEX_t index;
    INDEX_t const count = dimX * dimY;

    if (this == &rhs)
    {
        // Self assignment. Makes all values equal to 1.Use a fast approach.

      for (index = 0; index < count; index++)
        imagePlaneD[index] = 1;
    }
    else
    {
      for (index = 0; index < count; index++)
        imagePlaneD[index] /= rhs.imagePlaneD[index];
    };

    bMinMax = false;
    bMean = false;

    return (*this);
  }

  // Performs division by a fixed value.
  //
  // 2011-05-12/GGB - Function created.

  CImagePlane &CImagePlane::operator/=(double dbl)
  {
    INDEX_t index;
    INDEX_t const count = dimX * dimY;

    if (dbl == 0)
    {
      ACL_ERROR(0x2200);  // Divide by zero
    }
    else
    {
      for (index = 0; index < count; index++)
        imagePlaneD[index] /= dbl;

      bMinMax = false;
      bMean = false;
    };

    return (*this);
  }

  /// @brief Performs the absolute difference function
  //
  // 2010-12-28/GGB - Function created.

  CImagePlane &CImagePlane::operator%=(CImagePlane const &rhs)
  {
    INDEX_t index;
    INDEX_t const count = dimX * dimY;

    if (this == &rhs)
    {
      // Self assignment. Make all values = 0.

      for(index = 0; index < count; index++)
        imagePlaneD[index] = 0;
    }
    else
    {
      for(index = 0; index < count; index++)
        imagePlaneD[index] = abs(imagePlaneD[index] - rhs.imagePlaneD[index]);
    };

    bMinMax = false;
    bMean = false;

    return *this;
  }

  /// @brief Addition operator.
  /// @param[in] rhs - The right hand side value.
  /// @throws None.
  /// @details Uses operator +=
  /// @version 2010-12-10/GGB - Function created.

  const CImagePlane CImagePlane::operator+(const CImagePlane &rhs)
  {
    return CImagePlane(*this) += rhs;
  }

  // Subtraction operator.
  //
  /// @version 2010-12-10/GGB - Function created.

  const CImagePlane CImagePlane::operator-(const CImagePlane &rhs)
  {
    return CImagePlane(*this) -= rhs;
  }

  // Multiplication operator
  //
  // 2010-12-28/GGB - Function created.

  const CImagePlane CImagePlane::operator*(CImagePlane const &rhs)
  {
    return CImagePlane(*this) *= rhs;
  }

  // Division operator.
  //
  // 2010-12-11/GGB - Function created.

  const CImagePlane CImagePlane::operator/(const CImagePlane &rhs)
  {
    return CImagePlane(*this) /= rhs;
  }

  // Returns the absoulute difference.
  //
  // 2010-12-28/GGB - Function created.

  CImagePlane const CImagePlane::operator%(CImagePlane const &rhs)
  {
    return CImagePlane(*this) %= rhs;
  }

  /// @brief      Array operator.
  /// @param[in]  index: The array index to return the value of.
  /// @returns    The value at the specified array index.
  /// @throws     CRuntimeError
  /// @version    2013-01-26/GGB - Added support for native FITS formats.
  /// @version    2011-05-01/GGB - Function created.

  FP_t CImagePlane::operator [](INDEX_t index) const
  {
    if ( ((dimX * dimY) >= 0) && (static_cast<INDEX_t>(dimX * dimY) > index) )
    {
      return getValue(index);
    }
    else
    {
      RUNTIME_ERROR(boost::locale::translate("Array index out of bounds"));
    };
  }

  /// (x, y) Array operator
  /// EXCEPTIONS: 0x1200 - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
  ///             0x1203 - CIMAGEPLANE: Invalid BITPIX value.
  ///             0x1204 - CIMAGEPLANE: Invalid coordinates.
  //
  // 2013-01-27/GGB - Simplified to use function getValue(long)
  // 2012-11-29/GGB - Changed to handle native data types.
  // 2011-05-01/GGB - Function created.

  FP_t CImagePlane::operator()(AXIS_t xIndex, AXIS_t yIndex) const
  {
    if ( (xIndex < dimX) && (yIndex < dimY) && (xIndex >= 0) && (yIndex >= 0) )
    {
      return getValue(arrayIndex(xIndex, yIndex, dimX));
    }
    else
    {
      ACL_ERROR(0x1204);    // CIMAGEPLANE: Invalid coordinates.
    };
  }

  FP_t CImagePlane::bicubic(FP_t const &, FP_t const &) const
  {
    return 0;
  }

  /// @brief      Interpolates the value of the image for position (x, y) Uses the bilinear interpolation function.
  /// @param[in]  x: position x
  /// @param[in]  y: Coordinate y
  /// @returns    The interpolated value.
  /// @throws     CRuntimeError
  /// @version    2013-03-11/GGB - Changed storage type of imagePlanes to std::vector<>
  /// @version    2010-12-29/GGB - Function created.

  FP_t CImagePlane::bilinear(FP_t const &x, FP_t const &y) const
  {
    AXIS_t X = static_cast<AXIS_t>(x);
    AXIS_t Y = static_cast<AXIS_t>(y);
    INDEX_t index = Y * dimX + X;
    FP_t F00, F01, F10, F11;
    FP_t  xf = x - X;
    FP_t yf = y - Y;

    if ( ( x < 0) || (y < 0) || (x >= dimX) || (y >= dimY) )
    {
      RUNTIME_ERROR(boost::locale::translate("Invalid Coordinates"));
    };

    F00 = getValue(index);
    F10 = getValue(++index);
    index += dimX;
    F11 = getValue(index);
    F01 = getValue(--index);

    return F00 * (1-xf) * (1-yf) + F01 * (1-xf) * yf + F10 * xf * (1-yf) + F11 * xf * yf;
  }

  /// @brief Bins the pixels into larger pixels
  /// @param[in] nSize - Number of pixels to bin together. (0 < nSize <= 10)
  /// @throws RuntimeAssert.
  /// @note Always converts the underlying data type to double when the pixels are binned.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-08-31/GGB - Added code to force maxThreads to not be zero.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-01-27/GGB - Introduce multi-threading and solve bugs around native FITS types.
  /// @version 2011-06-11/GGB - Function created.

  void CImagePlane::binPixels(unsigned int nSize)
  {
    RUNTIME_ASSERT(nSize != 0, "Bin Pixels is only valid with a non-zero integer.");
    RUNTIME_ASSERT(nSize <= 10, "Bin Pixels is only valid with a binning value of <= 10.");

    AXIS_t yStep, yBegin, yEnd;
    size_t  numberOfThreads;
    size_t threadNumber;
    boost::thread_group threadGroup;
    boost::thread *thread;

    AXIS_t newWidth = dimX / nSize;
    AXIS_t newHeight = dimY / nSize;

    std::unique_ptr<double[]> newImagePlaneD(new double[newWidth * newHeight]);

      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows
      // in the image.

    maxThreads = std::max(size_t(1), maxThreads);

    if ((newHeight >= 0) && (static_cast<size_t>(newHeight) < maxThreads))
    {
      numberOfThreads = static_cast<size_t>(newHeight);
    }
    else
    {
      numberOfThreads = maxThreads;
    }

    yStep = newHeight / numberOfThreads;  // Size of each portion for threading.

       // Spawn the threads.

    yEnd = 0;

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      yBegin = yEnd;
      if (threadNumber == (numberOfThreads -1) )
      {
        yEnd = newHeight;
      }
      else
      {
        yEnd += yStep;
      };

      thread = new boost::thread(&CImagePlane::binPixelsThread<FP_t>, this, newImagePlaneD.get(),
                                 std::make_tuple(yBegin, yEnd), newWidth, nSize);
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    //deleteImagePlane();         // Delete the existing image plane.

    bitpix_ = DOUBLE_IMG;          // Change data type to double
    imagePlaneD.swap(newImagePlaneD);

    dimX = newWidth;
    dimY = newHeight;
    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Thread function for binning pixels.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-01-27/GGB - Function created.

  template<typename T>
  void CImagePlane::binPixelsThread(T *newImagePlane, std::tuple<AXIS_t, AXIS_t> vals, AXIS_t newWidth, unsigned int nsize)
  {
    AXIS_t xIndex, yIndex;
    AXIS_t xTemp, yTemp;
    int x_, y_;
    FP_t accum;

    for (xIndex = 0; xIndex < newWidth; xIndex++)
    {
      for (yIndex = std::get<0>(vals); yIndex < std::get<1>(vals); yIndex++)
      {
        xTemp = xIndex * nsize;
        yTemp = yIndex * nsize;
        accum = 0;
        for (x_ = 0; x_ < nsize; x_++)
        {
          for (y_ = 0; y_ < nsize; y_++)
          {
            accum += getValue(arrayIndex(xTemp + x_, yTemp + y_, dimX));
          };
        };

        newImagePlane[arrayIndex(xIndex, yIndex, newWidth)] = accum;
      };
    };
  }

  /// @brief Returns the values of BITPIX.
  /// @returns The current BITPIX value.
  /// @throws None.
  /// @version 2012-11-27/GGB - Function created.

  int CImagePlane::BITPIX() const
  {
    return bitpix_;
  }

  /// @brief Sets the value of BITPIX.
  /// @throws CError(ACL::0x1901) - HDB: Only supported BITPIX values are allowed
  /// @throws std::bad_alloc
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-03-09/GGB - Added code to convert between image plane types. (See also TO DO at top of file.)
  /// @version 2012-11-29/GGB - Function created.

  void CImagePlane::BITPIX(int bp)
  {
    if ( (bp == BYTE_IMG) || (bp == SHORT_IMG) || (bp == LONG_IMG) ||
         (bp == LONGLONG_IMG) || (bp == FLOAT_IMG) || (bp == DOUBLE_IMG) )
    {
      if (bp != bitpix_)
      {
          // Need to upgrade the array type.

        switch(bp)
        {
          case BYTE_IMG:
            imagePlane8 = std::make_unique<std::uint8_t[]>(dimX * dimY);
            switch (bitpix_)
            {
              case SHORT_IMG:
              {
                convertImagePlane(imagePlane16.get(), imagePlane8.get());
                imagePlane16.reset(nullptr);
                break;
              };
              case LONG_IMG:
              {
                convertImagePlane(imagePlane32.get(), imagePlane8.get());
                imagePlane32.reset(nullptr);
                break;
              };
              case LONGLONG_IMG:
              {
                convertImagePlane(imagePlane64.get(), imagePlane8.get());
                imagePlane64.reset(nullptr);
                break;
              };
              case BYTE_IMG:
              {
                convertImagePlane(imagePlaneF.get(), imagePlane8.get());
                imagePlaneF.reset(nullptr);
                break;
              };
              case DOUBLE_IMG:
              {
                convertImagePlane(imagePlaneD.get(), imagePlane8.get());
                imagePlaneD.reset(nullptr);
                break;
              };
            };
            break;
          case SHORT_IMG:    // (16)
            imagePlane16 = std::make_unique<std::int16_t[]>(dimX * dimY);
            switch (bitpix_)
            {
              case BYTE_IMG:
              {
                convertImagePlane(imagePlane8.get(), imagePlane16.get());
                imagePlane8.reset(nullptr);
                break;
              };
              case LONG_IMG:
              {
                convertImagePlane(imagePlane32.get(), imagePlane16.get());
                imagePlane32.reset(nullptr);
                break;
              };
              case LONGLONG_IMG:
              {
                convertImagePlane(imagePlane64.get(), imagePlane16.get());
                imagePlane64.reset(nullptr);
                break;
              };
              case FLOAT_IMG:
              {
                convertImagePlane(imagePlaneF.get(), imagePlane16.get());
                imagePlaneF.reset(nullptr);
                break;
              };
              case DOUBLE_IMG:
              {
                convertImagePlane(imagePlaneD.get(), imagePlane16.get());
                imagePlaneD.reset(nullptr);
                break;
              };
            };
            break;
          case LONG_IMG:
            imagePlane32 = std::make_unique<std::int32_t[]>(dimX * dimY);
            switch (bitpix_)
            {
              case BYTE_IMG:
                convertImagePlane(imagePlane8.get(), imagePlane32.get());
                imagePlane8.reset(nullptr);
                break;
              case SHORT_IMG:
                convertImagePlane(imagePlane16.get(), imagePlane32.get());
                imagePlane16.reset(nullptr);
                break;
              case LONGLONG_IMG:
              {
                convertImagePlane(imagePlane64.get(), imagePlane32.get());
                imagePlane64.reset(nullptr);
                break;
              };
              case FLOAT_IMG:
                convertImagePlane(imagePlaneF.get(), imagePlane32.get());
                imagePlaneF.reset(nullptr);
                break;
              case DOUBLE_IMG:
                convertImagePlane(imagePlaneD.get(), imagePlane32.get());
                imagePlaneD.reset(nullptr);
                break;
            };
            break;
          case LONGLONG_IMG:
            imagePlane64 = std::make_unique<std::int64_t[]>(dimX * dimY);
            switch (bitpix_)
            {
              case BYTE_IMG:
                convertImagePlane(imagePlane8.get(), imagePlane64.get());
                imagePlane8.reset(nullptr);
                break;
              case SHORT_IMG:
                convertImagePlane(imagePlane16.get(), imagePlane64.get());
                imagePlane16.reset(nullptr);
                break;
              case LONG_IMG:
                convertImagePlane(imagePlane32.get(), imagePlane64.get());
                imagePlane32.reset(nullptr);
                break;
              case FLOAT_IMG:
                convertImagePlane(imagePlaneF.get(), imagePlane64.get());
                imagePlaneF.reset(nullptr);
                break;
              case DOUBLE_IMG:
                convertImagePlane(imagePlaneD.get(), imagePlane64.get());
                imagePlaneD.reset(nullptr);
                break;
            };
            break;
          case FLOAT_IMG:
            imagePlaneF = std::make_unique<float[]>(dimX * dimY);
            switch (bitpix_)
            {
              case BYTE_IMG:
                convertImagePlane(imagePlane8.get(), imagePlaneF.get());
                imagePlane8.reset(nullptr);
                break;
              case USHORT_IMG:
              {
                convertImagePlane(imagePlaneU16.get(), imagePlaneF.get());
                imagePlaneU16.reset(nullptr);
                break;
              };
              case SHORT_IMG:
              {
                convertImagePlane(imagePlane16.get(), imagePlaneF.get());
                imagePlane16.reset(nullptr);
                break;
              };
              case LONG_IMG:
              {
                convertImagePlane(imagePlane32.get(), imagePlaneF.get());
                imagePlane32.reset(nullptr);
                break;
              };
              case LONGLONG_IMG:
                convertImagePlane(imagePlane64.get(), imagePlaneF.get());
                imagePlane64.reset(nullptr);
                break;
              case DOUBLE_IMG:
                convertImagePlane(imagePlaneD.get(), imagePlaneF.get());
                imagePlaneD.reset(nullptr);
                break;
            };
            break;
          case DOUBLE_IMG:
            imagePlaneD = std::make_unique<double[]>(dimX * dimY);
            switch (bitpix_)
            {
              case BYTE_IMG:
                convertImagePlane(imagePlane8.get(), imagePlaneD.get());
                imagePlane8.reset(nullptr);
                break;
              case SHORT_IMG:
                convertImagePlane(imagePlane16.get(), imagePlaneD.get());
                imagePlane16.reset(nullptr);
                break;
              case LONG_IMG:
                convertImagePlane(imagePlane32.get(), imagePlaneD.get());
                imagePlane32.reset(nullptr);
                break;
              case LONGLONG_IMG:
                convertImagePlane(imagePlane64.get(), imagePlaneD.get());
                imagePlane64.reset(nullptr);
                break;
              case FLOAT_IMG:
                convertImagePlane(imagePlaneF.get(), imagePlaneD.get());
                imagePlaneF.reset(nullptr);
                break;
            };
            break;
        };
        bitpix_ = bp;
      }
    }
    else
    {
      ACL_ERROR(0x1901);  // HDB: Only supported BITPIX values are allowed.
    };
  }

  /// Given an image and a starting point, walk the gradient to the brightest pixel and return its location, never going more than
  /// maxrad away.
  /// The return value is valid if the brightest pixel is found within maxsteps.
  /// Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  /// PARAMETERS: c0 - Center point to search from.
  ///             rmax - The maximum search radius
  ///             sensitivity - The search sensitivity
  /// @returns Centroid if found.
  /// @version 2013-08-02/GGB - Changed bp parameter to sensitivity.
  /// @version 2012-07-20/GGB - Function converted to C++

  std::optional<MCL::TPoint2D<AXIS_t> > CImagePlane::brightWalk(MCL::TPoint2D<AXIS_t> const &guess, AXIS_t rmax,
                                                                  int sensitivity) const
  {
    assert(guess.x() > 0);
    assert(guess.x() < (dimX - 1));
    assert(guess.y() > 0);
    assert(guess.y() < (dimY - 1));
    assert(rmax > 0);
    assert(sensitivity > 0);

    static AXIS_t dx[8] = {1,0,-1,1,-1, 1, 0,-1};
    static AXIS_t dy[8] = {1,1, 1,0, 0,-1,-1,-1};

    FP_t b, tmpb, newb;
    AXIS_t x, y, x1, y1, i, xa, ya;
    AXIS_t newx = 0;
    AXIS_t newy = 0;
    std::optional<MCL::TPoint2D<AXIS_t> > returnValue;
    INDEX_t imagePointsSize = MCL::pow2(2 * rmax + 1);

    FP_t *imagePoints = new FP_t[imagePointsSize];   // Create a vector to hold all the points.

    i = 0;

    AXIS_t xmin = std::max(static_cast<AXIS_t>(0), guess.x() - rmax);
    AXIS_t xmax = std::min(dimX - 1, guess.x() + rmax);
    AXIS_t ymin = std::max(static_cast<AXIS_t>(0), guess.y() - rmax);
    AXIS_t ymax = std::min(dimY- 1, guess.y() + rmax);

    for (x = xmin; x <= xmax; ++x)
    {
      for (y = ymin; y <= ymax; ++y)
      {
        imagePoints[i++] = getValue(x, y);
      };
    };

    std::optional<FP_t> mean = MCL::mean(imagePoints, imagePointsSize);
    std::optional<FP_t> stdev = MCL::stdev(imagePoints, imagePointsSize);

    if (mean && stdev)
    {

      /* start by assuming seed point is brightest */

      b = getValue(guess.x(), guess.y());
      x = guess.x();
      y = guess.y();
      xa = guess.x();
      ya = guess.y();

      /* walk towards any brighter pixel */

      for (;;)
      {
        newx = newy = 0;

        /* Find brightest pixel in 3x3 region */

        newb = b;
        for(i = 0; i < 8; i++)
        {
          x1 = x + dx[i];
          y1 = y + dy[i];
          tmpb = getValue(x1, y1);
          if (tmpb >= newb)
          {
            if (x1 == xa && y1 == ya)
              break;
            xa = x;
            ya = y;
            newx = x1;
            newy = y1;
            newb = tmpb;
          }
        }

        /* If brightest pixel is one in center of region, quit */

        if (newb == b)
          break;

        /* Otherwise, set brightest pixel to new center */

        x = newx;
        y = newy;
        b = newb;
        if (abs(x-guess.x()) > rmax || abs(y-guess.y()) > rmax)
          break;
      }

      if (getValue(x, y) >= *mean + (*stdev) * sensitivity)     // Sanity check, does the bright point fall above the surrounding area mean.
        if (abs(x-guess.x()) <= rmax || abs(y-guess.y()) <= rmax)
          returnValue = MCL::TPoint2D<AXIS_t>(x, y);
    };

    return returnValue;
  }

  /// @brief Returns the value of BSCALE
  /// @returns The value of BSCALE.
  /// @throws None.
  /// @version 2012-12-01/GGB - Function created.

  double CImagePlane::BSCALE() const
  {
    return bscale_;
  }

  /// @brief Sets the value of BSCALE.
  /// @param[in] bs - The new value for BSCALE.
  /// @throws None.
  /// @version 2012-12-01/GGB - Function created.

  void CImagePlane::BSCALE(double bs)
  {
    bscale_ = bs;
  }

  /// @brief Returns the value of BZERO
  /// @returns The value of BZERO
  /// @throws None.
  /// @version 2012-12-01/GGB - Function created.

  double CImagePlane::BZERO() const
  {
    return bzero_;
  }

  /// @brief Sets the value of BZERO.
  /// @param[in] bz - The new value of BZERO
  /// @throws None.
  /// @version 2012-12-01/GGB - Function created.

  void CImagePlane::BZERO(double bz)
  {
    bzero_ = bz;
  }

  /// @brief Converts an image plane of type T to an image plane of type U.
  /// @param[in] src - The source array
  /// @param[in] dest - The destination array
  /// @throws None.
  /// @details This function spawns the threads to do the actual conversion of the image plane data.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB - Moved into CImagePlane class.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-03-09/GGB - Function created.

  template<typename T, typename U>
  void CImagePlane::convertImagePlane(T const *src, U *dest)
  {
    AXIS_t dim = dimX * dimY;
    size_t numberOfThreads;
    size_t threadNumber;
    INDEX_t indexStep;
    INDEX_t indexBegin = 0, indexEnd = 0;
    boost::thread_group threadGroup;
    boost::thread *thread;

      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows

    maxThreads = std::max(size_t(1), maxThreads);

    if ( (dim >= 0) && (static_cast<size_t>(dim) < maxThreads) )
    {
      numberOfThreads = dim;
    }
    else
    {
      numberOfThreads = maxThreads;
    }

    indexStep = dim / numberOfThreads;

      // Spawn the threads.

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      indexBegin = indexEnd;
      if (threadNumber == (numberOfThreads -1) )
      {
        indexEnd = dim;
      }
      else
      {
        indexEnd += indexStep;
      };

      thread = new boost::thread(&CImagePlane::convertImagePlaneThread<T, U>, this,
                                 src, dest, indexBegin, indexEnd);
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.
  }

  /// @brief Thread function used when converting image plane types.
  /// @param[in] src - The source array
  /// @param[in] dest - The destination array
  /// @param[in] start - The starting index for this thread
  /// @param[in] end - The ending index for this thread.
  /// @throws None.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB - Moved into class CImagePlane.
  /// @version 2013-03-09/GGB - Function created.

  template<typename T, typename U>
  void CImagePlane::convertImagePlaneThread(T const *src, U *dest, INDEX_t start, INDEX_t end)
  {
    INDEX_t index;

    for (index = start; index < end; ++index)
    {
      dest[index] = static_cast<U>(src[index]);
    }
  }

  /// @brief Function to copy an image plane.
  /// @version 2015-09-02/GGB - Function created.

  template<typename T>
  void CImagePlane::copyImagePlane(T const *src, T *dest)
  {
    ACL::copyImagePlane(std::make_tuple(src, AXIS_t(0), AXIS_t(0), dimX), std::make_tuple(dest, dimX, dimY));
  }

  /// @brief Templated function for performing the image crop.
  /// @throws None.
  /// @version 2015-09-03/GGB - Function created.

  template<typename T>
  void CImagePlane::crop(T const *src, T *dest, MCL::TPoint2D<AXIS_t> const &o, MCL::TPoint2D<AXIS_t> const &d)
  {
    AXIS_t indexX, indexY;

    for (indexX = 0; indexX < d.x(); indexX++)
    {
      for (indexY = 0; indexY < d.y(); indexY++)
      {
        dest[arrayIndex(indexX, indexY, d.x())] = src[arrayIndex(indexX + o.x(), indexY + o.y(), dimX)];
      };
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Function to crop an image. Calls the alternate function to perform the work.
  /// @throws None.
  //
  // 2015-07-06/GGB - Remove unrequired try{} catch(...) throw; block
  // 2011-08-15/GGB - Function created.

  void CImagePlane::crop(AXIS_t xo, AXIS_t yo, AXIS_t xd, AXIS_t yd)
  {
    crop(MCL::TPoint2D<AXIS_t>(xo, yo), MCL::TPoint2D<AXIS_t>(xd, yd));
  }

  /// @brief Function to crop an image.
  /// @param[in] o - Origin of crop.
  /// @param[in] d - Dimensions of crop
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @throws CError(ACL::0x1203)
  /// @throws std::bad_alloc
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-03/GGB - (Bug 8) Corrected to work with all types of storage.
  /// @version 2015-08-29/GGB - Converted back to C-style arrays.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2011-08-15/GGB - Function created.

  void CImagePlane::crop(MCL::TPoint2D<AXIS_t> const &o, MCL::TPoint2D<AXIS_t> const &d)
  {
    RUNTIME_ASSERT(o.x() > 0, "Origin incorrect");
    RUNTIME_ASSERT(o.y() > 0, "Origin incorrect");
    RUNTIME_ASSERT(o.x() + d.x() < dimX, "Origin + dimension incorrect");
    RUNTIME_ASSERT(o.y() + d.y() < dimY, "Origin + dimension incorrect");

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        std::unique_ptr<std::uint8_t[]> newImagePlane8(new std::uint8_t[d.x() * d.y()]);
        crop(imagePlane8.get(), newImagePlane8.get(), o, d);
        imagePlane8.swap(newImagePlane8);
        break;
      };
      case SBYTE_IMG:
      {
        std::unique_ptr<std::int8_t[]> newImagePlaneS8(new std::int8_t[d.x() * d.y()]);
        crop(imagePlaneS8.get(), newImagePlaneS8.get(), o, d);
        imagePlaneS8.swap(newImagePlaneS8);
        break;
      }
      case USHORT_IMG:
      {
        std::unique_ptr<std::uint16_t[]> newImagePlaneU16(new std::uint16_t[d.x() * d.y()]);
        crop(imagePlaneU16.get(), newImagePlaneU16.get(), o, d);
        imagePlaneU16.swap(newImagePlaneU16);
        break;
      };
      case SHORT_IMG:
      {
        std::unique_ptr<std::int16_t[]> newImagePlane16(new std::int16_t[d.x() * d.y()]);
        crop(imagePlane16.get(), newImagePlane16.get(), o, d);
        imagePlane16.swap(newImagePlane16);
        break;
      };
      case ULONG_IMG:
      {
        std::unique_ptr<std::uint32_t[]> newImagePlaneU32(new std::uint32_t[d.x() * d.y()]);
        crop(imagePlaneU32.get(), newImagePlaneU32.get(), o, d);
        imagePlaneU32.swap(newImagePlaneU32);
        break;
      };
      case LONG_IMG:
      {
        std::unique_ptr<std::int32_t[]> newImagePlane32(new std::int32_t[d.x() * d.y()]);
        crop(imagePlane32.get(), newImagePlane32.get(), o, d);
        imagePlane32.swap(newImagePlane32);
        break;
      };
      case LONGLONG_IMG:
      {
        std::unique_ptr<std::int64_t[]> newImagePlane64(new std::int64_t[d.x() * d.y()]);
        crop(imagePlane64.get(), newImagePlane64.get(), o, d);
        imagePlane64.swap(newImagePlane64);
        break;
      };
      case FLOAT_IMG:
      {
        std::unique_ptr<float[]> newImagePlaneF(new float[d.x() * d.y()]);
        crop(imagePlaneF.get(), newImagePlaneF.get(), o, d);
        imagePlaneF.swap(newImagePlaneF);
        break;
      };
      case DOUBLE_IMG:
      {
        std::unique_ptr<double[]> newImagePlaneD(new double[d.x() * d.y()]);
        crop(imagePlaneD.get(), newImagePlaneD.get(), o, d);
        imagePlaneD.swap(newImagePlaneD);
        break;
      };
      default:
      {
        ERROR(ACL, 0x1203);  // CIMAGEPLANE: Invalid BITPIX value.
        break;
      };
    };

    dimX = d.x();
    dimY = d.y();

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Deletes the current image plane. No matter what the type of the image plane is.
  /// @throws None.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB - Use C-style arrays as storage type.
  /// @version 2015-09-02/GGB - Removed default clause to prevent error is the bitpix value has not been assigned.
  /// @version 2013-01-27/GGB - Function created.

  void CImagePlane::deleteImagePlane()
  {
    imagePlane8.reset(nullptr);
    imagePlaneS8.reset(nullptr);
    imagePlaneU16.reset(nullptr);
    imagePlane16.reset(nullptr);
    imagePlaneU32.reset(nullptr);
    imagePlane32.reset(nullptr);
    imagePlane64.reset(nullptr);
    imagePlaneF.reset(nullptr);
    imagePlaneD.reset(nullptr);

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Source Extraction function using the findStars routines from libWCS
  /// @param[out] sourceList:
  /// @param[in] sourceParameters:
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2014-02-01/GGB - Function created.

  void CImagePlane::findStars(TImageSourceContainer &sourceList, SFindSources const &sourceParameters)
  {
    std::unique_ptr<double[]> tempImage(new double[dimX * dimY]);

    switch(bitpix_)
    {
      case BYTE_IMG:
      {
        convertImagePlane(imagePlane8.get(), tempImage.get());
        break;
      };
      case SBYTE_IMG:
      {
        convertImagePlane(imagePlaneS8.get(), tempImage.get());
        break;
      };
      case USHORT_IMG:
      {
        convertImagePlane(imagePlaneU16.get(), tempImage.get());
        break;
      };
      case SHORT_IMG:
      {
        convertImagePlane(imagePlane16.get(), tempImage.get());
        break;
      };
      case ULONG_IMG:
      {
        convertImagePlane(imagePlaneU32.get(), tempImage.get());
        break;
      };
      case LONG_IMG:
      {
        convertImagePlane(imagePlane32.get(), tempImage.get());
        break;
      };
      case LONGLONG_IMG:
      {
        convertImagePlane(imagePlane64.get(), tempImage.get());
        break;
      };
      case FLOAT_IMG:
      {
        convertImagePlane(imagePlaneF.get(), tempImage.get());
        break;
      };
      case DOUBLE_IMG:
      {
        copyImagePlane(imagePlaneD.get(), tempImage.get());
        break;
      };
    };

    CFindSources sourceExtractor(tempImage.get(), dimX, dimY);

    sourceExtractor.setParameters(sourceParameters);

    sourceExtractor.findStars(sourceList);
  }

  /// @brief        Determines the FWHM of a star.
  /// @details      Searches within radius pixels of the center. Performs 2D curve fitting to function
  /// @param[in]    center: The center point to determine the FWHM from.
  /// @param[in]    radius: The maximum radius
  /// @returns      The FWHM of the object.
  /// @throws       0x1205 - IMAGEPLANE: FWHM Call Radius == 0.
  /// @version      2020-09-21/GGB - Refactored to use vector_sorted.
  /// @version      2018-05-12/GGB - Function created/

  std::optional<FP_t> CImagePlane::FWHM(MCL::TPoint2D<AXIS_t> const &center, AXIS_t radius) const
  {
    AXIS_t indexX, indexY, sizeX, sizeY;

    class CDataPoint
    {
    public:
      MCL::TPoint2D<AXIS_t> coordinates_;
      FP_t value_;

      CDataPoint() : coordinates_(MCL::TPoint2D<AXIS_t>(0,0)), value_(0) {}
      CDataPoint(MCL::TPoint2D<AXIS_t> const &coordinates, FP_t value) : coordinates_(coordinates), value_(value) {}
      CDataPoint(CDataPoint const &toCopy) : coordinates_(toCopy.coordinates_), value_(toCopy.value_) {}

      bool operator<(CDataPoint const &rhs) const
      {
        return (value_ < rhs.value_);
      }

      bool operator ==(CDataPoint const &rhs) const
      {
        return (value_ == rhs.value_);
      }
    };

    SCL::vector_sorted<CDataPoint> dataPoints;
    SCL::vector_sorted<CDataPoint>::iterator dataPointsIterator;

    if (radius <= 0)
    {
      ACL_ERROR(0x1205);    //IMAGEPLANE: FWHM Call Radius == 0.
    };

      // Create the sub image plane.

    CImagePlane *subPlane = new CImagePlane(this, center.x() - radius, center.x() + radius,
                                            center.y() - radius, center.y() + radius);

      // Upsample the image. by a factor of 10.

    //subPlane->resample(2 * radius * 10, 2 * radius * 10);  // This function also changes the BITPIX value to double.

      // Calculate the new center of the image.

    long centerX = radius;
    long centerY = radius;
    FP_t meanValue = getMeanValue();

    sizeX = sizeY = radius * 2;

    for (indexY = 0; indexY < sizeY; indexY++)
    {
      for (indexX = 0; indexX < sizeX; indexX++)
      {
        FP_t dataValue = subPlane->getValue(arrayIndex(indexX, indexY, sizeX));

        dataValue -= (meanValue + 3);   // Remove the noise floor. IE ensure that there is no offset on each of the values.
        if (dataValue > 0)
        {
            // Only store values that are positive. IE above the noise floor.

          CDataPoint dataPoint(MCL::TPoint2D<long>(indexX - centerX, indexY - centerY), dataValue);
          dataPoints.push_back(dataPoint);
        };
      };
    };

    if ( !dataPoints.empty() )
    {
      FP_t maxValue = dataPoints[0].value_;
      FP_t thirdMax = maxValue / 3;

        // Erase all values that are less than 1/3 of the max. This is to eliminate inaccuracy caused by the tail of the function.

      dataPointsIterator = dataPoints.begin();

      while ( (dataPointsIterator != dataPoints.end()) && ( (*dataPointsIterator).value_ > thirdMax) )
      {
        ++dataPointsIterator;
      };

      if (dataPointsIterator != dataPoints.end())
      {
        dataPoints.erase(dataPointsIterator, dataPoints.end());
      };

        // Must have greater that 4 points of data, as we are searching for 4 unknowns.

      if (dataPoints.size() < 5)
      {
        return std::optional<FP_t>();
      }
      else
      {
        // Now do a non-linear lease squares fit.

        std::vector<std::pair<input_vector, double> > dataSamples;
        input_vector input;

          // Copy all the data into the array for manipulating.

        for (dataPointsIterator = dataPoints.begin(); dataPointsIterator != dataPoints.end(); ++dataPointsIterator)
        {
          input(0) = (*dataPointsIterator).coordinates_.x();
          input(1) = (*dataPointsIterator).coordinates_.y();
          dataSamples.push_back( std::make_pair(input, (*dataPointsIterator).value_) );
        };
        return std::optional<FP_t>(ACL::FWHM(dataSamples));
      };
    }
    else
    {
      return std::optional<FP_t>();
    };
  }

  /// @brief Gets the image plane value at the specified point.
  /// @param[in] addr: The point to get (x, y)
  /// @returns The value of the point.
  /// @throws None.
  /// @version 2012-11-29/GGB - Changed to use native data types.
  /// @version 2012-11-10/GGB - Function created.

  FP_t CImagePlane::getValue(MCL::TPoint2D<AXIS_t> const &addr) const
  {
    return getValue(addr.x(), addr.y());
  }

  /// @brief Gets the image plane value at the specified point.
  /// @param[in] x - x coordinate (0 <= x < dimX).
  /// @param[in] y - y coordinate (0 <= y < dimY).
  /// @returns The image value at the specified point.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @version 2015-08-02/GGB - Added correction for bzero and bscale.
  /// @version 2012-11-29/GGB - Added native data support
  /// @version 2010-10-16/GGB - Function created.

  FP_t CImagePlane::getValue(AXIS_t x, AXIS_t y) const
  {
    RUNTIME_ASSERT(x >= 0, "Parameter x cannot be less than zero.");
    RUNTIME_ASSERT(x < dimX, "Parameter x > dimX.");

    RUNTIME_ASSERT(y >= 0, "Parameter y cannot be less than zero.");
    RUNTIME_ASSERT(y < dimY, "Parameter y > dimY.");

    INDEX_t index = arrayIndex(x, y, dimX);

    return getValue(index);
  }

  /// @brief Returns the value for a given index.
  /// @details The return value is corrected as follows: returnValue = bzero + bscale * value;
  /// @returns The image value at the specified point.
  /// @throws 0x1200 - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
  /// @throws 0x1202 - CIMAGEPLANE: GetValue(index). index is beyond end of array.
  /// @throws 0x1203 - CIMAGEPLANE: Invalid BITPIX value.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-29/GGB - Added native data support
  /// @version 2010-10-16/GGB - Function created.

  FP_t CImagePlane::CImagePlane::getValue(INDEX_t index) const
  {
    FP_t returnValue;

    if (index > static_cast<INDEX_t>(dimX * dimY) )
    {
      ACL_ERROR(0x1202);    // CIMAGEPLANE: GetValue(index). index is beyond end of array.
    }
    else
    {
      switch (bitpix_)
      {
        case BYTE_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlane8[index]);
          break;
        };
        case SBYTE_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlaneS8[index]);
          break;
        }
        case USHORT_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlaneU16[index]);
          break;
        }
        case SHORT_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlane16[index]);
          break;
        };
        case ULONG_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlaneU32[index]);
          break;
        }
        case LONG_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlane32[index]);
          break;
        };
        case LONGLONG_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlane64[index]);
          break;
        };
        case FLOAT_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlaneF[index]);
          break;
        };
        case DOUBLE_IMG:
        {
          returnValue = static_cast<FP_t>(imagePlaneD[index]);
          break;
        };
        default:
        {
          ERROR(ACL, 0x1203);  // CIMAGEPLANE: Invalid BITPIX value.
          break;
        };
      };

      return returnValue;
    };
  }

  /// @brief Mirrors the image vertically (about the X axis)
  /// @throws GCL::CError(ACL::0x1201) - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
  /// @throws GCL::CError(ACL::0x1203) - CIMAGEPLANE: Invalid BITPIX value.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-30/GGB - Added support for native FITS types
  /// @version 2010-12-28/GGB - Function created.

  void CImagePlane::mirrorAxisX()
  {
    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        mirrorAxisX(imagePlane8);
        break;
      };
      case SBYTE_IMG:
      {
        mirrorAxisX(imagePlaneS8);
        break;
      }
      case USHORT_IMG:
      {
        mirrorAxisX(imagePlaneU16);
        break;
      };
      case SHORT_IMG:
      {
        mirrorAxisX(imagePlane16);
        break;
      };
      case ULONG_IMG:
      {
        mirrorAxisX(imagePlaneU32);
        break;
      };
      case LONG_IMG:
      {
        mirrorAxisX(imagePlane32);
        break;
      };
      case LONGLONG_IMG:
      {
        mirrorAxisX(imagePlane64);
        break;
      };
      case FLOAT_IMG:
      {
        mirrorAxisX(imagePlaneF);
        break;
      };
      case DOUBLE_IMG:
      {
        mirrorAxisX(imagePlaneD);
        break;
      };
      default:
      {
        ERROR(ACL, 0x1203);  // CIMAGEPLANE: Invalid BITPIX value.
        break;
      };
    };
  }

  /// @brief Templated function to mirror an image around the X axis.
  /// @param[in] imagePlane: The imagePlane to mirror.
  /// @throws std::bad_alloc
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2015-09-20/GGB - (Bug 18) - Added input parameter verification code.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-27/GGB - Function created.

  template<typename T>
  void CImagePlane::mirrorAxisX(std::unique_ptr<T[]> &imagePlane)
  {
    RUNTIME_ASSERT(imagePlane, "Paramter imagePlane cannot be nullptr");

    std::unique_ptr<T[]> newImagePlane(new T[dimX * dimY]);
    size_t numberOfThreads;
    size_t threadNumber;
    AXIS_t colStep;
    boost::thread_group threadGroup;
    boost::thread *thread;
    AXIS_t colBegin, colEnd = 0;

      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows

    maxThreads = std::max(size_t(0), maxThreads);

    if ((dimX >=0) && (static_cast<size_t>(dimX) < maxThreads) )
    {
      numberOfThreads = static_cast<size_t>(dimX);
    }
    else
    {
      numberOfThreads = maxThreads;
    };

    colStep = dimX / numberOfThreads;

      // Spawn the threads.

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      colBegin = colEnd;
      if (threadNumber == (numberOfThreads -1) )
      {
        colEnd = dimX;
      }
      else
      {
        colEnd += colStep;
      };

      thread = new boost::thread(&CImagePlane::mirrorAxisXThread<T>, this, colBegin, colEnd, imagePlane.get(), newImagePlane.get());
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    imagePlane.swap(newImagePlane);
  }

  /// @brief Thread function to perform the mirror flip around the X axis.
  /// @param[in] rowBegin - The beginning row
  /// @param[in] rowEnd - The ending row
  /// @param[in] imagePlane - Pointer to the original image plane
  /// @param[in] newImagePlane - Pointer to the new image plane.
  /// @throws None.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-12-01/GGB - Function created.

  template<typename T>
  void CImagePlane::mirrorAxisXThread(AXIS_t colBegin, AXIS_t colEnd, T *imagePlane, T *newImagePlane)
  {
    AXIS_t indexX = colBegin;
    AXIS_t indexY;

    for (; indexX < colEnd; indexX++)
    {
      for (indexY = 0; indexY < dimY; indexY++)
      {
        newImagePlane[arrayIndex(indexX, indexY, dimX)] = imagePlane[arrayIndex(indexX, dimY - indexY - 1, dimX)];
      };
    };
  }

  /// @brief Procedure to float an image onto a larger canvas.
  /// @note The extra space is evenly added to the top/bottom and left/right of the image.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2011-06-04/GGB - Function created.

  void CImagePlane::floatImage(AXIS_t newWidth, AXIS_t newHeight, long newBkgnd)
  {
    RUNTIME_ASSERT(newWidth >= dimX, "Cannot have the x-dimension smaller when floating.");
    RUNTIME_ASSERT(newHeight >= dimY, "Cannot have the y-dimension smaller when floating.");

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        floatImage(imagePlane8, newWidth, newHeight, newBkgnd);
        break;
      };
      case SBYTE_IMG:
      {
        floatImage(imagePlaneS8, newWidth, newHeight, newBkgnd);
        break;
      };
      case USHORT_IMG:
      {
        floatImage(imagePlaneU16, newWidth, newHeight, newBkgnd);
        break;
      };
      case SHORT_IMG:
      {
        floatImage(imagePlane16, newWidth, newHeight, newBkgnd);
        break;
      };
      case ULONG_IMG:
      {
        floatImage(imagePlaneU32, newWidth, newHeight, newBkgnd);
        break;
      };
      case LONG_IMG:
      {
        floatImage(imagePlane32, newWidth, newHeight, newBkgnd);
        break;
      };
      case LONGLONG_IMG:
      {
        floatImage(imagePlane64, newWidth, newHeight, newBkgnd);
        break;
      };
      case FLOAT_IMG:
      {
        floatImage(imagePlaneF, newWidth, newHeight, newBkgnd);
        break;
      };
      case DOUBLE_IMG:
      {
        floatImage(imagePlaneD, newWidth, newHeight, newBkgnd);
        break;
      };
    };

    dimX = newWidth;
    dimY = newHeight;
    bMinMax = bMean = false;
  }

  /// @brief Internal float function
  /// @param[in] imagePlane: The image to float.
  /// @param[in] newWidth: The new width of the image.
  /// @param[in] newHeight: The new height of the image.
  /// @param[in] newBkgnd: The new background value.
  /// @throws
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-03-11/GGB - Function created.

  template<typename T>
  void CImagePlane::floatImage(std::unique_ptr<T[]> &imagePlane, AXIS_t newWidth, AXIS_t newHeight, long newBkgnd)
  {
    AXIS_t lox, loy, xdiff, ydiff;
    AXIS_t xIndex, yIndex;

    xdiff = newWidth - dimX;
    ydiff = newHeight - dimY;

    lox = xdiff / 2;
    loy = ydiff / 2;

    std::unique_ptr<T[]> newImagePlane(new T[newWidth * newHeight]);

    for (xIndex = 0; xIndex < newWidth; xIndex++)
    {
      for (yIndex = 0; yIndex < newHeight; yIndex++)
      {
        if ( (xIndex >= lox) && (xIndex < (dimX + lox)) && (yIndex >= loy) && (yIndex < (dimY + loy)) )
        {
          newImagePlane[arrayIndex(xIndex, yIndex, newWidth)] = imagePlane[arrayIndex(xIndex - lox, yIndex - loy, dimX)];
        }
        else
        {
          newImagePlane[arrayIndex(xIndex, yIndex, newWidth)] = newBkgnd;
        }
      };
    };

    imagePlane.swap(newImagePlane);
  }

  /// @brief Flops the image vertically (about the Y axis)
  /// @throws 0x1201 - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
  /// @throws 0x1203 - CIMAGEPLANE: Invalid BITPIX value.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-30/GGB - Added support for FITS native types.
  /// @version 2010-12-28/GGB - Function created.

  void CImagePlane::mirrorAxisY()
  {
    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        mirrorAxisY(imagePlane8);
        break;
      };
      case SBYTE_IMG:
      {
        mirrorAxisY(imagePlaneS8);
        break;
      };
      case USHORT_IMG:
      {
        mirrorAxisY(imagePlaneU16);
        break;
      };
      case SHORT_IMG:
      {
        mirrorAxisY(imagePlane16);
        break;
      };
      case ULONG_IMG:
      {
        mirrorAxisY(imagePlaneU32);
        break;
      };
      case LONG_IMG:
      {
        mirrorAxisY(imagePlane32);
        break;
      };
      case LONGLONG_IMG:
      {
        mirrorAxisY(imagePlane64);
        break;
      };
      case FLOAT_IMG:
      {
        mirrorAxisY(imagePlaneF);
        break;
      };
      case DOUBLE_IMG:
      {
        mirrorAxisY(imagePlaneD);
        break;
      };
      default:
      {
        ERROR(ACL, 0x1203);  // CIMAGEPLANE: Invalid BITPIX value.
        break;
      };
    };
  }

  /// @brief Templated function to mirror an image around the X axis.
  /// @param[in] imagePlane - The imagePlane to mirror.
  /// @returns The mirrored image plane.
  /// @throws std::bad_alloc
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-20/GGB - (Bug 19) Added check for valid parameters.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-12-01/GGB - Converted to multi-threaded
  /// @version 2012-11-30/GGB - Function created.

  template<typename T>
  void CImagePlane::mirrorAxisY(std::unique_ptr<T[]> &imagePlane)
  {
    RUNTIME_ASSERT(imagePlane, "Parameter imagePlane cannot be nullptr.");

    std::unique_ptr<T[]> newImagePlane(new T[dimX * dimY]);
    size_t numberOfThreads;
    size_t threadNumber;
    AXIS_t rowStep;
    boost::thread_group threadGroup;
    boost::thread *thread;
    AXIS_t rowBegin, rowEnd = 0;

      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows

    maxThreads = std::max(size_t(1), maxThreads);

    if ((dimY >= 0) && (static_cast<size_t>(dimY) < maxThreads))
    {
      numberOfThreads = static_cast<size_t>(dimY);
    }
    else
    {
      numberOfThreads = maxThreads;
    };

    rowStep = dimY / numberOfThreads;

      // Spawn the threads.

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      rowBegin = rowEnd;
      if (threadNumber == (numberOfThreads -1) )
      {
        rowEnd = dimY;
      }
      else
      {
        rowEnd += rowStep;
      };

      thread = new boost::thread(&CImagePlane::mirrorAxisYThread<T>, this, rowBegin, rowEnd, imagePlane.get(), newImagePlane.get());
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    imagePlane.swap(newImagePlane);
  }

  /// @brief Thread function for mirroring about the Y axis.
  /// @param[in] rowBegin - The beginning row
  /// @param[in] rowEnd - The ending row
  /// @param[in] imagePlane - Pointer to the original image plane
  /// @param[in] newImagePlane - Pointer to the new image plane.
  /// @throws None.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-12-01/GGB - Function created.

  template<typename T>
  void CImagePlane::mirrorAxisYThread(AXIS_t rowBegin, AXIS_t rowEnd, T *imagePlane, T *newImagePlane)
  {
    AXIS_t indexX;
    AXIS_t indexY = rowBegin;

    for (; indexY < rowEnd; indexY++)
    {
      for (indexX = 0; indexX < dimX; indexX++)
      {
        newImagePlane[arrayIndex(indexX, indexY, dimX)] = imagePlane[arrayIndex(dimX - indexX - 1, indexY, dimX)];
      };
    };
  }

  /// @brief This function takes a profile along the specified angle.
  /// @param[in] center - The center point to profile from
  /// @param[in] radius - The radius to profile to.
  /// @param[in] angle - The angle to profile along.
  /// @returns Array with the profile information.
  /// @throws
  /// @version 2012-10-11/GGB - Function created.

  boost::shared_array<FP_t> CImagePlane::objectProfile2D(MCL::TPoint2D<AXIS_t> const &center, AXIS_t radius, FP_t angle)
  {
    assert(radius > 0);

    INDEX_t length = static_cast<INDEX_t>(radius) * 2;
    INDEX_t counter;
    boost::shared_array<double> profile(new double[length]);
    FP_t dx = cos(angle * D_D2R);
    FP_t dy = sin(angle * D_D2R);
    FP_t xv = center.x() - radius * dx;
    FP_t yv = center.y() - radius * dy;
    AXIS_t xi, yi;

    for (counter = 0; counter <= length; counter++)
    {
      xi = (AXIS_t) xv;
      yi = (AXIS_t) yv;

      if ( (xi >= 0) && (xi < dimX) &&
           (yi >= 0) && (yi < dimY) )
      {
        profile[counter] = (getValue(xi, yi));
      };

      xv += dx;
      yv += dy;
    };

    return profile;
  }

  /// @brief Resample the image to the new width/height. Resampling changes the underlying data type to double in all circumstances.
  /// @param[in] newWidth - The new width of the image.
  /// @param[in] newHeight - The new height of the image.
  /// @throws
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-07-02/GGB - Added code to invalidate the Min,Max,Mean values. (Bug #1193741)
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-01-27/GGB - Added support for multi-threading and support for native FITS data types.
  /// @version 2011-03-07/GGB - Function created.

  void CImagePlane::resample(AXIS_t const &newWidth, AXIS_t const &newHeight)
  {
    FP_t xRatio = static_cast<FP_t>(newWidth) / static_cast<FP_t>(dimX);
    FP_t yRatio = static_cast<FP_t>(newHeight) / static_cast<FP_t>(dimY);
    AXIS_t yStep, yBegin, yEnd;
    std::unique_ptr<double[]> newImagePlane(new double[newWidth * newHeight]);
    size_t numberOfThreads;
    size_t threadNumber;
    boost::thread_group threadGroup;
    boost::thread *thread;

      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows
      // in the image.

    maxThreads = std::max(size_t(1), maxThreads);

    if (maxThreads > static_cast<size_t>(newHeight))
    {
      numberOfThreads = newHeight;
    }
    else
    {
      numberOfThreads = maxThreads;
    };

    yStep = newHeight / numberOfThreads;  // Size of each portion for threading.

       // Spawn the threads.

    yEnd = 0;

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      yBegin = yEnd;
      if (threadNumber == (numberOfThreads -1) )
      {
        yEnd = newHeight;
      }
      else
      {
        yEnd += yStep;
      };

      thread = new boost::thread(&CImagePlane::resampleThread, this, newImagePlane.get(), std::tuple<AXIS_t, AXIS_t>(yBegin, yEnd),
                                 newWidth, std::tuple<FP_t, FP_t>(xRatio, yRatio));
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    deleteImagePlane();
    bitpix_ = DOUBLE_IMG;
    imagePlaneD.swap(newImagePlane);		// Copy the new image plane.

    dimX = newWidth;
    dimY = newHeight;

    bMinMax = false;    // Min & Max values are not valid.
    bMean = false;       // Mean value is not correct.
  }

  /// @brief Resamples the image. This is the threaded procedure.
  //
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-01-27/GGB - Function created.

  void CImagePlane::resampleThread(double *newImagePlane, std::tuple<AXIS_t, AXIS_t> yVals, AXIS_t newWidth,
                                   std::tuple<FP_t, FP_t> ratios)
  {
    AXIS_t xp, yp;
    FP_t xVal, yVal;
    FP_t xFP, yFP;
    FP_t xIP, yIP;
    FP_t a, b, c, d;

    for(xp = 0; xp < newWidth; xp++)
    {
      for(yp = std::get<0>(yVals); yp < std::get<1>(yVals); yp++)
      {
        xVal = xp / std::get<0>(ratios);
        yVal = yp / std::get<1>(ratios);
        xFP = modf(xVal, &xIP);
        yFP = modf(yVal, &yIP);
        a = getValue(static_cast<long>(xIP), static_cast<AXIS_t>(yIP) );
        if (yIP >= (dimY-1))
        {
          b = 0;
        }
        else
        {
          b = getValue(static_cast<AXIS_t>(xIP), static_cast<AXIS_t>(yIP) + 1);
        }
        if (xIP >= (dimX-1))
        {
          c = 0;
        }
        else
        {
          c = getValue(static_cast<AXIS_t>(xIP) + 1 , static_cast<AXIS_t>(yIP));
        }
        if ( (xIP >= (dimX-1)) || (yIP >= (dimY-1)) )
        {
          d = 0;
        }
        else
        {
          d = getValue(static_cast<AXIS_t>(xIP) + 1, static_cast<AXIS_t>(yIP) + 1);
        }
        newImagePlane[arrayIndex(xp, yp, newWidth)] = a * (1-xFP) * (1-yFP) + b * (1-xFP) * yFP + c * xFP * (1-yFP) +
              d * xFP * yFP;
      };
    };
  }

  void CImagePlane::scale(FP_t newScale, MCL::TPoint2D<FP_t> const &)
  {
  }

  /// @brief Compute the fine location of the star peaking at [x0,y0]
  /// @note Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  //
  /// @version 2014-01-31/GGB - Ported to C++ and moved to findstar.cpp
  /// @version 2012-07-20/GGB - Function converted to C++.

  MCL::TPoint2D<FP_t> CImagePlane::starCentroid(MCL::TPoint2D<AXIS_t> const &guess) const
  {
    FP_t p1, p2, p22, p3, d;
    MCL::TPoint2D<FP_t> returnValue;

    /* Find maximum of best-fit parabola in each direction. see Bevington, page 210 */

    p1 = getValue(guess.x() - 1, guess.y());
    p2 = getValue(guess.x() , guess.y());
    p22 = 2*p2;
    p3 = getValue(guess.x() + 1, guess.y());
    d = p3 - p22 + p1;
    returnValue.x() = (d == 0) ? guess.x() : guess.x() + 0.5 - (p3 - p2)/d;
    returnValue.x() += 1.0;

    p1 = getValue(guess.x(), guess.y() - 1);
    p3 = getValue(guess.x(), guess.y() + 1);
    d = p3 - p22 + p1;
    returnValue.y() = (d == 0) ? guess.y() : guess.y() + 0.5 - (p3 - p2)/d;
    returnValue.y() += 1.0;

    return returnValue;
  }

  /// @brief Performs an image transformation (rotation, scaling and translation)
  /// @param[in] co - Rotation reference point.
  /// @param[in] ct - Image translation amount.
  /// @param[in] th - Image rotation amound (radians)
  /// @param[in] sc - Image scale amount
  /// @param[in] pix - Pixel sizes
  /// @param[in] maskPlane - The mask plane for the image (points set to true if not im image.) (May be nullptr)
  /// @details This function manages the startup of the thread functions. (uMaxThreads gives the maximum number of threads.)
  ///          The preliminary work and the close-out work is not multi-threaded. The code to perform the translation itself is
  ///          multi-threaded with a number of rows being passed to the tranformThread routine.
  /// @note The function does not expand the image while performing the TRS operation.
  /// @throws None.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-07-08/GGB - Invalidate the average and min/max. (Bug #1193634)
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-01-27/GGB - Function created.

  void CImagePlane::transform(MCL::TPoint2D<FP_t> const &c0, MCL::TPoint2D<FP_t> const &ct, FP_t th, FP_t sc,
                              MCL::TPoint2D<FP_t> const &pix,
                              std::unique_ptr<bool[]> &maskPlane)
  {
    AXIS_t yBegin = 0, yEnd = 0;
    size_t numberOfThreads;
    size_t threadNumber;
    boost::thread_group threadGroup;
    boost::thread *thread;
    SThreadData transformData(maskPlane);

    transformData.ct = ct;
    transformData.c0 = c0;
    transformData.th = th;
    transformData.sc = sc;
    transformData.pix = pix;

      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows
      // in the image.

    maxThreads = std::max(size_t(1), maxThreads);

    if (maxThreads > static_cast<INDEX_t>(dimY))
    {
      numberOfThreads = dimY;
    }
    else
    {
      numberOfThreads = maxThreads;
    };

    AXIS_t const yStep = dimY / numberOfThreads;  // Size of each portion for threading.

    transformData.newImagePlane = std::make_unique<double[]>(dimX * dimY);

      // Spawn the threads.

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      yBegin = yEnd;
      if (threadNumber == (numberOfThreads -1) )
      {
        yEnd = dimY;
      }
      else
      {
        yEnd += yStep;
      };
      thread = new boost::thread(&CImagePlane::transformThread, this, yBegin, yEnd, boost::cref(transformData));
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

      // Complete the function

    deleteImagePlane();   // Delete the current image plane.
    imagePlaneD.swap(transformData.newImagePlane);

    bitpix_ = DOUBLE_IMG;    // Using the double image plane now.
    bMinMax = bMean = false;
  }

  /// @brief Thread function to perform a transform. Called from the transform function.
  /// @param[in] yStart - The line to start on
  /// @param[in] yEnd - The line to end on
  /// @param,[in] transformData - The fixed data for the transformation.
  /// @details equation for translation/rotation/scaling is:
  ///                                                         x' = [xt + (x-x0-xt)costh - (y-y0-yt)sinth]s
  ///                                                         y' = [yt + (x-x0-xt)costh + (y-y0-yt)sinth]s
  /// @throws None
  /// @version 2017-09-02/GGB - Added support for image masking.
  /// @version 2012-11-29/GGB - Function created.

  void CImagePlane::transformThread(AXIS_t yStart, AXIS_t yEnd, SThreadData const &transformData)
  {
    FP_t const sinth = std::sin(transformData.th);
    FP_t const costh = std::cos(transformData.th);
    AXIS_t xp, yp;
    AXIS_t xtemp, ytemp, xmm, ymm, x, y;
    FP_t a, b, c, d;
    FP_t xf, yf;

      // Main loop

    for (yp = yStart; yp < yEnd; yp++)
    {
      for (xp =  0; xp < dimX; xp++)
      {
        x = xp - transformData.c0.x();
        y = yp - transformData.c0.y();

        x -= transformData.ct.x();
        y -= transformData.ct.y();

        xmm = x * transformData.pix.x();
        ymm = y * transformData.pix.y();
        xtemp = ((xmm * costh) - (ymm * sinth)) / transformData.sc;
        ytemp = ((xmm * sinth) + (ymm * costh)) / transformData.sc;

        x = transformData.c0.x() + xtemp / transformData.pix.x();
        y = transformData.c0.y() + ytemp / transformData.pix.y();

        if ( (MCL::INT(x) < 0) || (MCL::INT(x) >= (dimX - 1)) || (MCL::INT(y) < 0) || (MCL::INT(y) >= (dimY - 1)) )
        {
            // Zero the pixel as it lies outside the original image.

          transformData.newImagePlane[arrayIndex(xp, yp, dimX)] = 0;
          if (transformData.maskPlane)
          {
            transformData.maskPlane[arrayIndex(xp, yp, dimX)] = true;
          };
        }
        else
        {
          xf = x - MCL::INT(x);
          yf = y - MCL::INT(y);

          a = getValue(MCL::INT(x), MCL::INT(y));
          b = getValue(MCL::INT(x), MCL::INT(y)+1);
          c = getValue(MCL::INT(x)+1, MCL::INT(y));
          d = getValue(MCL::INT(x)+1, MCL::INT(y)+1);

          transformData.newImagePlane[arrayIndex(xp, yp, dimX)] = a * (1-xf) * (1-yf) +
                                                                  b * (1-xf) * yf +
                                                                  c * xf * (1-yf) +
                                                                  d * xf * yf;
        };
      };
    };
  }

  /// @brief  Translates an image to the new position.
  /// @details This is the integer translation form. There is also a float translation function However, this is optimised for
  ///          integer translations.
  /// @param[in] xt: amount to translate in the x dim (can be negative)
  /// @param[in] yt: amount to translate in the y dim (can be negative)
  /// @throws
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2014-12-31/GGB - Changed types to make x64 compliant.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2010-12-28/GGB - Function created.

  void CImagePlane::translate(AXIS_t const xt, AXIS_t const yt)
  {
    AXIS_t const newDimX = dimX + xt;
    AXIS_t const newDimY = dimY + yt;
    std::unique_ptr<double[]> newImagePlane(new double[newDimX * newDimY]);
    AXIS_t xIndex, yIndex;
    AXIS_t XO, YO;

    if ( (xt != 0) && (yt != 0) )		// If both values are zero, this is a trivial case.
    {
      for (xIndex = 0; xIndex < newDimX; xIndex++)
      {
        for (yIndex = 0; yIndex < newDimY; yIndex++)
        {
          XO = xIndex - xt;				// Correct the indexes to the old array indexes
          YO = yIndex - yt;

          if ( (XO < 0) || (YO < 0) || (XO >= dimX) || (YO >= dimY) )
          {
            newImagePlane[arrayIndex(xIndex, yIndex, newDimX)] = 0;			// Outside of original array.
          }
          else
          {
            newImagePlane[arrayIndex(xIndex, yIndex, newDimX)] = imagePlaneD[arrayIndex(XO, YO, dimX)];
          };
        };
      };

      deleteImagePlane();   // Delete the current image plane.
      imagePlaneD.swap(newImagePlane);
      bitpix_ = DOUBLE_IMG;
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Translation function using double values for the distance to be translated.
  /// @param[in]
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2011-03-13/GGB - Uses SCL::CArrray2DT
  /// @version 2010-12-29/GGB - Function created.

  void CImagePlane::translate(FP_t xt, FP_t yt)
  {
    AXIS_t xIndex, yIndex;
    AXIS_t const newDimX = static_cast<AXIS_t>(std::ceil(dimX + xt));
    AXIS_t const newDimY = static_cast<AXIS_t>(std::ceil(dimY + yt));
    FP_t XO, YO;
    std::unique_ptr<double[]> newImagePlane(new double[newDimX * newDimY]);

    if ( (xt != 0) && (yt != 0) )		// If both values are zero, this is a trivial case.
    {
      for (xIndex = 0; xIndex < newDimX; xIndex++)
      {
        for (yIndex = 0; yIndex < newDimY; yIndex++)
        {
          XO = xIndex - xt;				// Correct the indexes to the old array indexes
          YO = yIndex - yt;

          if ( (XO < 0) || (YO < 0) || (XO >= dimX) || (YO >= dimY) )
          {
            newImagePlane[arrayIndex(xIndex, yIndex, newDimX)] = 0;			// Outside of original array.
          }
          else
          {
            newImagePlane[arrayIndex(xIndex, yIndex, newDimX)] = bilinear(XO, YO);
          }
        };
      };

      deleteImagePlane();   // Delete the current image plane.
      bitpix_ = DOUBLE_IMG;
      imagePlaneD.swap(newImagePlane);
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Returns the maximum value on the array.
  /// @details If the maximum needs to be rechecked, the min and max are done at the same time.
  /// @throws 0x1200 -
  /// @throws 0x1203 - Invalid BITPIX value.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2012-11-30/GGB - Added support for native FITS types
  /// @version 2010-10-16/GGB - Function created.

  double CImagePlane::getMaxValue() const
  {
    if (!bMinMax)
    {
      switch (bitpix_)
      {
        case BYTE_IMG:
        {
          std::optional<std::tuple<std::uint8_t, std::uint8_t> > returnValue = MCL::minmax(imagePlane8.get(), dimX * dimY);
          if (returnValue)
          {
            fMin = std::get<0>(*returnValue);
            fMax = std::get<1>(*returnValue);
          };
         break;
        };
        case SBYTE_IMG:
        {
          std::optional<std::tuple<std::int8_t, std::int8_t> > returnValue = MCL::minmax(imagePlaneS8.get(), dimX * dimY);
          if (returnValue)
          {
            fMin = std::get<0>(*returnValue);
            fMax = std::get<1>(*returnValue);
          };
         break;
        };
        case USHORT_IMG:
        {
          std::optional<std::tuple<std::uint16_t, std::uint16_t> > returnValue = MCL::minmax(imagePlaneU16.get(), dimX * dimY);
          if (returnValue)
          {
            fMin = std::get<0>(*returnValue);
            fMax = std::get<1>(*returnValue);
          };
          break;
        };
      case SHORT_IMG:
      {
        std::optional<std::tuple<std::int16_t, std::int16_t> > returnValue = MCL::minmax(imagePlane16.get(), dimX * dimY);
        if (returnValue)
        {
          fMin = std::get<0>(*returnValue);
          fMax = std::get<1>(*returnValue);
        };
        break;
      };
        case ULONG_IMG:
        {
          std::optional<std::tuple<boost::uint32_t, boost::uint32_t> > returnValue = MCL::minmax(imagePlaneU32.get(), dimX * dimY);
          if (returnValue)
          {
            fMin = std::get<0>(*returnValue);
            fMax = std::get<1>(*returnValue);
          };
          break;
        };
      case LONG_IMG:
      {
        std::optional<std::tuple<boost::int32_t, boost::int32_t> > returnValue = MCL::minmax(imagePlane32.get(), dimX * dimY);
        if (returnValue)
        {
          fMin = std::get<0>(*returnValue);
          fMax = std::get<1>(*returnValue);
        };
        break;
      };

      case LONGLONG_IMG:
      {
        std::optional<std::tuple<std::int64_t, std::int64_t> > returnValue = MCL::minmax(imagePlane64.get(), dimX * dimY);
        if (returnValue)
        {
          fMin = std::get<0>(*returnValue);
          fMax = std::get<1>(*returnValue);
        };
      };
        break;
      case FLOAT_IMG:
      {
        std::optional<std::tuple<float, float> > returnValue = MCL::minmax(imagePlaneF.get(), dimX * dimY);
        if (returnValue)
        {
          fMin = std::get<0>(*returnValue);
          fMax = std::get<1>(*returnValue);
        };
      };
        break;
      case DOUBLE_IMG:
      {
        std::optional<std::tuple<double, double> > returnValue = MCL::minmax(imagePlaneD.get(), dimX * dimY);
        if (returnValue)
        {
          fMin = std::get<0>(*returnValue);
          fMax = std::get<1>(*returnValue);
        };
      };
        break;
      default:
        ACL_ERROR(0x1203);  // CIMAGEPLANE: Invalid BITPIX value.
        break;
      };

      bMinMax = true;
    };

    return fMax;
  }

  /// @brief Returns the minimum value on the array.
  /// @returns The minimum value in the image plane.
  /// @throws None.
  /// @note Calls getMaxValue() to perform the calculation.
  /// @version 2015-09-02/GGB - Function calls getMaxValue().
  /// @version 2012-11-30/GGB - Added support for native FITS types
  /// @version 2010-10-16/GGB - Function created.

  FP_t CImagePlane::getMinValue() const
  {
    if (!bMinMax)
    {
      getMaxValue();
    };
    return fMin;
  }

  /// @brief Returns the mean of the values in the imagePlane.
  /// @throws 0x1200 - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
  /// @throws 0x1201 - CIMAGEPLANE: Error when calculating image mean.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-22/GGB - Support for pre-stored values.
  /// @version 2012-11-29/GGB
  /// @li Support for native data types
  /// @li Use multi-threaded MCL::mean() function.
  /// @version 2011-05-01/GGB - Function created.

  FP_t CImagePlane::getMeanValue() const
  {
    std::optional<FP_t> returnValue;

    if (bMean)
    {
      return fMean;
    }
    else
    {
      switch (bitpix_)
      {
        case BYTE_IMG:
        {
          returnValue = MCL::mean(imagePlane8.get(), dimX * dimY);
          break;
        };
        case SBYTE_IMG:
        {
          returnValue = MCL::mean(imagePlaneS8.get(), dimX * dimY);
          break;
        };
        case USHORT_IMG:
        {
          returnValue = MCL::mean(imagePlaneU16.get(), dimX * dimY);
          break;
        };
        case SHORT_IMG:
        {
          returnValue = MCL::mean(imagePlane16.get(), dimX * dimY);
          break;
        };
        case ULONG_IMG:
        {
          returnValue = MCL::mean(imagePlaneU16.get(), dimX * dimY);
          break;
        };
        case LONG_IMG:
        {
          returnValue = MCL::mean(imagePlane32.get(), dimX * dimY);
          break;
        };
        case LONGLONG_IMG:
        {
          returnValue = MCL::mean(imagePlane64.get(), dimX * dimY);
          break;
        };
        case FLOAT_IMG:
        {
          returnValue = MCL::mean(imagePlaneF.get(), dimX * dimY);
          break;
        };
        case DOUBLE_IMG:
        {
          returnValue = MCL::mean(imagePlaneD.get(), dimX * dimY);
          break;
        };
        default:
        {
          ERROR(ACL, 0x1200); // CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
          break;
        };
      };

      if (returnValue)
      {
        fMean = *returnValue;

        bMean = true;
        return fMean;
      }
      else
      {
        bMean = false;
        ACL_ERROR(0x1201);  // CIMAGEPLANE: Error when calculating image mean.
      };
    };
  }

  /// @brief Returns the standard deviation of the values in the imagePlane.
  /// @throws 0x1203 - IMAGEPLANE: Invalid BITPIX value.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-21/GGB - Converted to use native data types and MCL::stdev rather than gsl_stdev.
  /// @version 2011-08-26/GGB - Function created.

  FP_t CImagePlane::getStDevValue() const
  {
    FP_t returnValue;

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        returnValue = *MCL::stdev(imagePlane8.get(), dimX * dimY);
        break;
      };
      case SBYTE_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneS8.get(), dimX * dimY);
        break;
      };
      case USHORT_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneU16.get(), dimX * dimY);
        break;
      };
      case SHORT_IMG:
      {
        returnValue = *MCL::stdev(imagePlane16.get(), dimX * dimY);
        break;
      };
      case ULONG_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneU32.get(), dimX * dimY);
        break;
      };
      case LONG_IMG:
      {
        returnValue = *MCL::stdev(imagePlane32.get(), dimX * dimY);
        break;
      };
      case LONGLONG_IMG:
      {
        returnValue = *MCL::stdev(imagePlane64.get(), dimX * dimY);
        break;
      };
      case FLOAT_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneF.get(), dimX * dimY);
        break;
      };
      case DOUBLE_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneD.get(), dimX * dimY);
        break;
      };
      default:
      {
        ERROR(ACL, 0x1203);    // IMAGEPLANE: Invalid BITPIX value.
        break;
      };
    };

    return returnValue;
  }

  /// @brief Determines the distance and value of each point from the centroid.
  /// @param[in] centroid - The centroid of the object
  /// @param[in] radius - The radius to centroid to.
  /// @param[out] data - The output data.
  /// @version 2013-05-18/GGB - Function created.

  void CImagePlane::objectProfile(MCL::TPoint2D<FP_t> centroid, AXIS_t radius, std::vector<std::tuple<FP_t, FP_t> > &data) const
  {
    AXIS_t minX = static_cast<AXIS_t>(centroid.x()) - radius;
    AXIS_t maxX = static_cast<AXIS_t>(centroid.x()) + radius;
    AXIS_t minY = static_cast<AXIS_t>(centroid.y()) - radius;
    AXIS_t maxY = static_cast<AXIS_t>(centroid.y()) + radius;
    FP_t r_2 = static_cast<FP_t>(MCL::pow2(radius));
    AXIS_t indexX, indexY;

    data.clear();

      // Perform some range checks.

    minX = std::max(AXIS_t(0), minX);
    minY = std::max(AXIS_t(0), minY);

    maxX = std::min(maxX, dimX);
    maxY = std::min(maxY, dimY);

    for (indexY = minY; indexY <= maxY; ++indexY)
    {
      for (indexX = minX; indexX <= maxX; ++indexX)
      {
        FP_t dist = MCL::pow2(static_cast<FP_t>(indexX) - centroid.x()) + MCL::pow2(static_cast<FP_t>(indexY) - centroid.y());
        if (dist <= r_2)
        {
          data.push_back(std::tuple<FP_t, FP_t>(std::sqrt(dist), getValue(indexX, indexY)));
        };
      };
    };
  }

  /// @brief Read data from a FITS file.
  /// @param[in] file - The FITS file to read from.
  /// @param[in] axis - The NAXIS3 value to read. (Ie the colour plane)
  /// @throws GCL::CError(ACL::0x1002)
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2017-09-20/GGB - Added try...catch for BSCALE and BZERO keywords.
  /// @version 2015-09-02/GGB - Function created.

  void CImagePlane::readFromFITS(fitsfile *file, AXIS_t axis)
  {
    RUNTIME_ASSERT(axis >= 1, "Parameter axis must be >= 1");
    RUNTIME_ASSERT(axis <= 999, "Parameter axis must be <= 999");
    RUNTIME_ASSERT(file != nullptr, "Parameter file cannot be nullptr");

    CFITSIO_TEST(fits_get_img_equivtype, file, &bitpix_);

      // These keywords are not mandatory.

    try
    {
      CFITSIO_TEST(fits_read_key, file, TDOUBLE, FITS_BSCALE.c_str(), &bscale_, nullptr);
    }
    catch(...)
    {
    };

    try
    {
      CFITSIO_TEST(fits_read_key, file, TDOUBLE, FITS_BZERO.c_str(), &bzero_, nullptr);
    }
    catch(...)
    {
    };

    {
      long naxis[2];
      CFITSIO_TEST(fits_get_img_size, file, 2, naxis);

      dimX = static_cast<AXIS_t>(naxis[0]);
      dimY = static_cast<AXIS_t>(naxis[1]);
    };

    LONGLONG startPixel = (dimX * dimY * (axis - 1)) + 1;
    LONGLONG readPixels = dimX * dimY;
    int anynul;

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        imagePlane8 = std::make_unique<std::uint8_t[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TBYTE, startPixel, readPixels, 0, imagePlane8.get(), &anynul);
        break;
      };
      case SBYTE_IMG:
      {
        imagePlaneS8 = std::make_unique<std::int8_t[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TSBYTE, startPixel, readPixels, 0, imagePlaneS8.get(), &anynul);
        break;
      }
      case USHORT_IMG:
      {
        imagePlaneU16 = std::make_unique<std::uint16_t[]>(readPixels);
        CFITSIO_TEST(fits_read_img, file, TUSHORT, startPixel, readPixels, 0, imagePlaneU16.get(), &anynul);
        break;
      }
      case SHORT_IMG:
      {
        imagePlane16 = std::make_unique<std::int16_t[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TSHORT, startPixel, readPixels, 0, imagePlane16.get(), &anynul);
        break;
      }
      case ULONG_IMG:
      {
        imagePlaneU32 = std::make_unique<std::uint32_t[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TULONG, startPixel, readPixels, 0, imagePlaneU32.get(), &anynul);
        break;
      };
      case LONG_IMG:
      {
        imagePlane32 = std::make_unique<std::int32_t[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TLONG, startPixel, readPixels, 0, imagePlane32.get(), &anynul);
        break;
      }
      case LONGLONG_IMG:
      {
        imagePlane64 = std::make_unique<std::int64_t[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TLONGLONG, startPixel, readPixels, 0, imagePlane64.get(), &anynul);
        break;
      }
      case FLOAT_IMG:
      {
        imagePlaneF = std::make_unique<float[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TFLOAT, startPixel, readPixels, 0, imagePlaneF.get(), &anynul);
        break;
      }
      case DOUBLE_IMG:
      {
        imagePlaneD = std::make_unique<double[]>(dimX * dimY);
        CFITSIO_TEST(fits_read_img, file, TDOUBLE, startPixel, readPixels, 0, imagePlaneD.get(), &anynul);
        break;
      }
      default:
      {
        ACL_ERROR(0x1002);  // CCFITS: Invalid BITPIX value.
        break;
      };
    };
    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Renders the image as an 8 bit greyscale image.
  /// @param[in] outputImage: Reference to the array that has been allocated for the output image.
  /// @throws GCL::CodeError(ACL)
  /// @throws std::bad_alloc
  /// @note Uses multi-threading for speedup.
  /// @note The function assumes that the output array has been allocated and is the correct size.
  /// @version 2015-06-30/GGB - Modified to use an externally supplied image array. (Bug 6)
  /// @version 2013-05-26/GGB - Added functionality for using different transfer functions.
  /// @version 2011-08-28/GGB - This function now serves as the kick-off routine for the multi-threading. All threads are started
  /// from this routine.
  /// @version 2011-05-29/GGB
  ///   @li Store values for the blackPoint and whitePoint in the class.
  ///   @li If blackPoint == whitePoint, the output value is set to zero.
  ///   @li Store the invert value
  /// @version 2011-03-07/GGB - Added parameter bInvert to header.
  /// @version 2010-10-17/GGB - Function created.

  void CImagePlane::renderImageGrey8(renderImage_t *outputImage)
  {
    size_t numberOfThreads;
    size_t threadIndex;
    boost::thread_group threadGroup;
    boost::thread *thread;
    INDEX_t indexBegin = 0, indexEnd = 0;
    INDEX_t indexStep;

    if ( (dimY >= 0) && (static_cast<size_t>(dimY) < maxThreads))
    {
      numberOfThreads = dimY;
    }
    else
    {
      numberOfThreads = maxThreads;
    };

    indexStep = (dimY * dimX) / numberOfThreads;

    for (threadIndex = 0; threadIndex < numberOfThreads; threadIndex++)
    {
      indexBegin = indexEnd;
      if (threadIndex == numberOfThreads - 1)
      {
        indexEnd = dimY * dimX - 1;
      }
      else
      {
        indexEnd += indexStep;
      };

      switch (transferFunction)
      {
        case ETF_NONE:
        {
          CODE_ERROR;
          break;
        };
        case ETF_LINEAR:
        {
          thread = new boost::thread(&CImagePlane::renderImageGrey8LinearThread, this, outputImage,
            std::make_tuple(indexBegin, indexEnd));
          break;
        };
      case ETF_GAMMA:
        thread = new boost::thread(&CImagePlane::renderImageGrey8GammaThread, this, outputImage,
          std::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_GAMMALOG:
      case ETF_CBRT:
        thread = new boost::thread(&CImagePlane::renderImageGrey8CbrtThread, this, outputImage,
          std::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_SQRT:
        thread = new boost::thread(&CImagePlane::renderImageGrey8SqrtThread, this, outputImage,
          std::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_SIGMOID:
        thread = new boost::thread(&CImagePlane::renderImageGrey8SigmoidThread, this, outputImage,
          std::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_SQUARE:
        thread = new boost::thread(&CImagePlane::renderImageGrey8SquareThread, this, outputImage,
          std::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_LOG:
        thread = new boost::thread(&CImagePlane::renderImageGrey8LogThread, this, outputImage,
          std::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      default:
        {
          CODE_ERROR;
          break;
        }
      };
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();
  }

  /// @brief Thread routine for rendering an image with a cuberoottransfer function.
  /// @details Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  /// f(p) = pmax*((p-pblack)/(pwhite-pblack))^(1/3)
  /// @param[out] oi: The array that the data will be written to
  /// @param[in] startEnd: The start and end index values that will be processed by this thread.
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8CbrtThread(renderImage_t *oi, std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;
    FP_t exponent = 1/3;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();

      // Typecast the output array to the correct type.

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    const FP_t fRange = whitePoint - blackPoint;

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);          // This is corrected for BZERO and BSCALE

      if (fRange == 0)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue <= blackPoint)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue >= whitePoint)
      {
        outputImage[lIndex] = maximumImageValue;
      }
      else
      {
        currentValue = std::pow(((currentValue - blackPoint) / fRange), exponent) * maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>(floor(currentValue + 0.5));
      };

      if (invert)
      {
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
      }
    };
  }

  /// Thread routine for rendering an image with a gamma transfer function.
  /// Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  /// f(p) = pmax*((p-pblack)/(pwhite-pblack))^(1/gamma)
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8GammaThread(renderImage_t *oi,
                                                 std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t const exponent = 1 / transferFunctionParameter;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = whitePoint - blackPoint;

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);

      if (fRange == 0)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue <= blackPoint)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue >= whitePoint)
      {
        outputImage[lIndex] = maximumImageValue;
      }
      else
      {
        currentValue = std::pow((currentValue - blackPoint) / fRange, exponent) * maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>( floor(currentValue + 0.5));
      };

      if (invert)
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
    };
  }

  /// Thread routine for rendering an image with a gamma transfer function.
  /// Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  /// f(p) = pmax*((p-pblack)/(pwhite-pblack))^(1/gamma)
  ///
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8GammaLogThread(renderImage_t *oi,
                                                   std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t const exponent = 1 / transferFunctionParameter;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = std::log(whitePoint - blackPoint);

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);

      if (fRange == 0)
        outputImage[lIndex] = minimumImageValue;
      else if (currentValue <= blackPoint)
        outputImage[lIndex] = minimumImageValue;
      else if (currentValue >= whitePoint)
        outputImage[lIndex] = maximumImageValue;
      else
      {
        currentValue = std::pow(std::log(currentValue - blackPoint) / fRange, exponent) * maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>( floor(currentValue + 0.5));
      };

      if (invert)
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
    };
  }

  /// @brief Thread routine for the multi-threading calculation.
  /// @param[out] oi - Output image. The array to render the image into.
  /// @details Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  /// @version 2016-04-02/GGB - Updated an error with the logic for calculating the output value.
  /// @version 2015-06-30/GGB - Added extra parameter for the outputImage.
  /// @version 2013-05-25/GGB - Changed name to reflect the linear nature of this transform.
  /// @version 2011-08-28/GGB - Function created.

  void CImagePlane::renderImageGrey8LinearThread(renderImage_t *oi, std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;

    renderImageGrey8_t minimumImageValue = std::numeric_limits<renderImageGrey8_t>::min();
    renderImageGrey8_t maximumImageValue = std::numeric_limits<renderImageGrey8_t>::max();

    FP_t fRange = (maximumImageValue - minimumImageValue);
    fRange /= (whitePoint - blackPoint);

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);

      if (fRange == 0)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue <= blackPoint)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue >= whitePoint)
      {
        outputImage[lIndex] = maximumImageValue;
      }
      else
      {
        currentValue = ((currentValue - blackPoint) * fRange);
        outputImage[lIndex] = static_cast<renderImage_t>(std::floor(currentValue + 0.5));
      };

      if (invert)
      {
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
      };
    };
  }

  /// Thread routine for the multi-threading calculation.
  /// Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8LogThread(renderImage_t *oi, std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = std::log(whitePoint - blackPoint);
    renderImageGrey8_t minimumImageValue = std::numeric_limits<renderImageGrey8_t>::min();
    renderImageGrey8_t maximumImageValue = std::numeric_limits<renderImageGrey8_t>::max();

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);

      if (fRange == 0)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue <= blackPoint)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue >= whitePoint)
      {
        outputImage[lIndex] = maximumImageValue;
      }
      else
      {
        currentValue = (std::log(currentValue - blackPoint) / fRange) * maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>(floor(currentValue + 0.5));
      };

      if (invert)
      {
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
      };
    };
  }

  /// Thread routine for the multi-threading calculation.
  /// Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-28/GGB - Function created.

  void CImagePlane::renderImageGrey8SigmoidThread(renderImage_t *oi, std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImageGrey8_t>::max();
    FP_t currentValue;
    renderImageGrey8_t minimumImageValue = std::numeric_limits<renderImageGrey8_t>::min();
    renderImageGrey8_t maximumImageValue = std::numeric_limits<renderImageGrey8_t>::max();
    const FP_t fRange = whitePoint - blackPoint;

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);

      if (fRange == 0)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue <= blackPoint)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue >= whitePoint)
      {
        outputImage[lIndex] = maximumImageValue;
      }
      else
      {
        currentValue = (currentValue - blackPoint) / fRange;
        currentValue = 1 / ( 1 + std::exp(-currentValue));
        currentValue *= maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>(floor(currentValue + 0.5));
      };

      if (invert)
      {
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
      };
    };
  }

  /// @brief Thread routine for the multi-threading calculation.
  /// @param[out] outputImage - The output image array
  /// @throws None.
  /// Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8SqrtThread(renderImage_t *outputImage, std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t currentValue;
    const FP_t fRange = whitePoint - blackPoint;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);

      if (fRange == 0)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue <= blackPoint)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue >= whitePoint)
      {
        outputImage[lIndex] = maximumImageValue;
      }
      else
      {
        currentValue = std::sqrt((currentValue - blackPoint) / fRange) * maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>(floor(currentValue + 0.5));
      };

      if (invert)
      {
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
      };
    };
  }

  /// Thread routine for the multi-threading calculation.
  /// Each of the transfer functions has it's own multi-threading function. This has been done for speed as the overhead of
  /// performing additional function calls can mount up on large images. (On a 1MP image, 1µS/pixel = 1 second, on a 10MP image
  /// this becomes 10s.
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8SquareThread(renderImage_t *outputImage, std::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t currentValue;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = whitePoint - blackPoint;

    for(lIndex = std::get<0>(startEnd); lIndex < std::get<1>(startEnd); lIndex++)
    {
      currentValue = getValue(lIndex);

      if (fRange == 0)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue <= blackPoint)
      {
        outputImage[lIndex] = minimumImageValue;
      }
      else if (currentValue >= whitePoint)
      {
        outputImage[lIndex] = maximumImageValue;
      }
      else
      {
        currentValue = MCL::pow2((currentValue - blackPoint) / fRange) * maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>(floor(currentValue + 0.5));
      };

      if (invert)
      {
        outputImage[lIndex] = outputImage[lIndex] ^ maximumImageValue;
      }
    };
  }

  /// @brief Rotates the image around the center of the image.
  /// @param[in] angle - Angle to rotate in radians.
  /// @details Calls the more general function to ensure one method of use.
  /// @note Earlier implementations were calling this function to rotate around the origin. This has now been corrected.
  //
  // 2012-12-01/GGB - Rotate around the center of the image.
  // 2011-05-30/GGB - Function created.

  void CImagePlane::rotate(FP_t angle)
  {
    rotate(dimX / 2, dimY / 2, angle);
  }

  /// @brief Rotates the image around the point (x0, y0) an amount of angle radians.
  /// @throws CError ACL::0x1203
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @li Use std::tuple rather than boost::tuple.
  /// @li Bug 63 - use std::max() and std::min()
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-12-01/GGB - Modified to support native data types.
  /// @version 2011-05-30/GGB - Function created.

  void CImagePlane::rotate(FP_t xOrigen, FP_t yOrigen, FP_t angle)
  {
    FP_t const cosa = cos(angle);
    FP_t const sina = sin(angle);
    FP_t X0, Y0, X1, Y1, X2, Y2, X3, Y3;
    FP_t  xMax, yMax, xMin, yMin;
    AXIS_t lxMax, lxMin, lyMax, lyMin, lxDim, lyDim;

      // Calculate the new image size. Work using the 4 corners to get the 4 coordinates of the new corners.

      // (X0, Y0) = transform(0, 0)
      // (X1, Y1) = transform(DimX, 0)
      // (X2, Y2) = transform(0, DimY)
      // (X3, Y3) = transform(DimX, DimY)

    X0 = xOrigen + (0 - xOrigen) * cosa + (0 - yOrigen) * sina;
    Y0 = yOrigen - (0 - xOrigen) * sina + (0 - yOrigen) * cosa;

    X1 = xOrigen + (static_cast<FP_t>(dimX) - xOrigen) * cosa + (0 - yOrigen) * sina;
    Y1 = yOrigen - (static_cast<FP_t>(dimX) - xOrigen) * sina + (0 - yOrigen) * cosa;

    X2 = xOrigen + (0 - xOrigen) * cosa + (static_cast<FP_t>(dimY) - yOrigen) * sina;
    Y2 = yOrigen - (0 - xOrigen) * sina + (static_cast<FP_t>(dimY) - yOrigen) * cosa;

    X3 = xOrigen + (static_cast<FP_t>(dimX) - xOrigen) * cosa + (static_cast<FP_t>(dimY) - yOrigen) * sina;
    Y3 = yOrigen - (static_cast<FP_t>(dimX) - xOrigen) * sina + (static_cast<FP_t>(dimY) - yOrigen) * cosa;

      // Find the max and min points.

    xMax = xMin = X0;
    yMax = yMin = Y0;

    xMin = std::min(X0, X1);
    xMin = std::min(xMin, X2);
    xMin = std::min(xMin, X3);

    xMax = std::max(X0, X1);
    xMax = std::max(xMax, X2);
    xMax = std::max(xMax, X3);

    yMin = std::min(Y0, Y1);
    yMin = std::min(yMin, Y2);
    yMin = std::min(yMin, Y3);

    yMax = std::max(Y0, Y1);
    yMax = std::max(yMax, Y2);
    yMax = std::max(yMax, Y3);

      // Convert to the AXIS_t values.

    lxMin = static_cast<AXIS_t>(std::floor(xMin));
    lxMax = static_cast<AXIS_t>(std::ceil(xMax));
    lyMin = static_cast<AXIS_t>(std::floor(yMin));
    lyMax = static_cast<AXIS_t>(std::ceil(yMax));

    lxDim = lxMax - lxMin;
    lyDim = lyMax - lyMin;

      // Create the new Image

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        rotate(imagePlane8, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      case SBYTE_IMG:
      {
        rotate(imagePlaneS8, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      case USHORT_IMG:
      {
        rotate(imagePlaneU16, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      case SHORT_IMG:
      {
        rotate(imagePlane16, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      case ULONG_IMG:
      {
        rotate(imagePlaneU32, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      case LONG_IMG:
      {
        rotate(imagePlane32, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      case LONGLONG_IMG:
      {
        rotate(imagePlane64, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
     };
      case FLOAT_IMG:
      {
        rotate(imagePlaneF, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      case DOUBLE_IMG:
      {
        rotate(imagePlaneD, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        break;
      };
      default:
      {
        ERROR(ACL, 0x1203);  // CIMAGEPLANE: Invalid BITPIX value.
        break;
      };
    };

    dimX = lxDim;
    dimY = lyDim;
    bMinMax = bMean = false;   // Min max values not valid.
  }

  /// @brief Function for rotating an image. Uses native FITS formats.
  /// @param[in,out] imagePlane: THe image plane to rotate.
  /// @param[in] l: Rotation parameters.
  /// @param[in] xOrigin:
  /// @param[in] yOrigin:
  /// @param[in] cosa:
  /// @param[in] sina:
  /// @throws None.
  /// @version 2018-08-25/GGB - Changed storage type to std::unique_ptr<>.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-12-01/GGB - Function created.

  template<typename T>
  void CImagePlane::rotate(std::unique_ptr<T[]> &imagePlane,
                           std::tuple<AXIS_t, AXIS_t, AXIS_t, AXIS_t> l, FP_t xOrigin, FP_t yOrigin, FP_t cosa, FP_t sina)
  {
    size_t numberOfThreads;
    size_t threadNumber;
    AXIS_t rowStep;
    boost::thread_group threadGroup;
    boost::thread *thread;
    AXIS_t rowBegin, rowEnd = std::get<2>(l);
    AXIS_t dy = std::get<3>(l) - std::get<2>(l);
    AXIS_t dx = std::get<1>(l) - std::get<0>(l);
    std::unique_ptr<T[]> newImagePlane(new T[dx * dy]);

      // Ensure that we are using a reasonable number of threads. Maximise the number of threads to the number of rows

    maxThreads = std::max(size_t(1), maxThreads);

    if (maxThreads > static_cast<size_t>(dy) )
    {
      numberOfThreads = static_cast<size_t>(dy);
    }
    else
    {
      numberOfThreads = maxThreads;
    };

    rowStep = dy / numberOfThreads;

      // Spawn the threads.

    for (threadNumber = 0; threadNumber < numberOfThreads; threadNumber++)
    {
      rowBegin = rowEnd;
      if (threadNumber == (numberOfThreads -1) )
      {
        rowEnd = std::get<3>(l);
      }
      else
      {
        rowEnd += rowStep;
      };

      thread = new boost::thread(&CImagePlane::rotateThread<T>, this, std::make_tuple(rowBegin, rowEnd),
                                 imagePlane.get(), newImagePlane.get(), l, xOrigin, yOrigin, cosa, sina);
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    imagePlane.swap(newImagePlane);
  }

  /// @brief Thread function for rotating an image.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-12-01/GGB - Function created.

  template<typename T>
  void CImagePlane::rotateThread(std::tuple<AXIS_t , AXIS_t> row, T *imagePlane, T *newImagePlane,
                                 std::tuple<AXIS_t, AXIS_t, AXIS_t, AXIS_t> l, FP_t xOrigen, FP_t yOrigen, FP_t cosa, FP_t sina)
  {
    FP_t x, y, xf, yf, xn, yn;
    FP_t a, b, c, d;
    AXIS_t xIndex, yIndex;
    AXIS_t lxMin = std::get<0>(l);
    AXIS_t lxMax = std::get<1>(l);
    AXIS_t lyMin = std::get<2>(l);
    AXIS_t rowBegin = std::get<0>(row);
    AXIS_t rowEnd = std::get<1>(row);
    AXIS_t lxDim = lxMax - lxMin;

      // Rotate the image by doing the transform from the old to the new.

    for(yIndex = rowBegin; yIndex < rowEnd; yIndex++)
    {
      for(xIndex = lxMin; xIndex < lxMax; xIndex++)
      {
        x = xOrigen + (xIndex - xOrigen) * cosa - (yIndex - yOrigen) * sina;
        y = yOrigen + (xIndex - xOrigen) * sina + (yIndex - yOrigen) * cosa;

        xf = modf(x, &xn);
        yf = modf(y, &yn);


        if ( (static_cast<AXIS_t>(xn) < 0) || (static_cast<AXIS_t>(xn) >= (dimX - 1)) ||
             (static_cast<AXIS_t>(yn) < 0) || (static_cast<AXIS_t>(yn) >= (dimY - 1)) )
        {
          newImagePlane[arrayIndex(xIndex, yIndex, lxMin, lyMin, lxDim)] = 0;   // Outside of bounds of original image.
        }
        else
        {
          a = imagePlane[arrayIndex((AXIS_t) xn, (AXIS_t) yn, dimX)];
          b = imagePlane[arrayIndex((AXIS_t) xn, (AXIS_t) yn + 1, dimX)];
          c = imagePlane[arrayIndex((AXIS_t) xn + 1, (AXIS_t) yn, dimX)];
          d = imagePlane[arrayIndex((AXIS_t) xn + 1, (AXIS_t) yn + 1, dimX)];
          newImagePlane[arrayIndex(xIndex, yIndex, lxMin, lyMin, lxDim)] = a * (1-xf) * (1-yf) +
                                                                           b * (1-xf) * yf +
                                                                           c * xf * (1-yf) +
                                                                           d * xf * yf;
        };
      };
    };
  }

  /// @brief Function to set the colour paramters for the image plane.
  /// @details The colour parameters are only used when rendering the image in colour. If rendering in greyscale, then the
  /// parameters are not used.
  /// @param[in] colour - The colour parameters for the image plane.
  /// @param[in] transparency_ - The transparency of the image plane.
  /// @version 2015-07-04/GGB - Function created.

  void CImagePlane::setImagePlaneColourValues(SColourRGB colour, FP_t transparency_)
  {
    planeColour = colour;
    transparency = transparency_;
  }

  /// @brief Sets rendering function parameters.
  /// @param[in] bp - Black Point.
  /// @param[in] wp - White Point
  /// @param[in] invert - Invert image
  /// @param[in] tf - Transfer function
  /// @param[in] tfp - Transfer function parameter.
  /// @version 2015-07-04/GGB - Function created.

  void CImagePlane::setImagePlaneRenderFunction(FP_t bp, FP_t wp, bool invert_, ETransferFunction tf, FP_t tfp)
  {
    blackPoint = bp;
    whitePoint = wp;
    invert = invert_;
    transferFunction = tf;
    transferFunctionParameter = tfp;
  }

  /// @brief Function to write the image plane to a FITS HDU.
  /// @param[in] hdu: The CCfits HDU to write to.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @throws ACL::CFitsException()
  /// @version 2017-09-21/GGB - Added writing of BZERO and BSCALE (Bug #116)
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-14/GGB - Function created.

  void CImagePlane::writeToFITS(fitsfile *file, AXIS_t axis)
  {
    RUNTIME_ASSERT(axis >= 1, "Parameter axis must be >= 1. (" + std::to_string(axis) + ")");
    RUNTIME_ASSERT(axis <= 999, "Parameter axis must be <= 999. (" + std::to_string(axis) + ")");
    RUNTIME_ASSERT(file != nullptr, "Parameter file cannot be nullptr");

    INDEX_t startPixel = (axis - 1) * dimX * dimY + 1;
    INDEX_t pixelCount = dimX * dimY;
    int status = 0;

    CFITSIO_TEST(fits_write_key, file, TFLOAT, FITS_BSCALE.c_str(), &bscale_, FITS_COMMENT_BSCALE.c_str());
    CFITSIO_TEST(fits_write_key, file, TFLOAT, FITS_BZERO.c_str(), &bzero_, FITS_COMMENT_BZERO.c_str());

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TBYTE, startPixel, pixelCount, imagePlane8.get());
        break;
      };
      case SBYTE_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TSBYTE, startPixel, pixelCount, imagePlaneS8.get());
        break;
      };
      case USHORT_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TUSHORT, startPixel, pixelCount, imagePlaneU16.get());
        break;
      };
      case SHORT_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TSHORT, startPixel, pixelCount, imagePlane16.get());
        break;
      };
      case ULONG_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TULONG, startPixel, pixelCount, imagePlaneU32.get());
        break;
      };
      case LONG_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TLONG, startPixel, pixelCount, imagePlane32.get());
        break;
      };
      case LONGLONG_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TLONGLONG, startPixel, pixelCount, imagePlane64.get());
        break;
      };
      case FLOAT_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TFLOAT, startPixel, pixelCount, imagePlaneF.get());
        break;
      };
      case DOUBLE_IMG:
      {
        CFITSIO_TEST(fits_write_img, file, TDOUBLE, startPixel, pixelCount, imagePlaneD.get());
        break;
      };
      default:
      {
        CODE_ERROR;
        break;
      };
    };
  }

}  // namespace ACL
