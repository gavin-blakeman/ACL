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
// HISTORY:             2017-08-13 GGB - Started development of classes.
//
//*********************************************************************************************************************************

#ifndef ACL_FITSMEMORYFILE
#define ACL_FITSMEMORYFILE

  // Standard C++ library

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <list>
#include <memory>
#include <utility>

  // Miscellaneous libraries

#include "boost/filesystem.hpp"

namespace ACL
{
  std::size_t const FITS_BLOCK = 2880;

  class CFITSMemoryFile
  {

  private:
    typedef std::pair<void *, CFITSMemoryFile *> instanceLink_t;
    typedef std::list<instanceLink_t> instanceLinkList_t;
    static instanceLinkList_t instanceLinks;
    void *memoryPointer_;                             ///< The memory pointer that is managed by cfitsio
    std::size_t memorySize_;                          ///< The size of the memory that is managed by cfitsio

    CFITSMemoryFile(CFITSMemoryFile const &) = delete;

  protected:
    virtual void memory_allocate(std::size_t) = 0;
    virtual void memory_reallocate(std::size_t) = 0;
    virtual void memory_free() = 0;

    void memorySize(std::size_t ns) { memorySize_ = ns; }
    void memoryPointer(void *np) { memoryPointer_ = np; }

  public:
    explicit CFITSMemoryFile();
    CFITSMemoryFile(void *mp, std::size_t ms);

    virtual ~CFITSMemoryFile();

    static void *reallocate(void *, std::size_t);

    virtual void readFromFile(boost::filesystem::path const &);
    virtual void writeToFile(boost::filesystem::path const &);

    std::size_t *memorySize() noexcept { return &memorySize_; }
    void **memoryPointer() noexcept { return &memoryPointer_; }
  };

}   // namespace ACL


#endif // ACL_FITSMEMORYFILE

