//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								targetComet
// SUBSYSTEM:						Comet position calculations
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:  MCL
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2018-2020 Gavin Blakeman.
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
// OVERVIEW:						This class provides methods of handling comets
//
// CLASSES INCLUDED:    CTargetComet - Class for managing comets
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetMinorPlanet
//                        CTargetMajorPlanet
//                        CTargetComet
//
// HISTORY:             2018-09-01 GGB - File created.
//
//*********************************************************************************************************************************

#include "include/targetComet.h"

  // Miscellaneous library header files

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/locale.hpp"
#include <GCL>

  // ACL library header files

#include "include/common.h"
#include "include/error.h"

namespace ACL
{

  struct SColumnData
  {
    std::uint_least8_t indexStart;
    std::uint_least8_t indexEnd;
  };

  // The start and stop of all columns. NOTE: This is one offset, not zero offset. The difference is handled in the code.
  // This has been intentionally done as the MPCORB format specifies columns wth a 1 offset.

  static std::vector<SColumnData> columns = { {   1,   4},
                                              {   5,   5},
                                              {   6,  12},
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

  boost::filesystem::path CTargetComet::filePath = "";
  boost::filesystem::path CTargetComet::fileName = "CometEls.txt";
  std::map<std::string, std::uint32_t> CTargetComet::cometCache;

  /// @brief        Creates and loads a comet target including elements.
  /// @param[in]    targetName: The designation of the comet.
  /// @throws       std::bad_alloc
  /// @throws       CRuntimeError(TargetComet: Comet specified not found.): E_TARGETCOMET_NOTFOUND
  /// @version      2018-09-02/GGB - Function created.

  CTargetComet::CTargetComet(std::string const &targetName)
    : CTargetAstronomy(targetName)
  {
    if (!loadCometData(targetName, elements_))
    {
      RUNTIME_ERROR(boost::locale::translate("TargetComet: Comet specified not found."), E_TARGETCOMET_NOTFOUND, LIBRARYNAME);
    };
  }

  /// @brief        Copy constructor for the class.
  /// @param[in]    toCopy: The instance to make a copy of.
  /// @throws       None.
  /// @version      2018-08-14/GGB - Function created.

  CTargetComet::CTargetComet(CTargetComet const &toCopy) : CTargetAstronomy(toCopy), elements_(toCopy.elements_)
  {
  }

  /// @brief        Creates an instance of a comet.
  /// @param[in]    cometName: The name or identifier for the comet.
  /// @returns      A unique pointer to a newly created instance.
  /// @version      2020-09-19/GGB - Function created.

  static std::unique_ptr<CTargetAstronomy> create(std::string const &cometName)
  {
    return std::move(std::make_unique<CTargetComet>(cometName));
  }

  /// @brief Creates a copy of this.
  /// @returns A constructed object with the same data as this.
  /// @throws std::bad_alloc
  /// @version 2018-09-14/GGB - Function created

  std::unique_ptr<CTargetAstronomy> CTargetComet::createCopy() const
  {
    std::unique_ptr<CTargetAstronomy> returnValue = std::make_unique<CTargetComet>(*this);

    return returnValue;
  }

  /// @brief        Sets the file path for the location of the CometEls.txt file.
  /// @param[in]    The path to where the file is stored.
  /// @throws       None.
  /// @version      2020-09-19/GGB - Function created.

  void CTargetComet::setFilePath(boost::filesystem::path const &newPath)
  {
    filePath = newPath;
  }

  /// @brief        Sets the file name for the location of the comet elements file.
  /// @param[in]    The name of the comet elements file.
  /// @throws       None.
  /// @version      2020-09-19/GGB - Function created.

  void CTargetComet::setFileName(boost::filesystem::path const &newName)
  {
    fileName = newName;
  }

  /// @brief      Searches the ComEls.txt file to find the specified object.
  /// @param[in]  cometName: The name of the minor planet, can be designation, or name. or number.
  /// @param[out] elements: The elements of the minor planet.
  /// @returns    true - The MP was found.
  /// @returns    false - The MP was not found.
  /// @throws     CRuntimeError - 0x3310: Unable to open CometEls.txt
  /// @version    2020-09-19/GGB - Moved function into CCometElements class.
  /// @version    2018-08-25/GGB - Function created.

  bool CTargetComet::loadCometData(std::string const &cometName, SCometElements &elements)
  {
    std::string szLine;
    std::string szName;
    bool returnValue = false;
    std::uint32_t lineIndex = 0;

    boost::filesystem::ifstream ifs(filePath / fileName, std::ios::in);
    if (ifs)
    {
      std::map<std::string, std::uint32_t>::const_iterator iterator = cometCache.find(cometName);
      if (iterator == cometCache.end())
      {
          /* We did not find the name in the cache. Need to search in the file.
           * The first cometCache.size() lines can be ignored as these particular lines would already be stored
           * in the cometCache.
           */

        lineIndex = cometCache.size();
        while (lineIndex)
        {
          std::getline(ifs, szLine);
          lineIndex--;
        };

        lineIndex = cometCache.size();

          // Now iterate over the file until the end of file, or the cometName is found.

        while ( (!ifs.eof()) && !returnValue)
        {
          std::getline(ifs, szLine);

          szName = szLine.substr(102, 56);

          cometCache.emplace(szName, lineIndex);

          if (szName.find(cometName) != std::string::npos)
          {
              // MP found.

            parseLine(szLine, elements);
            returnValue = true;
          };
          lineIndex++;
        };
      }
      else
      {
        lineIndex = iterator->second;

        while (lineIndex)
        {
          std::getline(ifs, szLine);
          lineIndex--;
        };

        parseLine(szLine, elements);
        returnValue = true;
      };

      ifs.close();
    }
    else
    {
      RUNTIME_ERROR(boost::locale::translate("CometElements: Unable to open ComEls.txt"), E_COMETUNABLETOOPEN, LIBRARYNAME);
    };

    return returnValue;
  }

  /// @brief      Parses a single line into the structure.
  /// @param[in]  szLine: The MPCorbital element line to parse.
  /// @param[out] elements: The elements parsed.
  /// @throws
  /// @version    2018-08-25/GGB - Function created.

  void CTargetComet::parseLine(std::string const &szLine, SCometElements &elements)
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
}
