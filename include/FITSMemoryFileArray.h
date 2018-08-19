//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSMemoryFile
// SUBSYSTEM:						Class for in-memory FITS files.
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2017-2018 Gavin Blakeman.
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
// OVERVIEW:						A class for in-memory management of FITS files.
//
// CLASSES INCLUDED:		CFITSMemoryFile
//
// CLASS HIERARCHY:     CFITSMemoryFile
//
// HISTORY:             2017-08-06 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_FITSMEMORYFILEARRAY
#define ACL_FITSMEMORYFILEARRAY

  // Standard C++ library

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <list>
#include <memory>
#include <utility>

  // ACL Header files.

#include "FITSMemoryFile.h"

namespace ACL
{
  class CFITSMemoryFileArray : public CFITSMemoryFile
  {

  private:
    bool memoryOwned_;                                ///< True if the class owns the memory. False otherwise.
    std::size_t memoryArraySize_;                     ///< The actual size of the memory array.
    void *memoryArray_;                               ///< Pointer to the memory buffer allocated with malloc.
    std::size_t nextResize_;

    CFITSMemoryFileArray() = delete;
    CFITSMemoryFileArray(CFITSMemoryFileArray const &) = delete;

  protected:
    virtual void memory_allocate(std::size_t);
    virtual void memory_free();
    virtual void memory_reallocate(std::size_t);


  public:
    CFITSMemoryFileArray(boost::filesystem::path const &);
    CFITSMemoryFileArray(std::size_t);
    CFITSMemoryFileArray(std::uint8_t *, std::size_t);

    virtual ~CFITSMemoryFileArray();
  };

}   // namespace ACL


#endif // ACL_FITSMEMORYFILEARRAY

