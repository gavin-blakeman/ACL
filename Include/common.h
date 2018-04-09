//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								common
// SUBSYSTEM:						Common definitions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2013-2016 Gavin Blakeman.
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
// OVERVIEW:						Common definitions that can be used by multiple files
//
//
// CLASSES INCLUDED:		None
//
// CLASS HIERARCHY:     None.
//
//
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2013-09-30 GGB - AIRDAS 2013.09 release.
//                      2013-05-25 GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_COMMON_H
#define ACL_COMMON_H

#include "config.h"

#include <string>

namespace ACL
{

#ifndef __XC32

    /// Enumerated transfer types.

  enum ETransferFunction
  {
    ETF_NONE = 0,
    ETF_LINEAR,             ///< Display with a linear transfer function
    ETF_GAMMA,              ///< A gamma transfer function
    ETF_SQRT,               ///< A square root transfer function
    ETF_CBRT,
    ETF_SIGMOID,
    ETF_SQUARE,
    ETF_GAMMALOG,
    ETF_LOG
  };

    // Enumerated values for colour planes.

  enum EColour
  {
    COLOUR_NONE,
    COLOUR_RED,
    COLOUR_GREEN,
    COLOUR_BLUE,
    COLOUR_LUMINANCE,
    COLOUR_CYAN,
    COLOUR_MAGENTA,
    COLOUR_YELLOW,
    COLOUR_GRAYSCALE
  };

  enum ERenderMode
  {
    RM_NONE,                    ///< No render mode specified
    RM_GREY8,                   ///< 8-bit grey scale.
    RM_GREY16,                  ///< 16-bit grey scale
    RM_RGB32,                   ///< 32 bit RGB format (0xffRRGGBB)
    RM_ARGB32,                  ///< 32 bit ARGB (0xAARRGGBB)
    RM_RGBD,                    ///< RGB double representation
    RM_ARGBD                    ///< ARGB double representation.
  };

  /// @brief The SColourRGB structure is used for defining colours using the 24 bit RGB colour space.

  struct SColourRGB
  {
    std::uint8_t R;
    std::uint8_t G;
    std::uint8_t B;
  };

  size_t const HDU_PRIMARY            = 0;

#endif // __XC32

  std::string const ACL_LIBNAME       = "ACL";

  std::string getVersionString();
  void loadErrorMessages();

#define ACL_ERROR(ERR) (ERROR(ACL, ERR))
#define ACL_CODE_ERROR CODE_ERROR(ACL)

}   // namespace ACL

#endif // ACL_COMMON_H
