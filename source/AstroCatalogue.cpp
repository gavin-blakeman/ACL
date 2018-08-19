//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroCatalogue.cpp
// SUBSYSTEM:						Astronomical Catalogue classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL
//											CCFits
//											CFitsIO
//											boost.
// NAMESPACE:						libAstroClass
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
// Copyright 2012-2013 Gavin Blakeman.
// This file is part of the Astronomy Class Library (libAstroClass)
//
// libAstroClass is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
//
// libAstroClass is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with libAstroClass.  If not, 
// see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images.
//											The classes are designed to present a single interface to the other astro library
//											classes while supporting several different file types.
//
// CLASSES INCLUDED:		CHDB            - Class to encapsulate header and data information.
//
// CLASS HIERARCHY:
//                      CHDB
//
// HISTORY:             0000.00.000 2012-08-01  GGB - Start development of classes for AIRDAS
//
//*********************************************************************************************************************************

#include "../Include/AstroCatalogue.h"

namespace libAstroClass
{
  //*******************************************************************************************************************************
  //
  // CAstroCatalogue
  //
  //*******************************************************************************************************************************

  CAstroCatalogue::CAstroCatalogue() : stellarObjectContainer()
  {
  };

  /// Adds a new stellar object into the list of stellar objects
  //
  // R/D/A: 1.00/2012-08-03/GGB

  void CAstroCatalogue::addStellarObject(PStellarObject toAdd)
  {
    stellarObjectContainer.push_back(toAdd);
  };

  /// Removes all the items from the list.
  //
  // R/D/A: 1.00/2012-08-03/GGB

  void CAstroCatalogue::empty()
  {
    stellarObjectContainer.empty();
  };

  /// Returns the first item in the list.
  /// EXCEPTIONS: 0x2400 - ASTROCATALOGUE: No items in list. Cannot return first item
  //
  // R/D/A: 1.00/2012-08-03/GGB

  PStellarObject CAstroCatalogue::moveFirst()
  {
    if (stellarObjectContainer.size() == 0)
      ACL_ERROR(0x2400);    // ASTROCATALOGUE: No items in list. Cannot return first item
    else
    {
      containerIterator = stellarObjectContainer.begin();
      return (*containerIterator);
    };
  };

  /// Returns the last item in the list.
  /// EXCEPTIONS: 0x2400 - ASTROCATALOGUE: No items in list. Cannot return first item
  //
  // R/D/A: 1.00/2012-08-03/GGB

  PStellarObject CAstroCatalogue::moveLast()
  {
    if (stellarObjectContainer.size() ==0)
      ACL_ERROR(0x2400);    // ASTROCATALOGUE: No items in list. Cannot return first item
    else
    {
      containerIterator = stellarObjectContainer.end();
      return (*containerIterator);
    };
  };

  /// Moves to the next object in the list and returns the next object.
  /// EXCEPTIONS: 0x2401 - ASTROCATALOGUE: Reached last item in list. Cannot return item.
  //
  // R/D/A: 1.00/2012-08-03/GGB

  PStellarObject CAstroCatalogue::moveNext()
  {
    if (containerIterator == stellarObjectContainer.end())
      ACL_ERROR(0x2401);    // ASTROCATALOGUE: Reached last item in list. Cannot return item.
    else if (++containerIterator == stellarObjectContainer.end())
      ACL_ERROR(0x2401);    // ASTROCATALOGUE: Reached last item in list. Cannot return item.
    else
      return (*containerIterator);
  };

  /// Returns the previous item in the list.
  /// EXCEPTIONS: 0x2402 - ASTROCATALOGUE: Reached first item in list. Cannot return item.
  //
  // R/D/A: 1.00/2012-08-03/GGB

  PStellarObject CAstroCatalogue::movePrevious()
  {
    if (containerIterator == stellarObjectContainer.begin())
      ACL_ERROR(0x2402);    //ASTROCATALOGUE: Reached first item in list. Cannot return item.
    else if (++containerIterator == stellarObjectContainer.begin())
      ACL_ERROR(0x2402);    //ASTROCATALOGUE: Reached first item in list. Cannot return item.
    else
      return (*containerIterator);
  };

}; // namespace libAstroClass