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
//                      Copyright 2013-2017 Gavin Blakeman.
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

#ifndef ACL_PHOTOMETRYAPERTURECIRCULAR_H
#define ACL_PHOTOMETRYAPERTURECIRCULAR_H

  // ACL header files

#include "config.h"
#include "PhotometryAperture.h"

  // Standard library

#include <cstdint>
#include <memory>

namespace ACL
{
  class CPhotometryApertureCircular : public CPhotometryAperture
  {
  public:
    typedef std::uint16_t TRadius;

    /// @note This class must use fixes size variables to ensure that it can be serialised correctly across platforms.
    /// Think of the case where we have written where a platform supports 32 bit ints and we try to read to a platform
    /// that supports 16 bit ints.
    /// By fixing the size of the variables, this problem goes away.

  private:
    TRadius r1_;
    TRadius r2_;
    TRadius r3_;

  public:
    explicit CPhotometryApertureCircular() { r1_ = r2_ = r3_ = 0; }
    CPhotometryApertureCircular(CPhotometryApertureCircular const &);
    CPhotometryApertureCircular(TRadius r1, TRadius r2, TRadius r3) { r1_ = r1; r2_ = r2; r3_ = r3;}

    virtual PPhotometryAperture createCopy() const;

    virtual void serialiseOut(std::vector<std::uint8_t> &) const;
    virtual void serialiseIn(std::valarray<std::uint8_t> const &);

    virtual EPhotometryApertureType apertureType() { return PAT_CIRCULAR; }

    virtual AXIS_t searchDiameter() const { return 2 * r2_; }
    virtual AXIS_t height() const { return (2 * r3_); }
    virtual AXIS_t width() const { return (2 * r3_); }
    virtual AXIS_t halfHeight() const { return r3_; }
    virtual AXIS_t halfWidth() const  { return r3_; }

    virtual bool isSource(MCL::TPoint2D<INDEX_t> const &, MCL::TPoint2D<INDEX_t> const &);
    virtual bool isSky(MCL::TPoint2D<INDEX_t> const &, MCL::TPoint2D<INDEX_t> const &);

    std::uint16_t radius1() { return r1_; }
    std::uint16_t radius2() { return r2_; }
    std::uint16_t radius3() { return r3_; }
  };

  typedef std::shared_ptr<CPhotometryApertureCircular> PPhotometryApertureCircular;
}

#endif // ACL_PHOTOMETRYAPERTURECIRCULAR_H
