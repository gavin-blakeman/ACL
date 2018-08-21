//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Config
// SUBSYSTEM:						Configuration file
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2013-2018 Gavin Blakeman.
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
// OVERVIEW:						Configuration values and typedefs for the library.
//
//
// CLASSES INCLUDED:		None
//
// CLASS HIERARCHY:     None.
//
//
// HISTORY:             2018-05-12/GGB - Changed AXIS_t to be 32bit by default, INDEX_t also changed to 32bit value.
//                      2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-05-08 GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_CONFIG_H
#define ACL_CONFIG_H

  // Standard C++ library header files.

#include <cstdint>
#include <cstddef>
#include <vector>

/// @file
/// The config.h file is used for storing configuration parameters and macros for the Astronomy Class Library. The parameters
/// should be used throughout the library. Changing the typedefs in this file and recompiling will allow the library to use
/// different sizes for various types.
/// @note There are two macros are used to determine the library that will be linked for the determination of stellar positions.
/// These are USE_NOVAS and USE_SOFA.<br>
/// One of the macros USE_NOVAS or USE_SOFA must be defined.

  // The section below implements switches for different compilers or applications. This should be extended as required to
  // capture other compilers.

#ifdef EMBEDDED
  #ifdef __XC32
    #define ACL_NOBOOST
    #define ACL_NOMT
    #define ACL_REALTIME
    #define ACL_NOSTRINGS
    #define ACL_CONTROL
  #endif  // __XC32
#else   // EMBEDDED
  #undef ACL_NOBOOST
  #undef ACL_NOMT
  #undef ACL_REALTIME
  #undef ACL_NOSTRINGS
  #undef ACL_CONTROL
#endif  // EMBEDDED

  // Boost library

#ifndef ACL_NOBOOST
#include <boost/shared_array.hpp>
#endif

namespace ACL
{

  /// @def USE_NOVAS
  /// If this macro is defined, then the code for the determination of stellar positions will use the NOVAS library.
  /// Only one of the macros USE_NOVAS and USE_SOFA must be defined.

  /// @def USE_SOFA
  /// If this macro is defined then the code for the determination of stellar positions will use the SOFA library.
  /// Only one of the macros USE_NOVAS and USE_SOFA must be defined.

#define USE_NOVAS
#define USE_SOFA
#undef USE_SOFA

#if defined(USE_NOVAS) && defined(USE_SOFA)
#error "Cannot have USE_SOFA and USE_NOVAS defined at the same time."
#elif !defined(USE_NOVAS) && !defined(USE_SOFA)
#error "One USE_NOVAS or USE_SOFA must be defined."
#endif


  typedef std::uint8_t base_t;                ///< Base type used for imagePlane storage.
  typedef std::int32_t AXIS_t;                ///< The type used for image axis dimensions. Must be a signed integer value.
  typedef std::uint64_t INDEX_t;              ///< The type used for indexes of arrays. (Can be bigger than AXIS_t)
                                              ///< This must be an unsigned integer type. (If you use a signed integer type,
                                              ///< a large number of comparisons and loops will have undetermined behaviour.
                                              ///< This type should be large enough to store AXIS_2 ^ 2.
  typedef double FP_t;                        ///< The type used for floating point calculations. This must be a floating point type.


  typedef std::uint16_t NAXIS_t;              ///< The type used for NAXIS values. (1->999)
  typedef std::uint32_t JD_t;                 ///< Type used for integer Julian dates

  typedef std::uint8_t renderImage_t;         ///< The type used to render images.
  typedef std::uint8_t renderImageGrey8_t;    ///< The type used for rendering Grey8 images.
  typedef std::uint8_t colourValue_t;         ///< The type used for a colour value in colour images.

#ifndef ACL_NOMT
  extern size_t maxThreads;                   ///< The maximun number of threads to use in multi-threaded functions.
                                              ///< The library will choose a number of threads to utilise up to a maximum of this
                                              ///< value. This is a run-time value that can be changed by the controlling
                                              /// application. The default value is 1.

#endif // ACL_NOMT

  struct SRGBHP
  {
    FP_t A;
    FP_t R;
    FP_t G;
    FP_t B;
  };

#ifndef ACL_NOBOOST
  typedef boost::shared_array<SRGBHP> SRGBHP_Ptr;
#endif

#if _cplusplus > 201402L

  static_assert(!std::numeric_limits<INDEX_t>::is_signed, "INDEX_t must be an unsigned value.");
  static_assert(std::numeric_limits<INDEX_t>::is_integer, "INDEX_t must be an integer value.");

  static_assert(std::numeric_limits<AXIS_t>::is_signed, "AXIS_t must be a signed value.");
  static_assert(std::numeric_limits<AXIS_t>::is_integer, "AXIS_t must be an integer value");

  static_assert(std::numeric_limits<INDEX_t>::max() >= std::numeric_limits<AXIS_t>::max(), "Condition not met: INDEX_t::max >= AXIS_t::max");

  static_assert(!std::numeric_limits<NAXIS_t>::is_signed, "NAXIS_t must be as unsigned value.");
  static_assert(std::numeric_limits<NAXIS_t>::is_integer, "NAXIS_t must be an integer value.");
  static_assert(std::numeric_limits<NAXIS_t>::max() >= 999, "NAXIS_t must have a maximum value >= 999");

  static_assert(!std::numeric_limits<FP_t>::is_integer, "FP_t must be a floating point type.");

#endif

}    // namespace ACL

#endif // ACL_CONFIG_H
