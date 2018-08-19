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
//*********************************************************************************************************************************

#include "../include/ImageStack.h"

  // Miscellaneous header files

#include <GCL>
#include <MCL>
#include "sofam.h"

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // CStackImageInformation
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor for the class.
  /// @throws None.
  /// @version 2013-06-09/GGB - Changed to use smart pointers and added outputFile.
  /// @version 2011-07-07/GGB - Function created

  CStackImageInformation::CStackImageInformation() : astroFile()
  {
  }

  /// @brief Constructor for the class.
  /// @throws None.
  /// @version 2013-06-09/GGB - Changed astroFile to a smart pointer.
  /// @version 2011-07-07/GGB - Function created

  CStackImageInformation::CStackImageInformation(PAstroFile af, std::vector<CHDB>::size_type hdb, MCL::TPoint2D<FP_t> const &a1,
                                                 MCL::TPoint2D<FP_t> const &a2) :
    astroFile(af), HDB(hdb), align1(a1), align2(a2)
  {
  }

  /// @brief Class destructor.
  /// @throws None.
  /// @version 2011-07-07/GGB - Function created

  CStackImageInformation::~CStackImageInformation()
  {
  }

  //*******************************************************************************************************************************
  //
  // CImageStack
  //
  // A class used for stacking images. This can be used for combining images to create master
  // darks, flats etc, or for stacking images to increase the SNR.
  //
  // The class supports rotation, scaling and translation of images to assist in stacking.
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor.
  /// @throws None.
  /// @version 2010-12-10/GGB - Function created.

  CImageStack::CImageStack() : maskPlane(), inputImages(), resultFile(), resultImage(nullptr), darkFrame(nullptr), flatFrame(nullptr),
    biasFrame(nullptr)
  {
  }

  /// @brief Class destructor. Ensures that all dynamically allocated memory is deleted correctly. Checks all the results and
  ///        deletes as required.
  /// @throws None.
  /// @version 2013-06-09/GGB - Changed resultFile to smart pointer.
  /// @version 2013-03-07/GGB - Added code to clean up the result file and result image.
  /// @version 2011-03-02/GGB - Function created.

  CImageStack::~CImageStack()
  {
    if (resultImage)
    {
      delete resultImage;
      resultImage = nullptr;
    };

    if (darkFrame)
    {
      delete darkFrame;
      darkFrame = nullptr;
    };

    if (flatFrame)
    {
      delete flatFrame;
      flatFrame = nullptr;
    };

    if (biasFrame)
    {
      delete biasFrame;
      biasFrame = nullptr;
    };
  }

  /// @brief Adds the new image plane into the planes to be stacked. New image is always added to the end of the stack.
  /// @param[in] toAdd - The image to add.
  /// @param[in] newa1 - Alignment point 1
  /// @param[in] news2 - Alignment point 2
  /// @throws None.
  /// @version 2013-03-02/GGB - Changed list type to smart pointers.
  /// @version 2011-03-05/GGB - Function created.

  void CImageStack::addFile(PAstroFile toAdd, MCL::TPoint2D<FP_t> &newa1, MCL::TPoint2D<FP_t> &newA2)
  {
    PStackImageInformation newImage(new CStackImageInformation(toAdd, 0, newa1, newA2) );

    inputFiles.push_back(newImage);
  }

  /// @brief Masks the unstacked portions of the image.
  /// @throws None.
  /// @version 2017-09-02/GGB - Function created.

  void CImageStack::applyMask()
  {
    for (INDEX_t index = 0; index < (resultImage->width() * resultImage->height()); ++index)
    {
      if (maskPlane[index])
      {
        resultImage->setValue(index, 0);
      };
    };
  }

  void CImageStack::calibrateImages()
  {
  }

  /// @brief Clears all stored images from the image stack.
  /// @throws None.
  /// @version 2013-07-28/GGB - Added code to clear the inputImages (Bug #1205803)
  /// @version 2011-03-05/GGB - Function created.

  void CImageStack::clearFiles()
  {
    inputFiles.clear();
    inputImages.clear();
  }

  /// @brief This function combines the images into a single image.
  /// @note This function is non-destructive in that the image planes are copied by the function addImage.
  ///
  /// Step 3 - Resample image to x2.
  /// Step 4 -	Rotate the images using Align1 as the rotation point and Align2 as the angle reference.
  /// Step 5 - Stack the images
  /// Step 6 - Store the results in addResult.
  /// @version 2014-12-26/GGB - Changed all usages of std::clog to use the GCL::logger functions.
  /// @version 2013-03-07/GGB - Function created.

  void CImageStack::combineSum()
  {
    resultImage = inputImages.front();    // First image becomes the result.
    resultImage->BITPIX(DOUBLE_IMG);       // Change to double representation.
    inputImages.pop_front();

    CAstroImage *tempImage;
    DImageStore::size_type imageCount = inputImages.size(), imageCounter = 1;

    GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Summing " + std::to_string(imageCount) + " images...");

    while (inputImages.size() > 0)
    {
      GCL::logger::defaultLogger().logMessage(GCL::logger::info,
                                              "Processing image " + std::to_string((imageCounter)) + " of " +
                                              std::to_string(imageCount) + ".");
      tempImage = inputImages.front();
      (*resultImage) += (*tempImage);
      inputImages.pop_front();
      delete tempImage;
      imageCounter++;
    };

    GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Completed summing images.");
  }

  /// @brief Computes the result image after the images have been TRS.
  /// @param[in] stackMode - Specifies the algorithm to use for stacking.
  /// @throws GCL::CCodeError(ACL)
  /// @version 2013-03-07/GGB - Function created.

  void CImageStack::computeResult(EStackMode stackMode)
  {
      // Use the first input image as a template for the output file.
      // This also copies all the data, including the WCS data that will be valid for the output image.
      // This is valid as the output image is a dimensional and positional copy of the first input image.

    resultFile.reset((*inputFiles.begin())->astroFile->createCopy());
    resultFile->keywordWrite(0, HEASARC_CREATOR, "ACL::ImageStack", HEASARC_COMMENT_CREATOR); // Add a comment that we have added

    switch (stackMode)
    {
      case SM_SUM:
      {
        combineSum();
        break;
      };
      case SM_MEAN:
      {
        combineMean();
        break;
      };
      case SM_MEDIAN:
      {
        combineMedian();
        break;
      };
      case SM_SIGMACLIP:
      default:
        break;
    };

    applyMask();

    resultFile->setAstroImage(0, resultImage);
    resultImage = nullptr;
  }

  /// This function checks all the input images for consistency.

  void CImageStack::consistencyCheckImages()
  {
  }

  /// @brief Performs a mean (average) combination of the image stack.
  /// @note This has been written differently to the approach used in the median combine to (hopefully) make this function quicker
  /// than the median combine function.
  /// @throws GCL::CCodeError(ACL, 0x0400) - IMAGESTACK: No Images, or Images zero size.
  /// @version 2017-08-26/GGB - Changed c-style casting to static_cast<>()
  /// @version 2014-12-26/GGB - Changed all std::clog logging to GCL::logger
  /// @version 2013-03-07/GGB - Function created.

  void CImageStack::combineMean()
  {
    DImageStore::iterator imageIterator;
    AXIS_t indexX = 0, indexY;
    AXIS_t dimX, dimY;
    const long countZ = inputImages.size();
    long double ldAverage;			// Use long double (if supported) to maintain maximum precision.
    CAstroImage *tempImage;

    LOGMESSAGE(info, "Starting MEAN combine...");

    resultImage = inputImages.front()->createCopy();
    resultImage->BITPIX(DOUBLE_IMG);       // Change to double representation.
    dimX = resultImage->width();
    dimY = resultImage->height();

      // Must calculate the mean of each point.

    for(; indexX < dimX; indexX++)
    {
      for(indexY = 0; indexY < dimY; indexY++)
      {
        ldAverage = 0;
        for (imageIterator = inputImages.begin(); imageIterator != inputImages.end(); imageIterator++)
        {
          ldAverage += static_cast<long double>((*imageIterator)->getValue(indexX, indexY));
        };
        ldAverage /= countZ;
        resultImage->setValue(indexX, indexY, 0, static_cast<double>(ldAverage));		// Set the value
      };
    };

      // Delete all the processed images.

    while (inputImages.size() > 0)
    {
      tempImage = inputImages.front();
      inputImages.pop_front();
      delete tempImage;
      tempImage = 0;
    };

    LOGMESSAGE(info, "Completed MEAN combine.");
   }

  /// @brief Performs a median combine. The gsl library is used to calculate the median.
  /// @note This is necessarily a slower function than the mean combine.
  /// @throws None.
  /// @version 2010-12-10/GGB - Function created

  void CImageStack::combineMedian()
  {
    AXIS_t indexX, indexY, indexZ;
    size_t countZ = inputImages.size();
    AXIS_t dimX, dimY;
    std::valarray<FP_t> medianArray(countZ);
    DImageStore::iterator imageIterator;

    LOGMESSAGE(info, "Starting MEDIAN combine...");

    resultImage = inputImages.front()->createCopy();
    resultImage->BITPIX(DOUBLE_IMG);       // Change to double representation.
    dimX = resultImage->width();
    dimY = resultImage->height();

    if (countZ != 1)
    {
        // Must calculate the median of each point.

      for(indexX = 0; indexX < dimX; ++indexX)
      {
        for(indexY = 0; indexY < dimY; ++indexY)
        {
          indexZ = 0;
          for(imageIterator = inputImages.begin(); imageIterator != inputImages.end(); ++imageIterator)
          {
            medianArray[indexZ++] = (*imageIterator)->getValue(indexX, indexY);
          };

          resultImage->setValue(indexX, indexY, 0, MCL::median(medianArray));
        };
      };
    };

    LOGMESSAGE(info, "Completed MEDIAN combine.");
  }

  /// @brief Adds keywords or new information.
  /// @throws None.
  /// @version 2017-08-30/GGB - Function created.

  void CImageStack::keywordsAdd()
  {
    resultFile->keywordWrite(0, SBIG_IMAGETYP, IMAGETYP_SYNTHETIC, IMAGETYP_SYNTHETIC_COMMENT);
  }

  /// @brief Modify some keywords after the images have been combined.
  /// @details The Exposure keywords need to be added up for all the images to give the total exposure.
  /// @throws None.
  /// @version 2017-08-30/GGB - Function created.

  void CImageStack::keywordsModify()
  {
    FP_t totalExposure = 0;

    for (auto file : inputFiles)
    {
      totalExposure += file->astroFile->imageExposure(file->HDB);
    };

    resultFile->keywordDelete(0, NOAO_EXPTIME);
    resultFile->keywordDelete(0, HEASARC_EXPOSURE);

    resultFile->keywordWrite(0, HEASARC_EXPOSURE, totalExposure, SBIG_COMMENT_EXPTIME);
  }

  /// @brief Removes unneeded or incorrect keywords from the outputFile.
  /// @details The keywords that are removed are
  ///         @li FLIPSTAT
  ///         @li SET-TEMP
  ///         @li CCD-TEMP
  ///         @li XBINNING
  ///         @li YBINNING
  ///         @li CALSTAT
  /// @throws None.
  /// @version 2017-08-30/GGB - Function created.

  void CImageStack::keywordsRemove()
  {
    resultFile->keywordDelete(0, MAXIM_FLIPSTAT);
    resultFile->keywordDelete(0, MAXIM_SET_TEMP);
    resultFile->keywordDelete(0, MAXIM_CCD_TEMP);
    resultFile->keywordDelete(0, MAXIM_CALSTAT);
    resultFile->keywordDelete(0, SBIG_XBINNING);
    resultFile->keywordDelete(0, SBIG_YBINNING);
    resultFile->keywordDelete(0, MAXIM_SWMODIFY);
    resultFile->keywordDelete(0, MAXIM_SWOWNER);
    resultFile->keywordDelete(0, MAXIM_XORGSUBF);
    resultFile->keywordDelete(0, MAXIM_YORGSUBF);
    resultFile->keywordDelete(0, MAXIM_SWCREATE);
    resultFile->keywordDelete(0, MAXIM_POSITIONANGLE);
    resultFile->keywordDelete(0, MAXIM_PIERSIDE);
    resultFile->keywordDelete(0, MAXIM_CLRBAND);
    resultFile->keywordDelete(0, ITELESCOPE_TELESCOPE);
    resultFile->keywordDelete(0, SBIG_FOCALLEN);
    resultFile->keywordDelete(0, SBIG_APTDIA);
    resultFile->keywordDelete(0, SBIG_APTAREA);
    resultFile->keywordDelete(0, SBIG_SBSTDVER);
    resultFile->keywordDelete(0, FITS_TELESCOPE);
    resultFile->keywordDelete(0, FITS_INSTRUMENT);
    resultFile->keywordDelete(0, SBIG_IMAGETYP);
    resultFile->keywordDelete(0, SBIG_AIRMASS);
  }

  /// @brief This function registers the images in the stack.
  /// @details Registration consists of several steps:
  ///           1. Determine the reference angle for each image
  ///           2. Determine the rotation angle and translation amount for each image
  ///           3. Determine the scale amount for each image
  ///           4. Resample each image.
  ///           5. TRS the image.
  ///          The first image is always used as the reference image. All the other images are rotated/scaled to fit the first
  ///          image.
  /// @version 2014-12-26/GGB - Changed all uses of std::clog to GCL::logger classes and functions.
  /// @version 2013-03-02/GGB - Function created

  void CImageStack::registerImages()
  {
    MCL::TPoint2D<FP_t> refOrigen;
    FP_t refAngle;
    FP_t refDist;
    FP_t xdiff, ydiff;
    MCL::TPoint2D<FP_t> align1, align2;
    CAstroImage *newImage = nullptr;
    DStackImagesStore::size_type imageCounter = 1;

    GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Starting register images function...");
    GCL::logger::defaultLogger().logMessage(GCL::logger::info, std::to_string(inputFiles.size() - 1) + " images to register.");

    DStackImagesStore::iterator fileIterator = inputFiles.begin();

    refOrigen = (*fileIterator)->align1;
    xdiff = (*fileIterator)->align2.x() - (*fileIterator)->align1.x();
    ydiff = (*fileIterator)->align2.y() - (*fileIterator)->align1.y();

    refDist = std::sqrt(xdiff * xdiff + ydiff * ydiff);
    refAngle = std::acos(xdiff/refDist);

    if ( (xdiff == 0) || (ydiff == 0) )
    {
    }
    else if ( (xdiff > 0) && (ydiff > 0) )
    {	// Quadrant 1
      // No changes to ref angle required
    }
    else if ( (xdiff > 0) && (ydiff < 0) )
    {	// Quadrant 4
      refAngle = D2PI - refAngle;		// 360 - angle
    }
    else if ( (xdiff < 0) && (ydiff > 0) )
    {	// Quadrant 2
      refAngle = DPI - refAngle;				// 180 - angle
    }
    else if ( (xdiff < 0) && (ydiff < 0) )
    {	// Quadrant 3
      refAngle += DPI;
    };

    LOGMESSAGE(info, "Reference Image: " + (*fileIterator)->astroFile->getImageName() + ". Reference distance: " +
               std::to_string(refDist) + " Reference angle: " + std::to_string(refAngle) + ".");

      // Now have the reference origen, angle and distance.
      // For each of the images calculate the rotation angle.

    newImage = (*fileIterator)->astroFile->getAstroImage(0)->createCopy();
    //newImage->resampleImage(200);   // Resample image to 200%
    inputImages.push_back(newImage);

    fileIterator++;

    maskPlane.reset(new bool[newImage->width() * newImage->height()]);   // Masking plane
    for (INDEX_t index = 0; index < (newImage->width() * newImage->height()); ++index)
    {
      maskPlane[index] = false;
    };

    newImage = nullptr;

      // Transform each of the images by the required amount.

    for (; fileIterator != inputFiles.end(); fileIterator++)
    {
      LOGMESSAGE(info, "Processing image " + std::to_string(imageCounter++) + " of " + std::to_string(inputFiles.size() - 1) + ".");
      align1 = (*fileIterator)->align1;
      align2 = (*fileIterator)->align2;

      xdiff = align2.x() - align1.x();
      ydiff = align2.y() - align1.y();

      (*fileIterator)->dist = std::sqrt(MCL::pow2(xdiff) + MCL::pow2(ydiff));
      (*fileIterator)->th = std::acos(xdiff/(*fileIterator)->dist);

      if ( (xdiff == 0) || (ydiff == 0) )
      {
      }
      else if ( (xdiff > 0) && (ydiff > 0) )
      {	// Quadrant 1
        // No changes to ref angle required
      }
      else if ( (xdiff > 0) && (ydiff < 0) )
      {	// Quadrant 4
        (*fileIterator)->th = D2PI - (*fileIterator)->th;		// 360 - angle
      }
      else if ( (xdiff < 0) && (ydiff > 0) )
      {	// Quadrant 2
        (*fileIterator)->th = DPI - (*fileIterator)->th;				// 180 - angle
      }
      else if ( (xdiff < 0) && (ydiff < 0) )
      {	// Quadrant 3
        (*fileIterator)->th += DPI;
      };

      (*fileIterator)->dth = refAngle - (*fileIterator)->th;
      (*fileIterator)->sc = refDist / (*fileIterator)->dist;
      (*fileIterator)->tr = refOrigen - align1;

        // Write the parameters to GCL::logger

      LOGMESSAGE(info, "Image: " + (*fileIterator)->astroFile->getImageName() + " Distance: " +
                 std::to_string((*fileIterator)->dist) + " Angle: " + std::to_string((*fileIterator)->th) + " Translation: x:" +
                 std::to_string((*fileIterator)->tr.x()) +  " y:" + std::to_string((*fileIterator)->tr.y()) + " Rotation angle: " +
                 std::to_string((*fileIterator)->dth) + " Scale: " + std::to_string((*fileIterator)->sc));

        // Now do the TRS function on the image.

      newImage = (*fileIterator)->astroFile->getAstroImage(0)->createCopy();

      //newImage->resampleImage(200);   // Resample image to 200%
      newImage->transform(align1, (*fileIterator)->tr, (*fileIterator)->dth, (*fileIterator)->sc, MCL::TPoint2D<FP_t>(1,1), maskPlane);
      inputImages.push_back(newImage);
      newImage = nullptr;
    };

      // Now mask the outputImage.

    LOGMESSAGE(info, "Completed register images function.");
  }

  /// @brief Function to stack the images. This function makes use of a number of helper functions.
  /// @param[in] stackMode - The stack mode to use.
  /// @note All errors are returned as exceptions.
  /// @details This is the only external interface to stacking the images. All the options and switches must be set before entry to
  ///          this function.
  /// @throws GCL::CCodeError(ACL, 0x0400) - IMAGESTACK: No Images, or Images zero size.
  /// @throws GCL::CCodeError(ACL, 0x0401) - IMAGESTACK: Invalid stacking mode.
  /// @version 2014-12-26/GGB - Changed all uses of std::clog to GCL::logger classes and functions.
  /// @version 2013-03-02/GGB - Function created

  std::unique_ptr<CAstroFile> &CImageStack::stackImages(EStackMode stackMode)
  {
    if (resultImage)
    {
      delete resultImage;
      resultImage = nullptr;
    };

    if (stackMode == SM_NONE)
    {
      ACL_ERROR(0x0401);    // IMAGESTACK: Invalid stacking mode.
    };

    if (inputFiles.size() <= 1)
    {
      ACL_ERROR(0x0400);    // IMAGESTACK: No Images, or Images zero size.
    };

    LOGMESSAGE(info, "Starting function stackImages...");

    consistencyCheckImages();
    calibrateImages();
    registerImages();
    computeResult(stackMode);
    keywordsModify();
    keywordsRemove();
    keywordsAdd();

    resultFile->commentWrite(0, "IMAGE CREATED BY STACKING " + std::to_string(inputFiles.size()) + " IMAGES");
    for (auto file : inputFiles)
    {
      resultFile->commentWrite(0, "IMAGE: " + file->astroFile->getImageName());
    }

    LOGMESSAGE(info, "Completed function stackImages.");

    return resultFile;    // Also allows ownership transfer
  }

}  // namespace ACL
