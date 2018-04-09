//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								photometryFilters
// SUBSYSTEM:						Photometry Filter class
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	MCL, boost.
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
// OVERVIEW:						A class to support photometry filters
//
// CLASSES INCLUDED:		CPhotometryFilter
//
// CLASS HIERARCHY:     CPhotometryFilter
//
// HISTORY:             2017-07-22 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#include "../Include/photometryFilters.h"

  // ACL Library

#include "../Include/common.h"

  // Miscellaneous libraries.

#include <GCL>

  // Standard libraries

#include <algorithm>
#include <limits>

#include "boost/algorithm/string.hpp"

namespace ACL
{
  DPhotometryFilterStorage CPhotometryFilterCollection::photometryFilterStorage
    { {0, "Unknown", {} },
      {1, "Uj", "Johnsan U", {"U"} },
      {2, "Bj", "Johnsan B", {"B"} },
      {3, "Vj", "Johnsan V", {"V"} },
      {4, "Rc", "Cousins R", {"R"} },
      {5, "Ic", "Cousins I", {"I"} },
      {6, "J", "NIR 1.2micron", {} },
      {7, "H", "NIR 1.6micron", {} },
      {8, "K", "NIR 2.2micron", {} },
      {9, "TG", "Green Filter", {} },
      {10, "Z", "Sloan Z", {} },
      {11, "CV", "Clear V-Band", {} },
      {12, "CR", "Clear R-Band", {} },
      {13, "SZ", "Sloan z", {"Z"} },
      {14, "SU", "Sloan u", {} },
      {15, "SG", "Sloan g", {} },
      {16, "SR", "Sloan r", {} },
      {17, "SI", "Sloan i", {} },
      {18, "STU", "Stromgren u", {} },
      {27, "Red", "Visual Red", {} },
      {28, "Green", "Visual Green", {} },
      {29, "Blue", "Visual Blue", {} },
      {30, "Luminance", "Luminance", {} }
    };

  CPhotometryFilter::CPhotometryFilter(std::uint16_t filterID, std::string filterName, std::string filterDescription,
                                       std::vector<std::string> alternateNames)
    : filterID_(filterID), filterName_(filterName), filterDescription_(filterDescription), alternateNames_(alternateNames)
  {
  }

  /// @brief Comparison function for the filter name.
  /// @details Also searches the alternate filter name.
  /// @param[in] filterName - The name of the filter to compare.
  /// @returns true - This is the correct filter.
  /// @returns false - This is not the correct filter.
  /// @throws None.
  /// @version 2017-07-23/GGB - Function created.

  bool CPhotometryFilter::operator==(std::string const &filterName)
  {
    if (filterName_ == filterName)
    {
      return true;
    }
    else
    {
      return (std::find(alternateNames_.begin(), alternateNames_.end(), filterName) != alternateNames_.end());
    }
  }

  /// @brief Returns the database ID of the selected string.
  /// @details The alternate names are also searched.
  /// @param[in] filterName - The name of the filter to search for.
  /// @returns The value of the databaseID associated with the filter.
  /// @returns 0 - If the database ID has not been set.
  /// @throws GCL::search_error()
  /// @version 2017-07-23/GGB - Function created.

  std::uint16_t CPhotometryFilterCollection::findFilterDatabaseID(std::string const &filterName)
  {
    auto iter = std::find(photometryFilterStorage.begin(), photometryFilterStorage.end(), filterName);

    if (iter != photometryFilterStorage.end())
    {
      return (*iter).filterDatabaseID();
    }
    else
    {
      throw GCL::search_error("Unable to find filter corresponding to " + filterName + ".");
    };
  }

  /// @brief Returns the filter ID of the selected string.
  /// @details The alternate names are also searched.
  /// @param[in] filterName - The name of the filter to search for.
  /// @returns The value of the databaseID associated with the filter.
  /// @returns 0 - If the database ID has not been set.
  /// @throws GCL::search_error()
  /// @version 2017-07-23/GGB - Function created.

  std::uint16_t CPhotometryFilterCollection::findFilterID(std::string filterName)
  {
    std::uint16_t returnValue = 0;

    boost::trim(filterName);
    auto iter = std::find(photometryFilterStorage.begin(), photometryFilterStorage.end(), filterName);

    if (iter != photometryFilterStorage.end())
    {
      returnValue = (*iter).filterDatabaseID();
    }
    else
    {
      throw GCL::search_error("Unable to find filter corresponding to " + filterName + ".");
    };

    return returnValue;
  }

  /// @brief Processes filter data from a database entry.
  /// @details  1. If the databaseID already exists, an error is thrown
  ///           2. Otherwise a search is done for the filter name.
  ///           3. If the filtername exists, the databaseID is assigned to the relevant entry.
  ///           4. Failing all this, a new entry is created using the next available filterID.
  /// @param[in] filterDatabaseID - The databaseID of the filter.
  /// @param[in] filterName - The name of the filter. (Generally 1-4 letters)
  /// @param[in] filterDescription - A longer description of the filter.
  /// @throws GCL::CError(ACL, 0x4000)
  /// @throws GCL::CError(ACL, 0x4001)
  /// @version 2017-07-23/GGB - Function created.

  void CPhotometryFilterCollection::processDatabaseEntry(std::uint16_t filterDatabaseID, std::string const &filterName,
                                                         std::string const & filterDescription)
  {
      // Does the databaseID already exist in the list.

    if (std::find_if(photometryFilterStorage.begin(), photometryFilterStorage.end(),
                     [filterDatabaseID](CPhotometryFilter &pf)->bool { return (pf.filterDatabaseID() == filterDatabaseID);} ) !=
        photometryFilterStorage.end())
    {
        // Database entry already exists.

      ACL_ERROR(0x4000);    // Repeated database ID's.

    };

      // Search and see if we already have the filter in the list.

    auto iter = std::find(photometryFilterStorage.begin(), photometryFilterStorage.end(), filterName);

    if (iter != photometryFilterStorage.end())
    {
        // Filter already exists. Just update the database ID.

      iter->filterDatabaseID(filterDatabaseID);

    }
    else
    {
        // Create the new entry. Need to find an unused value for the filterID

      std::uint16_t newID;

      for (newID = 0; newID < std::numeric_limits<std::uint16_t>::max(); newID++)
      {
        if (std::find(photometryFilterStorage.begin(), photometryFilterStorage.end(), newID) == photometryFilterStorage.end())
        {
          break;
        }
      }

      if (newID == std::numeric_limits<std::uint16_t>::max())
      {
        ACL_ERROR(0x4001);    // All available ID's used.
      }
      else
      {
        photometryFilterStorage.emplace_back(newID, filterName, filterDescription);
        photometryFilterStorage.back().filterDatabaseID(filterDatabaseID);
      };
    }
  }


} // namespace ACL
