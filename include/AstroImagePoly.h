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
//											These classes are tightly linked with the cfitsio library. This linkage
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
// CLASSES INCLUDED:		CAstroImagePoly			- A class that provides scientific functionality on a polychromatic image.
//
// CLASS HIERARCHY:     CAstroImage
//												- CAstroImageMono
//												- CAstroImagePoly
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-05-25 GGB - Moved CAstroImageMono & CAstroImagePoly to their own files.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROIMAGEPOLY_H
#define ACL_ASTROIMAGEPOLY_H

  // ACL Header files

#include "AstroImage.h"
#include "error.h"

namespace ACL
{
  class CAstroImagePoly : public CAstroImage
  {
  private:
    DImagePlaneStorage imagePlanes;

    CAstroImagePoly() = delete;
    CAstroImagePoly(CAstroImagePoly const &) = delete;
    CAstroImagePoly &operator=(CAstroImagePoly const &) = delete;

  protected:
    virtual void readFromFITS(fitsfile *);

  public:
    CAstroImagePoly(AXIS_t, AXIS_t, AXIS_t);
    CAstroImagePoly(CAstroImage const *);

    virtual CAstroImage *createCopy() const { ACL_CODE_ERROR; }

      // Operators

    virtual CAstroImage &operator+=(CAstroImage const &) {ACL_CODE_ERROR; }
    virtual CAstroImage &operator-=(CAstroImage const &) {ACL_CODE_ERROR;}
    virtual CAstroImage &operator*=(FP_t) {ACL_CODE_ERROR;}
    virtual CAstroImage &operator/=(FP_t) {ACL_CODE_ERROR;}

    // Functions to load images.

    virtual void loadFromRGBHP(SRGBHP_Ptr, EColour) ;

    virtual int BITPIX() const { ACL_CODE_ERROR; }
    virtual void BITPIX(int) { ACL_CODE_ERROR; }

    virtual double BSCALE() const { ACL_CODE_ERROR; }
    virtual void BSCALE(double) { ACL_CODE_ERROR; }

    virtual double BZERO() const { ACL_CODE_ERROR; }
    virtual void BZERO(double) { ACL_CODE_ERROR; }

    virtual int PEDESTAL() const { ACL_CODE_ERROR; }

    virtual NAXIS_t NAXIS() const { return 3;}
    virtual AXIS_t NAXISn(NAXIS_t) const {ACL_CODE_ERROR;}



    virtual std::valarray<double> *imageValarray() const {ACL_CODE_ERROR;}

      // Image Plane functions

    virtual std::optional<MCL::TPoint2D<FP_t> > findCentroid(MCL::TPoint2D<AXIS_t> const &, AXIS_t) const {ACL_CODE_ERROR;}
    virtual void objectProfile(MCL::TPoint2D<FP_t> centroid, AXIS_t radius, std::vector<boost::tuple<FP_t, FP_t> > &data) const { ACL_CODE_ERROR;}

    virtual void insertImagePlane(PImagePlane);

    virtual bool isMonoImage() const {return false;}  ///< @brief Checks for a monochrome image.
                                                      ///< @details A monochrome image has only one colour plane.
                                                      ///< @returns false = not a monochrome image.
                                                      ///< @throws Nothing.
                                                      /// @invariant Always returns false.
                                                      ///< @note !isMonoImage() is not equivalent to isPolyImage().

    virtual bool isPolyImage() const {return true;}   ///< @brief Checks for a polychrome image.
                                                      ///< @details A polychrome image has more than colour plane.
                                                      ///< @returns true = polychrome image
                                                      ///< @throws Nothing.
                                                      /// @invariant Always returns true.
                                                      ///< @note !isPolyImage() is not equivalent to isMonoImage().


    virtual void applyDark(CAstroImage const &) { ACL_CODE_ERROR; }

      // Information functions

    virtual FP_t getValue(AXIS_t, AXIS_t, AXIS_t=0) const {ACL_CODE_ERROR;}
    virtual void setValue(AXIS_t, AXIS_t, AXIS_t, FP_t) { ACL_CODE_ERROR; }

    virtual void findStars(TImageSourceContainer &, SFindSources const &) const { ACL_CODE_ERROR; }
    virtual std::optional<MCL::TPoint2D<FP_t> > centroid(MCL::TPoint2D<AXIS_t> const &, AXIS_t, int) const {ACL_CODE_ERROR;}
    virtual void photometry(SPPhotometryObservation /*photometryObservation*/) const {ACL_CODE_ERROR;}
    virtual std::optional<double> FWHM(MCL::TPoint2D<double> const &) const { ACL_CODE_ERROR; }
  };

} // namespace ACL

#endif // ACL_ASTROIMAGEPOLY_H
