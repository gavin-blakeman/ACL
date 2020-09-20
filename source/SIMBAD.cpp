//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								SIMBAD
// SUBSYSTEM:						A system to parse SIMBAD query returns.
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2016-2020 Gavin Blakeman.
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
// OVERVIEW:						A colllection of routines for parsing the returned data from SIMBAD database queries.
//                      This is integrated into the remainder of the ACL library.
//
//
// CLASSES INCLUDED:		None
//
// CLASS HIERARCHY:     None.
//
//
// HISTORY:             2016-04-24 GGB - File created.
//
//*********************************************************************************************************************************

#include "include/SIMBAD.h"

  // Standard C++ library header files.

#include <sstream>

  // Miscellaneous library header files.

#include <boost/format.hpp>
#include "boost/locale.hpp"

  // ACL library header files

#include "include/AstroFunctions.h"
#include "include/common.h"
#include "include/error.h"

namespace ACL
{

  std::map<CSIMBADScript::EFields, std::string> CSIMBADScript::fieldStrings =
  {
    {CSIMBADScript::IDLIST_1, "%IDLIST(1)"},
    {CSIMBADScript::COO_A,    "%COO(A)"},
    {CSIMBADScript::COO_D,    "%COO(D)"},
    {CSIMBADScript::PM_A,     "%PM(A)"},
    {CSIMBADScript::PM_D,     "%PM(D)"},
    {CSIMBADScript::PLX_V,    "%PLX(V)"},
    {CSIMBADScript::RV_V,     "%RV(V)"},
    {CSIMBADScript::OTYPE_S,  "%OTYPE(S)"},
  };

  std::map<CSIMBADScript::ERegions, std::string> CSIMBADScript::regionStrings =
  {
    {CSIMBADScript::BOX,        "box"},
    {CSIMBADScript::CIRCLE,     "circle"},
    {CSIMBADScript::ELLIPSE,    "ellipse"},
    {CSIMBADScript::ZONE,       "zone"},
    {CSIMBADScript::ROTATEDBOX, "rotatedbox"},
    {CSIMBADScript::POLYGON,    "polygon"}
  };

  /// @brief        Creates the script as a std::string to enable SIMBAD to be queried through the script query URL.
  /// @param        None.
  /// @returns      The script that has been built up.
  /// @throws       CRumtimeError(0x0701)
  /// @throws       CRuntimeError(0x0702)
  /// @version      2016-04-24/GGB - Function created.

  CSIMBADScript::operator std::string() const
  {
    std::ostringstream outputStream;

    outputStream << "http://simbad.u-strasbg.fr/simbad/sim-script?script=";
    outputStream << "output console=off script=off" << std::endl;
    outputStream << "format object \"";

      // Assemble the format strings. They need to be retrieved from the map.

    for (auto elem : queryFields)
    {
      std::map<EFields, std::string>::iterator it = fieldStrings.find(elem);

      if (it != fieldStrings.end())
      {
        outputStream << it->second << fieldDelimiter;
      }
      else
      {
        RUNTIME_ERROR(boost::locale::translate("SIMBAD: Invalid field name."), E_SIMBAD_INVALIDFIELD, LIBRARYNAME);
      }
    };
    outputStream << "\"" << std::endl;

      // Assemble the query

    outputStream << "query sample region(";

    std::map<ERegions, std::string>::iterator iter = regionStrings.find(regionType);
    if (iter != regionStrings.end())
    {
      outputStream << iter->second << ", ";
    }
    else
    {
      RUNTIME_ERROR(boost::locale::translate("SIMBAD: Invalid region type."), E_SIMBAD_INVALIDREGIONTYPE, LIBRARYNAME);
    };
    switch (regionType)
    {
      case CIRCLE:
      case ELLIPSE:
      case ZONE:
      {
        break;
      }
      case BOX:
      {
          // A box has a center, (RA Dec) and two dimensions (w h)

        outputStream << regionValues[0];
        outputStream << boost::format(" %+d, ") % regionValues[1];
        outputStream << regionValues[2] << "d " << regionValues[3] << "d)";
        break;
      }
      case ROTATEDBOX:
      case POLYGON:
      default:
      {
        RUNTIME_ERROR(boost::locale::translate("SIMBAD: Invalid region type."), E_SIMBAD_INVALIDREGIONTYPE, LIBRARYNAME);
        break;
      }
    }

      // Now add any other query parameters.

    for (auto elem: queryCriteria)
    {
      outputStream << " & ";
      outputStream << elem.first << " = " << elem.second.to_string();
    };

    outputStream << std::endl;

    return outputStream.str();
  }

  /// @brief Add the specified fields to the query.
  /// @param[in] fld: The format strings to add.
  /// @returns (*this)
  /// @throws None.
  /// @version 2016-04-25/GGB - Function created.

  CSIMBADScript &CSIMBADScript::addFields(std::initializer_list<EFields> fld)
  {
    for (auto elem : fld)
    {
      queryFields.push_back(elem);
    }

    return (*this);
  }

  /// @brief      Adds a region to the script.
  /// @param[in]  rt: The type of the region.
  /// @param[in]  values: The values to define the region. All in decimal degrees.
  /// @returns    (*this)
  /// @throws     CRuntimeError(E_SIMBAD_INVALIDREGIONTYPE)
  /// @note       The dimensions are specified in decimal degrees (floating point type).
  /// @version    2016-04-25/GGB - Function created.

  CSIMBADScript &CSIMBADScript::addRegion(ERegions rt, std::initializer_list<FP_t> values)
  {
      // Check for a valid region type.

    switch (rt)
    {
      case CIRCLE:
      case ELLIPSE:
      case ZONE:
      case BOX:
      case ROTATEDBOX:
      case POLYGON:
      {
        break;
      }
      default:
      {
        RUNTIME_ERROR(boost::locale::translate("SIMBAD: Invalid region type."), E_SIMBAD_INVALIDREGIONTYPE, LIBRARYNAME);
        break;
      }
    }

    regionType = rt;
    for (auto elem : values)
    {
      regionValues.push_back(elem);
    };

    return (*this);
  }

  /// @brief Function for adding a query parameter.
  /// @param[in] toAdd: The query parameter to add.
  /// @returns (*this)
  /// @throws None.
  /// @version 2016-04-24/GGB - Function created.

  CSIMBADScript &CSIMBADScript::addQueryCriteria(std::initializer_list<parameterPair> qry)
  {
    for (auto elem : qry)
    {
      queryCriteria.push_back(elem);
    }

    return (*this);
  }

  /// @brief Resets the script to receive new data.
  /// @param None.
  /// @returns None.
  /// @throws None.
  /// @version 2016-04-25/GGB - Function created.

  void CSIMBADScript::resetScript()
  {
    queryFields.clear();
    queryCriteria.clear();
  }
}
