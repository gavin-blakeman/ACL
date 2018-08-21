//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroCatalogue.h
// SUBSYSTEM:						Astronomical Catalogue classes
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	SCL, cfitsio, boost.
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2012-2018 Gavin Blakeman.
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
// OVERVIEW:
//
// CLASSES INCLUDED:
//
// CLASS HIERARCHY:
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2012-08-01 GGB - Start development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_ASTROCATALOGUE_H
#define ACL_ASTROCATALOGUE_H

  // ACL include files

#include "TargetAstronomy.h"
#include "TargetStellar.h"

  // Std include files

#include <list>
#include <memory>
#include <string>
#include <vector>


namespace ACL
{
  typedef std::list<std::shared_ptr<CTargetStellar>> TStellarObjectContainer;

  class CAstroCatalogue
  {
  private:
    TStellarObjectContainer stellarObjectContainer;
    TStellarObjectContainer::iterator containerIterator;

  protected:
  public:
    CAstroCatalogue();

    virtual void addStellarObject(std::shared_ptr<CTargetStellar>);
    virtual void empty();

    virtual std::shared_ptr<CTargetStellar> moveFirst();      /// @todo Should these be references?
    virtual std::shared_ptr<CTargetStellar> moveLast();
    virtual std::shared_ptr<CTargetStellar> moveNext();
    virtual std::shared_ptr<CTargetStellar> movePrevious();
  };

  struct SAstroCatalogue
  {
    std::string identifier;
    FP_t ra;
    FP_t dec;
    int off;
    FP_t CCDx;
    FP_t CCDy;
  };

  typedef std::shared_ptr<SAstroCatalogue> SAstroCatalogue_Ptr;
  typedef std::vector<SAstroCatalogue_Ptr> SAstroCatalogueContainer;
}

#endif // ACL_ASTROCATALOGUE_H
