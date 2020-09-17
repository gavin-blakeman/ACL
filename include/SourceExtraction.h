//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								SourceExtraction
// SUBSYSTEM:						Automatic Source Extraction
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2014-2020 Gavin Blakeman.
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
//
// CLASSES INCLUDED:		SImageSource - Structure for storing source information
//
// CLASS HIERARCHY:     SImageSource
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2014-02-09 GGB - File created
//
//*********************************************************************************************************************************

#ifndef ACL_SOURCEEXTRACTION_H
#define ACL_SOURCEEXTRACTION_H

  // Standard C++ library header files

#include <list>
#include <memory>

  // ACL Specific header files

#include "config.h"

  // Miscellaneous header files.

#include <MCL>

namespace ACL
{
  class SImageSource
  {
  public:
    MCL::TPoint2D<FP_t> center;
    AXIS_t radius;
    FP_t flux;
    FP_t background;
    FP_t peak;

    bool operator<(SImageSource const &) const;
  };

  typedef std::shared_ptr<SImageSource> PImageSource;
  typedef std::list<PImageSource> TImageSourceContainer;
}

#endif // ACL_SOURCEEXTRACTION_H
