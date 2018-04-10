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
//                      Copyright 2005-2015 Gavin Blakeman.
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
// CLASSES INCLUDED:    CStellarObject  - Class providing support for stellar objects.
//
// CLASS HIERARCHY:     CStellarObject
//
// HISTORY:             2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2011-06-30 GGB - Development of classes for AIRDAS
//
//*********************************************************************************************************************************

#include "../Include/StellarObjects.h"

#include "../Include/constants.h"
#include "../Include/AstroFunctions.h"
#include "../Include/FITSStrings.h"

#include "string.h"

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
  // CStellarObject
  //
  //*******************************************************************************************************************************

  /// Default contructor for class.
  //
  // 2012-01-12/GGB - Added support for location, weather and time of observations.
  // 2009-12-16/GGB - Function created.

  CStellarObject::CStellarObject() : CAstroObject(""), catalogCoordinates(), referenceSystem(RS_NONE), _pmRA(0), _pmDec(0),
    _radialVelocity(0), _parallax(0), Epoch((double) 0)
  {
  }

  /// Constructor and assigning values
  //
  // 2012-01-22/GGB - Added name string to constructor list.
  // 2009-12-16/GGB - Function created.

  CStellarObject::CStellarObject(std::string const &newName, CEC const &catCoords, EReferenceSystem eotType, double dEP,
    double dpmRA, double dpmDEC, double dRV, double dParallax) : CAstroObject(newName), catalogCoordinates(catCoords)
  {
    Epoch = dEP;
    _pmRA = dpmRA;
    _pmDec = dpmDEC;
    _radialVelocity = dRV;
    _parallax = dParallax;
  }

  /// Copy contructor for class.
  //
  // 2009-12-16/GGB - Updated to reflect changes to class members.
  // 2005-09-19/GGB - Function created.

  CStellarObject::CStellarObject(const CStellarObject &toCopy) :  CAstroObject(toCopy.objectName()), catalogCoordinates(toCopy.catalogCoordinates),
    referenceSystem(toCopy.referenceSystem), _pmRA(toCopy._pmRA), _pmDec(toCopy._pmDec), _radialVelocity(toCopy._radialVelocity),
    _parallax(toCopy._parallax), Epoch(toCopy.Epoch)
  {
  }

  /// Class Destructor
  //
  // 2009-12-16 - Function created.

  CStellarObject::~CStellarObject()
  {
  }

  /// Copy operator.
  //
  // 2009-12-16/GGB - Updated to reflect the changes to the class.

  CStellarObject &CStellarObject::operator=(const CStellarObject &toCopy)
  {
    catalogCoordinates = toCopy.catalogCoordinates;
    referenceSystem = toCopy.referenceSystem;
    _pmRA = toCopy._pmRA;
    _pmDec = toCopy._pmDec;
    _radialVelocity = toCopy._radialVelocity;
    _parallax = toCopy._parallax;
    Epoch = toCopy.Epoch;

    return (*this);
  }

  /// Calculates the observed place from FK4 coordinates.
  //
  //

  CEC const &CStellarObject::calculateFK4ObservedPlace(CAstroTime_Ptr &, TLocation_Ptr &, PWeather &)
  {
    CODE_ERROR(ACL);
  }


  /// Calculates the observed place from FK5 coordinates.
  //
  //

  CEC const &CStellarObject::calculateFK5ObservedPlace(CAstroTime_Ptr &, TLocation_Ptr &, PWeather &)
  {
    CODE_ERROR(ACL);
  }

  /// Calculates the observedPlace of an object that is specified in ICRS coordinates.
  /// Each step of the reduction is calculated by a different routine. Intermediate results are stored in the class.
  /// This routine calls each routine in turn to provide the reduction from ICRS coordinate frame to the observed frame.
  /// EXCEPTIONS:  0x2300 - STELLAROBJECTS: Must have valid location for detemining observed position.
  ///              0x2301 - STELLAROBJECTS: Must have valid time for determining observed position.
  //
  // 2012-01-12/GGB - Added observedPlaceValid() support.
  // 2011-07-09/GGB - Function created.

  CEC const &CStellarObject::calculateICRSObservedPlace(CAstroTime_Ptr &tt, TLocation_Ptr &location, PWeather &weather)
  {
#ifdef USE_SOFA
    ICRSPlace = object;

    icrsSpaceMotion();
    icrsParallax();
    icrsLightDeflection();
    icrsAberration();
    icrsFrameBias();
    icrsERA();
    icrsPolarMotion();
    icrsLongitude();              // Calculates the local apparent place.
    DiurnalAberration();
    DiurnalParallax();
    Latitude();
    Refraction();

    return observedPlace;
#elif defined USE_NOVAS
    cat_entry star;
    on_surface geo_loc;
    double xp, yp;
    double rat, dect, zd, az;

    ICRSPlace = catalogCoordinates;
    make_cat_entry("", "ICRS", 1, ICRSPlace.RA(false), ICRSPlace.Dec(false), pmRA(), pmDec(), parallax(), radialVelocity(), &star);

    make_on_surface(location->latitude(), location->longitude(), location->altitude(),
      weather->temperature()(PCL::TU_C), weather->pressure()(PCL::PU::BAR) / 1000, &geo_loc);

    topo_star((double) tt->TT(), tt->dT(), &star, &geo_loc, 0, &rat, &dect);

    CEarthOrientationData::getData((*tt), xp, yp);

  FP_t ra, dec;

    equ2hor((double) tt->UT1(), tt->dT(), 0, xp, yp, &geo_loc, rat, dect, 2, &zd, &az, &ra, &dec);

  _observedPlace.setCoordinates(ra, dec);

    observedPlaceValid(true);

    return _observedPlace;
#endif
  }

  /// Calculates the onserved place using the stored location, time and place. Calls the explicit parameter version to perform
  /// the observed place calculations.
  /// EXCEPTIONS: 0x2300 - STELLAROBJECTS: Must have valid location for detemining observed position.
  ///             0x2301 - STELLAROBJECTS: Must have valid time for determining observed position
  //
  // 2012-01-12/GGB - Function created.

  CEC const &CStellarObject::calculateObservedPlace()
  {
      return calculateObservedPlace(observationTime, observationLocation, observationWeather);
  }

  /// Calculates the observed position of the stellar object. Different methods of calculation are used depending on the coordinate
  /// system in use (FK4, FK5, ICRS) The position is only calculated if necessary. If the observedPlace is valid, then the value is
  /// simply returned.
  //
  // 2012-01-12/GGB - Added observedPlaceValid() support and immediate return.
  // 2011-07-04/GGB

  CEC const &CStellarObject::calculateObservedPlace(CAstroTime_Ptr &tt, TLocation_Ptr &location, PWeather &weather)
  {
    if (observedPlaceValid())
      return _observedPlace;
    else
    {
      if (!tt)
      {
        ERROR(ACL, 0x2301);        // STELLAROBJECTS: Must have valid time for determining observed position.
      }
      if (!location)
      {
        ERROR(ACL, 0x2300);        // STELLAROBJECTS: Must have valid location for detemining observed position.
      };

      switch (referenceSystem)
      {
        case RS_NONE:
          CODE_ERROR(ACL);     // Should not be able to get here.
          break;
        case RS_ICRS:
          return calculateICRSObservedPlace(tt, location, weather);
          break;
        case RS_FK4:
          return calculateFK4ObservedPlace(tt, location, weather);
          break;
        case RS_FK5:
          return calculateFK5ObservedPlace(tt, location, weather);
          break;
        default:
          CODE_ERROR(ACL);     // Should not be able to get here.
          break;
      };
    };
  }

  /// Returns the reference system as a string value.
  //
  // 2012-01-22/GGB - Function created.

  std::string CStellarObject::catalogSystemString()
  {
    std::string returnValue;

    switch (referenceSystem)
    {
    case RS_NONE:
      returnValue = COORDINATESYSTEM_NONE;
      break;
    case RS_ICRS:
      returnValue = COORDINATESYSTEM_ICRS;
      break;
    case RS_FK4:
      returnValue = COORDINATESYSTEM_FK4;
      break;
    case RS_FK5:
      returnValue = COORDINATESYSTEM_FK5;
      break;
    default:
      CODE_ERROR(ACL);
    };

    return returnValue;
  }

  /// Sets the catalogue coordinates for the object.
  /// The passed values are interpreted as per the ECoordinateFormat
  /// values.
  /// Return:			0 - Success
  ///							1 - Incorrect Parameter Format
  //
  // 2012-01-12/GGB - Added observedPlaceValid() support.
  // 2009-12-16/GGB - Function created.

  int CStellarObject::SetCoordinates(double RA, double Dec, ECoordinateFormat cfRA, ECoordinateFormat cfDec)
  {
    double dRA, dDec;
    double dInt;
    int nSign;
    int nRetVal = 0;

    switch (cfRA)
    {
    case CF_NONE:
    case CF_Dd:
      dRA = RA;
      break;
    case CF_DMSs:
      dRA = floor(RA/10000);
      RA -= dRA * 10000;
      dInt = floor(RA/100);				// Minutes to decimal
      RA -= dInt;
      dRA += dInt / 60;
      dRA += (RA / 100) / 3600;			// Seconds to decimal
      break;
    case CF_HMSs:

        // First convert the value to an hours fraction.

      dRA = floor(RA / 10000);				// Hours to fraction
      RA -= dRA * 10000;						// Delete the hours
      dInt = floor(RA/100);				// Minutes to decimal
      RA -= dInt * 100;
      dRA += dInt / 60;
      dRA += RA / 3600;			// Seconds to decimal

        // Now convert the hours fraction to degrees.

      dRA *= 15;										// Convert to Degrees.
      break;
    default:
      dRA = RA;
      break;
    };

    switch (cfDec)
    {
    case CF_NONE:
    case CF_Dd:
      dDec = Dec;
      break;
    case CF_DMSs:
      if (Dec < 0)
        nSign = -1;
      else nSign = 0;
      Dec = abs(Dec);									// Drop the sign
      dDec = floor(Dec/10000);
      Dec -= dDec * 10000;
      dInt = floor(Dec/100);
      Dec -= dInt * 100;
      dDec += dInt / 60;
      dDec += Dec / 3600;
      if (nSign)
        dDec = -dDec;
      break;
    case CF_HMSs:										// Illegal - Declination should be in
                                    // degrees only
      nRetVal = 1;
      break;

    default:
      dDec = Dec;
      break;
    };

    if (!nRetVal)
      catalogCoordinates.setCoordinates(dRA, dDec);

    observedPlaceValid(false);

    return nRetVal;
  }

  // Returns the distance of an object. This replaces an earlier version of the
  // class were the distance was stored in the class. This version uses the
  // parallax to calculate the distance. The distance is returned in light years.
  //
  // 2009-12-16/GGB - Function created.

  double CStellarObject::Get_Distance(void)
  {
      return (parallax() / 1000 * 3.26);  // Convert to arcseconds and then light years
  }

  // Corrects the Catalogue coordinates for ProperMotion.
  // Use is made of the IAU SOFA routines.
  //
  // 2011-07-09/GGB - Function created.

  CStellarObject CStellarObject::properMotion(TJD const &jd) const
  {
    CStellarObject retVal;
  FP_t ra, dec;
    int nRetVal;

    retVal.Epoch(0) = jd(0);
    retVal.Epoch(1) = jd(1);

    nRetVal = iauStarpm(catalogCoordinates.RA(false), catalogCoordinates.Dec(false),
      _pmRA * DAS2R / cos(catalogCoordinates.Dec(false)), _pmDec * DAS2R, _parallax, _radialVelocity, Epoch(0), Epoch(1), jd(0),
      jd(1), &ra, &dec, &retVal.pmRA(), &retVal.pmDec(), &retVal.parallax(),
      &retVal.radialVelocity());

  retVal.SetCoordinates(ra, dec);

        // RA and declination are in Radians

    retVal.pmRA() *= cos(retVal.catalogCoordinates.Dec(false));
    retVal.pmRA() *= D_R2D;
    retVal.pmDec() *= D_R2D;

    return retVal;
  }

  // Overloaded function to determine the proper motion for a date.
  //
  // 2011-07-09/GGB - Function created.

  CStellarObject CStellarObject::properMotion(double ep) const
  {
    TJD jd((double) ep);

    return ( properMotion(jd) );
  }

  /// Function to set the catalog coordinates and reference system.
  //
  // 2012-01-12/GGB - Added observedPlaceValid() support.
  // 2011-07-02/GGB - Function created.

  void CStellarObject::SetCoordinates(CEC const &newCatalogCoordinates, EReferenceSystem newReferenceSystem)
  {
    catalogCoordinates = newCatalogCoordinates;
    referenceSystem = newReferenceSystem;

    observedPlaceValid(false);
  }

  /// Converts the passed string into a Julian day epoch
  //
  // Return Values:	0 - Success
  //								1 - Numeric Value not correct
  //								2 - Form not correct (not Jxxxx or Bxxxx)
  //
  // 2012-01-12/GGB - Added observedPlaceValid() support.
  // 2009-12-18/GGB - Function created.

  int CStellarObject::SetEpoch(char *newEpoch)
  {
    int nRetVal = 0;
    double ep1a, ep1b;					// Julian day variables.

    nRetVal = ConvertEpoch(newEpoch, &ep1a, &ep1b);
    if (!nRetVal)
      Epoch = ep1a + ep1b;
    else Epoch = 0;

    observedPlaceValid(false);

    return (nRetVal);
  }

  /// @brief Stores the value of newEpoch into the Epoch value.
  /// @param[in] newEpoch - The new epoch value.
  /// @throws None.
  //
  // 2015-08-01/GGB - Corrected bug with assignment of value. Changed parameter to const &.
  // 2012-01-12/GGB - Added observedPlaceValid() support.
  // 2009-12-18/GGB - Function created.

  void CStellarObject::SetEpoch(TJD const &newEpoch)
  {
    Epoch = newEpoch;

    observedPlaceValid(false);
  }

}  // namespace ACL
