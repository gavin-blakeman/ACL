//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								MPCORB
// SUBSYSTEM:						Minor Planet Orbital File Reading
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

#include "../include/MPCORB.h"

  // Standard C++ library header files.

#include <cstdint>
#include <vector>

  // ACL library header files.

#include "../include/error.h"

  // Miscellaneous library header files

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/fstream.hpp"
#include <boost/lexical_cast.hpp>

namespace ACL
{
  namespace MPCORB
  {
    struct SColumnData
    {
      std::uint_least8_t indexStart;
      std::uint_least8_t indexEnd;
    };

    // Start stop of all columns. NOTE: This is one offset, not zero offset. The difference is handled in the code.
    // This has been intentionally done as the MPCORB format specifies columns wth a 1 offset.
    static std::vector<SColumnData> columns = { {   1,   7},
                                             {   9,  13},
                                             {  15,  19},
                                             {  21,  25},
                                             {  27,  35},
                                             {  38,  46},
                                             {  49,  57},
                                             {  60,  68},
                                             {  71,  79},
                                             {  81,  91},
                                             {  93, 103},
                                             { 106, 106},
                                             { 108, 116},
                                             { 118, 122},
                                             { 124, 126},
                                             { 128, 136},
                                             { 138, 141},
                                             { 143, 145},
                                             { 147, 149},
                                             { 151, 160},
                                             { 162, 165},
                                             { 167, 194},
                                             { 195, 202}
                                           };

    /// @brief Searches the MPCORB file to find the specified object.
    /// @param[in] fileName: Path and file name for the MPCORB.DAT file.
    /// @param[in] mpName: The name of the minor planet, can be designation, or name. or number.
    /// @param[out] elements: The elements of the minor planet.
    /// @returns true - The MP was found.
    /// @returns false - The MP was not found.
    /// @throws 0x3300: Unable to Open MPCORB.DAT
    /// @version 2018-08-25/GGB - Function created.

    bool loadMP(boost::filesystem::path const &fileName, std::string const &mpName, SMPCORB &elements)
    {
      std::string szLine;
      bool returnValue = false;
      boost::filesystem::ifstream ifs(fileName, std::ios::in);

      if (!ifs)
      {
        ACL_ERROR(0x3300);
      }
      else
      {
        // Iterate over the file until the end of file, or the mpName is found.

        while ( (!ifs.eof()) && !returnValue)
        {
          std::getline(ifs, szLine);

          if (szLine.find(mpName) != std::string::npos)
          {
            // MP found.

            parseLine(szLine, elements);
            returnValue = true;
          };
        };

        ifs.close();
      };

      return returnValue;
    }

    /// @brief Parses a single line into the structure.
    /// @param[in] szLine: The MPCorbital element line to parse.
    /// @param[out] elements: The elements parsed.
    /// @throws
    /// @version 2018-08-25/GGB - Function created.

    void parseLine(std::string const &szLine, SMPCORB elements)
    {
      std::vector<std::string> vectorValues;

      if (!szLine.empty() || szLine.size() < 202)
      {
          // Decompose the line into the component strings.

        for (SColumnData const &column : columns)
        {
          if (szLine.size() > column.indexEnd)
          {
            vectorValues.emplace_back(szLine.substr(column.indexStart - 1,
                                                    column.indexStart - column.indexEnd + 1));
          }
          else
          {

          };
        };

          // Remove all unnecessary whitespace from the strings.

        for (std::string &szValue : vectorValues)
        {
          boost::trim(szValue);
        };

          // Now process each string and write value.

        std::vector<std::string>::size_type index = 0;

        elements.designation = vectorValues[index++];
        elements.absoluteMagnitude = boost::lexical_cast<float>(vectorValues[index++]);
        elements.slopeParameter = boost::lexical_cast<float>(vectorValues[index++]);
        elements.epoch = vectorValues[index++];
        elements.meanAnomaly = boost::lexical_cast<double>(vectorValues[index++]);
        elements.argumentOfPerihelion = boost::lexical_cast<double>(vectorValues[index++]);
        elements.longitudeOfAscendingNode = boost::lexical_cast<double>(vectorValues[index++]);
        elements.inclination = boost::lexical_cast<double>(vectorValues[index++]);
        elements.eccentricity = boost::lexical_cast<double>(vectorValues[index++]);
        elements.meanDailyMotion = boost::lexical_cast<double>(vectorValues[index++]);
        elements.semiMajorAxis = boost::lexical_cast<double>(vectorValues[index++]);
        elements.uncertantyParameter = vectorValues[index++];
        elements.referece = vectorValues[index++];
        elements.numberOfObservations = boost::lexical_cast<std::uint64_t>(vectorValues[index++]);
        elements.numberOfOppositions = boost::lexical_cast<std::uint16_t>(vectorValues[index++]);

          // Two different interpretations needed here.

        index++;

        elements.rmsResidual = boost::lexical_cast<float>(vectorValues[index++]);
        elements.coursePerturbers = vectorValues[index++];
        elements.precisePerturbers = vectorValues[index++];
        elements.computerName = vectorValues[index++];

        if (vectorValues.size() > index)
        {
          elements.flags = boost::lexical_cast<std::uint16_t>(vectorValues[index++]);
        };
        if (vectorValues.size() > index)
        {
          elements.name = vectorValues[index++];
        };
        if (vectorValues.size() > index)
        {
          elements.dateOfLastObservation = vectorValues[index++];
        };

      };
    }

  } // namespace MPCORB
} // namespace ACL
