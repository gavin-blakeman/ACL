//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								telescope
// SUBSYSTEM:						Observation locations
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	PCL, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2017 Gavin Blakeman.
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
// OVERVIEW:						A class to store telescope information
//
// CLASSES INCLUDED:		CTelescope
//
// CLASS HIERARCHY:     CTelescope
//
// HISTORY:             2017-08-05 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_TELESCOPE_H
#define ACL_TELESCOPE_H

#include "config.h"

#include <string>

#include <PCL>

namespace ACL
{
  class CTelescope
  {
  private:
    std::string telescopeName_;
    std::string manufacturer_;
    std::string model_;
    FP_t aperture_;
    FP_t focalLength_;
    FP_t obstruction_;                  ///< Obstruction as a %

  protected:
  public:
    CTelescope() {}
    CTelescope(CTelescope const &);
    CTelescope(std::string const &telescopeName) : telescopeName_(telescopeName) {}

    virtual CTelescope *createCopy() const;

    virtual void telescopeName(std::string const &telescopeName) noexcept { telescopeName_ = telescopeName; }
    virtual std::string const &telescopeName() const noexcept { return telescopeName_; }
    virtual std::string &telescopeName() noexcept { return telescopeName_; }

    virtual void manufacturer(std::string const &manufacturer) noexcept { manufacturer_ = manufacturer; }
    virtual std::string const &manufacturer() const noexcept { return manufacturer_; }
    virtual std::string &manufacturer() noexcept { return manufacturer_; }

    virtual void model(std::string const &model) noexcept { model_ = model; }
    virtual std::string const &model() const noexcept { return model_; }
    virtual std::string &model() noexcept { return model_; }

    virtual void aperture(FP_t const &aperture) noexcept { aperture_ = aperture; }
    virtual FP_t const &aperture() const noexcept { return aperture_; }
    virtual FP_t &aperture() noexcept { return aperture_; }

    virtual void focalLength(FP_t const &focalLength) noexcept { focalLength_ = focalLength; }
    virtual FP_t const &focalLength() const noexcept { return focalLength_; }
    virtual FP_t &focalLength() noexcept { return focalLength_; }

    virtual void obstruction(FP_t const &obstruction) noexcept { obstruction_ = obstruction; }
    virtual FP_t const &obstruction() const noexcept { return obstruction_; }
    virtual FP_t &obstruction() noexcept { return obstruction_; }
  };

} // namespace ACL

#endif // ACL_TELESCOPE_H

