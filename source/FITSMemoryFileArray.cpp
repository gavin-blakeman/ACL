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

#include "../include/FITSMemoryFileArray.h"

  // Standard C++ libraries.

#include <algorithm>
#include <new>
#include <fstream>

  // ACL Library header files.

#include "../include/common.h"
#include "../include/error.h"

  // Miscellaneous libraries

#include <GCL>

namespace ACL
{
  /// @brief Constructs the memory file. This variant opens the file (if valid) creates a memory structure of the relevant size
  ///        and loads the file into the memory structure.
  /// @param[in] filePath - path to the file that will initialise the instance.
  /// @throws std::bad_alloc
  /// @version 2017-08-06/GGB - Function created.

  CFITSMemoryFileArray::CFITSMemoryFileArray(boost::filesystem::path const &filePath) : CFITSMemoryFile(), memoryOwned_(true),
    memoryArraySize_(0), memoryArray_(nullptr), nextResize_(FITS_BLOCK)
  {
    readFromFile(filePath);
  }

  /// @brief Constructs a memory file and allocates initial storage.
  /// @param[in] memorySize - Number of bytes to allocate.
  /// @throws std::bad_alloc
  /// @version 2017-08-06/GGB - Function created.

  CFITSMemoryFileArray::CFITSMemoryFileArray(std::size_t memorySize) : CFITSMemoryFile(), memoryOwned_(true), memoryArraySize_(0),
    memoryArray_(nullptr), nextResize_(FITS_BLOCK)
  {
    memory_allocate(memorySize);
  }

  /// @brief Constructor from existing array. This allows read-only access to the array.
  /// @param[in] ptr - Pointer to the memory array.
  /// @param[in] size - The size of the memory array. (Number of bytes)
  /// @throws std::bad_alloc
  /// @version 2017-08-12/GGB - Function created.

  CFITSMemoryFileArray::CFITSMemoryFileArray(std::uint8_t *ptr, std::size_t size) : CFITSMemoryFile(ptr, size), memoryOwned_(false),
    memoryArraySize_(size), memoryArray_(ptr), nextResize_(FITS_BLOCK)
  {
  }

  /// @brief Frees the memory array.
  /// @throws None.
  /// @version 2017-08-06/GGB - Function created.

  CFITSMemoryFileArray::~CFITSMemoryFileArray()
  {
    memory_free();
  }

  /// @brief Allocates memory for the memory array.
  /// @details If the memoryArray is already existing, then the array will only be allocated if a greater size is required.
  /// @param[in] memorySize - The size of the memoryRequested.
  /// @throws std::bad_alloc
  /// @version 2017-08-06/GGB - Function created.

  void CFITSMemoryFileArray::memory_allocate(std::size_t newMemorySize)
  {
    if (memoryOwned_)
    {
      if (memoryArray_)
      {
        if (memoryArraySize_ <= newMemorySize)
        {
          std::free(memoryArray_);
          memoryArray_ = std::malloc(newMemorySize);
          if (memoryArray_)
          {
            memoryArraySize_ = newMemorySize;
          }
          else
          {
            memoryArraySize_ = 0;
            throw std::bad_alloc();
          };
        }
      }
      else
      {
        memoryArray_ = std::malloc(newMemorySize);
        if (memoryArray_)
        {
          memoryArraySize_ = newMemorySize;
        }
        else
        {
          memoryArraySize_ = 0;
          throw std::bad_alloc();
        };
      };
      memoryPointer(memoryArray_);
      memorySize(memoryArraySize_);
      nextResize_ = FITS_BLOCK;
    }
    else
    {
      ACL_CODE_ERROR;
    }
  }

  /// @brief Frees the memory allocated to the memoryArray_
  /// @throws None.
  /// @version 2017-08-06/GGB - Function created.

  void CFITSMemoryFileArray::memory_free()
  {
    if (memoryOwned_)
    {
      if (memoryArray_)
      {
        free(memoryArray_);
        memoryArray_ = nullptr;
      };
      memorySize(0);
      memoryPointer(nullptr);
    }
    else
    {
      ACL_CODE_ERROR;
    }
  }

  /// @brief Reallocates a block of memory.
  /// @details - If the block size is smaller, no reallocation occurs.
  /// @param[in] memorySize - The new size to allocate.
  /// @throws std::bad_alloc
  /// @version 2017-08-06/GGB - Function created.

  void CFITSMemoryFileArray::memory_reallocate(std::size_t memorySize)
  {
    if (memoryOwned_)
    {
      if (memoryArray_)
      {
        if (memoryArraySize_ <= memorySize)
        {
          // Need to expand the memory size.

          memorySize = std::max(memorySize, memoryArraySize_ + nextResize_);
          nextResize_ *= 2;

          void *newArray = std::realloc(memoryArray_, memorySize);

          if (newArray)
          {
            // Reallocated sucessfully

            memoryArray_ = newArray;
            memoryArraySize_ = memorySize;
          }
          else
          {
            // Reallocation failed. The class is still in a defined state.

            throw std::bad_alloc();
          }
        }
        else
        { /* Do nothing */ };
      }
      else
      {
        memory_allocate(memorySize);      // May also throw.
      }
    }
    else
    {
      ACL_CODE_ERROR;
    }
  }

} // namespace ACL
