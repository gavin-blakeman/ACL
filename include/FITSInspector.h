//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSInspector
// SUBSYSTEM:						FITS Inspector
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
// OVERVIEW:						A class to view the HDU data for FITS files.
//
// CLASSES INCLUDED:		CFITSInspector
//
// CLASS HIERARCHY:     CFITSInspector
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-08-07/GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_FITSINSPECTOR_H
#define ACL_FITSINSPECTOR_H

  // Standard C++ libraries

#include <string>
#include <vector>

  // Miscellaneous libraries

#include "boost/filesystem.hpp"

namespace ACL
{

  /// @brief The CFITSInspector class is a lightweight class for inspecting the contents of the FITS file, specifically the
  ///        headers and the keywords, without loading the data.
  /// @details The class loads all the keywords as strings to simplify the class response and code.

  class CFITSInspector
  {
  public:
    class CHDU
    {
    public:
      class SFITSKeyword
      {
      private:
        SFITSKeyword() = delete;

      public:
        std::string keyword;
        std::string value;
        std::string comment;

        SFITSKeyword(std::string const &nk, std::string const &nv, std::string const &nc) : keyword(nk), value(nv), comment(nc) {}
      };
      typedef std::vector<SFITSKeyword> DKeyword;

      int hduNumber;
      int hduType;
      std::string hduName;
      DKeyword keywordData;
    };
    typedef std::vector<CHDU> DHDU;

  private:
    boost::filesystem::path fileName_;
    DHDU hduData;

    CFITSInspector() = delete;

  protected:

  public:
    CFITSInspector(boost::filesystem::path const &fn);

    virtual boost::filesystem::path const &fileName() const noexcept { return fileName_; }

    void loadHeaderData();

    DHDU::iterator begin() { return hduData.begin(); }
    DHDU::iterator end() { return hduData.end(); }
    CHDU::DKeyword::iterator begin(DHDU::size_type i) { return hduData[i].keywordData.begin(); }
    CHDU::DKeyword::iterator end(DHDU::size_type i) { return hduData[i].keywordData.end(); }
    CHDU::DKeyword::size_type keywordCount(DHDU::size_type i) { return hduData[i].keywordData.size(); }

    virtual void reset() { hduData.clear(); }

  };

}   // namespace ACL

#endif // ACL_FITSINSPECTOR_H
