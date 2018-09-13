//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								ImageRegister.cpp
// SUBSYSTEM:						Class for registering images
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost, GCL, MCL.
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

#include "../include/ImageRegister.h"

  // Standard C++ library header files.

#include <string>

  // Miscellaneous library header files.

#include "sofam.h"

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // CRegisterImageInformation
  //
  //*******************************************************************************************************************************

  /// @brief Default constructor for the class.
  /// @throws none.
  /// @version 2013-06-09/GGB - Changed to use smart pointers and added outputFile.
  /// @version 2011-07-07/GGB - Function created

  CRegisterImageInformation::CRegisterImageInformation() : inputFile(), outputFile(), HDB(0), mask(nullptr), align1(),
    align2(), dist(0), th(0), tr(0, 0), dth(0), sc(1), xMax(0), yMax(0), x0(0), y0(0), pixSize(1,1)
  {
  }

  /// @brief Constructor for the class.
  /// @param[in] af:
  /// @param[in] hdb:
  /// @param[in] a1:
  /// @param[in] a2:
  /// @throws None.
  /// @version 2013-06-09/GGB - Changed astroFile to a smart pointer.
  /// @version 2011-07-07/GGB - Function created

  CRegisterImageInformation::CRegisterImageInformation(std::shared_ptr<CAstroFile> af, std::vector<CHDB>::size_type hdb,
                                                       MCL::TPoint2D<FP_t> const &a1, MCL::TPoint2D<FP_t> const &a2) :
    inputFile(af), outputFile(), HDB(hdb), mask(nullptr), align1(a1), align2(a2), dist(0), th(0), tr(0, 0), dth(0), sc(1),
    xMax(0), yMax(0), x0(0), y0(0), pixSize(1,1)
  {
  }

  /// @brief Copy constructor.
  /// @throws None.
  /// @version 2018-09-13/GGB - Updated outputFile to be a std::unique_ptr.
  /// @version 2013-08-04/GGB - Function created.

  CRegisterImageInformation::CRegisterImageInformation(CRegisterImageInformation const &toCopy) : inputFile(toCopy.inputFile),
    outputFile(toCopy.outputFile->createCopy()), HDB(toCopy.HDB), mask(toCopy.mask), align1(toCopy.align1), align2(toCopy.align2),
    dist(toCopy.dist), th(toCopy.th), tr(toCopy.tr), dth(toCopy.dth), sc(toCopy.sc), xMax(toCopy.xMax), yMax(toCopy.yMax),
    x0(toCopy.x0), y0(toCopy.y0), pixSize(toCopy.pixSize)
  {
  }

  /// @brief Assignment operator for the class.
  /// @param[in] rhs: The value to copy.
  /// @returns (*this) The value of rhs copied to *this.
  /// @throws None.
  /// @version 2018-09-13/GGB - Function created.

  CRegisterImageInformation &CRegisterImageInformation::operator=(CRegisterImageInformation rhs)
  {
    swap(*this, rhs);

    return *this;
  }

  /// @brief Swaps the two objects.
  /// @param[in] first: The first instance.
  /// @param[in] second: The second paramter.
  /// @throws None.
  /// @version 2018-09-13/GGB - Function created.

  void swap(CRegisterImageInformation &first, CRegisterImageInformation &second)
  {
    std::swap(first.inputFile, second.inputFile);
    std::swap(first.outputFile, second.outputFile);
    std::swap(first.HDB, second.HDB);
    std::swap(first.mask, second.mask);
    std::swap(first.align1, second.align1);
    std::swap(first.align2, second.align1);
    std::swap(first.dist, second.dist);
    std::swap(first.th, second.th);
    std::swap(first.tr, second.tr);
    std::swap(first.dth, second.dth);
    std::swap(first.sc, second.sc);
    std::swap(first.xMax, second.xMax);
    std::swap(first.yMax, second.yMax);
    std::swap(first.x0, second.x0);
    std::swap(first.y0, second.y0);
    std::swap(first.pixSize, second.pixSize);
  }

  //*******************************************************************************************************************************
  //
  // CRegisterImages
  //
  //*******************************************************************************************************************************

  /// @brief Class constructor.
  /// @throws None.
  /// @version 2011-07-07/GGB - Function created.

  CRegisterImages::CRegisterImages() : imageStore()
  {
  }

  /// @brief Adds an image to the list of images.
  /// @param[in] astroFile: The astrofile to add to the list.
  /// @param[in] hdb: The HDB number to add to the list.
  /// @param[in] alignPoint1: The first alignment point.
  /// @param[in] alignPoint2: The second alignment point.
  /// @throws None.
  /// @version 2013-06-09/GGB - Changed astroFile to a smart pointer.
  /// @version 2011-08-27/GGB - Function created

  void CRegisterImages::addImage(PAstroFile astroFile, std::vector<CHDB>::size_type hdb, MCL::TPoint2D<FP_t> const &alignPoint1,
    MCL::TPoint2D<FP_t> const &alignPoint2)
  {
    std::shared_ptr<CRegisterImageInformation> newInformation(new CRegisterImageInformation(astroFile, hdb, alignPoint1, alignPoint2) );

    imageStore.push_back(newInformation);
  }

  /// @brief Clears all the images from the list of images.
  /// @throws None.
  /// @version 2011-07-07/GGB - Function created.

  void CRegisterImages::clearImages()
  {
    imageStore.clear();
  }

  /// Returns a reference to the image store.
  //
  // 2013-06-09/GGB - Function created.

  DRegisterImageStore const &CRegisterImages::getImages() const
  {
    return imageStore;
  }

  /// Returns the number of images stored.
  //
  // 2012-01-25/GGB - Function created.

  size_t CRegisterImages::imageCount() const
  {
    return imageStore.size();
  }

  /// @brief This function registers the images in the stack.
  /// @throws None.
  /// Registration consists of several steps:
  /// 1. Determine the reference angle for each image
  /// 2. Determine the rotation angle and translation amount for each image
  /// 3. Determine the scale amount for each image
  /// 4. TRS the image.
  /// The first image is always used as the reference image. All the other images are rotated/scaled to fit the first image.
  /// @version 2012-06-03/GGB - Function created

  void CRegisterImages::registerImages()
  {
    MCL::TPoint2D<FP_t> refOrigen;
    FP_t refAngle;
    FP_t refDist;
    FP_t xdiff, ydiff;
    MCL::TPoint2D<FP_t> align1, align2;
    DRegisterImageStore::size_type imageCounter = 1;
    std::unique_ptr<bool> maskPlane;

    GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Starting register images function...");
    GCL::logger::defaultLogger().logMessage(GCL::logger::info,
                                            std::to_string(imageStore.size() - 1) + " images to register.");

    DRegisterImageStore::iterator fileIterator = imageStore.begin();

    refOrigen = (*fileIterator)->align1;
    xdiff = (*fileIterator)->align2.x() - (*fileIterator)->align1.x();
    ydiff = (*fileIterator)->align2.y() - (*fileIterator)->align1.y();

    refDist = std::sqrt(xdiff * xdiff + ydiff * ydiff);
    refAngle = std::acos(xdiff/refDist);

    if ( (xdiff == 0) || (ydiff == 0) )
    {
      // Special case. No further action.
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

    GCL::logger::defaultLogger().logMessage(GCL::logger::info,
                                            "Reference Image: " + (*fileIterator)->inputFile->getImageName() + ". ");
    GCL::logger::defaultLogger().logMessage(GCL::logger::info,
                                            "Reference distance: " + std::to_string(refDist) + " Reference angle: " + std::to_string(refAngle));

      // Now have the reference origen, angle and distance.
      // For each of the images calculate the rotation angle.

      // Note the following line is non-intuitive. As the returned value is a copy and not a reference, it cannot
      // be used as the argument of the swap function. However the resultFile can be used as a reference, so
      // to enable compilation, the reversal of logic is required.
      // The outcome is the same as the two pointers are swapped and when the copy goes out of scope, it should be
      // destructed.

    (*fileIterator)->inputFile->createCopy().swap((*fileIterator)->outputFile);
    (*fileIterator)->dist = refDist;
    (*fileIterator)->th = refAngle;
    (*fileIterator)->dth = 0;
    (*fileIterator)->sc = 1;
    (*fileIterator)->tr = MCL::TPoint2D<FP_t>(0, 0);

    fileIterator++;

    for (; fileIterator != imageStore.end(); fileIterator++)
    {
      GCL::logger::defaultLogger().logMessage(GCL::logger::info,
                                              "Processing image " + std::to_string(imageCounter++) + " of " + std::to_string(imageStore.size() - 1));
      align1 = (*fileIterator)->align1;
      align2 = (*fileIterator)->align2;

      xdiff = align2.x() - align1.x();
      ydiff = align2.y() - align1.y();

      (*fileIterator)->dist = sqrt(xdiff * xdiff + ydiff * ydiff);
      (*fileIterator)->th = acos(xdiff/(*fileIterator)->dist);

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

        // Write the parameters to the logger

      GCL::logger::defaultLogger().logMessage(GCL::logger::info,
                                              "Image: " + (*fileIterator)->inputFile->getImageName() + " Distance: " +
                                              std::to_string((*fileIterator)->dist) + " Angle: " +
                                              std::to_string((*fileIterator)->th) + " Translation: x:" +
                                              std::to_string((*fileIterator)->tr.x()) + " y:" +
                                              std::to_string((*fileIterator)->tr.y()) + " Rotation angle: " +
                                              std::to_string((*fileIterator)->dth) + " Scale: " +
                                              std::to_string((*fileIterator)->sc));

        // Now do the TRS function on the image.

        // Note the following line is non-intuitive. As the returned value is a copy and not a reference, it cannot
        // be used as the argument of the swap function. However the resultFile can be used as a reference, so
        // to enable compilation, the reversal of logic is required.
        // The outcome is the same as the two pointers are swapped and when the copy goes out of scope, it should be
        // destructed.

      (*fileIterator)->inputFile->createCopy().swap((*fileIterator)->outputFile);
      (*fileIterator)->outputFile->TRS((*fileIterator)->HDB, align1, (*fileIterator)->tr, (*fileIterator)->dth,
                                       (*fileIterator)->sc, MCL::TPoint2D<FP_t>(1,1), maskPlane);
    };

    GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Completed register images function.");
  }

}  // namespace ACL
