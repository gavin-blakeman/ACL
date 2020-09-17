//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Photometry.h
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2013-2020 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting various aspects of photometry and photometric
//											reduction.
//											The focus of this subsystem is on taking raw photometric observations and
//											and converting them to transformed photometric observations.
//
// CLASSES INCLUDED:		CPhotometryObservation - A class for storing photometry observation information.
//                      CPhotometryObject		- A class for storage of photometry related information.
//                      CPhotometryAperture  - Storage class for the 3 radii for Photometry.
//
// CLASS HIERARCHY:
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-05-09 GGB - File created.
//
//*********************************************************************************************************************************

#include "include/PhotometryApertureCircular.h"

  // Miscellaneous library header files

#include "boost/locale.hpp"
#include <GCL>

  // ACL library header files

#include "include/common.h"
#include "include/error.h"

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CPhotometryApertureCircular
  //
  //*******************************************************************************************************************************

  /// @brief Copy constructor.
  /// @throws None.
  /// @version 2013-06-08/GGB - Function created.

  CPhotometryApertureCircular::CPhotometryApertureCircular(CPhotometryApertureCircular const &toCopy) : CPhotometryAperture(toCopy),
    r1_(toCopy.r1_), r2_(toCopy.r2_), r3_(toCopy.r3_)
  {
  }

  /// Creates a copy of *this.
  //
  // 2013-06-08/GGB - Function created.

  PPhotometryAperture CPhotometryApertureCircular::createCopy() const
  {
    PPhotometryAperture returnValue(new CPhotometryApertureCircular(*this) );

    return returnValue;
  }

  /// @brief Test if the passed coordinate lies within the annulus ring. (donut formed by r2_ and r3_)
  /// @param[in] center:  Center coordinate of the aperture
  /// @param[in] coordinate: The coordinate to test.
  /// @returns true - Coordinate is a sky coordinate
  /// @returns false - Coordinate is not a sky coordinate.
  /// @details The test is done by using the relationship R^2 = (x-xc)^2 + (y-yc)^2
  /// @version 2013-05-09/GGB - Function created.

  bool CPhotometryApertureCircular::isSky(MCL::TPoint2D<INDEX_t> const &center, MCL::TPoint2D<INDEX_t> const &coordinate)
  {
    INDEX_t r22 = MCL::pow2(r2_);
    INDEX_t r23 = MCL::pow2(r3_);
    INDEX_t d = MCL::pow2(coordinate.x() - center.x()) + MCL::pow2(coordinate.y() - center.y());

    return ( (d > r22) && ( d <= r23) );
  }

  /// Test if the passed coordinate lies within the aperture (r1_ ring)
  /// This is tested using the identity R = sqrt((x-xc)^2 + (y-yc^2)). In this particular case, we do not need to take the sqrt()
  /// and only use the squared values for the comparison.
  //
  // 2013-05-09/GGB - Function created.

  bool CPhotometryApertureCircular::isSource(MCL::TPoint2D<INDEX_t> const &center, MCL::TPoint2D<INDEX_t> const &coordinate)
  {
    INDEX_t r12 = MCL::pow2(r1_);
    INDEX_t d = MCL::pow2(coordinate.x() - center.x()) + MCL::pow2(coordinate.y() - center.y());

    return (d <= r12);
  }

  /// @brief Function to read the serialised data from a valarray.
  /// @throws 0x0501 - PHOTOMETRY: Unknown version number for circular aperture.
  /// @version 2013-05-09/GGB - Function created.

  void CPhotometryApertureCircular::serialiseIn(std::valarray<boost::uint8_t> const &inputArray)
  {
    size_t index = 1;       // First byte will be the type of aperture. This will have already been read to arrive at
                            // this point in the program.
    size_t counter;
    size_t size;
    boost::uint16_t temp;
    boost::uint16_t result;

    switch (inputArray[index++])    // Version number.
    {
    case 0:
      size = inputArray[index++];
      result = 0;
      for (counter = 0; counter < size; counter++)
      {
        temp = inputArray[index++];
        temp = temp << (8 * counter);
        result += temp;
      };
      r1_ = result;

      size = inputArray[index++];
      result = 0;
      for (counter = 0; counter < size; counter++)
      {
        temp = inputArray[index++];
        temp = temp << (8 * counter);
        result += temp;
      };
      r2_ = result;

      result = 0;
      size = inputArray[index++];
      for (counter = 0; counter < size; counter++)
      {
        temp = inputArray[index++];
        temp = temp << (8 * counter);
        result += temp;
      };
      r3_ = result;
      break;
      default:
      {
        RUNTIME_ERROR(boost::locale::translate("PHOTOMETRY: Unknown version number for circular aperture."),
                      E_PHOTOMETRY_UNKNOWNVERSION, LIBRARYNAME);
      };
    };
  }

  /// @brief Serialise the aperture data into a valarray.
  /// @param[out] outputArray: The array to write the output data to.
  /// @throws None.
  /// @details For a circular aperture, we just need to store the descriptor and the three radii.
  ///          The long types are stored LSB first.
  /// @note The storage of the aperture is:
  /// @li ApertureType - std::uint8_t
  /// @li Version - std::uint8_t
  /// @li r1 - 2 x std::uint8_t
  /// @li r2 - 2 x std::uint8_t
  /// @li r3 - 2 x std::uint8_t
  /// @version 2013-05-09/GGB - Function created.

  void CPhotometryApertureCircular::serialiseOut(std::vector<std::uint8_t> &outputArray) const
  {
    size_t recordSize = 0;
    size_t index = 0;
    size_t counter;
    std::uint8_t byte;
    TRadius temp;

    recordSize = sizeof(EPhotometryApertureType);
    recordSize++;                                     // Version number.
    recordSize += sizeof(std::uint16_t) * 3;          // 3 x ints
    recordSize += 3;                                  // 3 x sizeof(int)

    outputArray.resize(recordSize);
    outputArray[index++] = PAT_CIRCULAR;
    outputArray[index++] = 0;                         // Version number.

    temp = r1_;
    outputArray[index++] = sizeof(r1_);
    for (counter = 0; counter < sizeof(r1_); counter++)
    {
      byte = temp & 0xFF;          // Low byte
      outputArray[index++] = byte;
      temp = temp >> 8;
    };

    temp = r2_;
    outputArray[index++] = sizeof(r2_);
    for (counter = 0; counter < sizeof(r2_); counter++)
    {
      byte = temp & 0xFF;          // Low byte
      outputArray[index++] = byte;
      temp = temp >> 8;
    };

    temp = r3_;
    outputArray[index++] = sizeof(r3_);
    for (counter = 0; counter < sizeof(r3_); counter++)
    {
      byte = temp & 0xFF;          // Low byte
      outputArray[index++] = byte;
      temp = temp >> 8;
    };
  }

}  // namespace ACL
