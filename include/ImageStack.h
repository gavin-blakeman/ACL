//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								ImageStack
// SUBSYSTEM:						Image stacking class
// LANGUAGE:						C++
// TARGET OS:						WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman. (GGB)
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
// OVERVIEW:						A class for stacking images.
//
// CLASSES INCLUDED:    CImageStack
//
// CLASS HIERARCHY:     CImageStack
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2010-12-10 GGB - Development of classes for astroManager
//
//*************************************************************************************************

#ifndef ACL_IMAGESTACK_H
#define ACL_IMAGESTACK_H

  // Standard C++ library header files

#include <list>
#include <memory>
#include <vector>

  // ACL library header files

#include "AstroFile.h"
#include "AstroImage.h"
#include "ImageRegister.h"

namespace ACL
{
  struct CStackImageInformation
  {
  public:
    std::shared_ptr<CAstroFile> astroFile;  ///< The astroFile to be stacked.
    std::vector<CHDB>::size_type HDB;       ///< The HDB to be used.
    MCL::TPoint2D<FP_t> align1;             ///< First alignment point.
    MCL::TPoint2D<FP_t> align2;             ///< Second alignment point.
    FP_t dist;                              ///< Distance between the points.
    FP_t th;                                ///< Reference angle
    MCL::TPoint2D<FP_t> tr;                 ///< Translation amount
    FP_t dth;                               ///< rotation angle
    FP_t sc;                                ///< scale amount
    AXIS_t xMax;                            ///< Size of the x-axis
    AXIS_t yMax;                            ///< size of the y-axis
    AXIS_t x0;                              ///< x origen
    AXIS_t y0;                              ///< y origen
    MCL::TPoint2D<FP_t> pixSize;            ///< Pixel size

    CStackImageInformation();
    CStackImageInformation(std::shared_ptr<CAstroFile>, std::vector<CHDB>::size_type, MCL::TPoint2D<FP_t> const &, MCL::TPoint2D<FP_t> const &);
    ~CStackImageInformation();
  };
  typedef std::vector<std::shared_ptr<CStackImageInformation>> DStackImagesStore;
  typedef std::list<CAstroImage *> DImageStore;

  class CImageStack
  {
  public:
    enum EStackMode
    {
      SM_NONE,
      SM_SUM,
      SM_MEAN,
      SM_MEDIAN ,
      SM_SIGMACLIP
    };

  private:
    bool allowResize_;
    bool zeroExterior_;
    std::unique_ptr<bool[]> maskPlane;

  protected:
    DStackImagesStore inputFiles;
    DImageStore inputImages;
    std::unique_ptr<CAstroFile> resultFile;
    CAstroImage *resultImage;

    CAstroFile *darkFrame;
    CAstroFile *flatFrame;
    CAstroFile *biasFrame;

    void consistencyCheckImages();
    void calibrateImages();
    void registerImages();
    void computeResult(EStackMode);
    void applyMask();
    void keywordsAdd();
    void keywordsModify();
    void keywordsRemove();

    virtual void combineSum();
    virtual void combineMean();
    virtual void combineMedian();


  public:
    CImageStack();
    virtual ~CImageStack();

    virtual void addFile(PAstroFile, MCL::TPoint2D<FP_t> &, MCL::TPoint2D<FP_t> &);
    virtual void clearFiles();

    /*inline void setDarkFrame(CImagePlane *newDark) { darkFrame = newDark;};
    inline void setFlatFrame(CImagePlane *newFlat) { flatFrame = newFlat;};
    inline void setBiasFrame(CImagePlane *newBias) { biasFrame = newBias;};*/
    inline void setFlags(bool bResize, bool bZero) { allowResize_ = bResize; zeroExterior_ = bZero;}

    std::unique_ptr<CAstroFile> &stackImages(EStackMode);


  };

}  // namespace ACL

#endif // ACL_IMAGESTACK_H
