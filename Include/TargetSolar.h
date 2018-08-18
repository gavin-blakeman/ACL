//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								SolarObjects
// SUBSYSTEM:						Solar Object Positions and Calculations
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2011-2016 Gavin Blakeman.
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
// OVERVIEW:						These classes provide methods of handling solar observing objects (Objects that under the gravitational
//                      influence of the SUN.)
//
//                      Non-Solar (IE Stellar) objects support is provided by the StellarObject file. See the StellarObjects
//                      file for more information.
//
// CLASSES INCLUDED:    CSolarObject  - Class providing support for Solar objects.
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetSolar
//                          CTargetMinorPlanet
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2011-06-30 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_SOLAROBJECTS_H
#define ACL_SOLAROBJECTS_H

#include "AstroClass.h"
#include "TargetAstronomy.h"

#include <memory>

namespace ACL
{

  class CTargetSolar : public CTargetAstronomy
  {
  private:
  protected:
  public:
    CTargetSolar(std::string const &);
  };
  typedef std::shared_ptr<CTargetSolar> PTargetSolar;
}

#endif // ACL_SOLAROBJECTS_H
