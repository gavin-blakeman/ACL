//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								StellarObjects.h
// SUBSYSTEM:						Stellar Object Positions and Calculations
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:	SOFA, NOVAS
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2005-2017 Gavin Blakeman.
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
// OVERVIEW:						These classes provide methods of handling stellar and solar observing objects.
//                      The classes provide support for the FK4, FK5 and ICRS reference frames. and provide methods for converting
//                      from ICRS, FK4 and FK5 to observed positions.
//
//                      Two options are provided for determining the position of objects, the one is using the IAU SOFA library,
//                      the second using the USNO NOVAS library. The choice of library is controlled by the macros:
//                        USE_SOFA and
//                        USE_NOVAS, if no macro is supplied, the default of USE_SOFA is used.
//
//                      SOFA
//                      The routines used are as supplied by the IAU, Div 1, Commission 19, Standards of Fundemental Astronomy.
//                      These routines are provided in the SOFA library available from the above source.
//                      The classes provided in this file encapsulate and hide the use of the SOFA library from the user, to make
//                      the routies easy to use and understand, without having to understand the SOFA library itself.
//
//                      The classes supplied in this file requires the SOFA library to be linked in to operate.
//
//                      For more information, the user is directed to the following IAU publications:
//                        1) The SOFA library manual
//                        2) SOFA Time Scale and Calendar Tools
//                        3) SOFA Tools for Earth Attitude
//
//                      The above mentioned publications were used in the preparation of these classes.
//
//                      Non-Stellar (IE Solar System) objects support is provided by the SolarObject file. See the SolarObjects
//                      file for more information.
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. Functions from other libraries, referenced by this library can be written in C.
//
// CLASSES INCLUDED:    CTargetStellar  - Class providing support for stellar objects.
//
// CLASS HIERARCHY:     CTargetStellar
//
// HISTORY:             2015-09-22 GGB - AIRDAS 2015.09 release
//                      2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2011-06-30 GGB - Development of classes for AIRDAS
//
//*********************************************************************************************************************************

#include "../Include/TargetStellar.h"

#include "../Include/constants.h"
#include "../Include/AstroFunctions.h"
#include "../Include/FITSStrings.h"

  // Standard C++ library

#include <sstream>

  // NOVAS library

#ifdef USE_NOVAS
#ifndef _MSC_VER
extern "C"
{
#endif
#include "novas.h"
#include "novascon.h"
#include "nutation.h"
#ifndef _MSC_VER
}
#endif
#endif    // USE_NOVAS

  // SOFA Library

#include "sofa.h"
#include "sofam.h"

  // Boost Library

#include "boost/scoped_ptr.hpp"

  // Miscellaneous libraries

#include <GCL>


namespace ACL
{
  //*******************************************************************************************************************************
  //
  // CTargetStellar
  //
  //*******************************************************************************************************************************

  /// @brief Default contructor for class.
  /// @throws None.
  /// @version 2016-04-30/GGB - Updated pm, RV and PX to boost::optional<>
  /// @version 2012-01-12/GGB - Added support for location, weather and time of observations.
  /// @version 2009-12-16/GGB - Function created.

  CTargetStellar::CTargetStellar() : CTargetAstronomy(""), catalogCoordinates_(), pmRA_(), pmDec_(),
    radialVelocity_(), parallax_(), epoch_(double(0))
  {
  }

  CTargetStellar::CTargetStellar(std::string const &newName, CAstronomicalCoordinates const &catCoords) : CTargetAstronomy(newName),
    catalogCoordinates_(catCoords), pmRA_(), pmDec_(), radialVelocity_(), parallax_(), epoch_(double(0))
  {

  }

  /// @brief Constructor and assigning values
  /// @throws None.
  /// @version 2012-01-22/GGB - Added name string to constructor list.
  /// @version 2009-12-16/GGB - Function created.

  CTargetStellar::CTargetStellar(std::string const &newName, CAstronomicalCoordinates const &catCoords, FP_t dEP,
    FP_t dpmRA, FP_t dpmDEC, FP_t dRV, FP_t dParallax) : CTargetAstronomy(newName), catalogCoordinates_(catCoords)
  {
    epoch_ = dEP;
    pmRA_ = dpmRA;
    pmDec_ = dpmDEC;
    radialVelocity_ = dRV;
    parallax_ = dParallax;
  }

  /// @brief Copy contructor for class.
  /// @throws None.
  /// @version 2009-12-16/GGB - Updated to reflect changes to class members.
  /// @version 2005-09-19/GGB - Function created.

  CTargetStellar::CTargetStellar(const CTargetStellar &toCopy) :  CTargetAstronomy(toCopy.objectName()),
    catalogCoordinates_(toCopy.catalogCoordinates_), pmRA_(toCopy.pmRA_),
    pmDec_(toCopy.pmDec_), radialVelocity_(toCopy.radialVelocity_), parallax_(toCopy.parallax_), epoch_(toCopy.epoch_)
  {
  }

  /// @brief Class Destructor
  /// @throws None.
  /// @version 2009-12-16 - Function created.

  CTargetStellar::~CTargetStellar()
  {
  }

  /// @brief Copy operator.
  /// @throws None
  /// @version 2009-12-16/GGB - Updated to reflect the changes to the class.

  CTargetStellar &CTargetStellar::operator=(const CTargetStellar &toCopy)
  {
    catalogCoordinates_ = toCopy.catalogCoordinates_;
    pmRA_ = toCopy.pmRA_;
    pmDec_ = toCopy.pmDec_;
    radialVelocity_ = toCopy.radialVelocity_;
    parallax_ = toCopy.parallax_;
    epoch_ = toCopy.epoch_;

    return (*this);
  }


  /// Calculates the observedPlace of an object that is specified in ICRS coordinates.
  /// Each step of the reduction is calculated by a different routine. Intermediate results are stored in the class.
  /// This routine calls each routine in turn to provide the reduction from ICRS coordinate frame to the observed frame.
  /// EXCEPTIONS:  0x2300 - STELLAROBJECTS: Must have valid location for detemining observed position.
  ///              0x2301 - STELLAROBJECTS: Must have valid time for determining observed position.
  //
  // 2012-01-12/GGB - Added observedPlaceValid() support.
  // 2011-07-09/GGB - Function created.

//  CAstronomicalCoordinates const &CTargetStellar::calculateICRSObservedPlace(PAstroTime &tt, PLocation &location, PWeather &weather)
//  {
//#ifdef USE_SOFA
//    ICRSPlace = object;

//    icrsSpaceMotion();
//    icrsParallax();
//    icrsLightDeflection();
//    icrsAberration();
//    icrsFrameBias();
//    icrsERA();
//    icrsPolarMotion();
//    icrsLongitude();              // Calculates the local apparent place.
//    DiurnalAberration();
//    DiurnalParallax();
//    Latitude();
//    Refraction();

//    return observedPlace;
//#elif defined USE_NOVAS
//    cat_entry star;
//    on_surface geo_loc;
//    double xp, yp;
//    double rat, dect, zd, az;

//    ICRSPlace = catalogCoordinates;
//    make_cat_entry("", "ICRS", 1, ICRSPlace.RA(), ICRSPlace.Dec(), pmRA(), pmDec(), parallax(), radialVelocity(), &star);

//    make_on_surface(location->latitude(), location->longitude(), location->altitude(),
//      weather->temperature()(PCL::TU_C), weather->pressure()(PCL::PU::BAR) / 1000, &geo_loc);

//    topo_star((double) tt->TT(), tt->dT(), &star, &geo_loc, 0, &rat, &dect);

//    CEarthOrientationData::getData((*tt), xp, yp);

//  FP_t ra, dec;

//    equ2hor((double) tt->UT1(), tt->dT(), 0, xp, yp, &geo_loc, rat, dect, 2, &zd, &az, &ra, &dec);

//  observedPlace_.set(ra, dec);

//    return observedPlace_;
//#endif
//  }

  /// @brief Calculates the observed position of the stellar object.
  /// Different methods of calculation are used depending on the coordinate system in use (FK4, FK5, ICRS)
  /// The first step is always to convert the coordinates into ICRS coordinates.
  /// @note This routine makes use of the SOFA routines to convert the ICRS coordinates to the observed coordinates.
  /// @note The routine always calculates the current place of the object.
  /// @throws RUNTIME_ASSERT
  /// @version 2016-03-23/GGB - 1. Removed observedPlaceValid support.
  ///                           2. Removed location, weather and time from class members.
  ///                           3. Use SOFA routine for calculation.
  /// @version 2012-01-12/GGB - Added observedPlaceValid() support and immediate return.
  /// @version 2011-07-04/GGB

  CAstronomicalCoordinates const &CTargetStellar::calculateObservedPlace(PAstroTime &tt, PLocation &location, PWeather &weather)
  {
    double utc1, utc2;
    double dut1;
    double aob, zob, hob, dob, rob, eo;
    double rh, dh, drh, ddh, pxh, rvh;

      // Perform some error checking to ensure that the passed data is valid.

    RUNTIME_ASSERT(ACL, !tt, "Need a valid time to calculate observed place.");
    RUNTIME_ASSERT(ACL, !location, "Need a valid location to calculate observed place.");

      // Calculate the ICRS coordinates

    switch (catalogCoordinates_.getReferenceSystem())
    {
      case RS_NONE:
        CODE_ERROR(ACL);     // Should not be able to get here.
        break;
      case RS_ICRS:
        //return currentPlace = calculateCurrentPlaceICRS(tt, location, weather);
        break;
      case RS_FK4:
        //return currentPlace = calculateCurrentPlaceFK4O(tt, location, weather);
        break;
      case RS_FK5:
      {
        //iauFk52h(meanPlace.RA(), meanPlace.Dec(),
                 //pmRA_, pmDec_, parallax_, radialVelocity_,
                 //&rh, &dh, &drh, &ddh, &pxh, &rvh);
        break;
      };
      default:
        CODE_ERROR(ACL);     // Should not be able to get here.
        break;
    };

      // Perform the transform using the IAU SOFA functions.

    iauAtco13(rh, dh,
              drh, ddh, pxh, rvh,
              utc1, utc2, dut1,
              location->longitude(), location->latitude(), location->altitude(),
              0, 0,
              (*weather->pressure())(PCL::PU::PA) / 100, (*weather->temperature())(PCL::TU_C), *weather->RH(),
              495 * 1000,    // 495nm = wavelength of light. (Can be corrected in future.) (! I know it is a magic number)
              &aob, &zob, &hob, &dob, &rob, &eo);

    CAstronomicalCoordinates currentPlace(rob, dob);

    return currentPlace;

  }

  /// @brief Returns the reference system as a string value.
  /// @returns The catalog system as a string value.
  /// @version 2012-01-22/GGB - Function created.

  std::string CTargetStellar::catalogSystemString()
  {
    std::string returnValue;

    switch (catalogCoordinates_.getReferenceSystem())
    {
      case RS_NONE:
      {
        returnValue = COORDINATESYSTEM_NONE;
        break;
      };
      case RS_ICRS:
      {
        returnValue = COORDINATESYSTEM_ICRS;
        break;
      };
      case RS_FK4:
      {
        returnValue = COORDINATESYSTEM_FK4;
        break;
      };
      case RS_FK5:
      {
        returnValue = COORDINATESYSTEM_FK5;
        break;
      };
      default:
      {
        CODE_ERROR(ACL);
      };
    };

    return returnValue;
  }

  /// @brief Returns the distance of an object.
  /// @returns The distance to the object in light years.
  /// @throws none.
  /// @details This replaces an earlier version of the class were the distance was stored in the class. This version uses the
  /// parallax to calculate the distance. The distance is returned in light years.
  /// @version 2009-12-16/GGB - Function created.

  FP_t CTargetStellar::getDistance()
  {
    return (*parallax_ / 1000 * 3.26);  // Convert to arcseconds and then light years
  }

  /// @brief Corrects the Catalogue coordinates for ProperMotion.
  /// @param[in] jd - The julian day to correct to.
  /// @returns The corrected coordinates for the stellar object.
  /// @throws GCL::CCodeError
  /// @throws GCL::CError(ACL, 0x2302) - StellarObjects: SOFA library error, iteration did not converge.
  /// @throws GCL::CRuntimeAssert(ACL)
  /// @details Use is made of the IAU SOFA routines.
  /// @version 2017-09-23/GGB - Updated to use CAngle
  /// @version 2011-07-09/GGB - Function created.

  CAstronomicalCoordinates CTargetStellar::properMotion(TJD const &jd) const
  {
    RUNTIME_ASSERT(ACL, static_cast<double>(jd) >= 0, "The julian day cannot be less than zero.");

    CAstronomicalCoordinates retVal;
    FP_t ra, dec, pmRA, pmDec, parallax, radialVelocity;
    int nRetVal;

    nRetVal = iauStarpm(catalogCoordinates_.RA().radians(), catalogCoordinates_.DEC().radians(),
                        *pmRA_ * DAS2R / cos(catalogCoordinates_.DEC()),
                        *pmDec_ * DAS2R, *parallax_, *radialVelocity_, epoch_(0),
                        epoch_(1), jd(0),
                        jd(1), &ra, &dec, &pmRA, &pmDec, &parallax, &radialVelocity);

    if (nRetVal == -1)
    {
      CODE_ERROR(ACL);
    }
    else if (nRetVal & 0x04)
    {
      ERROR(ACL, 0x2302);
    };

    return CAstronomicalCoordinates(MCL::CAngle(ra), MCL::CAngle(dec));
  }

  /// @brief Overloaded function to determine the proper motion for a date.
  /// @throws None.
  /// @version 2011-07-09/GGB - Function created.

  CAstronomicalCoordinates CTargetStellar::properMotion(FP_t ep) const
  {
    TJD jd(static_cast<double>(ep));

    return ( properMotion(jd) );
  }

  /// @brief Function to set the catalog coordinates and reference system.
  /// @param[in] ncc - The new catalogue coordinates.
  /// @throws None.
  /// @version 2017-08-03/GGB - Removed referenceSystem_ member.
  /// @version 2012-01-12/GGB - Added observedPlaceValid() support.
  /// @version 2011-07-02/GGB - Function created.

  void CTargetStellar::catalogueCoordinates(CAstronomicalCoordinates ncc)
  {
    catalogCoordinates_ = ncc;
  }

  /// @brief Converts the passed string into a Julian day epoch
  /// @param[in] newEpoch - The epoch string to convert.
  /// @throws Several from convertEpoch
  /// @version 2017-08-05/GGB - Use new style convertEpoch.
  /// @version 2012-01-12/GGB - Added observedPlaceValid() support.
  /// @version 2009-12-18/GGB - Function created.

  void CTargetStellar::setEpoch(char *newEpoch)
  {
    epoch_ = convertEpoch(std::string(newEpoch));
  }

  /// @brief Stores the value of newEpoch into the Epoch value.
  /// @param[in] newEpoch - The new epoch value.
  /// @throws None.
  /// @version 2015-08-01/GGB - Corrected bug with assignment of value. Changed parameter to const &.
  /// @version 2012-01-12/GGB - Added observedPlaceValid() support.
  /// @version 2009-12-18/GGB - Function created.

  void CTargetStellar::setEpoch(TJD const &newEpoch)
  {
    epoch_ = newEpoch;
  }

}  // namespace ACL
