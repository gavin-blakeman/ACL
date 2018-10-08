//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								SIMBADParser
// SUBSYSTEM:						A system to parse SIMBAD query returns.
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2016-2018 Gavin Blakeman.
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

#ifndef ACL_SIMBADPARSER_H
#define ACL_SIMBADPARSER_H

  // Standard C++ library header files.

#include <cstdint>
#include <initializer_list>
#include <string>
#include <tuple>
#include <vector>

  // ACL library header files

#include "AstronomicalCoordinates.h"
#include "config.h"
#include "targetAstronomy.h"

  // Miscellaneous library header files.

#include <SCL>

namespace ACL
{

  typedef SCL::CAny parameter;
  typedef std::pair<std::string, parameter> parameterPair;
  typedef std::tuple<std::string, std::string, parameter> parameterTriple;

  typedef std::vector<parameterPair> pairStorage;
  typedef std::vector<parameterTriple> tripleStorage;

  class CSIMBADScript
  {
  public:
    /// The fields that can be included in the script for output. This is also used for parsing the
    /// returned values.
    enum EFields
    {
      IDLIST_1,
      COO_A,
      COO_D,
      PM_A,
      PM_D,
      PLX_V,
      RV_V,
      OTYPE_S,
    };
    /// The allowed values for a region query.
    enum ERegions
    {
      CIRCLE,
      ELLIPSE,
      ZONE,
      BOX,
      ROTATEDBOX,
      POLYGON
    };

  private:
    static std::map<EFields, std::string> fieldStrings;
    static std::map<ERegions, std::string> regionStrings;
    std::vector<FP_t> regionValues;
    std::vector<EFields> queryFields;
    pairStorage queryCriteria;
    ERegions regionType;
    std::string fieldDelimiter = ";";

  protected:

  public:
    CSIMBADScript() {}

    void resetScript();

    CSIMBADScript &addFields(std::initializer_list<EFields>);
    CSIMBADScript &addQueryCriteria(std::initializer_list<parameterPair>);
    CSIMBADScript &addRegion(ERegions, std::initializer_list<FP_t>);

    operator std::string() const;
  };

}   // namespace ACL

#endif // ACL_SIMBADPARSER_H
