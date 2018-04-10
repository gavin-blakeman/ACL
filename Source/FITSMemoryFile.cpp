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
//                      Copyright 2017 Gavin Blakeman.
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

#include "../Include/FITSMemoryFile.h"

#include "../Include/common.h"

  // Standard C++ library

#include <algorithm>

  // Miscellaneous libraries

#include <GCL>

namespace ACL
{
  std::list<std::pair<void *, CFITSMemoryFile *>> CFITSMemoryFile::instanceLinks;

  /// @brief Constructor for the class.
  /// @throws std::bad_alloc
  /// @version 2017-08-13/GGB - Function created.

  CFITSMemoryFile::CFITSMemoryFile() : memoryPointer_(nullptr), memorySize_(0)
  {
    instanceLinks.emplace_back(nullptr, this);
  }

  /// @brief Constructor for the class.
  /// @param[in] mp - The new memory pointer.
  /// @param[in] ms - The new memory size.
  /// @throws std::bad_alloc
  /// @version 2017-08-13/GGB - Function created.

  CFITSMemoryFile::CFITSMemoryFile(void *mp, std::size_t ms) : memoryPointer_(mp), memorySize_(ms)
  {
    instanceLinks.emplace_back(mp, this);
  }

  /// @brief Destuctor for the class. Ensures the instance is removed from the instance list.
  /// @throws None.
  /// @version 2017-08-13/GGB - Function created.

  CFITSMemoryFile::~CFITSMemoryFile()
  {
    instanceLinks.remove_if([this](std::pair<void *, CFITSMemoryFile *> &elem) { return elem.second == this; });
  }

  /// @brief Reads from a file into a newly sized memory buffer.
  /// @param[in] filePath - path to the file
  /// @note Any data in the class is released and lost.
  /// @throws GCL::CError(ACL, 0x0800)
  /// @version 2017-08-06/GGB - Function created.

  void CFITSMemoryFile::readFromFile(boost::filesystem::path const &filePath)
  {
    std::ifstream ifs;
    std::size_t fileSize = boost::filesystem::file_size(filePath);

    ifs.open(filePath.string(), std::ios_base::in | std::ios_base::binary);
    if (ifs.good())
    {
        // Allocate the memory to filesize.

      memory_allocate(fileSize);

        // Load the file into the memory.

      ifs.read(static_cast<char *>(memoryPointer_), fileSize);
      ifs.close();
    }
    else
    {
      ERRORMESSAGE("Unable to open file: " + filePath.string());
      ACL_ERROR(0x0800);
    };
  }

  /// @brief Reallocates a block of memory.
  /// @param[in] pointer - ignored
  /// @param[in] sizeOfFile - The new size to allocate.
  /// @returns Pointer to the newly allocated buffer.
  /// @note Some special stuff is done here. As the vector maintains all the memory itself, the return values are just
  ///       based on the vectory being resized.
  /// @throws std::bad_alloc
  /// @version 2017-08-13/GGB - Function created.

  void *CFITSMemoryFile::reallocate(void *ptr, std::size_t memorySize)
  {
    CFITSMemoryFile *instance = nullptr;

    auto iter = std::find_if(instanceLinks.begin(), instanceLinks.end(),
                             [&ptr](instanceLink_t &element)->bool {return element.first == ptr; });

    if (iter != instanceLinks.end())
    {
      instance = (*iter).second;

      if (instance)
      {
        instance->memory_reallocate(memorySize);
        (*iter).first = instance->memoryPointer_;
        return instance->memoryPointer_;
      }
      else
      {
        ACL_CODE_ERROR;
        return nullptr;   // Not necessary, but prevents the compiler from squealing.
      }
    }
    else
    {
      ACL_CODE_ERROR;
      return nullptr;   // Not necessary, but prevents the compiler from squealing.
    }
  }

  /// @brief Writes the contents of the memory to the specified file.
  /// @param[in] filePath - The filename and path to write to.
  /// @note The vector size is not used. The size in memorySize_ is managed by cfitsio and is used to write the file.
  /// @throws GCL::CError(ACL, 0x0800)
  /// @version 2017-08-06/GGB - Function created.

  void CFITSMemoryFile::writeToFile(boost::filesystem::path const &filePath)
  {
    std::ofstream ofs;

    ofs.open(filePath.string(), std::ios_base::out | std::ios_base::binary);
    if (ofs.good())
    {
      ofs.write(static_cast<char *>(memoryPointer_), memorySize_);
    }
    else
    {
      ERRORMESSAGE("Unable to open file: " + filePath.string());
      ACL_ERROR(0x0800);
    };
  }

} // namespace ACL
