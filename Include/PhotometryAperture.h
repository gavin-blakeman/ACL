//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								PhotometryAperture
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2017 Gavin Blakeman.
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
//                      2013-06-08 GGB - Moved CPhotometryObservation, PhotometryAperture  into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_PHOTOMETRYAPERTURE_H
#define ACL_PHOTOMETRYAPERTURE_H

  // ACL header files

#include "config.h"

  // Standard header files

#include <memory>
#include <valarray>

  // Miscellaneous Libraries

#include <GCL>
#include <MCL>

namespace ACL
{
  enum EPhotometryApertureType : std::uint8_t
  {
    PAT_NONE,
    PAT_CIRCULAR,
    PAT_ELLIPSE,
    PAT_SQUARE,
    PAT_RECTANGLE,
  } ;

  class CPhotometryAperture;
  typedef std::shared_ptr<CPhotometryAperture> PPhotometryAperture;

  class CPhotometryAperture
  {
  private:
  public:
    static CPhotometryAperture *createAperture(std::valarray<std::uint8_t> const &);
    CPhotometryAperture() {}
    CPhotometryAperture(CPhotometryAperture const &) {}
    virtual ~CPhotometryAperture() {}

    virtual PPhotometryAperture createCopy() const;

    virtual void serialiseOut(std::valarray<std::uint8_t> &) const { CODE_ERROR(ACL); }
    virtual void serialiseIn(std::valarray<std::uint8_t> const &) { CODE_ERROR(ACL); }

    virtual EPhotometryApertureType apertureType() { return PAT_NONE; }

    virtual AXIS_t searchDiameter() const { CODE_ERROR(ACL); }
    virtual AXIS_t height() const { CODE_ERROR(ACL); }
    virtual AXIS_t width() const { CODE_ERROR(ACL); }
    virtual AXIS_t halfHeight() const { CODE_ERROR(ACL); }
    virtual AXIS_t halfWidth() const  { CODE_ERROR(ACL); }
    virtual bool isSource(MCL::TPoint2D<INDEX_t> const &, MCL::TPoint2D<INDEX_t> const &) { CODE_ERROR(ACL); }
    virtual bool isSky(MCL::TPoint2D<INDEX_t> const &, MCL::TPoint2D<INDEX_t> const &) { CODE_ERROR(ACL); }
  };

}

#endif // ACL_PHOTOMETRYAPERTURE_H
