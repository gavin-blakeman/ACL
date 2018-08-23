//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								error
// SUBSYSTEM:						Error definitions
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2018 Gavin Blakeman.
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
// OVERVIEW:						Common definitions that can be used by multiple files
//
//
// CLASSES INCLUDED:		None
//
// CLASS HIERARCHY:     None.
//
//
// HISTORY:             2018-08-19 GGB - File created.
//
//*********************************************************************************************************************************

#ifndef ACL_ERROR_H
#define ACL_ERROR_H

  // Miscellaneous libraries

#include <GCL>

namespace ACL
{
#define ACL_ERROR(ERR) (ERROR(ACL, ERR))
#define ACL_CODE_ERROR CODE_ERROR(ACL)
}

#endif // ACL_ERROR_H

