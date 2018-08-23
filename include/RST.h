//*********************************************************************************************************************************
//
// PROJECT:			        Astronomy Class Library
// FILE:				        RST
// SUBSYSTEM:		        Rise Set Transit Times
// TARGET OS:		        WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	GCL, MCL, PCL
// AUTHOR:              Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2018 Gavin Blakeman.
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
//
// CLASS HIERARCHY:
//
// RELEASE HISTORY:     2018-08-21 GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_RST_H
#define ACL_RST_H

  // Standard C++ header files.

#include <memory>
#include <optional>

  // ACL library header files.

#include "julianDay.h"

namespace ACL
{
  class CRST
  {
  private:
    std::optional<TJD> riseTime_;
    std::optional<TJD> transitTime_;
    std::optional<TJD> setTime_;


    CRST(CRST const &) = delete;

  protected:
  public:
    CRST();

    void riseTime(TJD const &rt) {riseTime_ = rt; }
    void setTime(TJD const &st) { setTime_ = st; }
    void transitTime(TJD const &tt) { transitTime_ = tt; }


  };
}

#endif // ACL_RST_H

