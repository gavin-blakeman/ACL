//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								PhotometryObservation
// SUBSYSTEM:						Photometry classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
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
// OVERVIEW:						A number of classes supporting various aspects of photometry and photometric
//											reduction.
//											The focus of this subsystem is on taking raw photometric observations and
//											and converting them to transformed photometric observations.
//
// CLASSES INCLUDED:		CPhotometryObservation - A class for storing photometry observation information.
//                      CPhotometryObject		- A class for storage of photometry related information.
//                      CPhotometryAperture  - Storage class for the 3 radii for Photometry.
//
// CLASS HIERARCHY:     CObservation
//                        - CPhotometryObservation.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-06-08 GGB - Moved CPhotometryObservation into seperate file.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2010-06-23 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_PHOTOMETRYOBSERVATION_H
#define ACL_PHOTOMETRYOBSERVATION_H

  // Standard C++ libraries

#include <memory>
#include <string>

  // ACL header files

#include "config.h"
#include "Observation.h"
#include "PhotometryAperture.h"

namespace ACL
{
  class CPhotometryObservation : public CObservation
  {
  private:
    FP_t sourceADU_;
    FP_t sourceArea_;
    FP_t skyADU_;
    FP_t skyArea_;
    FP_t exposure_;
    FP_t gain_;
    std::optional<FP_t> FWHM_;
    PPhotometryAperture photometryAperture_;

  protected:
  public:
    CPhotometryObservation(std::string const &);
    CPhotometryObservation(CPhotometryObservation const &);
    CPhotometryObservation(std::shared_ptr<CTargetAstronomy>);
    virtual ~CPhotometryObservation() {}

    virtual SPObservation createCopy() const;

    virtual std::optional<FP_t> instrumentMagnitude();
    virtual FP_t magnitudeError() const;

    inline PPhotometryAperture &photometryAperture() { return photometryAperture_;}
    inline void photometryAperture(PPhotometryAperture pa) { photometryAperture_ = pa; }
    inline FP_t &sourceADU() { return sourceADU_ ;}
    inline void sourceADU(FP_t nv) { sourceADU_ = nv; }
    inline FP_t &sourceArea() {return sourceArea_;}
    inline void sourceArea(FP_t nv) { sourceArea_ = nv; }
    inline FP_t &skyADU() {return skyADU_;}
    inline void skyADU(FP_t nv) { skyADU_ = nv; }
    inline FP_t &skyArea() {return skyArea_;}
    inline void skyArea(FP_t nv) { skyArea_ = nv; }
    inline FP_t &exposure() {return exposure_;}
    inline void exposure(FP_t nv) { exposure_ = nv; }
    inline FP_t &gain() { return gain_;}
    inline void gain(FP_t nv) { gain_ = nv; }

    FP_t getSkyADU() const;
    FP_t getStarADU() const;
    FP_t getStarSkyADU() const;

    std::optional<FP_t> FWHM() const { return FWHM_;}
    void FWHM(std::optional<FP_t> const &fwhm) { FWHM_ = fwhm; }

    FP_t getStarE() const;
    FP_t getSNR() const;
  };
  typedef std::shared_ptr<CPhotometryObservation> SPPhotometryObservation;
}

#endif // ACL_PHOTOMETRYOBSERVATION_H
