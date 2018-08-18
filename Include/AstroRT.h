//*************************************************************************************************
//
// PROJECT:             Astronomy Class Library (ACL)
// FILE:                AstroRT
// SUBSYSTEM:           Realtime Target Position
// LANGUAGE:            C++
// TARGET OS:           None.
// LIBRARY DEPENDANCE:	None
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2010-2016 Gavin Blakeman.
//                      This file is part of the Astronomy Class Library (ACL)
//
//                      ACL is free software: you can redistribute it and/or modify it under the terms of the GNU General
//                      Public License as published by the Free Software Foundation, either version 2 of the License, or (at your
//                      option) any later version.
//
//                      ACL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
//                      implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//                      for more details.
//
//                      You should have received a copy of the GNU General Public License along with ACL.  If not,
//                      see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:            Class TAstronomicalCalc is the main calculation class for the position of an object.
//                      This class is multi-threaded and runs several threads to continually recalculate the position of date
//
// CLASSES INCLUDED:		CAstronomicalCalc			- A class to calculate the real-time pointing parameters for an target.
//
// CLASS HIERARCHY:     CAstronomicalCalc
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2015-03-25 GGB - Removed all deprecated functions.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2005-07-01 GGB - Development of classes for astroManager
//
//**********************************************************************************************************************************

#ifndef ACL_ASTRORT_H
#define ACL_ASTRORT_H

#include "AstroClass.h"
#include "config.h"
#include "AstronomicalTime.h"
#include "TargetAstronomy.h"

  // Boost library

#ifndef __XC32
#include "boost/thread.hpp"
#endif

/// @file
/// The AstroRT files provide the real-time position calculation functions that are used for telescope control. These functions
/// are not required as part of the standard build for the ACL library and must be explicitly included.


namespace ACL
{

#ifdef ACL_REALTIME

  /// @note The application needs to define these two functions.
  void enterCriticalSection();
  void leaveCriticalSection();

class CAstronomicalCalc
{
  private:
    TJD CJD;                              // Calculated Julian Date
    CAstroTime UT1;                       // Universal coordinated time 1

    FP_t dUT1;                            // Difference between UTC and UT1 (seconds)
    FP_t TT_TAI;
    FP_t TAI_UTC;
    CWeather weather;

    CTargetAstronomy *currentTarget;

    CAstronomicalCoordinates Coords_1; // Corrected Steller coordinates of the object
    CAstronomicalCoordinates Coords_2;
    CAstronomicalCoordinates Coords_3;
    CAstronomicalCoordinates Coords_4;
    CAstronomicalCoordinates Coords5;
    CAstronomicalCoordinates Coords6;
    CAstronomicalCoordinates Coords7;
    CAstronomicalCoordinates Coordst;

    FP_t T; // Julian centuries from J2000.0
    FP_t epsilon; // Obliquity of the ecliptic 5.16k
    FP_t DELTA_PSI; // EQU 5.9a
    FP_t OMEGA; // Longitude of the ascending node of the moons mean orbit on the ecliptic 5.8a
    FP_t l_; //Mean anomoly of sun 5.8c
    FP_t GMSTo; // Greenwich mean sidereal time @ 0h
    FP_t GASTo; // Greenwich apparent sidereal time @ 0h
    FP_t  GMST; // Geenwich mean sidereal time
    FP_t h; // Hour angle
    FP_t GAST; // Greenwich apparenent sidereal time
    FP_t LAST; // Local apparent sidereal time
    TLocation Location; // Observers location

#ifndef ACL_NOMT    // Mutexes are not needed for non-multi threaded code.
    mutable boost::mutex mtxLocation;          // Mutex for the Location Variable
    mutable boost::mutex mtxLAST;              // Mutex for the LAST variable
  //CRITICAL_SECTION csGAST; // Critical section for the GAST variable
  //CRITICAL_SECTION csUT1;  // Critical section for the UT1 variable.
  //CRITICAL_SECTION csGMST; // Critical section for GMST.
  //CRITICAL_SECTION csdUT1;
    mutable boost::mutex mtxh;
  //CRITICAL_SECTION csCurrObs; // Critical section for the Current Observation
  //CRITICAL_SECTION csCJD;
  //CRITICAL_SECTION csGMSTo;
  //CRITICAL_SECTION csGASTo;
  //CRITICAL_SECTION csT;
    mutable boost::mutex mtxCoords5;
    mutable boost::mutex mtxCoords6;
    mutable boost::mutex mtxCoords7;
    mutable boost::mutex mtxCoordst;
  //CRITICAL_SECTION csDELTA_PSI;
  //CRITICAL_SECTION csl_;
  //CRITICAL_SECTION csEpsilon;
#endif


    void Calculate_DPhi();
    void Calculate_GASTo();
    void Calculate_h();
    void Calculate_Time();

    void Calculate_AnnualAbberation();
    void Calculate_Precession();
    void calculateNutation();
    void Calculate_OrbitalMotion();
    void Calculate_ProperMotion();
    void Calculate_DiurnalAbberation();
    void Calculate_PlanetaryParallax();
    void Calculate_Refraction();

    void Calculate_ApparentPlace();

  protected:
    FP_t dtemp;

#ifndef ACL_NOMT
    static bool nQuitPending;
#endif

    void Calculate_SiderealTime();

  public:
    CAstronomicalCalc();
    virtual ~CAstronomicalCalc();

    void setTarget(CTargetAstronomy *);

    void Set_Location(const TLocation &);
    void Get_Location(TLocation &);

    void Set_dUT1(FP_t const);
    FP_t Get_dUT1();
    void Obtain_dUT1(char *);
    FP_t Get_TT_TAI() { return TT_TAI; }
    FP_t Get_TAI_UTC() { return TAI_UTC; }

    void realtimeFunction();
    void highPriorityFunction();
    void mediumPriorityFunction();
    void lowPriorityFunction();
};

#endif //REALTIME

}   // namespace ACL

#endif // ACL_ASTRORT_H
