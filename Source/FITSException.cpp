//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSException
// SUBSYSTEM:						FITS Exception class
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, CFitsIO, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2015-2018 Gavin Blakeman.
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
// OVERVIEW:						Provides a lightweight wrapper to convert fits errors into exceptions.
//
// CLASSES INCLUDED:		CFITSException
//
// CLASS HIERARCHY:     std::runtime_error
//                        CFITSException
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-09-05/GGB - File created.
//
//*********************************************************************************************************************************

#include "../Include/FITSException.h"

  // Standard C++

#include <algorithm>

// Miscellaneous libraries

#include "cfitsio/fitsio.h"
#include <GCL>

namespace ACL
{
  /// @brief Class constructor
  /// @details Obtains all the current error messages from the cfitsio library and stores them in the exception.
  /// @throws None.
  /// @version 2016-04-07/GGB - Removed intialisation value and updated storage to a pair.
  /// @version 2015-09-05/GGB - Function created.

  CFITSException::CFITSException() : std::runtime_error("cfitsio Error")
  {
    char errorMessage[FLEN_CARD];
    int errorNumber;

      // Capture multiple messages.

    while (errorNumber = fits_read_errmsg(errorMessage))
    {
      errorMessages.emplace_back(std::make_pair(errorNumber, std::string(errorMessage)));
    };
  }

  /// @brief Returns the string of the error message.
  /// @returns The error message.
  /// @version 2015-09-05/GGB - Function created.

  std::string CFITSException::errorMessage() const
  {
    std::string returnValue;

    std::for_each(errorMessages.begin(), errorMessages.end(), [&] (TErrorMessage msg)
        {
          returnValue += "Library: cfitsio. Error Code: " + std::to_string(msg.first) + " - " + msg.second;
        });

    return returnValue;
  }

  /// @brief Function to write the error message to a logFile.
  /// @details This is not automatically done in the library when an exception is thrown as the library may be able to recover from
  /// the exception without having to terminate.
  /// @brief 2015-09-05/GGB - Function created.

  void CFITSException::logErrorMessage() const
  {
    std::for_each(errorMessages.begin(), errorMessages.end(), [&] (TErrorMessage msg)
    {
      GCL::logger::defaultLogger().
          logMessage(GCL::logger::error, "Library: cfitsio. Error Code: " + std::to_string(msg.first) + " - " + msg.second);
    });
  }

}
