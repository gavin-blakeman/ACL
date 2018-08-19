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
//                      Copyright 2017-2018 Gavin Blakeman.
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

#ifndef ACL_PHOTOMETRYFILTERS_H
#define ACL_PHOTOMETRYFILTERS_H

  // Standard libraries

#include <cstdint>
#include <string>
#include <vector>

namespace ACL
{

  /// @class CPhotometryFilter
  /// @remark The photometry filters are fairly standard. However, they need to be supported in the database as well as being
  ///         supported by the ACL library. This is done by implmenting a class that support the database if supplied and if no
  ///         database is available, the code will continue to work correctly.

  class CPhotometryFilter
  {
  private:
    std::uint16_t filterID_;
    std::uint16_t filterDatabaseID_ = 0;
    std::string filterName_;
    std::string filterDescription_;
    std::vector<std::string> alternateNames_;

  protected:
  public:
    CPhotometryFilter(std::uint16_t filterID, std::string filterName, std::string filterDescription)
      : filterID_(filterID), filterName_(filterName), filterDescription_(filterDescription) {}
    CPhotometryFilter(std::uint16_t, std::string, std::string, std::vector<std::string>);

    bool operator==(std::string const &);
    bool operator==(std::uint16_t filterID) { return filterID_ == filterID; }

    void filterDatabaseID(std::uint16_t filterDatabaseID) { filterDatabaseID_ = filterDatabaseID; }

    std::uint16_t filterDatabaseID() const { return filterDatabaseID_; }
    std::uint16_t filterID() const { return filterID_; }
  };

  typedef std::vector<CPhotometryFilter> DPhotometryFilterStorage;

  class CPhotometryFilterCollection
  {
  private:
    static DPhotometryFilterStorage photometryFilterStorage;

    CPhotometryFilterCollection() = delete;
    CPhotometryFilterCollection(CPhotometryFilterCollection const &) = delete;

  protected:
  public:
    static std::uint16_t findFilterID(std::string);
    static std::uint16_t findFilterDatabaseID(std::string const &);
    static void processDatabaseEntry(std::uint16_t, std::string const &, std::string const &);

  };

} // namespace ACL

#endif // PHOTOMETRYFILTERS_H

