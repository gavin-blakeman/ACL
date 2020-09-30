//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								TTargetMinorPlanet
// SUBSYSTEM:						Object Positions and Calculations
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:  MCL
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
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
// OVERVIEW:						These classes provide methods of handling minor planets
//
// CLASSES INCLUDED:    CTargetMinorPlanet - Class for managing minor planets.
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetSolar
//                          CTargetMinorPlanet
//
// HISTORY:             2016-03-23 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "include/targetMinorPlanet.h"

  // Standard C++ library headers

#include <cstdint>
#include <cmath>
#include <fstream>
#include <vector>

  // Miscellaneous library header files.

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/locale.hpp"
#include <GCL>

  // ACL library header files.

#include "include/error.h"

namespace ACL
{
  boost::filesystem::path CTargetMinorPlanet::filePath;
  boost::filesystem::path CTargetMinorPlanet::fileName("MPCORB.DAT");

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


  /// @brief        Constructor constructs from an MPCORB 1 line record.
  /// @param[in]    targetName: The name of the target.
  /// @throws       CRuntimeError(E_TARGETMP_MPNOTFOUND)
  /// @version      2020-09-19/GGB - Removed fileName parameter.
  /// @version      2018-09-02/GGB - Added fileName parameter to constructor.
  /// @version      2016-03-25/GGB - Function created.

  CTargetMinorPlanet::CTargetMinorPlanet(std::string const &targetName)
    : CTargetAstronomy(targetName), i_(0), OMEGA_(0), omega_(0), M0_(0), n_(0)
  {
    if (!loadMPData(targetName, elements_))
    {
      RUNTIME_ERROR(boost::locale::translate("TargetMinorPlanet: Minor Planet specified not found."), E_TARGETMP_MPNOTFOUND, "ACL");
    };
  }

  /// @brief        Copy constructor.
  /// @param[in]    toCopy: Instance to make a copy of.
  /// @throws       None.
  /// @version      2018-09-15/GGB - Function created.

  CTargetMinorPlanet::CTargetMinorPlanet(CTargetMinorPlanet const &toCopy) : CTargetAstronomy(toCopy),
    designation_(toCopy.designation_), elements_(toCopy.elements_), epoch_(toCopy.epoch_), M0_(toCopy.M0_), omega_(toCopy.omega_),
    a_(toCopy.a_), e_(toCopy.e_), i_(toCopy.i_), OMEGA_(toCopy.OMEGA_), n_(toCopy.n_)
  {
  }

  /// @brief Creates a copy of this instance.
  /// @returns Pointer to a new copy.
  /// @throws std::bad_aSlloc
  /// @version 2018-09-15/GGB - Function created.

  std::unique_ptr<CTargetAstronomy> CTargetMinorPlanet::createCopy() const
  {
    std::unique_ptr<CTargetAstronomy> returnValue(new CTargetMinorPlanet(*this));

    return returnValue;
  }

  /// @brief      Searches the MPCORB file to find the specified object.
  /// @param[in]  mpName: The name of the minor planet, can be designation, or name. or number.
  /// @param[out] elements: The elements of the minor planet.
  /// @returns    true - The MP was found.
  /// @returns    false - The MP was not found.
  /// @throws     0x3300: Unable to Open MPCORB.DAT
  /// @version    2018-08-25/GGB - Function created.

  bool CTargetMinorPlanet::loadMPData(std::string const &mpName, SMPCORB &elements)
  {
    std::string szLine;
    bool returnValue = false;
    boost::filesystem::ifstream ifs(fileName, std::ios::in);

    if (!ifs)
    {
      RUNTIME_ERROR("MPCORB: Unable to open MPCORB.DAT", E_UNABLETOOPEN_MPCORB, "ACL");
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

  /// @brief Calculates and returns the ICRS position of a minor planet.
  /// @param[in] time: The time to compute the position.
  /// @returns The ICRS coordinates at the specified time.
  /// @throws
  /// @version 2018-08-25/GGB - Function created.

  CAstronomicalCoordinates CTargetMinorPlanet::positionICRS(CAstroTime const &time) const
  {

  }

  /// @brief      Calculates the observed place of a minor planet.
  /// @param[in]  UTC: Time to calculate the observed place.
  /// @param[in]  observatory: Details of the observatory
  /// @param[in]  weather: Details of the weather.
  /// @throws
  /// @version    2020-09-30/GGB - Function created.

  SObservedPlace CTargetMinorPlanet::positionObserved(CAstroTime const &UTC, CGeographicLocation const &, CWeather const &) const
  {
    MCL::CAngle M(0);
    CAstroTime deltaT = epoch_ - UTC;
    FP_t E0, E1;
    std::uint_least32_t loopCounter = 0;
    FP_t v, r;
    FP_t xv, yv;
    FP_t xh, yh, zh;

      // Calculate mean anomoly

    M = M0_ + (n_ * static_cast<FP_t>(deltaT.TT()));

      // Solve M = E - e sin(E)
      // f(E) = E - e sin(E) - M
      // f'(E) = 1 - e cos(E)
      // Use Newton-Raphson iteration

    E0 = MCL::PI;
    E1 = 0;

      // Iterate with a maxumun of 100,000 iterations looking for an accuracy of seven places.

    while ( ((E1 - E0) > 0.0000001) && (loopCounter < 100000))
    {
      E1 = E0 + (M.radians() + (e_ * std::sin(E0)) - E0) / (1 - e_ * std::cos(E0));
      loopCounter++;
    };

      // Find true anomoly (v) and Heliocentric distance (r)

    xv = a_ * ( std::cos(E1) - e_);
    yv = a_ * (std::sqrt(1.0 - MCL::pow2(e_)) * std::sin(E1));
    v = std::atan2(yv, xv);
    r = std::sqrt(MCL::pow2(xv) + MCL::pow2(yv));

      // Compute position in 3D space

    //xh = r * ( cos(N) * cos(v+w) - sin(N) * sin(v+w) * cos(i) );
    //yh = r * ( sin(N) * cos(v+w) + cos(N) * sin(v+w) * cos(i) );
    //zh = r * ( sin(v+w) * sin(i) )


  }

  /// @brief      Factory function taking the descriptor of the object to create.
  /// @param[in]  descriptor: The descriptor of the object to load/create.
  /// @returns    Pointer to the created CMinorPlanet structure
  /// @throws     0x2700 - TargetMinorPlanet: Minor Planet Specified not found.
  /// @version    2020-09-19/GGB - Removed fileName parameter.
  /// @version    2018-08-24/GGB - Function created.

  std::unique_ptr<CTargetMinorPlanet> CTargetMinorPlanet::create(std::string const &descriptor)
  {
    return std::move(std::make_unique<CTargetMinorPlanet>(descriptor));
  }

  /// @brief      Parses a single line into the structure.
  /// @param[in]  szLine: The MPCorbital element line to parse.
  /// @param[out] elements: The elements parsed.
  /// @throws
  /// @version    2018-08-25/GGB - Function created.

  void CTargetMinorPlanet::parseLine(std::string const &szLine, SMPCORB &elements)
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

  /// @brief      Sets the file path for the MPCORB elements file.
  /// @param[in]  newPath: The new path to the MPCORB file.
  /// @throws     None
  /// @version    2020-09-19/GGB - Function created.

  void CTargetMinorPlanet::setFilePath(boost::filesystem::path const &newPath)
  {
    filePath = newPath;
  }

  /// @brief      Sets the file path for the MPCORB elements file. (This can include the path)
  /// @param[in]  newName: The new name for the MPCORB file.
  /// @throws     None
  /// @version    2020-09-19/GGB - Function created.

  void CTargetMinorPlanet::setFileName(boost::filesystem::path const &newName)
  {
    fileName = newName;
  }

} // namespace ACL
