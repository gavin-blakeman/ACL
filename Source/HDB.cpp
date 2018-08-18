//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								HDB.h
// SUBSYSTEM:						Astronomical HDB classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, cfitsio, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2018 Gavin Blakeman.
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
// OVERVIEW:						A number of classes supporting file IO of astronomical data including images. The classes are designed to
//                      present a single interface to the other astro library classes while supporting several different file types.
//
// CLASSES INCLUDED:		CHDB            - Class to encapsulate header and data information.
//
// CLASS HIERARCHY:     CHDB
//                        - CImageHDB
//                        - CHDBAsciiTable
//                          - CAstrometryHDB
//                          - CHDBPhotometry
//                        - CHDBBinTable
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-03-17 GGB - Moved class CHDBPhotometry into seperated file.
//                      2013-02-10 GGB - Moved class CHDBImage into seperate file.
//                      2013-02-01 GGB - Moved class CAstrometryHDB into seperate file.
//                      2013-01-20 GGB - Release of astroManager 0000.00
//                      2011-12-10 GGB - Move classed from AstroFile.h to HDB.h
//
//*********************************************************************************************************************************

#include "../Include/HDB.h"

#include "../Include/AstroFile.h"
#include "../Include/common.h"
#include "../Include/FITS.h"
#include "../Include/FITSException.h"
#include "../Include/FITSKeyword.h"
#include "../Include/FITSKeywordBool.h"
#include "../Include/FITSKeywordDouble.h"
#include "../Include/FITSKeywordInt08.h"
#include "../Include/FITSKeywordInt16.h"
#include "../Include/FITSKeywordInt32.h"
#include "../Include/FITSKeywordInt64.h"
#include "../Include/FITSKeywordString.h"
#include "../Include/FITSKeywordUInt08.h"
#include "../Include/FITSKeywordUInt16.h"
#include "../Include/FITSKeywordUInt32.h"

  // Boost Library

#include <boost/algorithm/string.hpp>
#include "boost/lexical_cast.hpp"
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>

  // C++ Libraries

#include <algorithm>
#include <cstddef>
#include <typeinfo>

namespace ACL
{

  //*******************************************************************************************************************************
  //
  // class CHDB
  //
  //*******************************************************************************************************************************

  /// @brief Constructor taking the name of the new HDB
  /// @param[in] np - Parent Object
  /// @param[in] name - name of the HDB (HDU)
  /// @throws 0x1908 - HDB: parent cannot be == NULL.
  /// @version 2011-11-27/GGB - Function created.

  CHDB::CHDB(CAstroFile *np, const std::string &name) : parent_(np), bPrimary_(false), bSimple(true), naxis_(0),
    firstEdit_(true), extname_(name)
  {
    if (!parent_)
    {
      ACL_ERROR(0x1908);              // HDB: parent cannot be == NULL.
    };
    boost::algorithm::to_upper(extname_);    // Convert to upper case.
    if (extname_ == "PRIMARY")
    {
      bPrimary_ = true;
    };
  }

  /// @brief Copy constructor
  /// @param[in] toCopy - HDB to be copied.
  /// @throws None.
  /// @version 2013-06-07/GGB - Function created.

  CHDB::CHDB(CHDB const &toCopy) : parent_(toCopy.parent_),
    bPrimary_(toCopy.bPrimary_), bSimple(toCopy.bSimple), naxis_(toCopy.naxis_), pcount_(toCopy.pcount_), gcount_(toCopy.gcount_),
    naxisn_(toCopy.naxisn_), comments_(toCopy.comments_), firstEdit_(toCopy.firstEdit_), extname_(toCopy.extname_), history_(toCopy.history_)
  {
      // Need to copy all the keywords

    DKeywordStore::const_iterator keywordIterator;

    for (keywordIterator = toCopy.keywords_.begin(); keywordIterator != toCopy.keywords_.end(); ++keywordIterator)
    {
      PFITSKeyword newKeyword((*keywordIterator)->createCopy());

      keywords_.push_back(newKeyword);
    };
  }

  /// @brief Equality operator. Check for the name of the HDB being the same as the passed string. A case insensitive comparison is
  /// done.
  /// @param[in] name - name of HDB to check.
  /// @throws None.
  /// @version 2011-12-11/GGB - Function created.

  bool CHDB::operator==(std::string const &name) const
  {
    std::string tempName = name;

    boost::algorithm::to_upper(tempName);
    return (tempName == extname_);
  }

  /// @brief Adds a comment to the list of comments
  /// @param[in] newComment - The copy to add to the list of comments.
  /// @throws GCL::CRuntimeAssert
  /// @details The comments are not sorted and the new comment is simply added to the end of the comment list.
  /// @version 2015-08-10/GGB - Updated to use std::vector.
  /// @version 2011-12-10/GGB - Function created.

  void CHDB::commentWrite(std::string const &newComment)
  {
    RUNTIME_ASSERT(ACL, !newComment.empty(), "Parameter newComment cannot be empty.");

    comments_.emplace_back(newComment);
  }

  /// @brief Returns the comment string.
  /// @returns The comment string.
  /// @throws None.
  /// @version 2015-09-07/GGB - (Bug 73) Corrected bug. Using history_, not comments_ for function.
  /// @version 2015-08-11/GGB - Converted to support a vector of strings and return a single string.
  /// @version 2013-06-29/GGB - Changed return value to a "std::string" from a "std::string const &"
  /// @version 2011-12-10/GGB - Function created.

  std::string CHDB::commentGet() const
  {
    std::string returnValue;

    std::for_each(comments_.cbegin(), comments_.cend(), [&] (std::string s) {returnValue += s + "\n";});

    return returnValue;
  }

  /// @brief Copies the keywords from the reference HDB.
  /// @param[in] toCopy - Pointer to the HBD to copy.
  /// @throws None.
  /// @version 2014-05-31/GGB - Function created.

  void CHDB::copyKeywords(PHDB const &toCopy)
  {
    DKeywordStore::const_iterator iter = toCopy->keywords_.begin();

    for (; iter != toCopy->keywords_.end(); iter++)
    {
      keywordWrite(*iter);
    };
  }

  /// @brief Returns the exposure time from an image.
  /// @returns The Exposure time associated with the HDB. (s)
  /// @throws 0x190D - HDB: EXPOSURE or EXPTIME keyword not found.
  /// @todo This function should only appear in the image HDB.
  /// @version 2016-04-11/GGB - Updated to reflect use of FITSKeyword hierarchy
  /// @version 2013-04-09/GGB - Moved from CImageHDB to CHDB.
  /// @version 2012-01-31/GGB - Function created.

  FP_t CHDB::EXPOSURE()
  {
    FP_t exposureTime;

    if (keywordExists(NOAO_EXPTIME) )
    {
      exposureTime = static_cast<FP_t>(keywordData(NOAO_EXPTIME));
    }
    else if (keywordExists(HEASARC_EXPOSURE) )
    {
      exposureTime = static_cast<FP_t>((keywordData(HEASARC_EXPOSURE)));
    }
    else
    {
      ACL_ERROR(0x190D);      // HDB: EXPOSURE or EXPTIME keyword not found.
    };

    return exposureTime;
  }

  /// @brief Ensures that the history is added to show the software that has modified the file.
  /// @throws None.
  /// @version 2013-06-29/GGB - Function created.

  void CHDB::firstEdit()
  {
    if (firstEdit_)
    {
      firstEdit_ = false;

      std::string szText = "File modified by " + ACL_LIBNAME + " library. Build: " + getVersionString();

      historyWrite(szText);
    };
  }

  /// @brief Sets the GCOUNT value.
  /// @param[in] gcount - The new GCOUNT value.
  /// @throws None.
  /// @version 2012-01-12/GGB - Function created.

  void CHDB::GCOUNT(int gcount)
  {
    gcount_ = gcount;
  }

  /// @brief Returns the observation time from the parent object.
  /// @returns A smart pointer to the time of the observation.
  /// @throws 0x1908 - HDB: parent cannot be == NULL.
  /// @version 2011-12-23/GGB - Function created.

  CAstroTime const &CHDB::getObservationTime() const
  {
    if (parent_ != nullptr)
    {
      return parent_->getObservationTime();
    }
    else
    {
      ACL_ERROR(0x1908);      // HDB: parent cannot be == NULL.
    };
  }

  /// @brief Gets the observation weather from the parent.
  /// @returns A pointer to a weather instance.
  /// @throws 0x1908 - HDB: parent cannot be == NULL.
  /// @version 2011-12-23/GGB - Function created.

  std::unique_ptr<CWeather> &CHDB::getObservationWeather() const
  {
    if (parent_ != nullptr)
    {
      return parent_->getObservationWeather();
    }
    else
    {
      ACL_ERROR(0x1908);        // HDB: parent cannot be == NULL.
    };
  }

  /// @brief Adds a new line to the history
  /// @param[in] newHistory - New history line to add.
  /// @throws None.
  /// @version 2013-06-29/GGB - Changed history_ to a std::vector<>.
  /// @version 2011-12-10/GGB - Function created.

  void CHDB::historyWrite(std::string const &newHistory)
  {
    assert(newHistory.size() != 0);

    history_.push_back(newHistory);
  }

  /// @brief Returns the total history string
  /// @returns The history string.
  /// @throws None.
  /// @version 2015-09-07/GGB - Use C++14 labda function and std::for_each
  /// @version 2013-06-29/GGB - Changed history_ to a std::vector<>. Changed return value to a "std::string" from a "std::string
  /// const &"
  /// @version 2011-12-10/GGB - Function created.

  std::string CHDB::historyGet() const
  {
    std::string returnValue;

    std::for_each(history_.cbegin(), history_.cend(), [&] (std::string s) {returnValue += s + "\n";});

    return returnValue;
  }

  /// @brief Returns the number of keywords stored in the keyword array.
  /// @returns The number of keywords.
  /// @throws None.
  /// @version 2012-12-10/GGB - Function created.

  size_t CHDB::keywordCount() const
  {
    return keywords_.size();
  }

  /// @brief Returns the requested keyword.
  /// @details The list of keywords is searched for the first matching keyword and the pointer to the keyword is returned.
  /// @param[in] kwd - The keyword to return.
  /// @returns a constant reference to the keyword structure.
  /// @throws None.
  /// @version 2017-08-04/GGB - Removed all support for special keywords. Not required.
  /// @version 2012-01-11/GGB - Added support for XTENSION keyword
  /// @version 2011-12-04/GGB - Function created.

  CFITSKeyword const &CHDB::keywordData(std::string const &kwd) const
  {
    RUNTIME_ASSERT(ACL, !kwd.empty(), "Parameter kwd is empty.");

    DKeywordStore::const_iterator iter;
    PFITSKeyword returnPtr;
    bool bFound = false;

    for (iter = keywords_.begin(); (iter != keywords_.end()) && !bFound; ++iter)
    {
      if ( (*(*iter)) == kwd)
      {
        returnPtr = (*iter);
        bFound = true;
      };
    };

    return *returnPtr;
  }

  /// @brief Deletes the specified keyword.
  /// @param[in] kwd - The keyword name to delete.
  /// @returns true - The keyword was deleted
  /// @returns false - The keyword was not deleted.
  /// @note Calling with a keyword that does not exist will not result in an error.
  /// @throws GCL::CRuntimeAssert(ACL, "...")
  /// @version 2017-07-23/GGB - Function created.

  bool CHDB::keywordDelete(std::string const &kwd)
  {
    RUNTIME_ASSERT(ACL, kwd.size() != 0, "A keyword cannot have no characters");

    bool returnValue = false;

    auto iter = std::find_if(keywords_.begin(), keywords_.end(), [&kwd](PFITSKeyword keyword)->bool { return (*keyword == kwd);});

    if (iter != keywords_.end())
    {
      keywords_.erase(iter);
      returnValue = true;
    };

    return returnValue;
  }

  /// @brief Checks if the specified keyword exists in the HDB list of keywords.
  /// @param[in] kwd - The keyword to check for.
  /// @returns true - keyword exists
  /// @returns false - keyword is not in the list of keywords
  /// @throws GCL::CRuntimeAssert(ACL, "...")
  /// @version 2015-06-25/GGB - Changed logic flow for the return value.
  /// @version 2011-12-04/GGB - Function created.

  bool CHDB::keywordExists(std::string const &kwd) const
  {
    RUNTIME_ASSERT(ACL, kwd.size() != 0, "A keyword cannot have no characters");

    DKeywordStore::const_iterator iter;
    bool bFound = false;

    for (iter = keywords_.begin(); (iter != keywords_.end()) && !bFound; ++iter)
    {
      if ( (*(*iter)) == kwd)
      {
        bFound = true;
      };
    };

    return bFound;
  }

  /// @brief Moves the iterator to the first position and returns a valid keyword pointer.
  /// @returns Smart pointer to the first keyword.
  /// @throws None.
  /// @version 2011-12-10/GGB - Function created.

  PFITSKeyword CHDB::keywordIteratorFirst()
  {
    static PFITSKeyword nullPtr;

    keywordIterator_ = keywords_.begin();
    if ( (*keywordIterator_) )
    {
      return *keywordIterator_;
    }
    else
    {
      return nullPtr;
    };
  }

  /// @brief Moves the iterator to the last position and returns a valid keyword pointer.
  /// @returns Smart pointer to last keyword.
  /// @throws None.
  /// @version 2011-12-10/GGB - Function created.

  PFITSKeyword CHDB::keywordIteratorLast()
  {
    static PFITSKeyword nullPtr;

    keywordIterator_ = keywords_.end();

    if ( keywordIterator_ != keywords_.begin())
    {
      keywordIterator_--;
    };

    if ( *keywordIterator_ )
    {
      return *keywordIterator_;
    }
    else
    {
      return nullPtr;
    };
  }

  /// @brief Returns the next keyword from the collection of keywords.
  /// @returns Smart pointer to the next keyword.
  /// @throws None.
  /// @version 2011-12-10/GGB - Function created.

  PFITSKeyword CHDB::keywordIteratorNext()
  {
    static PFITSKeyword nullPtr;

    if (keywordIterator_ != keywords_.end() )
    {
      keywordIterator_++;
      if ( keywordIterator_ != keywords_.end() )
      {
        return *keywordIterator_;
      }
      else
      {
        return nullPtr;
      };
    }
    else
    {
      return nullPtr;
    };
  }

  /// @brief Returns the previous keyword.
  /// @returns Smart pointer to previous keyword.
  /// @throws None.
  /// @version 2011-12-10/GGB - Function created.

  PFITSKeyword CHDB::keywordIteratorPrev()
  {
    static PFITSKeyword nullPtr;

    if (keywordIterator_ == keywords_.begin() )
    {
      return nullPtr;
    }
    else
    {
      keywordIterator_--;
      return *keywordIterator_;
    };
  }

  /// @brief Returns a reference to the vector that stores the keywords.
  /// @throws None.
  /// @todo Either deprecate or make protected.
  /// @version 2011-12-10/GGB - Function created.

  DKeywordStore &CHDB::keywordStore()
  {
    return keywords_;
  }

  /// @brief Returns the type of a keyword.
  /// @param[in] kwd - The keyword to return the type of.
  /// @returns The type of the keyword.
  /// @throws None.
  /// @version 2011-12-04/GGB - Function created.

  KWType CHDB::keywordType(std::string const &kwd) const
  {
    RUNTIME_ASSERT(ACL, kwd.size() != 0, "A keyword cannot have no characters");

    DKeywordStore::const_iterator iter;
    bool bFound = false;

    iter = keywords_.begin();

    while ( (iter != keywords_.end()) && !bFound )
    {
      if ( (*(*iter)) == kwd)
      {
        bFound = true;
      }
      else
      {
        ++iter;
      };
    };

    if (!bFound)
    {
      return KWT_NONE;
    }
    else
    {
      return (*iter)->type();
    };
  }

  void CHDB::keywordWrite(std::string const &, std::int8_t const &, std::string const &)
  {

  }

  void CHDB::keywordWrite(std::string const &, std::int16_t const &, std::string const &)
  {

  }

  void CHDB::keywordWrite(std::string const &, std::int32_t const &, std::string const &)
  {

  }

  void CHDB::keywordWrite(std::string const &, std::int64_t const &, std::string const &)
  {

  }

  void CHDB::keywordWrite(std::string const &, std::uint8_t const &, std::string const &)
  {

  }

  void CHDB::keywordWrite(std::string const &, std::uint16_t const &, std::string const &)
  {

  }

  void CHDB::keywordWrite(std::string const &, std::uint32_t const &, std::string const &)
  {

  }

  void CHDB::keywordWrite(std::string const &, float const &, std::string const &)
  {

  }

  /// @brief Adds a double keyword to the HDB.
  /// @param[in] keyword - The keyword to write.
  /// @param[in] value - The value of the keyword.
  /// @param[in] comment - The comment for the keyword.
  /// @throws None.
  /// @version 2016-04-11/GGB - Function created.

  void CHDB::keywordWrite(std::string const &keyword, double const &value, std::string const &comment)
  {
    PFITSKeyword kwd(new CFITSKeywordDouble(keyword, value, comment));

    keywordWrite(kwd);
  }

  /// @brief Adds a string keyword to the HDB.
  /// @param[in] keyword - The keyword to write.
  /// @param[in] value - The value of the keyword.
  /// @param[in] comment - The comment for the keyword.
  /// @throws None.
  /// @version 2016-04-11/GGB - Function created.

  void CHDB::keywordWrite(std::string const &keyword, std::string const &value, std::string const &comment)
  {
    PFITSKeyword kwd(new CFITSKeywordString(keyword, value, comment));

    keywordWrite(kwd);
  }

  /// @brief Writes the passed information to the keyword array.
  /// @details If the keyword already exists in the keyword store, then the keyword is updated.
  /// If the keyword does not exist in the keyword store, then it is added to the keyword store.
  /// @param[in] kwd - pointer to the keyword to add or update.
  /// @throws None.
  /// @post hasData == true
  /// @post isDirty == true
  /// @version 2011-12-18/GGB - Function created.

  void CHDB::keywordWrite(PFITSKeyword const &kwd)
  {
    if ( keywordExists(kwd->keyword()) )
    {
      keywordDelete(kwd->keyword());
      keywords_.push_back(kwd);
    }
    else
    {
      keywords_.push_back(kwd);
    };

    parent_->hasData(true);
    parent_->isDirty(true);
  }

//  /// @brief Loads the comments from the FITS file.
//  /// @param[in] keyword - Keyword to check.
//  /// @returns true - Keyword was a comment keyword and been processed.
//  /// @returns false - Keyword is not a comment keyword.
//  /// @throws None.
//  /// @version 2015-08-10/GGB - Changed parameter to PKeyword. (cfitsio implementation)
//  /// @version 2013-04-07/GGB - Changed return value to void
//  /// @version 2013-03-13/GGB - Changed parameter to CCfits::HDU *
//  /// @version 2011-12-13/GGB - Function created.

//  bool CHDB::processComment(PFITSKeyword keyword)
//  {
//    bool returnValue = false;

//    if (keyword->keyword() == FITS_COMMENT)
//    {
//      comments_.push_back(keyword->value());
//    };

//    return returnValue;
//  }

//  /// @brief Loads the history from the HDU
//  /// @param[in] keyword - Keyword to check.
//  /// @returns true - Keyword was a history keyword and been processed.
//  /// @returns false - Keyword is not a history keyword.
//  /// @throws None.
//  /// @version 2015-08-10/GGB - Changed parameter to PKeyword. (cfitsio implementation)
//  /// @version 2013-06-29/GGB - Changed history_ to a std::vector<>.
//  /// @version 2013-04-07/GGB - Changed return type to void.
//  /// @version 2013-03-13/GGB - Name changed to readHistory and parameter changed to pointer.
//  /// @version 2011-12-13/GGB - Function created.

//  bool CHDB::processHistory(PFITSKeyword keyword)
//  {
//    bool returnValue = false;

//    if (keyword->keyword() == FITS_HISTORY)
//    {
//      //history_.push_back(keyword->value());
//    };

//    return returnValue;
//  }

  /// @brief Loads all the common FITS HDU information.
  /// @param[in] file - The FITS file to open from.
  /// @pre The file must be pointed to the HDB to load.
  /// @throws None.
  /// @details This includes some keywords, as well as the keyword information. This function never loads data of any sort.
  /// @version 2013-03-13/GGB - Changed name to readFromFITS() and changed to pointer to HDU.
  /// @version 2012-12-30/GGB - Removed the check on the number of axes.
  /// @version 2011-12-13/GGB - Function created.

  void CHDB::readFromFITS(fitsfile *file)
  {
    NAXIS_t index;
    int naxis;
    LONGLONG naxisn[NAXIS_MAX];

    CFITSIO_TEST(fits_get_img_dim, file, &naxis);
    naxis_ = static_cast<NAXIS_t>(naxis);           // NAXIS_t is an unsigned int, while naxis is an int.

      // Capture the size of each axis.

    naxisn_.clear();
    CFITSIO_TEST(fits_get_img_sizell, file, NAXIS_MAX, naxisn);

    for (index = 1; index <= naxis_; index++)
    {
      naxisn_.push_back(naxisn[index-1]);
    };

      // Now read the keywords into memory

    readKeywords(file);
  }

  /// @brief Loads the keywords into the HDB.
  /// @param[in] file - The FITS file to load from.
  /// @throws CODE_ERROR
  /// @throws 0x000C
  /// @throws std::bad_alloc
  /// @throws boost::bad_lexical_cast
  /// @version 2015-09-12/GGB - Updated to cfitsio.
  /// @version 2013-04-07/GGB - Changed return type to void
  /// @version 2013-03-13/GGB - name changed to readKeywords and parameter changed to pointer.
  /// @version 2011-12-14/GGB - Funciton created.

  void CHDB::readKeywords(fitsfile *file)
  {
    PFITSKeyword keyword;
    char szKeyword[FLEN_KEYWORD];
    char szValue[FLEN_VALUE];
    char szComment[FLEN_COMMENT];
    int keywordCount, nIndex;
    char dType;
    int neg;
    int dataType;

    CFITSIO_TEST(fits_get_hdrspace, file, &keywordCount, nullptr);   // Get the number of keywords in the header.

      // Iterate through each keyword to process

    for (nIndex = 1; nIndex <= keywordCount; ++nIndex)
    {
      CFITSIO_TEST(fits_read_keyn, file, nIndex, szKeyword, szValue, szComment);

      if (szValue[0] != '\0')
      {
        CFITSIO_TEST(fits_get_keytype, szValue, &dType);

        switch(dType)
        {
          case 'C':
          {
            keyword.reset(new CFITSKeywordString(szKeyword, std::string(szValue), szComment));
            if ( !specialKeyword(keyword) )
            {
              keywords_.push_back(keyword);
            };
            break;
          };
          case 'L':
          {
            keyword.reset(new CFITSKeywordBool(szKeyword, (std::string(szValue) == "T"), szComment));

            if ( !specialKeyword(keyword) )
            {
              keywords_.push_back(keyword);
            };
            break;
          };
          case 'I':
          {
            neg = 0;    // cfitsio does not reset this value.
            CFITSIO_TEST(fits_get_inttype, szValue, &dataType, &neg);
            switch(dataType)
            {
              case TSBYTE:
              case TSHORT:
              {
                std::int16_t value;
                value = boost::lexical_cast<std::int16_t>(szValue);
                keyword.reset(new CFITSKeywordInt16(szKeyword, value, szComment));
                if (!specialKeyword(keyword))
                {
                  keywords_.push_back(keyword);
                }
                break;
              };
              case TBYTE:
              case TUSHORT:
              {
                std::uint16_t value;
                value = boost::lexical_cast<std::uint16_t>(szValue);
                keyword.reset(new CFITSKeywordUInt16(szKeyword, value, szComment));
                if (!specialKeyword(keyword))
                {
                  keywords_.push_back(keyword);
                }
                break;
              };
              case TINT:
              {
                std::int32_t value;
                value = boost::lexical_cast<std::int32_t>(szValue);
                keyword.reset(new CFITSKeywordInt32(szKeyword, value, szComment));
                if (!specialKeyword(keyword))
                {
                  keywords_.push_back(keyword);
                }
                break;
              };
              case TUINT:
              {
                std::uint32_t value;
                value = boost::lexical_cast<std::uint32_t>(szValue);
                keyword.reset(new CFITSKeywordUInt32(szKeyword, value, szComment));
                if (!specialKeyword(keyword))
                {
                  keywords_.push_back(keyword);
                }
                break;
              };
              case TLONGLONG:
              {
                std::int64_t value;
                value = boost::lexical_cast<std::int64_t>(szValue);
                keyword.reset(new CFITSKeywordInt64(szKeyword, value, szComment));
                if (!specialKeyword(keyword))
                {
                  keywords_.push_back(keyword);
                }
                break;
              };
              default:
              {
                CODE_ERROR(ACL);
                break;
              };
            };
            break;
          }
          case 'F':
          {
            double value;
            value = boost::lexical_cast<double>(szValue);
            keyword.reset(new CFITSKeywordDouble(szKeyword, value, szComment));
            if (!specialKeyword(keyword))
            {
              keywords_.push_back(keyword);
            }
            break;
          };
          case 'X':
          {
            RUNTIME_ASSERT(ACL, false, "Complex numbers not implemented.");
            break;
          }
          default:
          {
            CODE_ERROR(ACL);
            break;
          };
        }
      }
      else
      {
          // Likely a comment or history value.

        std::string test(szKeyword);

        if (test == FITS_HISTORY)
        {
          history_.emplace_back(szComment);
        }
        else if (test == FITS_COMMENT)
        {
          comments_.emplace_back(szComment);
        }
        else if (test.empty())    // Blank keyword == comments for file.
        {
          comments_.emplace_back(szComment);
        }
        else
        {
          CODE_ERROR(ACL);
        }
      }
    };

    keyword.reset();
  }

  /// @brief Sets the new value of NAXIS. Error checking is performed.
  /// @param[in] n - The new NAXIS value. (1-999)
  /// @throws 0x1902 - HDB: 0 <= NAXIS <= 999.
  /// @version 2011-12-18/GGB - Function created.

  void CHDB::NAXIS(NAXIS_t n)
  {
    if ( (n <= 0) || (n > 999) )
    {
      ACL_ERROR(0x1902);    // HDB: 0 <= NAXIS <= 999.
    }
    else
    {
      naxis_ = n;
      naxisn_.resize(n);    // Resize the array to the correct number of axes.
    };
  }

  /// @brief Returns the NAXISn value specified.
  /// @returns The NAXISn number specified.
  /// @throws 0x2004  - NAXIS value not found or does not exist.
  /// @throws 0x2008  - Invalid NAXISn. n < 1 || n > 999
  /// @version 2011-11-27/GGB - Function created.

  AXIS_t CHDB::NAXISn(NAXIS_t n) const
  {
    if ( (n < 1) || (n > 999) )
    {
      ACL_ERROR(0x2008);    // Invalid NAXISn. n < 1 || n > 999
    }
    else if (n > naxisn_.size())
    {
      ACL_ERROR(0x2004);    // NAXIS value not found or does not exist.
    }
    else
    {
      return naxisn_[n-1];
    };
  }

  /// @brief Sets the specified value of NAXISn
  /// @throws 0x2004  - NAXIS value not found or does not exist.
  /// @throws 0x2008  - Invalid NAXISn. n < 1 || n > 999
  /// @version 2011-12-18/GGB - Function created.

  void CHDB::NAXISn(std::vector<AXIS_t>::size_type nax, AXIS_t n)
  {
    if ( (nax < 1) || (nax > 999) )
    {
      ACL_ERROR(0x2008);  // HDB: Cannot set value of NAXISn, NAXIS not correctly defined.
    }
    else if  ((nax > naxisn_.size()) )
    {
      ACL_ERROR(0x2004);    // NAXIS value not found or does not exist.
    }
    else
    {
      naxisn_[nax-1] =  n;
    };
  }

  /// @brief Sets the PCOUNT value
  //
  // 2012-01-12/GGB - Function created.

  void CHDB::PCOUNT(int pcount)
  {
    pcount_ = pcount;
  }

  /// @brief Returns the value of the SIMPLE keyword.
  /// @returns true - Simple HDB
  /// @returns false - Not a simple HDB
  /// @throws 0x190A - HDB: SIMPLE only allowed in PRIMARY header.
  //
  // 2012-01-11/GGB - Function created.

  bool CHDB::SIMPLE() const
  {
    if (!PRIMARY())
    {
      ACL_ERROR(0x190A);    // HDB: SIMPLE only allowed in PRIMARY header.
    }
    else
    {
      return bSimple;
    };
  }

  /// @brief Checks if the passed keyword has any special meaning.
  /// @details If there is a special meaning for the keyword, the appropriate routine is called to set the special data value.
  /// Special data values are stored directly in the HDB and not as keywords.
  /// The keyword value is explicitely discarded from the list of keywords when the data is loaded.
  /// @param[in] keyword
  /// @returns true - keyword was special and was handled.
  /// @returns false - keyword was not special and was not handled.
  /// @throws None.
  /// @version 2017-09-28/GGB - Removed checks for comments and history. These are handled when loaded in the keyword function.
  /// @version 2015-08-10/GGB - Added checks for SIMPLE and XTENSION, as well as history and comments. (Update to cfitsio)
  /// @version 2011-09-28/GGB - Function created.

  bool CHDB::specialKeyword(PFITSKeyword keyword)
  {
    bool returnValue = false;

    if (keyword->keyword() == FITS_SIMPLE)
    {
      bSimple = static_cast<bool>(*keyword);
      bPrimary_ = true;
      extname_ = astroManager_HDB_PRIMARY;
      keyword.reset();
      returnValue = true;
    }
    else if (keyword->keyword() == FITS_XTENSION)
    {
      bPrimary_ = false;
      keyword.reset();
      returnValue = true;
    }
//    else if (processHistory(keyword))
//    {
//      keyword.reset();
//      returnValue = true;
//    }
//    else if (processComment(keyword))
//    {
//      keyword.reset();
//      returnValue = true;
//    }

    return returnValue;
  }

  /// @brief Writes the comments to a FITS file.
  /// @param[in] file - The FITS file to write to.
  /// @throws None.
  /// @version 2015-09-07/GGB - Function created.

  void CHDB::writeCommentsToFITS(fitsfile *file) const
  {
    std::for_each(comments_.begin(), comments_.end(),
                  [&] (std::string const &s) {CFITSIO_TEST(fits_write_comment, file, s.c_str());});
  }

  /// @brief Writes the history to a FITS file.
  /// @param[in] file - The FITS file to write to.
  /// @throws None.
  /// @version 2015-09-07/GGB - Function created.

  void CHDB::writeHistoryToFITS(fitsfile *file) const
  {
    std::for_each(history_.begin(), history_.end(),
                  [&] (std::string const &s) {CFITSIO_TEST(fits_write_history, file, s.c_str());});
  }

  /// @brief Writes the keywords to the passed HDU.
  /// @param[in] file - The FITS file to write to.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @details All the keywords are iterated, and the keywords already handled in the CCfits code are stripped out and not passed
  /// as keywords.
  /// @version 2015-08-11/GGB - Converted to cfitsio.
  /// @version 2013-03-14/GGB - Added function writeToFITS() to TKeyword class.
  /// @version 2012-01-15/GGB - Function created.

  void CHDB::writeKeywordsToFITS(fitsfile *file) const
  {
    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr");

    DKeywordStore::const_iterator iter;

    for (iter = keywords_.begin(); iter != keywords_.end(); iter++)
    {
      if ( ((*(*iter)) != FITS_SIMPLE) &&
           ((*(*iter)) != FITS_NAXIS) &&
           ((*(*iter)) != FITS_XTENSION) &&
           ((*(*iter)) != FITS_BITPIX) &&
           ((*(*iter)) != FITS_GCOUNT) &&
           ((*(*iter)) != FITS_PCOUNT) &&
           ((*(*iter)) != FITS_BZERO) &&
           ((*(*iter)) != FITS_BSCALE) &&
           ( (*iter)->keyword().find(FITS_NAXIS) == std::string::npos) )
      {
          // Not a special keyword. Must be sent to the fitsfile for storage.

        (*iter)->writeToFITS(file);
      };
    };
  }

  /// @brief Writes the HDB to the passed FITS data structure.
  /// @param[in] file - Pointer to the FITS file to write to.
  /// @throws 0x1906 - HDB: Invalid HDU passed to function
  /// @details If this is the primary extension, then the data is written to the primary extension, otherwise a new extension is
  /// created.
  /// @version 2015-08-11/GGB - Converted to cfitsio.
  /// @version 2012-01-14/GGB - Function created.

  void CHDB::writeToFITS(fitsfile *file)
  {
    RUNTIME_ASSERT(ACL, file != nullptr, "Parameter file cannot be nullptr");

    writeKeywordsToFITS(file);
    writeCommentsToFITS(file);
    writeHistoryToFITS(file);
  }

}  // namespace ACL
