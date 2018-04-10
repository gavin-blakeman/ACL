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

#include "../Include/telescope.h"

namespace ACL
{
  /// @brief Copy constructor.
  /// @param[in] toCopy - The instance to make a copy of.
  /// @throws std::bad_alloc
  /// @version 2017-08-26/GGB - Function created.

  CTelescope::CTelescope(CTelescope const &toCopy) : telescopeName_(toCopy.telescopeName_),  manufacturer_(toCopy.manufacturer_),
    model_(toCopy.model_), aperture_(toCopy.aperture_), focalLength_(toCopy.focalLength_), obstruction_(toCopy.obstruction_)
  {
  }

  /// @brief Creates a copy of this instance.
  /// @returns a new instance that is a copy of this.
  /// @throws std::bad_alloc
  /// @version 2017-08-26/GGB - Function created.

  CTelescope *CTelescope::createCopy() const
  {
    return new CTelescope(*this);
  }

} // namespace ACL
