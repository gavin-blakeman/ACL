//**********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroImageCalibration.h
// SUBSYSTEM:						Astronomical Image Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
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
// OVERVIEW:
//
// CLASSES INCLUDED:
//                      CMasterFrame   - Virtual base class for
//                                            calibration frames.
//                      CMasterDarkFrame    - A Class for creating master dark
//                                            frames from dark frames and
//                                            optionally bias frames.
//                      CMasterFlatFrame    - A class for creating master flat
//                                            frames from flat frames.
//                      CMasterBiasFrame    - A class for creating master bias
//                                            frames from flat frames.
//                      CCalibratedFrame    - A class to calibrate an image
//                                            frame by applyins bias, flat and
//                                            dark calibrations to the image.
//
// CLASS HIERARCHY:
//                      CMasterFrame
//                        - CMasterDarkFrame
//                        - CMasterFlatFrame
//                        - CMasterBiasFrame
//                        - CCalibratedFrame
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//
//**********************************************************************************************************************************

#include "../Include/AstroImageCalibration.h"

namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CMasterFrame
  //
  //*******************************************************************************************************************************

  /// @brief Constructor for the class.
  /// @param[in] ofn - Name of the output file.
  /// @throws None
  /// @version 2011-05-21/GGB - Function created.

  CMasterFrame::CMasterFrame(std::string const &ofn) : outputFileName(ofn), masterFrame(nullptr), masterFrameValid(false)
  {
  }

  /// @brief Class destructor. Ensures that all owned objects are correctly deleted when the class is destroyed.
  /// @throws None
  /// @version 2011-05-21/GGB - Function created.

  CMasterFrame::~CMasterFrame()
  {
    if (masterFrame)
    {
      delete masterFrame;
      masterFrame = nullptr;
    };
  }

  /// @brief Procedure to create the master frame. Calls the relevant (virtual) funtions
  /// @param[in] mode - The mode to use for the combination of the frames.
  /// @throws None.
  /// @version 2011-05-15/GGB - Function created.

  void CMasterFrame::createMasterFrame(EImageCombine mode)
  {
    switch (mode)
    {
    case EIC_MEAN:
      meanCombine();
      break;
    case EIC_MEDIAN:
      medianCombine();
      break;
    default:
      ACL_CODE_ERROR;
    };
  }

  /// @brief Common procedure to test that a sequence of frames have the same characteristics.
  /// @param[in] frames - The list of frames to verify.
  /// @param[in] iter - The iterator for the frames.
  /// @details Checks that the following details are the same
  ///           1. naxis
  ///           2. naxisn
  ///           3. exposure.
  ///           4. ... may be extended to other parameters such as temperature etc.
  /// @version 2017-08-10/GGB - Remove iterator parameter.
  /// @version 2011-05-17/GGB - Function created.

  bool CMasterFrame::verifyFrames(std::vector<std::string> const &frames) const
  {
    NAXIS_t naxis;
    AXIS_t naxis1;
    AXIS_t naxis2;
    FP_t exptime, dExptime;
    bool bExptime = false;
    bool bnaxis = false;
    bool bnaxisn = false;
    bool bExptimeValid = false;
    PAstroFile astroFrame;
    CFITSKeyword *keywordData = nullptr;
    DStringVector::const_iterator iter;

    iter = frames.begin();
    astroFrame.reset(new CAstroFile());
    astroFrame->loadFromFile(*iter);

    naxis = astroFrame->NAXIS(0);
    naxis1 = astroFrame->NAXISn(0, 1);
    naxis2 = astroFrame->NAXISn(0, 2);

    if ( astroFrame->keywordExists(0, NOAO_EXPTIME) )
    {
      exptime = static_cast<FP_t>(astroFrame->keywordData(0, NOAO_EXPTIME));
      bExptimeValid = true;
    }
    else if ( astroFrame->keywordExists(0, HEASARC_EXPOSURE) )
    {
      exptime = static_cast<FP_t>(astroFrame->keywordData(0, HEASARC_EXPOSURE));
      bExptimeValid = true;
    }
    else
      bExptimeValid = false;

    iter++;

    while ( (iter != frames.end()) && !bExptime && !bnaxis && !bnaxisn )
    {
      astroFrame.reset(new CAstroFile());
      astroFrame->loadFromFile(*iter);        // load the keywords

      if (naxis != astroFrame->NAXIS(0))
      {
        ACL_ERROR(0x2100);
      }
      else if (naxis1 != astroFrame->NAXISn(0, 1))
      {
        ACL_ERROR(0x2101);
      }
      else if (naxis2 != astroFrame->NAXISn(0, 2))
      {
        ACL_ERROR(0x2101);
      }
      else if (bExptimeValid)
      {
        if ( astroFrame->keywordExists(0, NOAO_EXPTIME))
        {
          dExptime = static_cast<FP_t>(astroFrame->keywordData(0, NOAO_EXPTIME));
        }
        else
        {
          dExptime = static_cast<FP_t>(astroFrame->keywordData(0, HEASARC_EXPOSURE));
        };

        if (dExptime != exptime)
        {
          ACL_ERROR(0x2102);
        }
      };

      iter++;
    };
    return true;
  }

  /// @brief Performs the save action for the Bias frame.
  //
  // 2011-05-22/GGB - Function created.

  void CMasterFrame::saveToFile(boost::filesystem::path const &fileName) const
  {
    masterFrame->save(fileName);
  }

  //********************************************************************************************************************************
  //
  // CMasterDarkFrame
  //
  // The CMasterDarkFrame class supports the loading of the individual images making up a dark frame, checking that the images are
  // consistent, (same size, same temperature, same date) and also the reduction of the individual images into a dark frame.
  //
  //********************************************************************************************************************************

  // Default constructor.
  //
  // 2011-05-21/GGB - Function created.

  CMasterDarkFrame::CMasterDarkFrame() : CMasterFrame(), masterBiasFile(NULL)
  {
  }

  // Constructor providing an output file name.
  //
  // 2011-05-08/GGB - Function created.

  CMasterDarkFrame::CMasterDarkFrame(std::string const &opf) : CMasterFrame(opf), masterBiasFile(NULL)
  {
  }

  // Adds a new file name into the list of file names.
  //
  // 2010-12-11/GGB - Function created.

  void CMasterDarkFrame::addDarkFrame(std::string const &toAdd)
  {
    darkFrames.push_back(toAdd);
  }

  // Returns the master dark frame.
  // If the master dark frame is not valid then NULL is returned.
  //
  // 2011-05-21/GGB - Function created.

  CAstroFile *CMasterDarkFrame::getMasterDarkFile()
  {
    return masterFrame;
  }

  /// @brief Combines the frames in a median fashion.
  /// @throws
  /// @version 2011-05-08/GGB - Function created.

  void CMasterDarkFrame::meanCombine()
  {
    CAstroImage *masterDarkImage;
    PAstroFile darkFrame;
    CAstroImage *biasImage;
    boost::filesystem::path filePath;

    darkFrameIterator = darkFrames.begin();

    darkFrame.reset(new CAstroFile());
    darkFrame->loadFromFile(*darkFrameIterator);    // Load the file
    masterDarkImage = darkFrame->getAstroImage(0)->createCopy();

    darkFrameIterator++;

    while (darkFrameIterator != darkFrames.end() )
    {
      darkFrame.reset(new CAstroFile());
      darkFrame->loadFromFile(*darkFrameIterator);    // Load the file

        // Add the new image to the results image

      (*masterDarkImage) += *(darkFrame->getAstroImage(0));

      darkFrameIterator++;
    };

    if (masterBiasFile)
    {
        // This works as a mean combine is a commutative type of combine.

      biasImage = masterBiasFile->getAstroImage(0)->createCopy();

      (*biasImage) *= darkFrames.size();
      (*masterDarkImage) -= (*biasImage);
    };

      // All the images are now combined into the resultImage.

    (*masterDarkImage) /= darkFrames.size();      // Calculate the average.

      // The new object takes control of the astro image.

    if (!masterFrame)
    {
      masterFrame = new CAstroFile(masterDarkImage);
    }
    else
    {
      masterFrame->setAstroImage(0, masterDarkImage);
    }

    masterDarkImage = nullptr;

      // Add the filenames to the comments.

    if (!masterBiasFile)
    {
      masterFrame ->commentWrite(0, std::string("MASTER DARK FRAME CONSTRUCTED USING BASIC PROTOCOL"));
    }
    else
    {
      masterFrame->commentWrite(0, std::string("MASTER DARK FRAME CONSTRUCTED USING ADVANCED PROTOCOL"));
      filePath = masterBiasFile->getImageName();
      masterFrame->commentWrite(0, std::string("BIAS FRAME: ") + filePath.filename().string());    // Drop the paths and only include the filename.
    };

      // Add all the filenames to the comments.

    for (darkFrameIterator = darkFrames.begin(); darkFrameIterator != darkFrames.end(); darkFrameIterator++)
    {
      filePath = (*darkFrameIterator);
      masterFrame->commentWrite(0, std::string("DARK FRAME: ") + filePath.filename().string());    // Drop the paths and only include the filename.
    };
  }

  // Combines the frames in a median fashion.
  //


  void CMasterDarkFrame::medianCombine()
  {
  }

  // Returns the inconsistent file name. (Where the initial fault occurred.
  // If there was no fault, this function will throw an error.
  //
  // 2011-05-08/GGB - Function created.

  std::string const &CMasterDarkFrame::inconsistentFrame()
  {
    return ( *darkFrameIterator );
  }

  // Sets the master bias file to the new value.
  //
  // 2011-05-22/GGB - Function created.

  void CMasterDarkFrame::setMasterBiasFile(CAstroFile *newFile)
  {
    masterBiasFile = newFile;
  }

  // Procedure to set the master dark frame with a specified master dark frame.
  // The class takes ownership of the new master dark frame and will delete it on destructor.
  // If the newImage == NULL, then the old image is not deleted, but is passed out as the
  // return value. Otherwise the return value is NULL.
  //
  // 2011-05-21/GGB - Function created.

  CAstroFile *CMasterDarkFrame::setMasterDarkFile(CAstroFile *newImage)
  {
    CAstroFile *retVal = nullptr;

    if (newImage == nullptr)
      retVal = masterFrame;
    else
    {
      retVal = nullptr;
      if (masterFrame)
        delete masterFrame;
    };

    masterFrame = newImage;

    return retVal;
  }

  /// @brief Checks each of the files that have been passed as dark frames. Uses the common CMasterFrame routine.
  /// @throws
  /// @version 2011-05-17/GGB - Function created.

  bool CMasterDarkFrame::verifyFrames()
  {
    return CMasterFrame::verifyFrames(darkFrames);
  }

  //***************************************************************************
  //
  // CMasterFlatFrame
  //
  //***************************************************************************

  // Default constructor for the class.
  //
  // 2011-05-16/GGB - Function created.

  CMasterFlatFrame::CMasterFlatFrame(std::string const &nfp) : CMasterFrame(nfp), masterFlatDarkFile(NULL)
  {
  }

  // Adds the file name to the end of the vector.
  //
  // 2011-05-16/GGB - Function created.

  void CMasterFlatFrame::addFlatFrame(std::string const &toAdd)
  {
    flatFrames.push_back(toAdd);
  }

  // Creates the master flat using a median combine.
  //
  // 2013-06-09/GGB - Change to smart pointers for astroFile.
  // 2011-05-19/GGB - Function created.

  void CMasterFlatFrame::meanCombine()
  {
    CAstroImage *masterFlatImage = nullptr;
    CAstroImage *masterDarkImage = nullptr;
    PAstroFile flatFrame;
    boost::filesystem::path filePath;

    flatFrameIterator = flatFrames.begin();

    flatFrame.reset(new CAstroFile());
    flatFrame->loadFromFile(*flatFrameIterator);    // Load the file

    masterFlatImage =  flatFrame->getAstroImage(0)->createCopy();

    flatFrameIterator++;

    while (flatFrameIterator != flatFrames.end() )
    {
      flatFrame.reset(new CAstroFile());
      flatFrame->loadFromFile(*flatFrameIterator);    // Load the file

        // Add the new image to the results image

      (*masterFlatImage) += *(flatFrame->getAstroImage(0));

      flatFrameIterator++;
    };

      // All the images are now combined into the resultImage.

    (*masterFlatImage) /= flatFrames.size();      // Calculate the average.

    if (masterFlatDarkFile)
      (*masterFlatImage ) -= (*masterFlatDarkFile->getAstroImage(0));

      // Now save the flat frame

    if (masterFrame)
    {
      masterFrame->setAstroImage(0, masterFlatImage);
    }
    else
    {
      masterFrame = new CAstroFile(masterFlatImage);
    };

    masterFlatImage = nullptr;

      // Add the filenames to the comments.

    for (flatFrameIterator = flatFrames.begin(); flatFrameIterator != flatFrames.end(); flatFrameIterator++)
    {
      filePath = (*flatFrameIterator);
      masterFrame->commentWrite(0, std::string("FLAT FRAME: ") + filePath.filename().string());    // Drop the paths and only include the filename.
    };

    if (masterFlatDarkFile)
    {
      masterFrame->commentWrite(0, std::string("DARK FRAME: ") + masterFlatDarkFile->getImageName());
    }
  }

  void CMasterFlatFrame::medianCombine()
  {
  }

  // Sets the value of masterFlatDarkFile
  //
  // 2011-05-22/GGB - Function created.

  void CMasterFlatFrame::setMasterFlatDarkFile(CAstroFile *newFile)
  {
    masterFlatDarkFile = newFile;
  }

  // Verifys the flat frames. Uses the common routine to verify the group of frames.
  //
  // 2011-05-17/GGB - Function created.

  bool CMasterFlatFrame::verifyFrames()
  {
    return CMasterFrame::verifyFrames(flatFrames);
  }

  //*******************************************************************************************************************************
  //
  // CMasterBiasFrame
  //
  //*******************************************************************************************************************************

  // Default constructor for the class.
  //
  // 2011-05-21/GGB - Function created.

  CMasterBiasFrame::CMasterBiasFrame() : CMasterFrame()
  {
  }

  // Constructor for the class.
  //
  // 2011-05-17/GGB - Function created.

  CMasterBiasFrame::CMasterBiasFrame(std::string const &np) : CMasterFrame(np)
  {
  }

  // Adds the bias frame filename to the list.
  //
  // 2011-05-21/GGB - Function created.

  void CMasterBiasFrame::addBiasFrame(const std::string &toAdd)
  {
    biasFrames.push_back(toAdd);
  }

  // Performs a mean combination of the frames.
  // This does not save the astrofile at the end of the process.
  //
  // 2013-06-09/GGB - Use smart pointers for astroFile.
  // 2011-05-22/GGB - Function created.

  void CMasterBiasFrame::meanCombine()
  {
    PAstroFile biasFrame;
    CAstroImage *biasImage;
    boost::filesystem::path filePath;

    biasFrameIterator = biasFrames.begin();

    biasFrame.reset(new CAstroFile());
    biasFrame->loadFromFile(*biasFrameIterator);    // Load the file

    biasImage =  biasFrame->getAstroImage(0)->createCopy();

    biasFrameIterator++;

    while (biasFrameIterator != biasFrames.end() )
    {
      biasFrame.reset(new CAstroFile());
      biasFrame->loadFromFile(*biasFrameIterator);    // Load the file

        // Add the new image to the results image

      (*biasImage) += *(biasFrame->getAstroImage(0));

      biasFrameIterator++;
    };

      // All the images are now combined into the resultImage.

    (*biasImage) /= biasFrames.size();      // Calculate the average.

      // Now save the bias frame

    if (!masterFrame)
    {
       masterFrame = new CAstroFile(biasImage);
    }
    else
    {
      masterFrame->setAstroImage(0, biasImage);
    };

    biasImage = nullptr;

      // Add the filenames to the comments.

    for (biasFrameIterator = biasFrames.begin(); biasFrameIterator != biasFrames.end(); biasFrameIterator++)
    {
      filePath = (*biasFrameIterator);
      masterFrame->commentWrite(0, std::string("BIAS FRAME: ") + filePath.filename().string());    // Drop the paths and only include the filename.
    };
  }

  void CMasterBiasFrame::medianCombine()
  {
  }

  // Procedure to set the master dark frame with a specified master dark frame.
  // The class takes ownership of the new master dark frame and will delete it on destructor.
  // If the newImage == NULL, then the old image is not deleted, but is passed out as the
  // return value. Otherwise the return value is NULL.
  //
  // 2011-05-22/GGB - Function created.

  CAstroFile *CMasterBiasFrame::setMasterBiasFile(CAstroFile *newImage)
  {
    CAstroFile *retVal = nullptr;

    if (newImage == NULL)
      retVal = masterFrame;
    else
    {
      retVal = NULL;
      if ( masterFrame )
        delete masterFrame;
    };

    masterFrame = newImage;

    return retVal;
  }

  // Verifys the bias frames. Uses the common routine to verify the group of frames.
  //
  // 2011-05-17/GGB - Function created.

  bool CMasterBiasFrame::verifyFrames()
  {
    return CMasterFrame::verifyFrames(biasFrames);
  }

  //*******************************************************************************************************************************
  //
  // CCalibrateFrame
  //
  //*******************************************************************************************************************************

  // Class constructor.
  //
  // 2011-05-25/GGB - Function created.

  CCalibrateFrame::CCalibrateFrame() : darkFrame(""), flatFrame(""), biasFrame("")
  {
  }

  // Adds a new frame to the list of frames to be calibrated.
  //
  // 2011-05-25/GGB - Function created.

  void CCalibrateFrame::addFrame(std::string const &toAdd)
  {
    frameList.push_back(toAdd);
  }

  // Calibrates a single frame
  //
  // 2011-05-27/GGB - Function created.

  void CCalibrateFrame::calibrateFrame(CAstroFile *cFile, CAstroFile *dFile, CAstroFile *fFile, CAstroFile *bFile,
    CAstroFile *oFile) const
  {
    CAstroImage *oImage;
    CAstroImage *sdImage;

    oImage = cFile->getAstroImage(0);

    if (dFile)
    {
      if (!bFile)
      {
          // Non-scaleable dark

        (*oImage) += *(dFile->getAstroImage(0));
      }
      else
      {
            // Dark must be scaled as required.

        (*oImage) -= (*sdImage);
      };
    };

    //if (fFile)
      //(*oImage) /= *(fFile->getAstroImage(0));

    oFile->setAstroImage(0, oImage);

    oImage = NULL;

    if (sdImage)
    {
      delete sdImage;
      sdImage = NULL;
    };
  };

  /// @brief Calibrates the frames.
  //
  // 2011-05-25/GGB - Function created.

  void CCalibrateFrame::calibrateFrames()
  {
    std::vector<std::string>::const_iterator iter;
    CAstroFile *darkFile = NULL;
    CAstroFile *flatFile = NULL;
    CAstroFile *biasFile = NULL;
    CAstroFile *calibrationFile = NULL;
    CAstroFile *outputFile = NULL;
    unsigned int newHDB;

    boost::filesystem::path outputFilePath;
    boost::filesystem::path outputFileDirectory;
    boost::filesystem::path outputFileName;
    boost::filesystem::path renameFileName;

    if ( !darkFrame.empty() )
    {
        // Load the darkFrame

      darkFile = new CAstroFile();
      darkFile->loadFromFile(darkFrame);
    };

    if ( !flatFrame.empty() )
    {
        // Load the flat frame

      flatFile = new CAstroFile();
      flatFile->loadFromFile(flatFrame);
    };

    if ( !biasFrame.empty() )
    {   // Load the bias frame
      biasFile = new CAstroFile();
      biasFile->loadFromFile(biasFrame);
    };

      // Iterate the list and calibrate each frame at a time.

    for (iter = frameList.begin(); iter != frameList.end(); iter++)
    {
      try
      {
        calibrationFile = new CAstroFile();
        calibrationFile->loadFromFile(*iter);
        verifyFrames(calibrationFile, darkFile, flatFile, biasFile);

        outputFile = new CAstroFile();    // Create without output filename.

        calibrateFrame(calibrationFile, darkFile, flatFile, biasFile, outputFile);

          // Does the output file exist.

        if ( outputDirectory.empty() )
        {
            // No output directory, we are saving over the original file names.

          if (bOverwrite)
            {
              boost::filesystem::remove(outputFileName);
            }
            else
            {
                  // Need to back the file up (rename to .bak)

                renameFileName = outputFileName;
                renameFileName.replace_extension( ".bak" );
                if ( boost::filesystem::exists(renameFileName) )
                {
                    // Delete the existing file.

                  boost::filesystem::remove(renameFileName);
                };

                  // Rename the exisiting file name to the backup filename.

                boost::filesystem::rename(outputFileName, renameFileName);
            };
        }
        else
        {
          // Check that we are saving in a new directory.

          outputFilePath = (*iter);

          outputFileName = boost::filesystem::path(outputDirectory + outputFilePath.filename().string());   // This is the total output path + filename.


          outputFileDirectory = outputDirectory;
          if ( outputFilePath.root_path() == outputFileDirectory.root_directory() )
          {
              // Saving in the same directory

            if (bOverwrite)
            {
              boost::filesystem::remove(outputFileName);
            }
            else
            {
                  // Need to back the file up (rename to .bak)

                renameFileName = outputFileName;
                renameFileName.replace_extension( ".bak" );
                if ( boost::filesystem::exists(renameFileName) )
                {
                    // Delete the existing file.

                  boost::filesystem::remove(renameFileName);
                };

                  // Rename the exisiting file name to the backup filename.

                boost::filesystem::rename(outputFileName, renameFileName);
            };
          }
          else
          {
              // Saving in a new directory

            if ( boost::filesystem::exists(outputFileName) )
            {
                // The file already exists

              if (bOverwrite)
              {
                  // Delete the existing file

                boost::filesystem::remove(outputFileName);
              }
              else
              {
                  // Need to back the file up (rename to .bak)

                renameFileName = outputFileName;
                renameFileName.replace_extension( ".bak" );
                if ( boost::filesystem::exists(renameFileName) )
                {
                    // Delete the existing file.

                  boost::filesystem::remove(renameFileName);
                };

                  // Rename the exisiting file name to the backup filename.

                boost::filesystem::rename(outputFileName, renameFileName);

              };
            };
          };
        };

        if (bSaveOriginal)
        {
            // Save the original image as an extra HDB.

          //newHDB = outputFile->addHDB(calibrationFile->getHDB(0));

            // Set the keywords to indicate that this is the original frame.


            // Save the Dark Frame as the 3rd HDB

          if (darkFile)
          {
            //newHDB = outputFile->addHDB(darkFile->getHDB(0));
          };

            // Save the Flat frame as the 4th HDB

          if (flatFile)
          {
            //newHDB = outputFile->addHDB(flatFile->getHDB(0));
          };

            // Save the bias frame as the 5th HDB.

          if (biasFile)
          {
            //newHDB = outputFile->addHDB(biasFile->getHDB(0));
          };
        };

        outputFile->save(outputFileName);

        delete outputFile;
        outputFile = nullptr;

        delete calibrationFile;
        calibrationFile = nullptr;

      }
      catch (...)
      {
        throw;
      };
    };    // for

      // Cleanup

    if (darkFile)
    {
      delete darkFile;
      darkFile = nullptr;
    };
    if (flatFile)
    {
      delete flatFile;
      flatFile = nullptr;
    };
    if (biasFile)
    {
      delete biasFile;
      biasFile = nullptr;
    };
  }

  // Sets the bias frame name.
  //
  // 2011-05-25/GGB - Function created.

  void CCalibrateFrame::setBiasFrame(std::string const &newBias)
  {
    biasFrame = newBias;
  }

  // Sets the dark frame name.
  //
  // 2011-05-25/GGB - Function created.

  void CCalibrateFrame::setDarkFrame(std::string const &newDark)
  {
    darkFrame = newDark;
  }

  // Allows the calling function to set the flags before the calibration is done.
  //
  // 2011-06-04/GGB - Function created.

  void CCalibrateFrame::setFlags(bool SaveOriginal, bool BackupExisting, bool Overwrite)
  {
    bSaveOriginal = SaveOriginal;
    bBackupExisting = BackupExisting;
    bOverwrite = Overwrite;
  }

  // Sets the flat frame name.
  //
  // 2011-05-25/GGB - Function created.

  void CCalibrateFrame::setFlatFrame(std::string const &newFlat)
  {
    flatFrame = newFlat;
  }

  // Verifys that the frames are all compatible.
  // 1. All have the same number of axes
  // 2. All have the same axis dimensions
  // 3. All come from the same imager.
  //
  // 2011-05-27/GGB - Function created.

  bool CCalibrateFrame::verifyFrames(CAstroFile *cFile, CAstroFile *dFile, CAstroFile *fFile, CAstroFile *bFile) const
  {
    NAXIS_t naxis = 0;
    std::vector<AXIS_t> naxisn;
    NAXIS_t index;
    bool retVal = true;

      // Get the initial information

    naxis = cFile->NAXIS(0);

    for (index = 0; index < naxis; index++)
      naxisn.push_back(cFile->NAXISn(0, index));

      // Now check the other frames.

    if (dFile)
    {
      if (dFile->NAXIS(0) != naxis)
        retVal = false;
      for (index = 0; index < naxis; index++)
        if (naxisn[index] != dFile->NAXISn(0, index))
          retVal = false;
    };

    if (cFile && retVal)
    {
      if (cFile->NAXIS(0) != naxis)
        retVal = false;
      for (index = 0; index < naxis; index++)
        if (naxisn[index] != cFile->NAXISn(0, index))
          retVal = false;
    };

    if (bFile && retVal)
    {
      if (bFile->NAXIS(0) != naxis)
        retVal = false;
      for (index = 0; index < naxis; index++)
        if (naxisn[index] != bFile->NAXISn(0, index))
          retVal = false;
    };

    return retVal;
  }

}  // namespace ACL
