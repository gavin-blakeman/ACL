//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								IERS
// SUBSYSTEM:						Import IERS Bulletin A data.
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:
// NAMESPACE:						ACL::IERS
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
// OVERVIEW:						This module provides code to read data from the IERS Bulletin A. (In text file format)
//
// CLASSES INCLUDED:
//
// CLASS HIERARCHY:
//
// HISTORY:             2018-09-04 GGB File Created
//
//*********************************************************************************************************************************

#include "../include/IERS.h"

  // Standard C++ library header files


  // ACL library header files.

#include "../include/error.h"

  // Miscellaneous library header files.

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"

namespace ACL
{
  namespace IERS
  {

    void parseIAUCelestialPoleOffset_BA(boost::filesystem::path const &, std::vector<SIAUOffset> &)
    {

    }

    void parseNEOSCelestialPoleOffset_BA(boost::filesystem::path const &, std::vector<SNEOSOffset> &)
    {

    }

    /// @brief Parse the prediced values from an IERS Bulletin A text file.
    /// @param[in] fileName: The file path and name.
    /// @param[out] predictions: The predictionjs value from the IERS Bulletin A.
    /// @throws 0x3320: IERS: Unable to open Bulletin A file specified.
    /// @throws 0x3321: IERS: Unable to parse file. Is the file name and type correct.
    /// @note 1. The predictions vector is cleared on entry to this function.
    /// @version 2018-09-04/GGB - Function created.

    void parsePredictions_BA(boost::filesystem::path const &fileName, std::vector<SPredictions> &predictions)
    {
      boost::filesystem::ifstream ifs(fileName, std::ios::in);
      std::string szLine;
      bool textFound = false;

      predictions.clear();

      if (!ifs)
      {
        ACL_ERROR(0x3320);
      }
      else
      {
          // Read lines until we get to a line that includes the text "PREDICTIONS:" This is the start of the predictions
          // section.

        do
        {
          std::getline(ifs, szLine);
        }
        while ( (!ifs.eof()) && (szLine.find("PREDICTIONS") == std::string::npos) );

        if (ifs.eof())
        {
          ifs.close();
          ACL_ERROR(0x3321);
        }
        else
        {
            // At the start of the predictions section.
            // Read lines until we get to the line before the start of the data.

          do
          {
            std::getline(ifs, szLine);
          }
          while ( (!ifs.eof()) && (szLine.find("UT1-UTC(sec)") == std::string::npos));

          if (ifs.eof())
          {
            ifs.close();
            ACL_ERROR(0x3321);
          }
          else
          {
              // Next line is the start of the data.
              // The data ends when the text "These predictions are based on all announced leap seconds." is found.

            do
            {
              std::vector<std::string> szTokens;

              std::getline(ifs, szLine);

              boost::char_separator<char> seperator(" ");
              boost::tokenizer<boost::char_separator<char>> tokens(szLine, seperator);
              for (const auto &t : tokens)
              {
                szTokens.emplace_back(t);
              };

                // Don't need to read all the columns.

            }
            while ( (!ifs.eof()) && (szLine.find("These predictions are based on all announced leap seconds.") == std::string::npos));
          }

        }

        ifs.close();
      };
    }

    void parseRapidValues_BA(boost::filesystem::path const &, std::vector<SRapidValues> &)
    {

    }

  } // namespace IERS
} // namespace ACL
