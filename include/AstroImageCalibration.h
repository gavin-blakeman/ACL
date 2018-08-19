//*********************************************************************************************************************************
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
// OVERVIEW:						This subsystem provides a number of classes for calibrating images. These helper classes provide the working
//                      classes to create masterFrames when provided the file names, as well as to calibrate images when provided
//                      the appropriate information.
//                      The classes are all low-level classes and provide no user-interaction functionallity. They are all
//                      designed to be called from the user-interaction classes/functions.
//                      These classes make extensive use of the classes provided in the AstroImaging file and form an integral
//                      part of the ACL library.
//
// CLASSES INCLUDED:	  CMasterFrame        - Virtual base class for calibration frames.
//                      CMasterDarkFrame    - A Class for creating master dark frames from dark frames and optionally bias frames.
//                      CMasterFlatFrame    - A class for creating master flat frames from flat frames.
//                      CMasterBiasFrame    - A class for creating master bias frames from flat frames.
//                      CCalibrateFrame     - A class to calibrate an image frame by applying bias, flat and dark calibrations to
//                                            the image.
//                      SCalibrateImage     - Structure to pass calibration data
//
// CLASS HIERARCHY:
//                      CMasterFrame
//                        - CMasterDarkFrame
//                        - CMasterFlatFrame
//                        - CMasterBiasFrame
//                      CCalibrateFrame
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2011-05-25 GGB - Development of classes and code for implementation into astroManager.
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROIMAGECALIBRATION_H
#define ACL_ASTROIMAGECALIBRATION_H

  // Include files for ACL library

#include "../include/AstroFile.h"
#include "../include/FITSStrings.h"

  // Standard library include files

#include <vector>

  // Bosot library include files

#include "boost/shared_ptr.hpp"

namespace ACL
{

  enum EImageCombine
  {
    EIC_NONE = 0,
    EIC_MEAN,
    EIC_MEDIAN
  };

  typedef std::vector<std::string> DStringVector;

  class CMasterFrame
  {
  private:
  protected:
    mutable bool masterFrameValid;
    CAstroFile *masterFrame;

    std::string outputFileName;

    bool verifyFrames(DStringVector const &) const;

    virtual void meanCombine() = 0;
    virtual void medianCombine() = 0;

  public:
    explicit CMasterFrame() : outputFileName(std::string("")) {}
    CMasterFrame(std::string const &);
    virtual ~CMasterFrame();

    void createMasterFrame(EImageCombine mode = EIC_MEAN);

    virtual void saveToFile(boost::filesystem::path const &) const;
  };

  class CMasterDarkFrame : public CMasterFrame
  {
  private:
    DStringVector darkFrames;		// List of file names in the class.
    std::vector<std::string>::const_iterator darkFrameIterator;

    CAstroFile *masterBiasFile;

    explicit CMasterDarkFrame();

  protected:
    void meanCombine();
    void medianCombine();

  public:
    CMasterDarkFrame(std::string const &);

    void addDarkFrame(std::string const &);

    CAstroFile *setMasterDarkFile(CAstroFile *);
    CAstroFile *getMasterDarkFile();

    void setMasterBiasFile(CAstroFile *);
    CAstroFile *getMasterBiasFile() const;

      // Consistency verification routines

    bool verifyFrames() ;
    std::string const &inconsistentFrame();
  };

  class CMasterFlatFrame : public CMasterFrame
  {
  private:
    DStringVector flatFrames;
    std::vector<std::string>::const_iterator flatFrameIterator;

    CAstroFile *masterFlatDarkFile;

  protected:
    void meanCombine();
    void medianCombine();

  public:
    CMasterFlatFrame(std::string const &);

    void addFlatFrame(std::string const &);

    CAstroFile *setMasterFlatFile(CAstroFile *);
    CAstroFile *getMasterFlatFile();

    void setMasterFlatDarkFile(CAstroFile *);
    CAstroFile *getMasterFlatDarkFile();

    bool verifyFrames();
    std::string const &inconsistentFrame();
  };

  class CMasterBiasFrame : public CMasterFrame
  {
  private:
    std::vector<std::string> biasFrames;
    std::vector<std::string>::const_iterator biasFrameIterator;

    explicit CMasterBiasFrame();

    virtual void meanCombine();
    virtual void medianCombine();

  protected:
  public:
    CMasterBiasFrame(std::string const &);

    void addBiasFrame(std::string const &);

    CAstroFile *setMasterBiasFile(CAstroFile *);
    CAstroFile *getMasterBiasFile() const;

    virtual bool verifyFrames();
    std::string const &inconsistentFrame();
  };

  class CCalibrateFrame
  {
  private:
    std::vector<std::string> frameList;
    std::string darkFrame;
    std::string flatFrame;
    std::string biasFrame;
    std::string outputDirectory;
    bool bBackupExisting;
    bool bOverwrite;
    bool bSaveOriginal;

  protected:
    bool verifyFrames(CAstroFile *, CAstroFile *, CAstroFile *, CAstroFile *) const ;
    void calibrateFrame(CAstroFile *, CAstroFile *, CAstroFile *, CAstroFile *, CAstroFile *) const;

  public:
    CCalibrateFrame();

    void addFrame(std::string const &);
    void setDarkFrame(std::string const &);
    void setFlatFrame(std::string const &);
    void setBiasFrame(std::string const &);
    void setOutputDirectory(std::string const &);
    void setFlags(bool SaveOriginal, bool BackupExisting, bool Overwrite);

    void calibrateFrames();
  };

  typedef boost::shared_ptr<CCalibrateFrame> CCalibrateFrame_Ptr;

}  // namespace

#endif // ACL_ASTROIMAGECALIBRATION
