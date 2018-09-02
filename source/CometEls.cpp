//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								CometEls
// SUBSYSTEM:						Comet Element File Reading
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:
// NAMESPACE:						ACL::MPCORB
// AUTHOR:							Gavin Blakeman.
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
// OVERVIEW:						These classes provide methods of handling minor planets
//
// CLASSES INCLUDED:
//
// CLASS HIERARCHY:
//
// HISTORY:             2018-08-25 GGB File Created
//
//*********************************************************************************************************************************

#include "../include/CometEls.h"

namespace ACL
{
  namespace COMETELS
  {
    struct SColumnData
    {
      std::uint_least8_t indexStart;
      std::uint_least8_t indexEnd;
    };

    // Start stop of all columns. NOTE: This is one offset, not zero offset. The difference is handled in the code.
    // This has been intentionally done as the MPCORB format specifies columns wth a 1 offset.
    static std::vector<SColumnData> columns = { {   1,   4},
                                                {   5,  5},
                                                {  6,  12},
                                                {  15,  18},
                                                {  20,  21},
                                                {  23,  29},
                                                {  31,  39},
                                                {  42,  49},
                                                {  52,  59},
                                                {  62,  69},
                                                {  72,  79},
                                                {  82,  85},
                                                {  86,  87},
                                                {  88,  89},
                                                {  92,  95},
                                                {  97, 100},
                                                { 103, 158},
                                                { 160, 168},
                                              };
  } // namespace COMETELS
} // namespace ACL
