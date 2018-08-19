//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								ImageRegister.h
// SUBSYSTEM:						Class for registering images
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost, MCL, GCL.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2018 Gavin Blakeman.
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
// OVERVIEW:						Implements image registration. Translates, scales and shifts a group of images to match the first image
//                      in the group.
//
// CLASSES INCLUDED:		SRegisterImageInformation
//                      CRegisterImages
//
// CLASS HIERARCHY:     SRegisterImageInformation
//											CRegisterImages (SRegisterImageInformation)
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2011-08-07 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_IMAGEREGISTER_H
#define ACL_IMAGEREGISTER_H

#include "AstroFile.h"

  // Standard include files

#include <vector>

  // Miscellaneouslibrary header files

#include "boost/shared_ptr.hpp"
#include <GCL>
#include <MCL>

namespace ACL
{
  class CRegisterImageInformation
  {
  public:
    PAstroFile inputFile;
    PAstroFile outputFile;
    std::vector<CHDB>::size_type HDB;
    CImagePlane *mask;			        // Mask used for masking out incomplete areas of the image.
    MCL::TPoint2D<FP_t> align1;
    MCL::TPoint2D<FP_t> align2;
    FP_t dist;
    FP_t th;								        // Reference angle
    MCL::TPoint2D<FP_t> tr;				  // Translation amount
    FP_t dth;							          // rotation angle
    FP_t sc;								        // scale amount
    AXIS_t xMax;							      // Size of the x-axis
    AXIS_t yMax;							      // size of the y-axis
    AXIS_t x0;								      // x origen
    AXIS_t y0;								      // y origen
    MCL::TPoint2D<FP_t> pixSize;    // Pixel size

    CRegisterImageInformation();
    CRegisterImageInformation(PAstroFile, std::vector<CHDB>::size_type, MCL::TPoint2D<FP_t> const &, MCL::TPoint2D<FP_t> const &);
    CRegisterImageInformation(CRegisterImageInformation const &);
    ~CRegisterImageInformation();
  };
  typedef boost::shared_ptr<CRegisterImageInformation> PRegisterImageInformation;
  typedef std::vector<PRegisterImageInformation> DRegisterImageStore;

  class CRegisterImages
  {
  private:
    DRegisterImageStore imageStore;

  protected:
  public:
    CRegisterImages();
    virtual ~CRegisterImages();

    void addImage(PAstroFile, std::vector<CHDB>::size_type, MCL::TPoint2D<FP_t> const &, MCL::TPoint2D<FP_t> const &);
    DRegisterImageStore const &getImages() const;
    void clearImages();

      // Information functions

    virtual size_t imageCount() const;

      // Worker functions

    void registerImages();
  };

}  // namespace ACL

#endif  // ACL_IMAGEREGISTER_H
