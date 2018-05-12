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
// CLASSES INCLUDED:		CImagePlane					- A single plane image.
//
// CLASS HIERARCHY:     CImagePlane
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-04-14 GGB - Changed filename from CImagePlane.cpp to ImagePlane.cpp
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../Include/ImagePlane.h"

// Standard Libraries

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

// Boost library

#include "boost/thread/thread.hpp"
#include "boost/tuple/tuple.hpp"

  // ACL include files

#include "../Include/constants.h"
#include "../Include/findstar.h"
#include "../Include/FITSException.h"
#include "../Include/FITSStrings.h"
#include "../Include/FWHM.h"

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // General function - only used by this file, but not part of the class.
  //
  //*******************************************************************************************************************************

  /// @brief Thread function to perform the copy of the data.
  /// @param[in] srcData<0> -> sourceImage
  /// @param[in] srcData<1> -> startX
  /// @param[in] srcData<2> -> startY
  /// @param[in] srcData<3> - dimX
  /// @param[in] destData<0> -> destination image
  /// @param[in] destData<1> -> dimX
  /// @param[in] destData<2> -> rowBegin
  /// @param[in] destData<3> -> rowEnd
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

  /// @brief Copies part of an image plane from src to dest.
  /// @details This is a templated function that can copy any type of image plane. This function starts the thread functions that
  /// perform the actual copy. The work is divided evenly over the threads.<br>
  /// The types of the source and destination image planes must be the same.
  /// @param[in] srcData<0> imagePlane
  /// @param[in] srcData<1> startX
  /// @param[in] srcData<2> startY
  /// @param[in] srcData<3> dimX
  /// @param[in,out] destData<0> imagePlane
  /// @param[in] destData<1> dimX
  /// @param[in] destData<2> dimY
  /// @throws None.
  /// @version 2013-04-21/GGB - Function created.

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

  /// @brief Default constructor for the class.
  /// @throws None.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-27/GGB - Added support for native image planes and bitpix_
  /// @version 2010-10-16/GGB - Function created.

  CImagePlane::CImagePlane() : dimX(0), dimY(0), fMin(0), fMax(0), fMean(0), fStDev(0), bMinMax(false), bMean(false),
    bitpix_(0), bscale_(1), bzero_(0), pedestal_(0), imagePlane8(nullptr), imagePlaneS8(nullptr), imagePlaneU16(nullptr),
    imagePlane16(nullptr), imagePlaneU32(nullptr), imagePlane32(nullptr), imagePlane64(nullptr), imagePlaneF(nullptr),
    imagePlaneD(nullptr)

  {
  }

  /// @brief Copy constructor for the class.
  /// @details Uses the operator= function to perform the copy.
  /// @param[in] toCopy - The imagePlane to copy to this.
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
  /// @param[in] oldPlane The image plane to copy the data from.
  /// @param[in] xStart The starting x-value fo the sub image.
  /// @param[in] End The ending x-value of the sub image.
  /// @param[in] yStart The starting y-value for the sub image.
  /// @param[in] yEnd The ending y value for the sub image.
  /// @throws CError(ACL::0x1002) - CCFITS: Invalid BITPIX value.
  /// @throws std::bad_alloc
  /// @throws CRuntimeAssert.
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

    RUNTIME_ASSERT(ACL, oldPlane, "Parameter oldPlane == nullptr");
    RUNTIME_ASSERT(ACL, xStart >= 0, "Parameter xStart < 0");
    RUNTIME_ASSERT(ACL, xEnd >= 0, "Parameter xEnd < 0");
    RUNTIME_ASSERT(ACL, xStart < xEnd, "Paramter xStart >= xEnd");
    RUNTIME_ASSERT(ACL, yStart >= 0, "Parameter yStart < 0");
    RUNTIME_ASSERT(ACL, yEnd >= 0, "Parameter yEnd < 0");
    RUNTIME_ASSERT(ACL, yStart < yEnd, "Paramter yStart >= yEnd");

    bitpix_ = oldPlane->BITPIX();
    dimX = xEnd - xStart;
    dimY = yEnd - yStart;

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        imagePlane8 = new std::uint8_t[dimX * dimY];
        copyImagePlane(oldPlane->imagePlane8, imagePlane8);
        break;
      };
      case SBYTE_IMG:
      {
        imagePlaneS8 = new std::int8_t[dimX * dimY];
        copyImagePlane(oldPlane->imagePlaneS8, imagePlaneS8);
        break;
      };
      case USHORT_IMG:
      {
        imagePlaneU16 = new std::uint16_t[dimX * dimY];
        copyImagePlane(oldPlane->imagePlaneU16, imagePlaneU16);
        break;
      };
      case SHORT_IMG:
      {
        imagePlane16 = new std::int16_t[dimX * dimY];
        copyImagePlane(oldPlane->imagePlane16, imagePlane16);
        break;
      };
      case ULONG_IMG:
      {
        imagePlaneU32 = new std::uint32_t[dimX * dimY];
        copyImagePlane(oldPlane->imagePlaneU32, imagePlaneU32);
        break;
      };
      case LONG_IMG:
      {
        imagePlane32 = new std::int32_t[dimX * dimY];
        copyImagePlane(oldPlane->imagePlane32, imagePlane32);
        break;
      };
      case LONGLONG_IMG:
      {
        imagePlane64 = new std::int64_t[dimX * dimY];
        copyImagePlane(oldPlane->imagePlane64, imagePlane64);
        break;
      };
      case FLOAT_IMG:
      {
        imagePlaneF = new float[dimX * dimY];
        copyImagePlane(oldPlane->imagePlaneF, imagePlaneF);
        break;
      };
      case DOUBLE_IMG:
      {
        imagePlaneD = new double[dimX * dimY];
        copyImagePlane(oldPlane->imagePlaneD, imagePlaneD);
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
  /// @param[in] newx - New x-dimension
  /// @param[in] newy - New y-dimension.
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
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Changed storage type of imagePlanes to std::vector<>
  /// @version 2012-11-27/GGB - Added support for native image planes.
  /// @version 2010-10-16/GGB - Function created.

  CImagePlane::~CImagePlane()
  {
    if (imagePlane8)
    {
      delete [] imagePlane8;
      imagePlane8 = nullptr;
    };
    if (imagePlaneS8)
    {
      delete [] imagePlaneS8;
      imagePlaneS8 = nullptr;
    };
    if (imagePlaneU16)
    {
      delete [] imagePlaneU16;
      imagePlaneU16 = nullptr;
    }
    if (imagePlane16)
    {
      delete [] imagePlane16;
      imagePlane16 = nullptr;
    };
    if (imagePlaneU32)
    {
      delete [] imagePlaneU32;
      imagePlaneU32 = nullptr;
    }
    if (imagePlane32)
    {
      delete [] imagePlane32;
      imagePlane32 = nullptr;
    };
    if (imagePlane64)
    {
      delete [] imagePlane64;
      imagePlane64 = nullptr;
    };
    if (imagePlaneF)
    {
      delete [] imagePlaneF;
      imagePlaneF = nullptr;
    };
    if (imagePlaneD)
    {
      delete [] imagePlaneD;
      imagePlaneD = nullptr;
    };
  }

  /// @brief Assignment operator
  /// @param[in] rhs - The image plane to copy to this.
  /// @throws std::bad_alloc
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
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
          imagePlane8 = new std::uint8_t[dimX * dimY];
          copyImagePlane(rhs.imagePlane8, imagePlane8);
          break;
        };
        case SBYTE_IMG:
        {
          imagePlaneS8 = new std::int8_t[dimX * dimY];
          copyImagePlane(rhs.imagePlaneS8, imagePlaneS8);
          break;
        };
        case USHORT_IMG:
        {
          imagePlaneU16 = new std::uint16_t[dimX * dimY];
          copyImagePlane(rhs.imagePlaneU16, imagePlaneU16);
          break;
        };
        case SHORT_IMG:
        {
          imagePlane16 = new std::int16_t[dimX * dimY];
          copyImagePlane(rhs.imagePlane16, imagePlane16);
          break;
        };
        case ULONG_IMG:
        {
          imagePlaneU32 = new std::uint32_t[dimX * dimY];
          copyImagePlane(rhs.imagePlaneU32, imagePlaneU32);
          break;
        };
        case LONG_IMG:
        {
          imagePlane32 = new std::int32_t[dimX * dimY];
          copyImagePlane(rhs.imagePlane32, imagePlane32);
          break;
        };
        case LONGLONG_IMG:
        {
          imagePlane64 = new std::int64_t[dimX * dimY];
          copyImagePlane(rhs.imagePlane64, imagePlane64);
          break;
        };
        case FLOAT_IMG:
        {
          imagePlaneF = new float[dimX * dimY];
          copyImagePlane(rhs.imagePlaneF, imagePlaneF);
          break;
        };
        case DOUBLE_IMG:
        {
          imagePlaneD = new double[dimX * dimY];
          copyImagePlane(rhs.imagePlaneD, imagePlaneD);
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
      ACL_ERROR(0x0004);    // Inconsistent image sizes.

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
      ACL_ERROR(0x0004);    // Inconsistent image sizes.

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
    RUNTIME_ASSERT(ACL, rhs.dimX != dimX, "this->dimX != rhs.dimX");
    RUNTIME_ASSERT(ACL, rhs.dimY != dimY, "this->dimY != rhs.dimY");

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

  /// @brief Array operator.
  /// @param[in] index - The array index to return the value of.
  /// @returns The value at the specified array index.
  /// @throws CError(ACL::0x000B) - Array index out of array bounds.
  /// @version 2013-01-26/GGB - Added support for native FITS formats.
  /// @version 2011-05-01/GGB - Function created.

  FP_t CImagePlane::operator [](INDEX_t index) const
  {
    if ( ((dimX * dimY) >= 0) && (static_cast<INDEX_t>(dimX * dimY) > index) )
    {
      return getValue(index);
    }
    else
    {
      ACL_ERROR(0x000B);
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

  /// @brief Interpolates the value of the image for position (x, y) Uses the bilinear interpolation function.
  /// @param[in] x - position x
  /// @param[in] y - Coordinate y
  /// @returns The interpolated value.
  /// @throws CError(ACL::0x0001) - Invalid coordinates.
  /// @version 2013-03-11/GGB - Changed storage type of imagePlanes to std::vector<>
  /// @version 2010-12-29/GGB - Function created.

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
      ACL_ERROR(0x0001);    // Invalid coordinates.
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
    RUNTIME_ASSERT(ACL, nSize != 0, "Bin Pixels is only valid with a non-zero integer.");
    RUNTIME_ASSERT(ACL, nSize <= 10, "Bin Pixels is only valid with a binning value of <= 10.");

    AXIS_t yStep, yBegin, yEnd;
    size_t  numberOfThreads;
    size_t threadNumber;
    boost::thread_group threadGroup;
    boost::thread *thread;
    double *newImagePlaneD;

    AXIS_t newWidth = dimX / nSize;
    AXIS_t newHeight = dimY / nSize;

    newImagePlaneD = new double[newWidth * newHeight];

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

      thread = new boost::thread(&CImagePlane::binPixelsThread<FP_t>, this, newImagePlaneD,
                                 std::make_tuple(yBegin, yEnd), newWidth, nSize);
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    deleteImagePlane();         // Delete the existing image plane.

    bitpix_ = DOUBLE_IMG;          // Change data type to double
    imagePlaneD = newImagePlaneD;
    newImagePlaneD = nullptr;

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
          imagePlane8 = new std::uint8_t[dimX * dimY];
          switch (bitpix_)
          {
          case SHORT_IMG:
            convertImagePlane(imagePlane16, imagePlane8);
            if (imagePlane16)
            {
              delete imagePlane16;
              imagePlane16 = nullptr;
            };
            break;
          case LONG_IMG:
            convertImagePlane(imagePlane32, imagePlane8);
            if (imagePlane32)
            {
              delete imagePlane32;
              imagePlane32 = nullptr;
            };
            break;
          case LONGLONG_IMG:
            convertImagePlane(imagePlane64, imagePlane8);
            if (imagePlane64)
            {
              delete imagePlane64;
              imagePlane64 = nullptr;
            };
            break;
          case BYTE_IMG:
            convertImagePlane(imagePlaneF, imagePlane8);
            if(imagePlaneF)
            {
              delete imagePlaneF;
              imagePlaneF = nullptr;
            };
            break;
          case DOUBLE_IMG:
            convertImagePlane(imagePlaneD, imagePlane8);
            if (imagePlaneD)
            {
              delete imagePlaneD;
              imagePlaneD = nullptr;
            };
            break;
          };
          break;
        case SHORT_IMG:    // (16)
          imagePlane16 = new std::int16_t[dimX * dimY];
          switch (bitpix_)
          {
          case BYTE_IMG:
            convertImagePlane(imagePlane8, imagePlane16);
            if (imagePlane8)
            {
              delete imagePlane8;
              imagePlane8 = nullptr;
            };
            break;
          case LONG_IMG:
            convertImagePlane(imagePlane32, imagePlane16);
            if (imagePlane32)
            {
              delete [] imagePlane32;
              imagePlane32 = nullptr;
            };
            break;
          case LONGLONG_IMG:
            convertImagePlane(imagePlane64, imagePlane16);
            if (imagePlane64)
            {
              delete imagePlane64;
              imagePlane64 = nullptr;
            };
            break;
          case FLOAT_IMG:
            convertImagePlane(imagePlaneF, imagePlane16);
            if (imagePlaneF)
            {
              delete imagePlaneF;
              imagePlaneF = nullptr;
            };
            break;
          case DOUBLE_IMG:
            convertImagePlane(imagePlaneD, imagePlane16);
            if (imagePlaneD)
            {
              delete imagePlaneD;
              imagePlaneD = nullptr;
            };
            break;
          };
          break;
        case LONG_IMG:
          imagePlane32 = new std::int32_t[dimX * dimY];
          switch (bitpix_)
          {
          case BYTE_IMG:
            convertImagePlane(imagePlane8, imagePlane32);
            if (imagePlane8)
            {
              delete imagePlane8;
              imagePlane8 = nullptr;
            };
            break;
          case SHORT_IMG:
            convertImagePlane(imagePlane16, imagePlane32);
            if (imagePlane16)
            {
              delete imagePlane16;
              imagePlane16 = nullptr;
            };
            break;
          case LONGLONG_IMG:
            convertImagePlane(imagePlane64, imagePlane32);
            delete imagePlane64;
            imagePlane64 = nullptr;
            break;
          case FLOAT_IMG:
            convertImagePlane(imagePlaneF, imagePlane32);
            if (imagePlaneF)
            {
              delete imagePlaneF;
              imagePlaneF = nullptr;
            };
            break;
          case DOUBLE_IMG:
            convertImagePlane(imagePlaneD, imagePlane32);
            if (imagePlaneD)
            {
              delete imagePlaneD;
              imagePlaneD = nullptr;
            };
            break;
          };
          break;
        case LONGLONG_IMG:
          imagePlane64 = new std::int64_t[dimX * dimY];
          switch (bitpix_)
          {
          case BYTE_IMG:
            convertImagePlane(imagePlane8, imagePlane64);
            if (imagePlane8)
            {
              delete imagePlane8;
              imagePlane8 = nullptr;
            };
            break;
          case SHORT_IMG:
            convertImagePlane(imagePlane16, imagePlane64);
            delete imagePlane16;
            imagePlane16 = nullptr;
            break;
          case LONG_IMG:
            convertImagePlane(imagePlane32, imagePlane64);
            delete imagePlane32;
            imagePlane32 = nullptr;
            break;
          case FLOAT_IMG:
            convertImagePlane(imagePlaneF, imagePlane64);
            delete imagePlaneF;
            imagePlaneF = nullptr;
            break;
          case DOUBLE_IMG:
            convertImagePlane(imagePlaneD, imagePlane64);
            delete imagePlaneD;
            imagePlaneD = nullptr;
            break;
          };
          break;
        case FLOAT_IMG:
          imagePlaneF = new float[dimX * dimY];
          switch (bitpix_)
          {
          case BYTE_IMG:
            convertImagePlane(imagePlane8, imagePlaneF);
            delete imagePlane8;
            imagePlane8 = nullptr;
            break;
            case USHORT_IMG:
            {
              convertImagePlane(imagePlaneU16, imagePlaneF);
              delete imagePlaneU16;
              imagePlane16 = nullptr;
              break;
            };
          case SHORT_IMG:
            convertImagePlane(imagePlane16, imagePlaneF);
            delete imagePlane16;
            imagePlane16 = nullptr;
            break;
          case LONG_IMG:
            convertImagePlane(imagePlane32, imagePlaneF);
            delete imagePlane32;
            imagePlane32 = nullptr;
            break;
          case LONGLONG_IMG:
            convertImagePlane(imagePlane64, imagePlaneF);
            delete imagePlane64;
            imagePlane64 = nullptr;
            break;
          case DOUBLE_IMG:
            convertImagePlane(imagePlaneD, imagePlaneF);
            delete imagePlaneD;
            imagePlaneD = nullptr;
            break;
          };
          break;
        case DOUBLE_IMG:
          imagePlaneD = new double[dimX * dimY];
          switch (bitpix_)
          {
          case BYTE_IMG:
            convertImagePlane(imagePlane8, imagePlaneD);
            delete imagePlane8;
            imagePlane8 = nullptr;
            break;
          case SHORT_IMG:
            convertImagePlane(imagePlane16, imagePlaneD);
            delete imagePlane16;
            imagePlane16 = nullptr;
            break;
          case LONG_IMG:
            convertImagePlane(imagePlane32, imagePlaneD);
            delete imagePlane32;
            imagePlane32 = nullptr;
            break;
          case LONGLONG_IMG:
            convertImagePlane(imagePlane64, imagePlaneD);
            delete imagePlane64;
            imagePlane64 = nullptr;
            break;
          case FLOAT_IMG:
            convertImagePlane(imagePlaneF, imagePlaneD);
            delete imagePlaneF;
            imagePlaneF = nullptr;
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

  boost::optional<MCL::TPoint2D<AXIS_t> > CImagePlane::brightWalk(MCL::TPoint2D<AXIS_t> const &guess, AXIS_t rmax,
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
    boost::optional<MCL::TPoint2D<AXIS_t> > returnValue;
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

    boost::optional<FP_t> mean = MCL::mean(imagePoints, imagePointsSize);
    boost::optional<FP_t> stdev = MCL::stdev(imagePoints, imagePointsSize);

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
  /// @version 2015-09-02/GGB - Moved into CImagePlane class.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-03-09/GGB - Function created.

  template<typename T, typename U>
  void CImagePlane::convertImagePlane(T *src, U *dest)
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

      thread = new boost::thread(&CImagePlane::convertImagePlaneThread<T, U>, this, src, dest, indexBegin, indexEnd);
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
  /// @version 2015-09-02/GGB - Moved into class CImagePlane.
  /// @version 2013-03-09/GGB - Function created.

  template<typename T, typename U>
  void CImagePlane::convertImagePlaneThread(T const *src, U *dest, INDEX_t start, INDEX_t end)
  {
    INDEX_t index;

    for (index = start; index < end; index++)
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
  /// @version 2015-09-03/GGB - (Bug 8) Corrected to work with all types of storage.
  /// @version 2015-08-29/GGB - Converted back to C-style arrays.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2011-08-15/GGB - Function created.

  void CImagePlane::crop(MCL::TPoint2D<AXIS_t> const &o, MCL::TPoint2D<AXIS_t> const &d)
  {
    std::uint8_t  *newImagePlane8 = nullptr;
    std::int8_t   *newImagePlaneS8 = nullptr;
    std::uint16_t *newImagePlaneU16 = nullptr;
    std::int16_t  *newImagePlane16 = nullptr;
    std::uint32_t *newImagePlaneU32 = nullptr;
    std::int32_t  *newImagePlane32 = nullptr;
    std::int64_t  *newImagePlane64 = nullptr;
    float         *newImagePlaneF = nullptr;
    double        *newImagePlaneD= nullptr;

    RUNTIME_ASSERT(ACL, o.x() > 0, "Origin incorrect");
    RUNTIME_ASSERT(ACL, o.y() > 0, "Origin incorrect");
    RUNTIME_ASSERT(ACL, o.x() + d.x() < dimX, "Origin + dimension incorrect");
    RUNTIME_ASSERT(ACL, o.y() + d.y() < dimY, "Origin + dimension incorrect");

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        newImagePlane8 = new std::uint8_t[d.x() * d.y()];
        crop(imagePlane8, newImagePlane8, o, d);
        delete [] imagePlane8;
        imagePlane8 = newImagePlane8;
        newImagePlane8 = nullptr;
        break;
      };
      case SBYTE_IMG:
      {
        newImagePlaneS8 = new std::int8_t[d.x() * d.y()];
        crop(imagePlaneS8, newImagePlaneS8, o, d);
        delete [] imagePlaneS8;
        imagePlaneS8 = newImagePlaneS8;
        newImagePlaneS8 = nullptr;
        break;
      }
      case USHORT_IMG:
      {
        newImagePlaneU16 = new std::uint16_t[d.x() * d.y()];
        crop(imagePlaneU16, newImagePlaneU16, o, d);
        delete [] imagePlaneU16;
        imagePlaneU16 = newImagePlaneU16;
        newImagePlaneU16 = nullptr;
        break;
      };
      case SHORT_IMG:
      {
        newImagePlane16 = new std::int16_t[d.x() * d.y()];
        crop(imagePlane16, newImagePlane16, o, d);
        delete [] imagePlane16;
        imagePlane16 = newImagePlane16;
        newImagePlane16 = nullptr;
        break;
      };
      case ULONG_IMG:
      {
        newImagePlaneU32 = new std::uint32_t[d.x() * d.y()];
        crop(imagePlaneU32, newImagePlaneU32, o, d);
        delete [] imagePlaneU32;
        imagePlaneU32 = newImagePlaneU32;
        newImagePlaneU32 = nullptr;
        break;
      };
      case LONG_IMG:
      {
        newImagePlane32 = new std::int32_t[d.x() * d.y()];
        crop(imagePlane32, newImagePlane32, o, d);
        delete [] imagePlane32;
        imagePlane32 = newImagePlane32;
        newImagePlane32 = nullptr;
        break;
      };
      case LONGLONG_IMG:
      {
        newImagePlane64 = new std::int64_t[d.x() * d.y()];
        crop(imagePlane64, newImagePlane64, o, d);
        delete [] imagePlane64;
        imagePlane64 = newImagePlane64;
        newImagePlane64 = nullptr;
        break;
      };
      case FLOAT_IMG:
      {
        newImagePlaneF = new float[d.x() * d.y()];
        crop(imagePlaneF, newImagePlaneF, o, d);
        delete [] imagePlaneF;
        imagePlaneF = newImagePlaneF;
        newImagePlaneF = nullptr;
        break;
      };
      case DOUBLE_IMG:
      {
        newImagePlaneD = new double[d.x() * d.y()];
        crop(imagePlaneD, newImagePlaneD, o, d);
        delete [] imagePlaneD;
        imagePlaneD = newImagePlaneD;
        newImagePlaneD = nullptr;
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
  /// @version 2015-09-02/GGB - Use C-style arrays as storage type.
  /// @version 2015-09-02/GGB - Removed default clause to prevent error is the bitpix value has not been assigned.
  /// @version 2013-01-27/GGB - Function created.

  void CImagePlane::deleteImagePlane()
  {
    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        if (imagePlane8)
        {
          delete [] imagePlane8;
          imagePlane8 = nullptr;
        };
        break;
      };
      case SBYTE_IMG:
      {
        if (imagePlaneS8)
        {
          delete [] imagePlaneS8;
          imagePlaneS8 = nullptr;
        };
        break;
      }
      case USHORT_IMG:
      {
        if (imagePlaneU16)
        {
          delete [] imagePlaneU16;
          imagePlaneU16 = nullptr;
        };
        break;
      };
      case SHORT_IMG:
      {
        if (imagePlane16)
        {
          delete [] imagePlane16;
          imagePlane16 = nullptr;
        };
        break;
      };
      case ULONG_IMG:
      {
        if (imagePlaneU32)
        {
          delete [] imagePlaneU32;
          imagePlaneU32 = nullptr;
        };
        break;
      };
      case LONG_IMG:
      {
        if (imagePlane32)
        {
          delete [] imagePlane32;
          imagePlane32 = nullptr;
        };
        break;
      };
      case LONGLONG_IMG:
      {
        if (imagePlane64)
        {
          delete [] imagePlane64;
          imagePlane64 = nullptr;
        };
        break;
      };
      case FLOAT_IMG:
      {
        if (imagePlaneF)
        {
          delete [] imagePlaneF;
          imagePlaneF = nullptr;
        };
        break;
      };
      case DOUBLE_IMG:
      {
        if (imagePlaneD)
        {
          delete [] imagePlaneD;
          imagePlaneD = nullptr;
        };
        break;
      };
      default:
      {
        break;
      };
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Source Extraction function using the findStars routines from libWCS
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2014-02-01/GGB - Function created.

  void CImagePlane::findStars(TImageSourCAstronomicalCoordinatesontainer &sourceList, SFindSources const &sourceParameters)
  {
    double *tempImage = new double[dimX * dimY];

    switch(bitpix_)
    {
      case BYTE_IMG:
      {
        convertImagePlane(imagePlane8, tempImage);
        break;
      };
      case SBYTE_IMG:
      {
        convertImagePlane(imagePlaneS8, tempImage);
        break;
      };
      case USHORT_IMG:
      {
        convertImagePlane(imagePlaneU16, tempImage);
        break;
      };
      case SHORT_IMG:
      {
        convertImagePlane(imagePlane16, tempImage);
        break;
      };
      case ULONG_IMG:
      {
        convertImagePlane(imagePlaneU32, tempImage);
        break;
      };
      case LONG_IMG:
      {
        convertImagePlane(imagePlane32, tempImage);
        break;
      };
      case LONGLONG_IMG:
      {
        convertImagePlane(imagePlane64, tempImage);
        break;
      };
      case FLOAT_IMG:
      {
        convertImagePlane(imagePlaneF, tempImage);
        break;
      };
      case DOUBLE_IMG:
      {
        copyImagePlane(imagePlaneD, tempImage);
        break;
      };
    };

    CFindSources sourceExtractor(tempImage, dimX, dimY);

    sourceExtractor.setParameters(sourceParameters);

    sourceExtractor.findStars(sourceList);
  }

  /// @brief Determines the FWHM of a star.
  /// @details Searches within radius pixels of the center. Performs 2D curve fitting to function
  /// @param[in] center - The center point to determine the FWHM from.
  /// @param[radius] - The maximum radius
  /// @returns The FWHM of the object.
  /// @throws 0x1205 - IMAGEPLANE: FWHM Call Radius == 0.
  /// @version 2018-05-12/GGB - Function created/

  boost::optional<FP_t> CImagePlane::FWHM(MCL::TPoint2D<AXIS_t> const &center, AXIS_t radius) const
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

    SCL::TVectorSorted<CDataPoint> dataPoints;
    SCL::TVectorSorted<CDataPoint>::iterator dataPointsIterator;

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
          dataPoints.insert(dataPoint);
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
        return boost::optional<FP_t>();
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
        return boost::optional<double>(ACL::FWHM(dataSamples));
      };
    }
    else
    {
      return boost::optional<FP_t>();
    };
  }

  /// @brief Gets the image plane value at the specified point.
  //
  // 2012-11-29/GGB - Changed to use native data types.
  // 2012-11-10/GGB - Function created.

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
    RUNTIME_ASSERT(ACL, x >= 0, "Parameter x cannot be less than zero.");
    RUNTIME_ASSERT(ACL, x < dimX, "Parameter x > dimX.");

    RUNTIME_ASSERT(ACL, y >= 0, "Parameter y cannot be less than zero.");
    RUNTIME_ASSERT(ACL, y < dimY, "Parameter y > dimY.");

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
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-30/GGB - Added support for native FITS types
  /// @version 2010-12-28/GGB - Function created.

  void CImagePlane::mirrorAxisX()
  {
    std::uint8_t  *newImagePlane8 = nullptr;
    std::int8_t   *newImagePlaneS8 = nullptr;
    std::uint16_t *newImagePlaneU16 = nullptr;
    std::int16_t  *newImagePlane16 = nullptr;
    std::uint32_t *newImagePlaneU32 = nullptr;
    std::int32_t  *newImagePlane32 = nullptr;
    std::int64_t  *newImagePlane64 = nullptr;
    float         *newImagePlaneF = nullptr;
    double        *newImagePlaneD= nullptr;

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        newImagePlane8 = mirrorAxisX(imagePlane8);
        delete [] imagePlane8;
        imagePlane8 = newImagePlane8;
        newImagePlane8 = nullptr;
        break;
      };
      case SBYTE_IMG:
      {
        newImagePlaneS8 = mirrorAxisX(imagePlaneS8);
        delete [] imagePlaneS8;
        imagePlaneS8 = newImagePlaneS8;
        newImagePlaneS8 = nullptr;
        break;
      }
      case USHORT_IMG:
      {
        newImagePlaneU16 = mirrorAxisX(imagePlaneU16);
        delete [] imagePlaneU16;
        imagePlaneU16 = newImagePlaneU16;
        newImagePlaneU16 = nullptr;
        break;
      };
      case SHORT_IMG:
      {
        newImagePlane16 = mirrorAxisX(imagePlane16);
        delete [] imagePlane16;
        imagePlane16 = newImagePlane16;
        newImagePlane16 = nullptr;
        break;
      };
      case ULONG_IMG:
      {
        newImagePlaneU32 = mirrorAxisX(imagePlaneU32);
        delete [] imagePlaneU32;
        imagePlaneU32 = newImagePlaneU32;
        newImagePlaneU32 = nullptr;
        break;
      };
      case LONG_IMG:
      {
        newImagePlane32 = mirrorAxisX(imagePlane32);
        delete [] imagePlane32;
        imagePlane32 = newImagePlane32;
        newImagePlane32 = nullptr;
        break;
      };
      case LONGLONG_IMG:
      {
        newImagePlane64 = mirrorAxisX(imagePlane64);
        delete [] imagePlane64;
        imagePlane64 = newImagePlane64;
        newImagePlane64 = nullptr;
        break;
      };
      case FLOAT_IMG:
      {
        newImagePlaneF = mirrorAxisX(imagePlaneF);
        delete [] imagePlaneF;
        imagePlaneF = newImagePlaneF;
        newImagePlaneF = nullptr;
        break;
      };
      case DOUBLE_IMG:
      {
        newImagePlaneD = mirrorAxisX(imagePlaneD);
        delete [] imagePlaneD;
        imagePlaneD = newImagePlaneD;
        newImagePlaneD = nullptr;
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
  /// @throws std::bad_alloc
  /// @version 2015-09-20/GGB - (Bug 18) - Added input parameter verification code.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-27/GGB - Function created.

  template<typename T>
  T *CImagePlane::mirrorAxisX(T *imagePlane)
  {
    RUNTIME_ASSERT(ACL, imagePlane, "Paramter imagePlane cannot be nullptr");

    T *newImagePlane = new T[dimX * dimY];
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

      thread = new boost::thread(&CImagePlane::mirrorAxisXThread<T>, this, colBegin, colEnd, boost::ref(imagePlane), boost::ref(newImagePlane));
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    return newImagePlane;
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
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2011-06-04/GGB - Function created.

  void CImagePlane::floatImage(AXIS_t newWidth, AXIS_t newHeight, long newBkgnd)
  {
    std::uint8_t *newImagePlane8 = nullptr;
    std::int8_t *newImagePlaneS8 = nullptr;
    std::uint16_t *newImagePlaneU16 = nullptr;
    std::int16_t *newImagePlane16 = nullptr;
    std::uint32_t *newImagePlaneU32 = nullptr;
    std::int32_t *newImagePlane32 = nullptr;
    std::int64_t *newImagePlane64 = nullptr;
    float        *newImagePlaneF = nullptr;
    double       *newImagePlaneD = nullptr;

    RUNTIME_ASSERT(ACL, newWidth >= dimX, "Cannot have the x-dimension smaller when floating.");
    RUNTIME_ASSERT(ACL, newHeight >= dimY, "Cannot have the y-dimension smaller when floating.");

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        newImagePlane8 = new std::uint8_t[newWidth * newHeight];
        floatImage(imagePlane8, newImagePlane8, newWidth, newHeight, newBkgnd);
        delete [] imagePlane8;
        imagePlane8 = newImagePlane8;
        newImagePlane8 = nullptr;
        break;
      };
      case SBYTE_IMG:
      {
        newImagePlaneS8 = new std::int8_t[newWidth * newHeight];
        floatImage(imagePlaneS8, newImagePlaneS8, newWidth, newHeight, newBkgnd);
        delete [] imagePlaneS8;
        imagePlaneS8 = newImagePlaneS8;
        newImagePlaneS8 = nullptr;
        break;
      };
      case USHORT_IMG:
      {
        newImagePlaneU16 = new std::uint16_t[newWidth * newHeight];
        floatImage(imagePlaneU16, newImagePlaneU16, newWidth, newHeight, newBkgnd);
        delete [] imagePlaneU16;
        imagePlaneU16 = newImagePlaneU16;
        newImagePlaneU16 = nullptr;
        break;
      };
      case SHORT_IMG:
      {
        newImagePlane16 = new std::int16_t[newWidth * newHeight];
        floatImage(imagePlane16, newImagePlane16, newWidth, newHeight, newBkgnd);
        delete [] imagePlane16;
        imagePlane16 = newImagePlane16;
        newImagePlane16 = nullptr;
        break;
      };
      case ULONG_IMG:
      {
        newImagePlaneU32 = new std::uint32_t[newWidth * newHeight];
        floatImage(imagePlaneU32, newImagePlaneU32, newWidth, newHeight, newBkgnd);
        delete [] imagePlaneU32;
        imagePlaneU32 = newImagePlaneU32;
        newImagePlaneU32 = nullptr;
        break;
      };
      case LONG_IMG:
      {
        newImagePlane32 = new std::int32_t[newWidth * newHeight];
        floatImage(imagePlane32, newImagePlane32, newWidth, newHeight, newBkgnd);
        delete [] imagePlane32;
        imagePlane32 = newImagePlane32;
        newImagePlane32 = nullptr;
        break;
      };
      case LONGLONG_IMG:
      {
        newImagePlane64 = new std::int64_t[newWidth * newHeight];
        floatImage(imagePlane64, newImagePlane64, newWidth, newHeight, newBkgnd);
        delete [] imagePlane64;
        imagePlane64 = newImagePlane64;
        newImagePlane64 = nullptr;
        break;
      };
      case FLOAT_IMG:
      {
        newImagePlaneF = new float[newWidth * newHeight];
        floatImage(imagePlaneF, newImagePlaneF, newWidth, newHeight, newBkgnd);
        delete [] imagePlaneF;
        imagePlaneF = newImagePlaneF;
        newImagePlaneF = nullptr;
        break;
      };
      case DOUBLE_IMG:
      {
        newImagePlaneD = new double[newWidth * newHeight];
        floatImage(imagePlaneD, newImagePlaneD, newWidth, newHeight, newBkgnd);
        delete [] imagePlaneD;
        imagePlaneD = newImagePlaneD;
        newImagePlaneD = nullptr;
        break;
      };
    };

    dimX = newWidth;
    dimY = newHeight;
    bMinMax = bMean = false;
  }

  /// @brief Internal float function
  //
  // 2013-03-11/GGB - Function created.

  template<typename T>
  void CImagePlane::floatImage(T *imagePlane, T *newImagePlane, AXIS_t newWidth, AXIS_t newHeight, long newBkgnd)
  {
    AXIS_t lox, loy, xdiff, ydiff;
    AXIS_t xIndex, yIndex;

    xdiff = newWidth - dimX;
    ydiff = newHeight - dimY;

    lox = xdiff / 2;

    loy = ydiff / 2;

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
  }

  /// @brief Flops the image vertically (about the Y axis)
  /// @throws 0x1201 - CIMAGEPLANE: No image plane available BITPIX = BP_NONE.
  /// @throws 0x1203 - CIMAGEPLANE: Invalid BITPIX value.
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-11-30/GGB - Added support for FITS native types.
  /// @version 2010-12-28/GGB - Function created.

  void CImagePlane::mirrorAxisY()
  {
    std::uint8_t  *newImagePlane8 = nullptr;
    std::int8_t *newImagePlaneS8 = nullptr;
    std::uint16_t *newImagePlaneU16 = nullptr;
    std::int16_t *newImagePlane16 = nullptr;
    std::uint32_t *newImagePlaneU32 = nullptr;
    std::int32_t *newImagePlane32 = nullptr;
    std::int64_t *newImagePlane64 = nullptr;
    float           *newImagePlaneF = nullptr;
    double          *newImagePlaneD = nullptr;

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        newImagePlane8 = mirrorAxisY(imagePlane8);
        delete [] imagePlane8;
        imagePlane8 = newImagePlane8;
        newImagePlane8 = nullptr;
        break;
      };
      case SBYTE_IMG:
      {
        newImagePlaneS8 = mirrorAxisY(imagePlaneS8);
        delete [] imagePlaneS8;
        imagePlaneS8 = newImagePlaneS8;
        newImagePlaneS8 = nullptr;
        break;
      };
      case USHORT_IMG:
      {
        newImagePlaneU16 = mirrorAxisY(imagePlaneU16);
        delete [] imagePlaneU16;
        imagePlaneU16 = newImagePlaneU16;
        newImagePlaneU16 = nullptr;
        break;
      };
      case SHORT_IMG:
      {
        newImagePlane16 = mirrorAxisY(imagePlane16);
        delete [] imagePlane16;
        imagePlane16 = newImagePlane16;
        newImagePlane16 = nullptr;
        break;
      };
      case ULONG_IMG:
      {
        newImagePlaneU32 = mirrorAxisY(imagePlaneU32);
        delete [] imagePlaneU32;
        imagePlaneU32 = newImagePlaneU32;
        newImagePlaneU32 = nullptr;
        break;
      };
      case LONG_IMG:
      {
        newImagePlane32 = mirrorAxisY(imagePlane32);
        delete [] imagePlane32;
        imagePlane32 = newImagePlane32;
        newImagePlane32 = nullptr;
        break;
      };
      case LONGLONG_IMG:
      {
        newImagePlane64 = mirrorAxisY(imagePlane64);
        delete [] imagePlane64;
        imagePlane64 = newImagePlane64;
        newImagePlane64 = nullptr;
        break;
      };
      case FLOAT_IMG:
      {
        newImagePlaneF = mirrorAxisY(imagePlaneF);
        delete [] imagePlaneF;
        imagePlaneF = newImagePlaneF;
        newImagePlaneF = nullptr;
        break;
      };
      case DOUBLE_IMG:
      {
        newImagePlaneD = mirrorAxisY(imagePlaneD);
        delete [] imagePlaneD;
        imagePlaneD = newImagePlaneD;
        newImagePlaneD = nullptr;
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
  /// @version 2015-09-20/GGB - (Bug 19) Added check for valid parameters.
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2012-12-01/GGB - Converted to multi-threaded
  /// @version 2012-11-30/GGB - Function created.

  template<typename T>
  T *CImagePlane::mirrorAxisY(T *imagePlane)
  {
    RUNTIME_ASSERT(ACL, imagePlane, "Parameter imagePlane cannot be nullptr.");

    T *newImagePlane = new T[dimX * dimY];
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

      thread = new boost::thread(&CImagePlane::mirrorAxisYThread<T>, this, rowBegin, rowEnd, boost::ref(imagePlane), boost::ref(newImagePlane));
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    return newImagePlane;
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
  /// @version 2013-07-02/GGB - Added code to invalidate the Min,Max,Mean values. (Bug #1193741)
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-01-27/GGB - Added support for multi-threading and support for native FITS data types.
  /// @version 2011-03-07/GGB - Function created.

  void CImagePlane::resample(AXIS_t const &newWidth, AXIS_t const &newHeight)
  {
    FP_t xRatio = static_cast<FP_t>(newWidth) / static_cast<FP_t>(dimX);
    FP_t yRatio = static_cast<FP_t>(newHeight) / static_cast<FP_t>(dimY);
    AXIS_t yStep, yBegin, yEnd;
    double *newImagePlane = new double[newWidth * newHeight];
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

      thread = new boost::thread(&CImagePlane::resampleThread, this, newImagePlane, boost::tuple<AXIS_t, AXIS_t>(yBegin, yEnd),
                                 newWidth, boost::tuple<FP_t, FP_t>(xRatio, yRatio));
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.

    deleteImagePlane();
    bitpix_ = DOUBLE_IMG;
    imagePlaneD = newImagePlane;		// Copy the new image plane.
    newImagePlane = nullptr;

    dimX = newWidth;
    dimY = newHeight;

    bMinMax = false;    // Min & Max values are not valid.
    bMean = false;       // Mean value is not correct.
  }

  /// @brief Resamples the image. This is the threaded procedure.
  //
  /// @version 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  /// @version 2013-01-27/GGB - Function created.

  void CImagePlane::resampleThread(double *newImagePlane, boost::tuple<AXIS_t, AXIS_t> yVals, AXIS_t newWidth,
                                   boost::tuple<FP_t, FP_t> ratios)
  {
    AXIS_t xp, yp;
    FP_t xVal, yVal;
    FP_t xFP, yFP;
    FP_t xIP, yIP;
    FP_t a, b, c, d;

    for(xp = 0; xp < newWidth; xp++)
    {
      for(yp = yVals.get<0>(); yp < yVals.get<1>(); yp++)
      {
        xVal = xp / ratios.get<0>();
        yVal = yp / ratios.get<1>();
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

    transformData.newImagePlane = new FP_t[dimX * dimY];

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
    imagePlaneD = transformData.newImagePlane;
    transformData.newImagePlane = nullptr;

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

  // Translates an image to the new position.
  // This is the integer translation form. There is also a float translation function
  // However, this is optimised for integer translations.
  //
  // INPUT:			xt - amount to translate in the x dim (can be negative)
  //						yt - amount to translate in the y dim (can be negative)
  //
  // 2014-12-31/GGB - Changed types to make x64 compliant.
  // 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  // 2010-12-28/GGB - Function created.

  void CImagePlane::translate(AXIS_t const xt, AXIS_t const yt)
  {
    AXIS_t const newDimX = dimX + xt;
    AXIS_t const newDimY = dimY + yt;
    double *newImagePlane = new double[newDimX * newDimY];
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
      imagePlaneD = newImagePlane;
      newImagePlane = nullptr;
      bitpix_ = DOUBLE_IMG;
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  // Translation function using double values for the distance to be translated.
  //
  // 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  // 2011-03-13/GGB - Uses SCL::CArrray2DT
  // 2010-12-29/GGB - Function created.

  void CImagePlane::translate(FP_t xt, FP_t yt)
  {

    AXIS_t xIndex, yIndex;
    AXIS_t const newDimX = static_cast<AXIS_t>(std::ceil(dimX + xt));
    AXIS_t const newDimY = static_cast<AXIS_t>(std::ceil(dimY + yt));
    FP_t XO, YO;
    double *newImagePlane = new double[newDimX * newDimY];

    if ( (xt != 0) && (yt != 0) )		// If both values are zero, this is a trivial case.
    {
      for (xIndex = 0; xIndex < newDimX; xIndex++)
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

      deleteImagePlane();   // Delete the current image plane.
      bitpix_ = DOUBLE_IMG;
      imagePlaneD = newImagePlane;
      newImagePlane = nullptr;
    };

    bMinMax = bMean = false;     // Min max and average have changed. Need to be recalculated.
  }

  /// @brief Returns the maximum value on the array.
  /// @details If the maximum needs to be rechecked, the min and max are done at the same time.
  /// @throws 0x1200 -
  /// @throws 0x1203 - Invalid BITPIX value.
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
          boost::optional<boost::tuple<std::uint8_t, std::uint8_t> > returnValue = MCL::minmax(imagePlane8, dimX * dimY);
          if (returnValue)
          {
            fMin = (*returnValue).get<0>();
            fMax = (*returnValue).get<1>();
          };
         break;
        };
        case SBYTE_IMG:
        {
          boost::optional<boost::tuple<std::int8_t, std::int8_t> > returnValue = MCL::minmax(imagePlaneS8, dimX * dimY);
          if (returnValue)
          {
            fMin = (*returnValue).get<0>();
            fMax = (*returnValue).get<1>();
          };
         break;
        };
        case USHORT_IMG:
        {
          boost::optional<boost::tuple<std::uint16_t, std::uint16_t> > returnValue = MCL::minmax(imagePlaneU16, dimX * dimY);
          if (returnValue)
          {
            fMin = (*returnValue).get<0>();
            fMax = (*returnValue).get<1>();
          };
          break;
        };
      case SHORT_IMG:
      {
        boost::optional<boost::tuple<std::int16_t, std::int16_t> > returnValue = MCL::minmax(imagePlane16, dimX * dimY);
        if (returnValue)
        {
          fMin = (*returnValue).get<0>();
          fMax = (*returnValue).get<1>();
        };
        break;
      };
        case ULONG_IMG:
        {
          boost::optional<boost::tuple<boost::uint32_t, boost::uint32_t> > returnValue = MCL::minmax(imagePlaneU32, dimX * dimY);
          if (returnValue)
          {
            fMin = (*returnValue).get<0>();
            fMax = (*returnValue).get<1>();
          };
          break;
        };
      case LONG_IMG:
      {
        boost::optional<boost::tuple<boost::int32_t, boost::int32_t> > returnValue = MCL::minmax(imagePlane32, dimX * dimY);
        if (returnValue)
        {
          fMin = (*returnValue).get<0>();
          fMax = (*returnValue).get<1>();
        };
        break;
      };

      case LONGLONG_IMG:
      {
        boost::optional<boost::tuple<std::int64_t, std::int64_t> > returnValue = MCL::minmax(imagePlane64, dimX * dimY);
        if (returnValue)
        {
          fMin = (*returnValue).get<0>();
          fMax = (*returnValue).get<1>();
        };
      };
        break;
      case FLOAT_IMG:
      {
        boost::optional<boost::tuple<float, float> > returnValue = MCL::minmax(imagePlaneF, dimX * dimY);
        if (returnValue)
        {
          fMin = (*returnValue).get<0>();
          fMax = (*returnValue).get<1>();
        };
      };
        break;
      case DOUBLE_IMG:
      {
        boost::optional<boost::tuple<double, double> > returnValue = MCL::minmax(imagePlaneD, dimX * dimY);
        if (returnValue)
        {
          fMin = (*returnValue).get<0>();
          fMax = (*returnValue).get<1>();
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
    boost::optional<FP_t> returnValue;

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
          returnValue = MCL::mean(imagePlane8, dimX * dimY);
          break;
        };
        case SBYTE_IMG:
        {
          returnValue = MCL::mean(imagePlaneS8, dimX * dimY);
          break;
        };
        case USHORT_IMG:
        {
          returnValue = MCL::mean(imagePlaneU16, dimX * dimY);
          break;
        };
        case SHORT_IMG:
        {
          returnValue = MCL::mean(imagePlane16, dimX * dimY);
          break;
        };
        case ULONG_IMG:
        {
          returnValue = MCL::mean(imagePlaneU16, dimX * dimY);
          break;
        };
        case LONG_IMG:
        {
          returnValue = MCL::mean(imagePlane32, dimX * dimY);
          break;
        };
        case LONGLONG_IMG:
        {
          returnValue = MCL::mean(imagePlane64, dimX * dimY);
          break;
        };
        case FLOAT_IMG:
        {
          returnValue = MCL::mean(imagePlaneF, dimX * dimY);
          break;
        };
        case DOUBLE_IMG:
        {
          returnValue = MCL::mean(imagePlaneD, dimX * dimY);
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
        returnValue = *MCL::stdev(imagePlane8, dimX * dimY);
        break;
      };
      case SBYTE_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneS8, dimX * dimY);
        break;
      };
      case USHORT_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneU16, dimX * dimY);
        break;
      };
      case SHORT_IMG:
      {
        returnValue = *MCL::stdev(imagePlane16, dimX * dimY);
        break;
      };
      case ULONG_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneU32, dimX * dimY);
        break;
      };
      case LONG_IMG:
      {
        returnValue = *MCL::stdev(imagePlane32, dimX * dimY);
        break;
      };
      case LONGLONG_IMG:
      {
        returnValue = *MCL::stdev(imagePlane64, dimX * dimY);
        break;
      };
      case FLOAT_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneF, dimX * dimY);
        break;
      };
      case DOUBLE_IMG:
      {
        returnValue = *MCL::stdev(imagePlaneD, dimX * dimY);
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

  void CImagePlane::objectProfile(MCL::TPoint2D<FP_t> centroid, AXIS_t radius, std::vector<boost::tuple<FP_t, FP_t> > &data) const
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
          data.push_back(boost::tuple<FP_t, FP_t>(std::sqrt(dist), getValue(indexX, indexY)));
        };
      };
    };
  }

  /// @brief Read data from a FITS file.
  /// @param[in] file - The FITS file to read from.
  /// @param[in] axis - The NAXIS3 value to read. (Ie the colour plane)
  /// @throws GCL::CError(ACL::0x1002)
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2017-09-20/GGB - Added try...catch for BSCALE and BZERO keywords.
  /// @version 2015-09-02/GGB - Function created.

  void CImagePlane::readFromFITS(fitsfile *file, AXIS_t axis)
  {
    RUNTIME_ASSERT(ACL, axis >= 1, "Parameter axis must be >= 1");
    RUNTIME_ASSERT(ACL, axis <= 999, "Parameter axis must be <= 999");
    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr");

    int status = 0;

    CFITSIO_TEST(fits_get_img_equivtype(file, &bitpix_, &status));

      // These keywords are not mandatory.

    try
    {
      CFITSIO_TEST(fits_read_key(file, TDOUBLE, FITS_BSCALE.c_str(), &bscale_, nullptr, &status));
    }
    catch(...)
    {
      status = 0;
    };

    try
    {
      CFITSIO_TEST(fits_read_key(file, TDOUBLE, FITS_BZERO.c_str(), &bzero_, nullptr, &status));
    }
    catch(...)
    {
      status = 0;
    };

    {
      long naxis[2];
      CFITSIO_TEST(fits_get_img_size(file, 2, naxis, &status));

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
        imagePlane8 = new std::uint8_t[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TBYTE, startPixel, readPixels, 0, imagePlane8, &anynul, &status));
        break;
      };
      case SBYTE_IMG:
      {
        imagePlaneS8 = new std::int8_t[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TSBYTE, startPixel, readPixels, 0, imagePlaneS8, &anynul, &status));
        break;
      }
      case USHORT_IMG:
      {
        imagePlaneU16 = new std::uint16_t[readPixels];
        CFITSIO_TEST(fits_read_img(file, TUSHORT, startPixel, readPixels, 0, imagePlaneU16, &anynul, &status));
        break;
      }
      case SHORT_IMG:
      {
        imagePlane16 = new std::int16_t[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TSHORT, startPixel, readPixels, 0, imagePlane16, &anynul, &status));
        break;
      }
      case ULONG_IMG:
      {
        imagePlaneU32 = new std::uint32_t[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TULONG, startPixel, readPixels, 0, imagePlaneU32, &anynul, &status));
        break;
      };
      case LONG_IMG:
      {
        imagePlane32 = new std::int32_t[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TLONG, startPixel, readPixels, 0, imagePlane32, &anynul, &status));
        break;
      }
      case LONGLONG_IMG:
      {
        imagePlane64 = new std::int64_t[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TLONGLONG, startPixel, readPixels, 0, imagePlane64, &anynul, &status));
        break;
      }
      case FLOAT_IMG:
      {
        imagePlaneF = new float[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TFLOAT, startPixel, readPixels, 0, imagePlaneF, &anynul, &status));
        break;
      }
      case DOUBLE_IMG:
      {
        imagePlaneD = new double[dimX * dimY];
        CFITSIO_TEST(fits_read_img(file, TDOUBLE, startPixel, readPixels, 0, imagePlaneD, &anynul, &status));
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
  /// @param[in] outputImage - Reference to the array that has been allocated for the output image.
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
          CODE_ERROR(ACL);
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
          boost::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_GAMMALOG:
      case ETF_CBRT:
        thread = new boost::thread(&CImagePlane::renderImageGrey8CbrtThread, this, outputImage,
          boost::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_SQRT:
        thread = new boost::thread(&CImagePlane::renderImageGrey8SqrtThread, this, outputImage,
          boost::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_SIGMOID:
        thread = new boost::thread(&CImagePlane::renderImageGrey8SigmoidThread, this, outputImage,
          boost::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_SQUARE:
        thread = new boost::thread(&CImagePlane::renderImageGrey8SquareThread, this, outputImage,
          boost::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      case ETF_LOG:
        thread = new boost::thread(&CImagePlane::renderImageGrey8LogThread, this, outputImage,
          boost::tuple<INDEX_t, INDEX_t>(indexBegin, indexEnd));
        break;
      default:
        {
          CODE_ERROR(ACL);
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
  /// @param[out] oi - The array that the data will be written to
  /// @param[in] startEnd - The start and end index values that will be processed by this thread.
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8CbrtThread(renderImage_t *oi, boost::tuple<INDEX_t, INDEX_t> const &startEnd)
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

    for(lIndex = startEnd.get<0>(); lIndex < startEnd.get<1>(); lIndex++)
    {
      currentValue = getValue(lIndex);          // This is corrected for BZERO and BSCALE

      if (fRange == 0)
        outputImage[lIndex] = minimumImageValue;
      else if (currentValue <= blackPoint)
        outputImage[lIndex] = minimumImageValue;
      else if (currentValue >= whitePoint)
        outputImage[lIndex] = maximumImageValue;
      else
      {
        currentValue = std::pow(((currentValue - blackPoint) / fRange), exponent) * maxVal;
        outputImage[lIndex] = static_cast<renderImage_t>(floor(currentValue + 0.5));
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
  //
  // 2015-06-30/GGB - Added extra parameter for the outputImage.
  // 2013-05-26/GGB - Function created.

  void CImagePlane::renderImageGrey8GammaThread(renderImage_t *oi,
                                                 boost::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t const exponent = 1 / transferFunctionParameter;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = whitePoint - blackPoint;

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = startEnd.get<0>(); lIndex < startEnd.get<1>(); lIndex++)
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
                                                   boost::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t const exponent = 1 / transferFunctionParameter;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = std::log(whitePoint - blackPoint);

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = startEnd.get<0>(); lIndex < startEnd.get<1>(); lIndex++)
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

  void CImagePlane::renderImageGrey8LogThread(renderImage_t *oi, boost::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    FP_t currentValue;
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = std::log(whitePoint - blackPoint);
    renderImageGrey8_t minimumImageValue = std::numeric_limits<renderImageGrey8_t>::min();
    renderImageGrey8_t maximumImageValue = std::numeric_limits<renderImageGrey8_t>::max();

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = startEnd.get<0>(); lIndex < startEnd.get<1>(); lIndex++)
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

  void CImagePlane::renderImageGrey8SigmoidThread(renderImage_t *oi, boost::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImageGrey8_t>::max();
    FP_t currentValue;
    renderImageGrey8_t minimumImageValue = std::numeric_limits<renderImageGrey8_t>::min();
    renderImageGrey8_t maximumImageValue = std::numeric_limits<renderImageGrey8_t>::max();
    const FP_t fRange = whitePoint - blackPoint;

    renderImageGrey8_t *outputImage = reinterpret_cast<renderImageGrey8_t *>(oi);

    for(lIndex = startEnd.get<0>(); lIndex < startEnd.get<1>(); lIndex++)
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

  void CImagePlane::renderImageGrey8SqrtThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t currentValue;
    const FP_t fRange = whitePoint - blackPoint;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();

    for(lIndex = startEnd.get<0>(); lIndex < startEnd.get<1>(); lIndex++)
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

  void CImagePlane::renderImageGrey8SquareThread(renderImage_t *outputImage, boost::tuple<INDEX_t, INDEX_t> const &startEnd)
  {
    INDEX_t lIndex;
    FP_t const maxVal = std::numeric_limits<renderImage_t>::max();
    FP_t currentValue;
    renderImage_t minimumImageValue = std::numeric_limits<renderImage_t>::min();
    renderImage_t maximumImageValue = std::numeric_limits<renderImage_t>::max();
    const FP_t fRange = whitePoint - blackPoint;

    for(lIndex = startEnd.get<0>(); lIndex < startEnd.get<1>(); lIndex++)
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

    std::uint8_t  *newImagePlane8 = nullptr;
    std::int8_t *newImagePlaneS8 = nullptr;
    std::uint16_t *newImagePlaneU16 = nullptr;
    std::int16_t *newImagePlane16 = nullptr;
    std::uint32_t *newImagePlaneU32 = nullptr;
    std::int32_t *newImagePlane32 = nullptr;
    std::int64_t *newImagePlane64 = nullptr;
    float           *newImagePlaneF = nullptr;
    double          *newImagePlaneD = nullptr;

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
        newImagePlane8 = new std::uint8_t[lxDim * lyDim];
        rotate(imagePlane8, newImagePlane8, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlane8;
        imagePlane8 = newImagePlane8;
        newImagePlane8 = nullptr;
        break;
      };
      case SBYTE_IMG:
      {
        newImagePlaneS8 = new std::int8_t[lxDim * lyDim];
        rotate(imagePlaneS8, newImagePlaneS8, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlaneS8;
        imagePlaneS8 = newImagePlaneS8;
        newImagePlaneS8 = nullptr;
        break;
      };
      case USHORT_IMG:
      {
        newImagePlaneU16 = new std::uint16_t[lxDim * lyDim];
        rotate(imagePlaneU16, newImagePlaneU16, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlaneU16;
        imagePlaneU16 = newImagePlaneU16;
        newImagePlane16 = nullptr;
        break;
      };
      case SHORT_IMG:
      {
        newImagePlane16 = new std::int16_t[lxDim * lyDim];
        rotate(imagePlane16, newImagePlane16, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlane16;
        imagePlane16 = newImagePlane16;
        newImagePlane16 = nullptr;
        break;
      };
      case ULONG_IMG:
      {
        newImagePlaneU32 = new std::uint32_t[lxDim * lyDim];
        rotate(imagePlaneU32, newImagePlaneU32, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlaneU32;
        imagePlaneU32 = newImagePlaneU32;
        newImagePlaneU32 = nullptr;
        break;
      };
      case LONG_IMG:
      {
        newImagePlane32 = new std::int32_t[lxDim * lyDim];
        rotate(imagePlane32, newImagePlane32, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlane32;
        imagePlane32 = newImagePlane32;
        newImagePlane32 = nullptr;
        break;
      };
      case LONGLONG_IMG:
      {
        newImagePlane64 = new std::int64_t[lxDim * lyDim];
        rotate(imagePlane64, newImagePlane64, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlane64;
        imagePlane64 = newImagePlane64;
        newImagePlane64 = nullptr;
        break;
     };
      case FLOAT_IMG:
      {
        newImagePlaneF = new float[lxDim * lyDim];
        rotate(imagePlaneF, newImagePlaneF, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlaneF;
        imagePlaneF = newImagePlaneF;
        newImagePlaneF = nullptr;
        break;
      };
      case DOUBLE_IMG:
      {
        newImagePlaneD = new double[lxDim * lyDim];
        rotate(imagePlaneD, newImagePlaneD, std::make_tuple(lxMin, lxMax, lyMin, lyMax), xOrigen, yOrigen, cosa, sina);
        delete [] imagePlaneD;
        imagePlaneD = newImagePlaneD;
        newImagePlane16 = nullptr;
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
  //
  // 2013-03-11/GGB - Converted to use std::vector<> as storage type.
  // 2012-12-01/GGB - Function created.

  template<typename T>
  void CImagePlane::rotate(T *imagePlane, T *newImagePlane,
                           std::tuple<AXIS_t, AXIS_t, AXIS_t, AXIS_t> l, FP_t xOrigen, FP_t yOrigen, FP_t cosa, FP_t sina)
  {
    size_t numberOfThreads;
    size_t threadNumber;
    AXIS_t rowStep;
    boost::thread_group threadGroup;
    boost::thread *thread;
    AXIS_t rowBegin, rowEnd = std::get<2>(l);
    AXIS_t dy = std::get<3>(l) - std::get<2>(l);

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
                                 boost::ref(imagePlane), boost::ref(newImagePlane), l, xOrigen, yOrigen, cosa, sina);
      threadGroup.add_thread(thread);
      thread = nullptr;
    };

    threadGroup.join_all();     // Wait for all the threads to finish.
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
  /// @param[in] hdu - The CCfits HDU to write to.
  /// @throws CRuntimeAssert
  /// @version 2017-09-21/GGB - Added writing of BZERO and BSCALE (Bug #116)
  /// @version 2015-09-02/GGB
  /// @li Use C-style arrays as storage type.
  /// @li Use cfitsio rather than CCfits for accessing FITS files.
  /// @li Add additional data types as supported by cfitsio V3.3
  /// @version 2013-03-14/GGB - Function created.

  void CImagePlane::writeToFITS(fitsfile *file, AXIS_t axis)
  {
    RUNTIME_ASSERT(ACL, axis >= 1, "Parameter axis must be >= 1. (" + std::to_string(axis) + ")");
    RUNTIME_ASSERT(ACL, axis <= 999, "Parameter axis must be <= 999. (" + std::to_string(axis) + ")");
    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr");

    INDEX_t startPixel = (axis - 1) * dimX * dimY + 1;
    INDEX_t pixelCount = dimX * dimY;
    int status = 0;

    CFITSIO_TEST(fits_write_key(file, TFLOAT, FITS_BSCALE.c_str(), &bscale_, FITS_COMMENT_BSCALE.c_str(), &status));
    CFITSIO_TEST(fits_write_key(file, TFLOAT, FITS_BZERO.c_str(), &bzero_, FITS_COMMENT_BZERO.c_str(), &status));

    switch (bitpix_)
    {
      case BYTE_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TBYTE, startPixel, pixelCount, imagePlane8, &status));
        break;
      };
      case SBYTE_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TSBYTE, startPixel, pixelCount, imagePlaneS8, &status));
        break;
      };
      case USHORT_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TUSHORT, startPixel, pixelCount, imagePlaneU16, &status));
        break;
      };
      case SHORT_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TSHORT, startPixel, pixelCount, imagePlane16, &status));
        break;
      };
      case ULONG_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TULONG, startPixel, pixelCount, imagePlaneU32, &status));
        break;
      };
      case LONG_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TLONG, startPixel, pixelCount, imagePlane32, &status));
        break;
      };
      case LONGLONG_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TLONGLONG, startPixel, pixelCount, imagePlane64, &status));
        break;
      };
      case FLOAT_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TFLOAT, startPixel, pixelCount, imagePlaneF, &status));
        break;
      };
      case DOUBLE_IMG:
      {
        CFITSIO_TEST(fits_write_img(file, TDOUBLE, startPixel, pixelCount, imagePlaneD, &status));
        break;
      };
      default:
      {
        CODE_ERROR(ACL);
        break;
      };
    };
  }

}  // namespace ACL
