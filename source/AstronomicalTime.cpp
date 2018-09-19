//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstronomicalTime
// SUBSYSTEM:						Time Classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost, SOFA, Novas
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2005-2018 Gavin Blakeman.
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
// OVERVIEW:						This file provides a class for storing time and performing time conversions between different time scales.
//
//
// CLASSES INCLUDED:		TJD           - Julian Date
//                      CAstroTime    - Stores time (date/time) as a TT value. Can produce values converted to any of the other
//                                      time scales on demand.
//
// CLASS HIERARCHY:     TJD
//                      CAstroTime (TJD)
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. Functions from other libraries, referenced by this library can be written in C.
//                      3. New functions should be written to be multi-thread safe.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-03-29 GGB - Renamed to AstronomicalTime.h
//                      2013-03-20 GGB - astroManager 2013.03 release.
//                      2013-01-22 GGB - astroManager 000.00 release.
//                      2005-06-19 GGB - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#include "../include/AstronomicalTime.h"

  // Standard C++ library header files. (std)

#include <cmath>
#include <fstream>
#include <iostream>

  // ACL Library

#include "../include/AstroFunctions.h"
#include "../include/AstroClass.h"

  // SOFA Library

#include "sofa.h"
#include "sofam.h"

  // Miscellaneous Libraries

#include "boost/algorithm/string.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/lexical_cast.hpp"
#include <GCL>

namespace ACL
{
  FP_t const SECONDSDAY = 86400;

  //*****************************************************************************************************************************
  //
  // Data access/management/loading functions
  //
  //*****************************************************************************************************************************

  /// @brief Loads the dAT information from a text file and stores it in the CAstroTime class.
  /// @details The dAT value is required to convert TAI to UTC using the formuala dAT = TAI-UTC. All values are converted to
  /// Modified Julian Days (MJD).
  /// @param[in] ifn File Name of the text file that contains the dAT data values.
  /// @throws ACL::CACLError(0x3205) - ASTROTIME: Error opening TAI-UTC file.
  /// @post The dAT values are loaded and available to any instance of the CAstroTime class.
  /// @version 2018-09-19/GGB - Bug #154 - Allow use of different seperators.
  ///                           Improved error handling.
  /// @version 2015-06-20/GGB - Moved function into CAstroTime class.
  /// @version 2014-12-25/GGB - Removed std::clog error reporting. Exception should be sufficient.
  /// @version 2013-09-16/GGB - Changed MJD to an unsigned long.
  /// @version 2013-01-23/GGB - Changed to use data from a standalone file and not from a database and moved into AstroTime.
  /// @version 2011-07-07/GGB - Function created.

  void CAstroTime::load_dAT(boost::filesystem::path const &ifn)
  {
    boost::filesystem::fstream ifs;            // Input file stream
    std::string textLine;
    size_t commaPosn;
    std::string szValue;
    unsigned long MJD;
    int offset;

    ifs.open(ifn);

    if (!ifs)
    {
      ERRORMESSAGE("Unable to open dAT data file. '" + ifn.string() + "'");
      ERROR(ACL, 0x3205);    // ASTROTIME: Error opening TAI-UTC file.
    }
    else
    {
      std::getline(ifs, textLine);   // First line of file is headers

      try
      {
        while (ifs.good())
        {
          std::getline(ifs, textLine);
          if (textLine.size() > 1)
          {
            commaPosn = textLine.find_first_of(",; \t",0);
            szValue = textLine.substr(0, commaPosn);
            boost::trim(szValue);
            MJD = boost::lexical_cast<unsigned long>(szValue);
            szValue = textLine.substr(commaPosn+1, textLine.npos);
            boost::trim(szValue);
            offset = boost::lexical_cast<int>(szValue);
            ACL::CAstroTime::add_dAT(MJD, offset);
          };
        };
      }
      catch(boost::bad_lexical_cast &)
      {
        ERRORMESSAGE("TAI-UTC file format is incorrect, unable to open TAI-UTC file.");
        ERRORMESSAGE("All time conversions are likely to be incorrect until this is resolved.");
      }
      catch(...)
      {
        ERROR(ACL, 0x3205);    // ASTROTIME: Error opening TAI-UTC file.
      }
      ifs.close();
    };
  }

  /// @brief Loads the dUT1 information from the file and stores it in the CAstroTime class.
  /// @details The dUT1 value is required to convert UT1 to UTC using the formuala dUT1 = UTC - UT1. All values are converted to
  ///          Modified Julian Days (MJD).
  /// @param[in] ifn: Path and file Name of the text file that contains the dUT1 data values.
  /// @throws ACL::CACLError(0x3206) - ASTROTIME: Error opening UTC-UT1 file.
  /// @post The dUT1 values are loaded and available to any instance of the CAstroTime class.
  /// @version 2018-09-19/GGB - Expanded seperators to use ' ' and '\t'
  ///                           Improved error handling when loading the file.
  /// @version 2015-06-20/GGB - Moved function into CAstroTime class.
  /// @version 2014-12-25/GGB - Removed std::clog error reporting and rely on exception.
  /// @version 2013-09-16/GGB - Changed MJD to an unsigned long.
  /// @version 2013/01-26/GGB - Moved into AstroTime file.
  /// @version 2011-07-07/GGB - Function created.

  void CAstroTime::load_dUT1(boost::filesystem::path const &ifn)
  {
    boost::filesystem::fstream ifs;            // Input file stream
    std::string textLine;
    size_t commaPosn;
    std::string szValue;
    unsigned long MJD;
    FP_t dUT1;
    std::string const seperators(",; \t");

    ifs.open(ifn);

    if (!ifs)
    {
      ERRORMESSAGE("Unable to open dUT1 data file. '" + ifn.string() + "'");
      ERROR(ACL, 0x3206);  // ASTROTIME: Error opening UTC-UT1 file.
    }
    else
    {
      std::getline(ifs, textLine);   // First line of file is headers

      try
      {
        while (ifs.good())
        {
          std::getline(ifs, textLine);
          if (textLine.size() > 1)
          {
            commaPosn = textLine.find_first_of(seperators, 0);
            szValue = textLine.substr(0, commaPosn);
            boost::trim(szValue);
            MJD = boost::lexical_cast<unsigned long>(szValue);

            // Now need the 5th value after the MJD

            commaPosn = textLine.find_first_of(seperators, commaPosn+1);
            commaPosn = textLine.find_first_of(seperators, commaPosn+1);
            commaPosn = textLine.find_first_of(seperators, commaPosn+1);
            commaPosn = textLine.find_first_of(seperators, commaPosn+1);

            textLine = textLine.substr(commaPosn+1, textLine.npos);
            commaPosn = textLine.find_first_of(seperators,0);
            szValue = textLine.substr(0, commaPosn);
            boost::trim(szValue);
            if (szValue.size() > 0)
            {
              dUT1 = boost::lexical_cast<double>(szValue);
              ACL::CAstroTime::add_dUT1(MJD, dUT1);
            };
          };
        };
      }
      catch(boost::bad_lexical_cast &)
      {
        ERRORMESSAGE("UTC-UT1 file format is incorrect, unable to open UTC-UT1 file.");
        ERRORMESSAGE("All time conversions are likely to be incorrect until this is resolved.");
      }
      catch(...)
      {
        ERROR(ACL, 0x3206);  // ASTROTIME: Error opening UTC-UT1 file.
      }
      ifs.close();
    };
  }

  //*******************************************************************************************************************************
  //
  // CAstroTime
  //
  //*******************************************************************************************************************************

  std::map<unsigned long, FP_t> CAstroTime::dUT1Array = std::map<unsigned long, FP_t>();
  std::map<unsigned long, int> CAstroTime::dATArray = std::map<unsigned long, int>();

  FP_t const dTAI = 32.184;   // Delta TAI.

  /// @brief Default constructor for the class.
  /// @throws None
  /// @version 2011-07-13/GGB - Function created.

  CAstroTime::CAstroTime() : TT_()
  {
  }

  /// @brief Copy constructor.
  /// @param[in] toCopy - The instance to copy from.
  /// @throws std::bad_alloc
  /// @version 2017-07-31/GGB - Function created.

  CAstroTime::CAstroTime(CAstroTime const &toCopy) : TT_(toCopy.TT_)
  {
  }

  /// @brief Constructor using a @a std::time_t value.
  /// @details The @a std::time_t value is considered to be a UTC value.
  /// @param[in] time The @a std::time_t value returned by a @a std::time(...) function call.
  /// @throws None.
  /// @version 2011-12-25/GGB - Function created.

  CAstroTime::CAstroTime(time_t const &time) : TT_()
  {
    TJD jd(time);

    constructClass(jd, ACL::TS_UTC);
  }

  /// @brief Constructor taking a julian day and time scale.
  /// @param[in] jd: The JD to construct the instance with.
  /// @param[in] timeScale: The time scale to construct the class with.
  /// @throws std::bad_alloc
  /// @version 2011-07-13/GGB - Function created.

  CAstroTime::CAstroTime(TJD const &jd, ETimeScale timeScale) : TT_()
  {
    constructClass(jd, timeScale);
  }

  /// @brief Constructor taking one doubles.
  /// @param[in] jd: The JD to construct the instance with.
  /// @param[in] timeScale: The time scale to construct the class with.
  /// @throws std::bad_alloc
  /// @version 2011-07-13/GGB - Function created.

  CAstroTime::CAstroTime(FP_t jd, ETimeScale timeScale) : TT_()
  {
    TJD JD((FP_t) jd);

    constructClass(JD, timeScale);
  }

  // Constructor taking two doubles.
  //
  // 2011-07-13/GGB - Function created.

  CAstroTime::CAstroTime(FP_t jd0, FP_t jd1, ETimeScale timeScale) : TT_()
  {
    TJD jd(jd0, jd1);

    constructClass(jd, timeScale);
  }

  /// @brief Constructor for YYYY-MM-DD HH-MM-SS time.
  //
  /// @version 2011-07-16/GGB - Function created.

  CAstroTime::CAstroTime(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int min,
    FP_t second, ETimeScale timeScale) : TT_()
  {
    TJD jd(year, month, day, hour, min, second);

    constructClass(jd, timeScale);
  }

  /// @brief Constructor taking a struct tm type.
  /// @param[in] time - The struct tm having the time parameters.
  /// @param[in] timeScle - The timeScale to use.
  /// @throws std::bad_alloc
  /// @version 2017-07-30/GGB - Function created.

  CAstroTime::CAstroTime(std::tm const *time, ETimeScale timeScale) : TT_()
  {
    TJD jd(time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

    constructClass(jd, timeScale);
  }

  /// @brief Addition operator for class.
  /// @throws None.
  /// @version 2016-03-25/GGB - Function created.

  const CAstroTime CAstroTime::operator+(CAstroTime const &rhs) const
  {
    CAstroTime returnValue(*this);

    returnValue += rhs;

    return returnValue;
  }

  /// @brief Subtraction operator for class.
  /// @throws None.
  /// @version 2016-03-25/GGB - Function created.

  const CAstroTime CAstroTime::operator-(const CAstroTime &rhs) const
  {
    CAstroTime returnValue(*this);

    returnValue -= rhs;

    return returnValue;
  }

  /// @brief Addition operator for class.
  /// @throws None.
  /// @version 2016-03-25/GGB - Function created.

  CAstroTime &CAstroTime::operator +=(CAstroTime const &rhs)
  {
    TT_ += rhs.TT_;

    return *this;
  }

  /// @brief Subtraction operator for class.
  /// @throws None.
  /// @version 2016-03-25/GGB - Function created.

  CAstroTime &CAstroTime::operator-=(CAstroTime const &rhs)
  {
    TT_ -= rhs.TT_;

    return *this;
  }

  // Adds the value into the array.
  //
  // 2013-01-23/GGB - Changed first parameter to long.
  // 2011-07-09/GGB - Function created.

  void CAstroTime::add_dAT(unsigned long mjd, int val)
  {
    dATArray[mjd] = val;
  }

  // Adds the value into the array.
  //
  // 2013-01-23/GGB - Changed first parameter to long.
  // 2011-07-09/GGB - Function created.

  void CAstroTime::add_dUT1(unsigned long mjd, FP_t val)
  {
    dUT1Array[mjd] = val;
  }

  /// @brief Constructs the class using a JD and time scale.
  /// @param[in] jd: The Julian day to construct with.
  /// @param[in] timeScale: The time scale to use.
  /// @throws ACL::CACLError(0x3200)
  /// @throws ACL::CACLCodeError()
  /// @version 2011-07-13/GGB - Function created.

  void CAstroTime::constructClass(const TJD &jd, ETimeScale timeScale)
  {
    switch (timeScale)
    {
      case TS_NONE:
      {
        ERROR(ACL, 0x3200);
        break;
      }
    case TS_UTC:
    case TS_UT1:
    case TS_TAI:
    case TS_TDB:
      TT_ = convertTime(jd, timeScale, TS_TT);
      break;
    case TS_TT:
      TT_ = jd;           // No conversion necessary.
      break;
      default:
      {
        CODE_ERROR(ACL);
        break;
      };
    };
  }

  /// @brief Converts time from one time scale to another.
  /// @details Use is made of other funcitons and the conversion is done in a two step process.
  ///           Step 1.  Convert to TT
  ///           Step 2.  Convert from TT to the required time scale.
  /// @param[in] jd:
  /// @param[in] ts1:
  /// @param[in] ts2:
  /// @throws
  /// @version 2011-07-13/GGB - Function created.

  TJD CAstroTime::convertTime(TJD const &jd, ETimeScale ts1, ETimeScale ts2)
  {
    TJD interJD;
    TJD retJD;

    switch (ts1)
    {
      case TS_NONE:
      {
        ERROR(ACL, 0x3200);
        break;
      };
    case TS_UTC:
      interJD = UTCtoTT(jd);
      break;
    case TS_UT1:
      interJD = UT1toTT(jd);
      break;
    case TS_TAI:
      interJD = TAItoTT(jd);
      break;
    case TS_TDB:
      interJD = TDBtoTT(jd);
      break;
    case TS_TT:
      interJD = jd;
      break;
      default:
      {
        CODE_ERROR(ACL);
        break;
      };

    };

    switch (ts2)
    {
      case TS_NONE:
      {
        ERROR(ACL, 0x3200);
      };
    case TS_UTC:
      retJD = TTtoUTC(interJD);
      break;
    case TS_UT1:
      retJD = TTtoUT1(interJD);
      break;
    case TS_TAI:
      retJD = TTtoTAI(interJD);
      break;
    case TS_TDB:
      retJD = TTtoTDB(interJD);
      break;
    case TS_TT:
      retJD = interJD;
      break;
      default:
      {
        CODE_ERROR(ACL);
        break;
      };
    };

    return retJD;
  }

  /// @brief Returns the value of dAT for the given date.
  /// @param[in] JD:
  /// @throws 0x3207 - ASTROTIME: Cannot get TAI-UTC before 1973.
  /// @todo Convert function from unsigned long to properly defined type.
  // 2011-07-07/GGB - Function created.

  FP_t CAstroTime::dAT(unsigned long JD)
  {
    bool found = false;
    int returnValue = 0;

    std::map<unsigned long, int>::const_iterator iter;

    iter = dATArray.end();

    while (iter != dATArray.begin() && !found)
    {
      if ((*iter).first < JD)
      {
        returnValue = (*iter).second;
        found = true;
      }
      else
        iter--;
    };

    if (!found)
    {
      ERROR(ACL, 0x3207);    // ASTROTIME: Cannot get TAI-UTC before 1973.
    }
    else
      return returnValue;
  }

  /// @brief Returns the dAT for this objects tt.
  /// @returns the dAT value for this instance tt.
  /// @version  2011-07-09/GGB - Function created.

  FP_t CAstroTime::dAT() const
  {
    return dAT(TT_.MJD());
  }

  // Returns the dAT for the specified time.
  //
  // 2011-07-09/GGB - Function created.

  FP_t CAstroTime::dAT(TJD const &jd, ETimeScale ts)
  {
    return dAT(jd.MJD());
  }

  // Calculates the value of dT
  //
  // 2011-07-13/GGB - Function created.

  FP_t CAstroTime::dT() const
  {
    TJD jd;
    FP_t returnValue = dTAI;

    jd = TTtoUTC(TT_);
    returnValue += dAT(jd.MJD());
    returnValue += dUT1(jd.MJD());
    return returnValue;
  }

  // Calculates the value of dT
  //
  // 2011-07-13/GGB - Function created.

  FP_t CAstroTime::dT(TJD const &jd, ETimeScale ts)
  {
    TJD jd1;
    FP_t returnValue = dTAI;

    jd1 = convertTime(jd, ts, TS_UTC);
    returnValue += dAT(jd1.MJD());
    returnValue += dUT1(jd1.MJD());
    return returnValue;
  }

  // Returns the value of dUT1
  //
  // 2011-07-07/GGB - Function created.

  FP_t CAstroTime::dUT1(unsigned long JD)
  {
    FP_t returnVal;

    if (dUT1Array.count(JD) < 1)
    {
      ERROR(ACL, 0x3201);    // No dUT for dates less than 1973.
    }
    else
      returnVal = dUT1Array.find(JD)->second;

    return returnVal;
  }

  // Return the dUT1 value for this object.
  //
  // /2011-07-09/GGB - Function created.

  FP_t CAstroTime::dUT1() const
  {
    return dUT1(TT_.MJD());
  }

  // Returns the dUT1 value for the object.
  //
  // 2011-07-09/GGB - Function created.

  FP_t CAstroTime::dUT1(TJD const &jd, ETimeScale ts)
  {
    return dUT1(jd.MJD());
  }

  // Converts the stored time to the specified time scale.
  //
  // 2011-07-05/GGB - Function created.

  TJD CAstroTime::time(ETimeScale timeScale) const
  {
    TJD returnValue;

    switch (timeScale)
    {
      case TS_NONE:
      {
        ERROR(ACL, 0x3200);
        break;
      };
    case TS_UTC:
      returnValue = UTC();
      break;
    case TS_UT1:
      returnValue = UT1();
      break;
    case TS_TT:
      returnValue = TT();
      break;
    case TS_TAI:
      returnValue = TAI();
      break;
    case TS_TDB:
      returnValue = TDB();
      break;
      default:
      {
        CODE_ERROR(ACL);
        break;
      };
    };

    return returnValue;
  }

  /// @brief Returns the time value in the TAI scale.
  /// @returns The time value as a TAI value.
  /// @throws None.
  /// @version 2011-07-05/GGB - Function created.

  TJD CAstroTime::TAI() const
  {
    return ((TJD) TT_ - static_cast<double>(dTAI));
  }

  // Converts a time in TAI to TT.
  //
  // 2011-07-13/GGB - Function created.

  TJD CAstroTime::TAItoTT(TJD const &toConvert)
  {
    TJD returnValue(toConvert);

    returnValue += dTAI/SECONDSDAY;

    return returnValue;
  }

  TJD CAstroTime::TDB() const
  {
    return TT_;
  }

  // Converts a time in TBD to TT.
  //
  //

  TJD CAstroTime::TDBtoTT(TJD const &toConvert)
  {
    TJD returnValue;

    return returnValue;
  }

  /// Converts TDT to TT (trivial as they are the same.
  //
  // 2011-07-13/GGB - Function created.

  TJD CAstroTime::TDTtoTT(TJD const &toConvert)
  {
    return toConvert;
  }

  /// Returns the terrestial time.
  //
  // 2011-07-05/GGB - Function created.

  TJD CAstroTime::TT() const
  {
    return TT_;
  }

  // Converts TT to TAI.
  //
  // 2011-07-13/GGB - Function created.

  TJD CAstroTime::TTtoTAI(TJD const &toConvert)
  {
    TJD returnValue(toConvert);

    returnValue -= dTAI/SECONDSDAY;

    return returnValue;
  }

  // Calculates the value of TDB from TT
  //
  // 2011-07-13/GGB - Function created.

  TJD CAstroTime::TTtoTDB(TJD const &toConvert)
  {
    TJD returnValue(toConvert);

    FP_t g = 357.53 * DD2R + .9856003 * (toConvert.floor() - 2451545.0);

    returnValue += 0.001658/SECONDSDAY*sin(g) + 0.000014/SECONDSDAY*sin(2*g);

    return returnValue;
  }

  // Converts TT to TDT (trivial as they are the same)
  //
  // 2011-07-13/GGB - Function created.

  TJD CAstroTime::TTtoTDT(TJD const &toConvert)
  {
    return toConvert;
  }

  // Converts TT to UT1
  //
  // 2011-07-13/GGB - Function created.

  TJD CAstroTime::TTtoUT1(TJD const &toConvert)
  {
    TJD retVal;

    try
    {
      retVal = TTtoUTC(toConvert);
      retVal -= dUT1(toConvert.MJD())/SECONDSDAY;
      return retVal;
    }
    catch (GCL::CError &)
    {
      return TTtoUTC(toConvert);
    };
  }

  /// @brief Converts TT to UTC
  /// @param[in] toConvert - The time to convert.
  /// @returns The converted time
  /// @version 2011-07-13/GGB - Function created.

  TJD CAstroTime::TTtoUTC(TJD const &toConvert)
  {
    TJD jd0(toConvert);
    TJD retVal(toConvert);

    retVal -= dTAI/SECONDSDAY;
    retVal -= dAT(jd0.MJD())/SECONDSDAY;

    return retVal;
  }

  // Calculates and returns the value of UT1
  //
  // 2011-07-07/GGB - Function created.

  TJD CAstroTime::UT1() const
  {
    return TTtoUT1(TT_);
  }

  /// @brief Converts a date/time in the UT1 system to TT.
  /// @param[in] toConvert - time to convert.
  /// @returns The time as a TT
  /// @throws None.
  /// @version 2017-08-18/GGB - Function created.

  TJD CAstroTime::UT1toTT(TJD const &toConvert)
  {
    TJD jd0(toConvert);
    TJD returnValue(toConvert);

    returnValue += dAT(jd0.MJD())/SECONDSDAY;
    returnValue += dTAI/SECONDSDAY;
    returnValue -= dUT1(jd0, TS_UT1);

    return returnValue;
  }

  /// @brief calculates and returns the value of UTC
  /// @throws None.
  /// @version 2011-07-07/GGB - Function created.

  TJD CAstroTime::UTC() const
  {
    return TTtoUTC(TT_);
  }

  /// @brief Converts a date/time in UTC to TT
  /// @param[in] toConvert the time to convert
  /// @returns The converted time.
  /// @version 2011-12-31/GGB - Function created.

  TJD CAstroTime::UTCtoTT(TJD const &toConvert)
  {
    TJD jd0(toConvert);
    TJD returnValue(toConvert);

    returnValue += dAT(jd0.MJD())/SECONDSDAY;
    returnValue += dTAI/SECONDSDAY;

    return returnValue;
  }

}  // namespace ACL
