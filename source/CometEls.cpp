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

  // Standard C++ library header files.

#include <vector>

  // ACL library header files.

#include "../include/error.h"

  // Miscellaneous library header files

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/fstream.hpp"
#include <boost/lexical_cast.hpp>

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

    /// @brief Searches the ComEls.txt file to find the specified object.
    /// @param[in] fileName: Path and file name for the MPCORB.DAT file.
    /// @param[in] mpName: The name of the minor planet, can be designation, or name. or number.
    /// @param[out] elements: The elements of the minor planet.
    /// @returns true - The MP was found.
    /// @returns false - The MP was not found.
    /// @throws 0x3310: Unable to open CometEls.txt
    /// @version 2018-08-25/GGB - Function created.

    bool loadComet(boost::filesystem::path const &fileName, std::string const &cometName, SCometElements &elements)
    {
      std::string szLine;
      bool returnValue = false;
      boost::filesystem::ifstream ifs(fileName, std::ios::in);

      if (!ifs)
      {
        ACL_ERROR(0x3310);
      }
      else
      {
        // Iterate over the file until the end of file, or the cometName is found.

        while ( (!ifs.eof()) && !returnValue)
        {
          std::getline(ifs, szLine);

          if (szLine.find(cometName) != std::string::npos)
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

    void parseLine(std::string const &szLine, SCometElements &elements)
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

        elements.cometNumber = vectorValues[index++];
        elements.orbitType = vectorValues[index++];
        elements.provisionalDesignation = vectorValues[index++];
        elements.perihelionPassageYear = boost::lexical_cast<std::uint16_t>(vectorValues[index++]);
        elements.perihelionPassageMonth = boost::lexical_cast<std::uint16_t>(vectorValues[index++]);
        elements.perihelionPassageDay = boost::lexical_cast<float>(vectorValues[index++]);
        elements.perihelionDistance = boost::lexical_cast<double>(vectorValues[index++]);
        elements.orbitalEccentricity = boost::lexical_cast<double>(vectorValues[index++]);
        elements.argumentOfPerihelion = boost::lexical_cast<double>(vectorValues[index++]);
        elements.longitudeOfAscendingNode = boost::lexical_cast<double>(vectorValues[index++]);
        elements.inclination = boost::lexical_cast<double>(vectorValues[index++]);
        elements.epochYear = boost::lexical_cast<std::uint16_t>(vectorValues[index++]);
        elements.epochMonth = boost::lexical_cast<std::uint16_t>(vectorValues[index++]);
        elements.epochDay = boost::lexical_cast<std::uint16_t>(vectorValues[index++]);
        elements.absoluteMagnitude = boost::lexical_cast<float>(vectorValues[index++]);
        elements.slopeParameter = boost::lexical_cast<float>(vectorValues[index++]);
        elements.nameOfComet = vectorValues[index++];
        elements.designation = vectorValues[index++];
      };
    }

  } // namespace COMETELS
} // namespace ACL
