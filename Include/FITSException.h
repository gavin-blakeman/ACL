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
//                      Copyright 2015-2017 Gavin Blakeman.
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
// OVERVIEW:						Provides exception support for cfitsio
//
// CLASSES INCLUDED:		CFITSException
//
// CLASS HIERARCHY:     std::runtime_error
//                        CFITSException
//
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2015-09-05/GGB - File created.
//
//*********************************************************************************************************************************

#ifndef CFITSIOERROR_H
#define CFITSIOERROR_H

  // Standard C++ libraries

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ACL
{
  class CFITSException : std::runtime_error
  {
  private:
    typedef std::pair<int, std::string> TErrorMessage;
    std::vector<TErrorMessage> errorMessages;

  protected:
  public:
    explicit CFITSException();

    std::string errorMessage() const;
    void logErrorMessage() const;
  };

  #define CFITSIO_TEST(EXPRESSION) {if (int err_ = EXPRESSION) { if (err_) throw ACL::CFITSException(); };}

}    // namespace ACL

#endif // CFITSIOERROR_H
