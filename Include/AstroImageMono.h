//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroImageMono
// SUBSYSTEM:						Monochrome astronomical image class.
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, cfitsio, gsl, boost.
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
// CLASSES INCLUDED:		CAstroImageMono			- A class that provides scientific functionality on an astronomical image.
//
// CLASS HIERARCHY:     CAstroImage
//												- CAstroImageMono
//												- CAstroImagePoly
//
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release
//                      2013-05-25 GGB - Moved CAstroImageMono to it's own file
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-11-27 GGB - Removed class CImagePlane into file CImagePlane
//                      2010-10-16 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROIMAGEMONO_H
#define ACL_ASTROIMAGEMONO_H

  // ACL header files.

#include "AstroImage.h"
#include "common.h"
#include "config.h"
#include "PhotometryObservation.h"

namespace ACL
{
  class CAstroImageMono : public CAstroImage
  {
  private:
  protected:

    CAstroImageMono(const CAstroImageMono &);

  public:
      // Constructor

    CAstroImageMono(AXIS_t newX, AXIS_t newY);

    virtual CAstroImage *createCopy() const;
//    template<typename T>
//    void setImageData(std::valarray<T> const &data)
//    {
//      if (imagePlane)
//        imagePlane->setImageData(data);
//    };

    virtual void loadFromRGBHP(SRGBHP_Ptr, EColour) { ACL_CODE_ERROR; }

      // File functions

    virtual void readFromFITS(fitsfile *);

    virtual int BITPIX() const;
    virtual void BITPIX(int);

    virtual NAXIS_t NAXIS() const { return 2;}

    virtual void BSCALE(FP_t);

    virtual FP_t BZERO() const;
    virtual void BZERO(FP_t);

    virtual int PEDESTAL() const;

      // Image information functions

    virtual bool isMonoImage() const {return true;}
    virtual bool isPolyImage() const {return false;}
    virtual void objectProfile(MCL::TPoint2D<FP_t> centroid, AXIS_t radius, std::vector<boost::tuple<FP_t, FP_t> > &data) const;

      // Operators

    virtual CAstroImage &operator+=(CAstroImage const &);
    virtual CAstroImage &operator-=(CAstroImage const &);

      // Astromonical object functions

    virtual boost::optional<MCL::TPoint2D<FP_t> > centroid(MCL::TPoint2D<AXIS_t> const &, AXIS_t, int) const;
    virtual boost::optional<MCL::TPoint2D<FP_t> > findCentroid(MCL::TPoint2D<AXIS_t> const &, AXIS_t) const;

      // Photometry functions

    virtual void photometry(SPPhotometryObservation photometryObservation) const;
    virtual boost::optional<FP_t> FWHM(const MCL::TPoint2D<FP_t> &) const;

      // Calibration functions

    //virtual void applyCalibration(CMasterDarkFrame const &, CFlatFrame const &);
    virtual void applyDark(CAstroImage const &) ;
    //virtual void applyFlat(CFlatFrame const &);

      // Image Analysis functions

    virtual void findStars(TImageSourCAstronomicalCoordinatesontainer &, SFindSources const &) const;
  };
}

#endif // ACL_ASTROIMAGEMONO_H_
