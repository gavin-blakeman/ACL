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
// OVERVIEW:            Class CAstronomicalCalc is the main calculation class for the position of an object.
//                      This class is multi-threaded and runs several threads to continually recalculate the position of date
//
// CLASSES INCLUDED:		CAstronomicalCalc			- A class to calculate the real-time pointing parameters for an target.
//
// CLASS HIERARCHY:     CAstronomicalCalc
//
// HISTORY:             2015-03-25 GGB - Removed all deprecated functions.
//                      2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2005-07-01 GGB - Development of classes for AIRDAS
//
//**********************************************************************************************************************************

#include "../Include/AstroRT.h"

#include <cmath>
#include <ctime>

#include <MCL>

#include "../Include/constants.h"

namespace ACL
{
#define THREAD_T_TIME 10
#define THREAD_H_TIME 50	// 50ms -> 20 times/second
#define THREAD_M_TIME	2500  // 5000ms -> 5 seconds
#define THREAD_L_TIME	60000  // 60000ms -> 1 minutes


//**********************************************************************************************************************************
// CAstronomicalCalc
// Performs real-time calculation of the position of an object in RA/DEC coordinates.
// Uses multiple threads to update the position of the object as required.
//
// TAstronomicalCalc implements 4 threads. 
// ThreadProc_H -> Realtime thread. Called 20 times/second
// ThreadProc_M -> Called every 2.5 seconds
// ThreadProc_L -> Called every 1 minutes
// ThreadProc_T	-> Called 100 times/second
//
// TAstronomicalCalc is used in a multi-threaded manner.
//
// 2009-09-29/GGB - 1) Moved from file TCSCLASS to ASTROCLASS.
//                  2) Made to be application indendant. Removed all references to TCS and the TCS specific classes.
//                  3) Made to a standalone class. IE no friends allowed. All access via functions.
//                  4) Removed class member CEphemeris *Ephemeris.
// 2005-07-01/GGB - Function created.
//
// ************************** FUNCTIONS - Where Used *******************************************************************************
// Function											|	realtime    	| highPriority  |	mediumPriority| lowPriority   | MainThread	|
// Calculate_Time								|				X				|								|								|								|							|
// Calculate_AnnualAbberation		|								|								|								|				X				|							|
// Calculate_Precession					|								|								|								|				X				|							|
// Calculate_Nutation						|								|								|								|				X				|							|
// Calculate_OrbitalMotion			|								|								|								|				X				|							|
// Calculate_ProperMotion				|								|								|								|				X				|							|
// Calculate_DiurnalAbberation	|								|								|				X				|								|							|
// Calculate_PlanetaryParallax	|								|								|				X				|								|							|
// Calculate_Refraction					|								|				X				|								|								|							|
// Calculate_h									|				X				|								|								|								|							|
//**********************************************************************************************************************************
//
// ************************** SYNCHRONISATION OBJECTS *********************************
//*****************************************************************************************************
// Variable		|  ThreadProc_T	|	ThreadProc_H	|	ThreadProc_M	| ThreadProc_L	| MainThread	|		MT		|
//-----------------------------------------------------------------------------------------------------
//	dUT1			|			R					|								|								|								|		RW				|		CS		|
//	CJD				|			 W				|								|								|								|		R					|		CS		|
//	TZ				|								|								|								|								|							|					|
//	T					|			RW				|								|								|				R				|							|					|
//	epsilon 	|			R					|								|								|				RW 			|							|		CS		|
//	DELTA_PSI	|			R					|								|								|     	RW     	|							|		CS		|
//	OMEGA			|								|								|								|				RW 			|							|					|
//	l_				|								|								|								|				RW 			|							|		CS		|
//	GMSTo			|			RW				|								|								|								|		R					|		CS		|
//	GASTo			|			RW				|								|								|								|		R					|		CS		|
//	GMST			|			 W				|								|								|								|		R					|		CS		|
//	GAST			|			RW				|								|								|								|		R					|		CS		|
//	LAST			|			 W				|								|								|								|		R					|		CS		|
//	Location	|			R					|								|				R				|								|		 W				|		CS		|
//	h					|			RW				|								|				R 			|								|		R					|		CS		|
// CurrObs		|								|								|								|				R				|		 W				|		CS		|
// Coords_1		|								|								|								|				RW 			|							|					|
// Coords_2		|								|								|								|				RW			|							|					|
// Coords_3		|								|								|								|				RW			|							|					|
// Coords_4		|								|								|								|				RW			|							|					|
// Coords_5		|								|								|				R				|				 W			|							|		CS		|
// Coords_6		|								|								|				RW			|								|							|					|
// Coords_7		|								|				R				|				 W			|								|							|		CS		|
// Coords_t		|								|				 W			|								|								|		R					|		CS		|
// R - Read; W - Write; CS - Critical section																													*
//*****************************************************************************************************
//
// Critical Section Object - Useage Order
//***********************
// Object			|	Order		|
//-----------------------
// csT				|		1			|
//

/// @brief Constructor for the class. Initialise relevant variables.
//
// 2009-08-30/GGB - 1) Added dPressure as a variable. Changes to make code generic rather than tied to TCS application.
//                  2) Changed HKEY in function header to void.
//                  3) Default dUT1 to 0. Value of dUT1 must be set by calling application either via a call to  SetdUT1() or by a 
//                      call to ObtaindUT1().
//                  4) Removed class member Ephemeris.
// 2005-07-06/GGB - Function created.

  CAstronomicalCalc::CAstronomicalCalc() : UT1(), dUT1(0.387), CJD(), Location()
  {
	
    dUT1 = 0;							// Changed from ObtaindUT1 30/8/2009
	
    // Initialise all the class variables

    calculateTime();
    calculatePrecession();
    calculateNutation();
  }

/// @brief Class destructor
//
/// @version 2016-01-14/GGB - Updated for use with embedded systems and mutexes.
/// @version 2005-08-14/GGB - Function created.

  CAstronomicalCalc::~CAstronomicalCalc()
  {
  }

// Just a pass through function at this stage. Further functionallity to be added later.
// Routine called 20 times per second.
// Input:		Coords_7
// Output:	Coords_t
//
// 2015-03-26/GGB - Converted to FP_t and use boost::mutex.
// 2005-07-01/GGB - Function created.

  void CAstronomicalCalc::Calculate_Refraction()
  {
    static FP_t R;
    static FP_t P = 101;
    static FP_t Tc = 25;
    static FP_t dLat, dDec, dRA, dDecd, dRAd, dDecd1;
    static FP_t Z;
    static int Counter = 0;
    static FP_t Rfactor = 1;
    static FP_t cosn, sinn, dh, dhd;
	
      // Only fetch the Pressure and temperature on alternating half seconds. Counter is used to monitor this.

    Counter++;
    if (Counter == 10)
    {
      P = weather.pressure() * 10.0; // Convert to mBar
      Rfactor = (283 * P) / (101 * (273 + Tc));
    }
    else if (Counter == 20)
    {
      Tc = weather.temperature();
      Rfactor = (283 * P) / (101 * (273 + Tc));
      Counter = 0;
    };

    enterCriticalSection();
    dLat = Location.latitude() * D_D2R;
    dDecd = dDec = Coords7.Dec(false);
    dRAd = dRA = Coords7.RA(false);
    dhd = dh = h *D_D2R;
    leaveCriticalSection();

      // Iteration loop to perform the calculation

    do
    {
      dDecd1 = dDecd;
      Z = std::acos(sin(dLat) * sin(dDec) + cos(dLat) * cos(dDec) * cos(dRA)); // Leave in radians
      R = Rfactor * (1.02 / (std::tan((MCL::D_PIDIV2 - Z) + (10.3 * D_D2R / ((MCL::D_PIDIV2 - Z)+ 5.11 * D_D2R) +
          0.0019279 * D_D2R)))) /60; // Convert to degrees of arc
      R *= DD2R; // Convert to radians
      cosn = (std::sin(dLat) - std::cos(Z-R)	* std::sin(dDecd)) / (std::sin(Z-R) * std::cos(dDecd));
      sinn = (cos(dLat)*cos(dhd))/sin(Z-R);
      dDecd = dDec + R*cosn;
      dhd = dh - R * sinn/cos(dDecd);
      dRAd = dRA - R * sinn/cos(dDecd);
    }
    while ( (dDecd - dDecd1) > 1/3600);


    enterCriticalSection();
    Coordst.setCoordinates(dRAd, dDecd, false);
    leaveCriticalSection();
  }

// Calculates the Diurnal abberation.
// This calculation is called by the medium speed thread procedure every 2.5 seconds
//
// Input:		Coords_5
// Output:	Coords_6
// Class variables accessed:	Coords_5
// Class variables modified:	csCoords_5
//														Coords_6
//
// Multi-thread safe.
//
// 2015-03-26/GGB - Converted to use boost::mutex
// 2005-07-01/GGB - Function created.

  void CAstronomicalCalc::Calculate_DiurnalAbberation()
  {
    FP_t dRAErr, dDecErr;
    FP_t dRA, dDec;
    FP_t r;
    FP_t dLat, cosLat, sinLat;

    mtxCoords5.lock();
    dRA = Coords5.RA(false);
    dDec = Coords5.Dec(false);
    mtxCoords5.unlock();

    mtxLocation.lock();
    dLat = Location.latitude() * D_D2R;
    mtxLocation.unlock();

    sinLat = std::sin(dLat);
    cosLat = std::cos(dLat);
    r = (21.4 * sinLat * sinLat + 6378140) / 6378140;
    dRAErr = 0.0213/3600 * r * cosLat * std::cos(h * D_D2R) / std::cos(dDec * D_D2R);
    dDecErr = 0.320/3600 * r * cosLat * std::sin(h * D_D2R) * std::sin(dDec * D_D2R);

    mtxCoords6.lock();
    Coords6.setCoordinates(dRA + dRAErr, dDec + dDecErr, false);
    mtxCoords6.unlock();
  }

// Calculates the planetary parallax.
// No calculation performed at this stage
// Called from the medium speed thread procedure
//
// Input:		Coords_6
// Output:	Coords_7
// Class variables accessed:	Coords_6
// Class variables modified:	csCoords_7
//														Coords_7
// 
// Multi-thread safe
//
// 2015-03-26/GGB - Converted to boost::mutex
// 2005-07-01/GGB - Function created.

  void CAstronomicalCalc::Calculate_PlanetaryParallax()
  {
    mtxCoords6.lock();
    mtxCoords7.lock();
    Coords7 = Coords6;
    mtxCoords7.unlock();
    mtxCoords6.unlock();
  }

/// @brief Calculates the nutation
/// Called from the low priority thread
// Input:		Coords_2
// Output:	Coords_3
// Class variables accessed:	T
//														epsilon
//														l_
//														Coords_2
// Class variables modified:	csT
//														csEpsilon
//														csl_
//														DELTA_PSI
//														csDELTA_PSI
//														Coords_3
// 
// Multi-threading safe
//
/// @version 2005-07-01/GGB - Function created.

  void CAstronomicalCalc::calculateNutation()
  {
    FP_t dT, dT2, dT3;
    FP_t Ms, Mm, F, D, DELTA_epsilon;
    FP_t dRAErr, dRA;
    FP_t dDecErr, dDec;
    FP_t dDELTA_PSI, dEpsilon, dl_;
    int i;
    FP_t arg;

    // Mm Ms F  D Om            psisin     t sin    eps cos     t cos
    static FP_t NC[106][9] = {
      { 0,  0,  0,  0, 1, -171996.0, -174.2,  92025.0,   8.9},
      { 0,  0,  2, -2, 2,  -13187.0,   -1.6,   5736.0,  -3.1},
      { 0,  0,  2,  0, 2,   -2274.0,   -0.2,    977.0,  -0.5},
      { 0,  0,  0,  0, 2,    2062.0,    0.2,   -895.0,   0.5},
      { 0,  1,  0,  0, 0,    1426.0,   -3.4,     54.0,  -0.1},
      { 1,  0,  0,  0, 0,     712.0,    0.1,     -7.0,   0.0},
      { 0,  1,  2, -2, 2,    -517.0,    1.2,    224.0,  -0.6},
      { 0,  0,  2,  0, 1,    -386.0,   -0.4,    200.0,   0.0},
      { 1,  0,  2,  0, 2,    -301.0,    0.0,    129.0,  -0.1},
      { 0, -1,  2, -2, 2,     217.0,   -0.5,    -95.0,   0.3},
      { 1,  0,  0, -2, 0,    -158.0,    0.0,     -1.0,   0.0},	//????
      { 0,  0,  2, -2, 1,     129.0,    0.1,    -70.0,   0.0},
      {-1,  0,  2,  0, 2,     123.0,    0.0,    -53.0,   0.0},
      { 1,  0,  0,  0, 1,      63.0,    0.1,    -33.0,   0.0},
      { 0,  0,  0,  2, 0,      63.0,    0.0,     -2.0,   0.0},  // ????
      {-1,  0,  2,  2, 2,     -59.0,    0.0,     26.0,   0.0},
      {-1,  0,  0,  0, 1,     -58.0,   -0.1,     32.0,   0.0},
      { 1,  0,  2,  0, 1,     -51.0,    0.0,     27.0,   0.0},
      { 2,  0,  0, -2, 0,      48.0,    0.0,      1.0,   0.0},
      {-2,  0,  2,  0, 1,      46.0,    0.0,    -24.0,   0.0},
      { 0,  0,  2,  2, 2,     -38.0,    0.0,     16.0,   0.0},
      { 2,  0,  2,  0, 2,     -31.0,    0.0,     13.0,   0.0},
      { 2,  0,  0,  0, 0,      29.0,    0.0,     -1.0,   0.0}, // ????????
      { 1,  0,  2, -2, 2,      29.0,    0.0,    -12.0,   0.0},
      { 0,  0,  2,  0, 0,      26.0,    0.0,     -1.0,   0.0},
      { 0,  0,  2, -2, 0,     -22.0,    0.0,      0.0,   0.0},
      {-1,  0,  2,  0, 1,      21.0,    0.0,    -10.0,   0.0},
      { 0,  2,  0,  0, 0,      17.0,   -0.1,      0.0,   0.0},
      { 0,  2,  2, -2, 2,     -16.0,    0.1,      7.0,   0.0},
      {-1,  0,  0,  2, 1,      16.0,    0.0,     -8.0,   0.0},
      { 0,  1,  0,  0, 1,     -15.0,    0.0,      9.0,   0.0},
      { 1,  0,  0, -2, 1,     -13.0,    0.0,      7.0,   0.0},
      { 0, -1,  0,  0, 1,     -12.0,    0.0,      6.0,   0.0},
      { 2,  0, -2,  0, 0,      11.0,    0.0,      0.0,   0.0},
      {-1,  0,  2,  2, 1,     -10.0,    0.0,      5.0,   0.0},
      { 1,  0,  2,  2, 2,      -8.0,    0.0,      3.0,   0.0},
      { 0, -1,  2,  0, 2,      -7.0,    0.0,      3.0,   0.0},
      { 0,  0,  2,  2, 1,      -7.0,    0.0,      3.0,   0.0},
      { 1,  1,  0, -2, 0,      -7.0,    0.0,      0.0,   0.0},
      { 0,  1,  2,  0, 2,       7.0,    0.0,     -3.0,   0.0},
      {-2,  0,  0,  2, 1,      -6.0,    0.0,      3.0,   0.0},
      { 0,  0,  0,  2, 1,      -6.0,    0.0,      3.0,   0.0},
      { 2,  0,  2, -2, 2,       6.0,    0.0,     -3.0,   0.0},
      { 1,  0,  0,  2, 0,       6.0,    0.0,      0.0,   0.0},
      { 1,  0,  2, -2, 1,       6.0,    0.0,     -3.0,   0.0},
      { 0,  0,  0, -2, 1,      -5.0,    0.0,      3.0,   0.0},
      { 0, -1,  2, -2, 1,      -5.0,    0.0,      3.0,   0.0},
      { 2,  0,  2,  0, 1,      -5.0,    0.0,      3.0,   0.0},
      { 1, -1,  0,  0, 0,       5.0,    0.0,      0.0,   0.0},
      { 1,  0,  0, -1, 0,      -4.0,    0.0,      0.0,   0.0},
      { 0,  0,  0,  1, 0,      -4.0,    0.0,      0.0,   0.0},
      { 0,  1,  0, -2, 0,      -4.0,    0.0,      0.0,   0.0},
      { 1,  0, -2,  0, 0,       4.0,    0.0,      0.0,   0.0},
      { 2,  0,  0, -2, 1,       4.0,    0.0,     -2.0,   0.0},
      { 0,  1,  2, -2, 1,       4.0,    0.0,     -2.0,   0.0},
      { 1,  1,  0,  0, 0,      -3.0,    0.0,      0.0,   0.0},
      { 1, -1,  0, -1, 0,      -3.0,    0.0,      0.0,   0.0},
      {-1, -1,  2,  2, 2,      -3.0,    0.0,      1.0,   0.0},
      { 0, -1,  2,  2, 2,      -3.0,    0.0,      1.0,   0.0},
      { 1, -1,  2,  0, 2,      -3.0,    0.0,      1.0,   0.0},
      { 3,  0,  2,  0, 2,      -3.0,    0.0,      1.0,   0.0},
      {-2,  0,  2,  0, 2,      -3.0,    0.0,      1.0,   0.0},
      { 1,  0,  2,  0, 0,       3.0,    0.0,      0.0,   0.0},
      {-1,  0,  2,  4, 2,      -2.0,    0.0,      1.0,   0.0},
      { 1,  0,  0,  0, 2,      -2.0,    0.0,      1.0,   0.0},
      {-1,  0,  2, -2, 1,      -2.0,    0.0,      1.0,   0.0},
      { 0, -2,  2, -2, 1,      -2.0,    0.0,      1.0,   0.0},
      {-2,  0,  0,  0, 1,      -2.0,    0.0,      1.0,   0.0},
      { 2,  0,  0,  0, 1,       2.0,    0.0,     -1.0,   0.0},
      { 3,  0,  0,  0, 0,       2.0,    0.0,      0.0,   0.0},
      { 1,  1,  2,  0, 2,       2.0,    0.0,     -1.0,   0.0},
      { 0,  0,  2,  1, 2,       2.0,    0.0,     -1.0,   0.0},
      { 1,  0,  0,  2, 1,      -1.0,    0.0,      0.0,   0.0},
      { 1,  0,  2,  2, 1,      -1.0,    0.0,      1.0,   0.0},
      { 1,  1,  0, -2, 1,      -1.0,    0.0,      0.0,   0.0},
      { 0,  1,  0,  2, 0,      -1.0,    0.0,      0.0,   0.0},
      { 0,  1,  2, -2, 0,      -1.0,    0.0,      0.0,   0.0},
      { 0,  1, -2,  2, 0,      -1.0,    0.0,      0.0,   0.0},
      { 1,  0, -2,  2, 0,      -1.0,    0.0,      0.0,   0.0},
      { 1,  0, -2, -2, 0,      -1.0,    0.0,      0.0,   0.0},
      { 1,  0,  2, -2, 0,      -1.0,    0.0,      0.0,   0.0},
      { 1,  0,  0, -4, 0,      -1.0,    0.0,      0.0,   0.0},
      { 2,  0,  0, -4, 0,      -1.0,    0.0,      0.0,   0.0},
      { 0,  0,  2,  4, 2,      -1.0,    0.0,      0.0,   0.0},
      { 0,  0,  2, -1, 2,      -1.0,    0.0,      0.0,   0.0},
      {-2,  0,  2,  4, 2,      -1.0,    0.0,      1.0,   0.0},
      { 2,  0,  2,  2, 2,      -1.0,    0.0,      0.0,   0.0},
      { 0, -1,  2,  0, 1,      -1.0,    0.0,      0.0,   0.0},
      { 0,  0, -2,  0, 1,      -1.0,    0.0,      0.0,   0.0},
      { 0,  0,  4, -2, 2,       1.0,    0.0,      0.0,   0.0},
      { 0,  1,  0,  0, 2,       1.0,    0.0,      0.0,   0.0},
      { 1,  1,  2, -2, 2,       1.0,    0.0,     -1.0,   0.0},
      { 3,  0,  2, -2, 2,       1.0,    0.0,      0.0,   0.0},
      {-2,  0,  2,  2, 2,       1.0,    0.0,     -1.0,   0.0},
      {-1,  0,  0,  0, 2,       1.0,    0.0,     -1.0,   0.0},
      { 0,  0, -2,  2, 1,       1.0,    0.0,      0.0,   0.0},
      { 0,  1,  2,  0, 1,       1.0,    0.0,      0.0,   0.0},
      {-1,  0,  4,  0, 2,       1.0,    0.0,      0.0,   0.0},
      { 2,  1,  0, -2, 0,       1.0,    0.0,      0.0,   0.0},
      { 2,  0,  0,  2, 0,       1.0,    0.0,      0.0,   0.0},
      { 2,  0,  2, -2, 1,       1.0,    0.0,     -1.0,   0.0},
      { 2,  0, -2,  0, 1,       1.0,    0.0,      0.0,   0.0},
      { 1, -1,  0, -2, 0,       1.0,    0.0,      0.0,   0.0},
      {-1,  0,  0,  1, 1,       1.0,    0.0,      0.0,   0.0},
      {-1, -1,  0,  2, 1,       1.0,    0.0,      0.0,   0.0},
      { 0,  1,  0,  1, 0,       1.0,    0.0,      0.0,   0.0}};

    enterCriticalSection();
    dT = T;
    leaveCriticalSection();

    enterCriticalSection();
    dEpsilon = epsilon * DD2R;
    leaveCriticalSection();

    enterCriticalSection();
    dl_ = l_ * DD2R;
    leaveCriticalSection();

    dT2 = dT * dT;
    dT3 = dT2 * dT;
    OMEGA = 125.04452 - (1934.136261 * dT) + (0.0020708 * dT2) + (dT3 / 450000); // degrees
    OMEGA -= std::floor(OMEGA / 360) * 360; // Normlise
    OMEGA *= D_D2R;  // Convert to radians

    Ms = 357.52772 + (35999.050340 * dT) + (0.0001603 * dT2) + (dT3 / 300000); // degrees
    Ms -= std::floor(Ms / 360) * 360; // Normalise
    Ms *= D_D2R; // = radians

    F = 93.27191 + (483202.017538 * dT) - (0.0036825 * dT2) + (dT3 / 327270); //degrees
    F -= std::floor(F / 360) * 360;  // Normalise
    F *= D_D2R;  // = radians

    D = 297.85036 + (445267.111480 * dT) - (0.0019142 * dT2) + (dT3 / 189474); // degrees
    D -= std::floor(D / 360) * 360;  // Normalise
    D *= D_D2R;  // D = radians

    Mm = 134.96298 + 477198.867398 * dT + 0.0086972 * dT2 + dT3/56250;
    Mm -= std::floor(Mm / 360) * 360;  // Normalise
    Mm *= D_D2R;  // Radians

    dDELTA_PSI = 0;
    DELTA_epsilon = 0;

    for (i = 0; i < 106; i++)
    {
      arg = (NC[i][0] * Mm) + (NC[i][1] * Ms) + (NC[i][2] * F) + (NC[i][3] * D) + (NC[i][4] * OMEGA);  // arg already in radians.
      arg -= std::floor(arg / D2PI) * D2PI;     // normalise
      dDELTA_PSI += (NC[i][5] + NC[i][6] * dT) * std::sin(arg);
      DELTA_epsilon += (NC[i][7] + NC[i][8] * dT) * std::cos(arg);
    };
	
    dDELTA_PSI *= 0.0001 / 3600;  // Normalise and convert to degrees
    DELTA_epsilon *= 0.0001 / 3600;  // Normalise and convert to degrees
		
    //dDELTA_PSI -= floor(dDELTA_PSI/360)*360;
	
    //DELTA_epsilon -= floor(DELTA_epsilon/360)*360;

    dRA = Coords_2.RA() * DD2R;
    dDec = Coords_2.DEC() * DD2R;
    dDecErr = sin(dEpsilon) * cos(dRA) * dDELTA_PSI + sin(dRA) * DELTA_epsilon;
    Coords_3.SetDec(Coords_2.DEC() + dDecErr);
    dRAErr = (cos(dEpsilon) + sin(dEpsilon) * sin(dRA) * tan(dDec))*dDELTA_PSI;
    dRAErr -= cos(Coords_2.RA() * DD2R) * tan(dDec) * DELTA_epsilon;
    Coords_3.SetRA(Coords_2.RA() + dRAErr);

    enterCriticalSection();
    DELTA_PSI = dDELTA_PSI;
    leaveCriticalSection();
  }

// Calculate Precession
// Called by the low speed thread once per minute
//
// Input:		Coords_1
// Output:	Coords_2
// Class variables accessed:	T
// Class variables modified:	csT

  void TAstronomicalCalc::Calculate_Precession()
  {
    FP_t dT, dT2, dT3;
    FP_t dt, dt2, dt3;
    FP_t ZA, zeta_o, THETA, q;
    FP_t dRAErr;
    FP_t dDecErr;

    dt = 0;
    dt2 = dt * dt;
    dt3 = dt2 * dt;

    EnterCriticalSection(&csT);
    dT = T;
    LeaveCriticalSection(&csT);

    dT2 = dT * dT;
    dT3 = dT2 * dT;

    ZA = zeta_o = (2306.2181/3600+1.39656/3600*dT-0.000139/3600*dT2)*dt;
    zeta_o += (0.30188/3600-0.000344/3600*dT)*dt2+0.017998/3600*dt3;
    ZA += (1.09468/3600+0.000066/3600*dT)*dt2+0.018203/3600*dt3;
    THETA = (2004.3109/3600-0.8533*dT-0.000217/3600*dT2)*dt-(0.42665/3600+0.000217/3600*dT)*dt2-0.041833/3600*dt3;
    q = sin(THETA* DD2R)*(tan(Coords_1.DEC()* DD2R)+tan(THETA/2* DD2R)*cos((Coords_1.RA() + zeta_o)* DD2R));
    dRAErr = zeta_o + ZA + atan((q*sin((Coords_1.RA() + zeta_o)* DD2R))/(1-q*cos((Coords_1.RA() + zeta_o)* DD2R)));
    dDecErr = THETA * (cos((Coords_1.RA() + zeta_o)* DD2R)-sin((Coords_1.RA() + zeta_o)* DD2R)*tan((dRAErr-zeta_o-ZA)* DD2R));
    Coords_2.SetCoordinates(Coords_1.RA() + dRAErr, Coords_1.DEC() + dDecErr);
  }

// Calculates the Annual Abberation and the Stellar Parallax
// 
// Input: CurrObs.Coords
// Output: Coords_1
// Class variables accessed: T -
// Class variables modified:	Coords_1
//														csT - Critical section for T

  void TAstronomicalCalc::Calculate_AnnualAbberation()
  {
    FP_t dT, dT2, dT3;
    FP_t Lm, Lt;
    FP_t e, omega, v, pi;
    FP_t E, E2, R, X, Y;
    FP_t dRA, dDec, dRAO, dDecO, dRAErr, dDecErr;
    FP_t c, d, c_, d_, C, D, DELTA_C, DELTA_D;

    EnterCriticalSection(&csT);
    dT = T;
    LeaveCriticalSection(&csT);

    dT2 = MCL::pow2(dT);
    dT3 = dT2 * dT;
    Lm = 280+ 27/60 + 57.820/3600 + (360 * 100 + 46/60 + 11.270 / 3600) * dT + 1.089 * dT2; // 5.16m
    l_ = 1287099.804/3600 + (99 * 260 + 1292581.224 / 3600) * dT - 0.577 / 3600 * dT2 - 0.012 / 3600 * dT3;
    l_ -= std::floor(l_ / 360) * 360;
    e = 0.01670832 - 0.000042229 * dT + 1.26E-7 * dT3;
    E = l_ + e;
    E2 = 0;
    while (abs(E-E2) > 1e-10)
    {
      E2 = E;
      E = l_ + e * std::sin(E2* D_D2R);
    };
    omega = 102 + 26/60 + 18.046/3600 + (1 + 43/60 + 10.046 / 3600) * dT;

    EnterCriticalSection(&csCurrObs);
    if (CurrObs->Get_Distance() == 0)
    {
      pi = 0;
    }
    else
    {
      pi = (1/3600)/CurrObs->Get_Distance();
    }
    //dRAO = dRA = CurrObs->RA();
    //dDecO = dDec = CurrObs->DEC();
    LeaveCriticalSection(&csCurrObs);

    v=2 * std::atan(std::sqrt((1+e)/(1-e))*tan(E/2* D_D2R));
    Lt = Lm + v - l_;
    R = (1.0000002 * (1- e*e))/(1+e * std::cos(v* D_D2R));
    X = R * std::cos(Lt * D_D2R);
    epsilon = (84381.448/3600) + (46.8150 * dT) + (0.00059 * dT2) + (0.001813 * dT3);
    Y = R * std::sin(Lt * D_D2R) * std::cos(epsilon * D_D2R);
    dRA *= D_D2R;
    dDec *= D_D2R;
    c = std::cos(dRA) / std::cos(dDec);
    d = std::sin(dRA) / std::cos(dDec);
    d_ = std::cos(dRA) * std::sin(dDec);
    c_ = std::tan(epsilon * D_D2R) * std::cos(dDec) - std::sin(dRA) * std::sin(dDec);
    C = -20.49552/3600 * std::cos(epsilon* D_D2R) * cos(Lt* D_D2R);
    D = -20.49552/3600 * std::sin(Lt* D_D2R);
    dRAErr = (C+pi*Y)*c+(D-pi*X)*d;			// 5.21a
    dDecErr = (C+pi*Y)*c_-(D-pi*X)*d_;	// 5.21b
    DELTA_C = 20.49552/3600*e*cos(omega* D_D2R)*cos(epsilon* D_D2R);
    DELTA_D = 20.49552/3600*e*sin(omega);
    dRAErr += (c * DELTA_C) +  (d * DELTA_D);
    dDecErr = (c_*DELTA_C) + (d_*DELTA_D);
    Coords_1.SetCoordinates(dRAO + dRAErr, dDecO + dDecErr);
  }

// Performs all the time calculations.
// Called from ThreadProc_T
// First gets the time. (At this stage from on-board clock), later
// will follow to get time from on-board GPS receiver.
//
// Inputs: None
// Outputs: 
// Class variables accessed:
// Class variables modified:	csCJD	
//														CJD
//														csUTC
//														UTC
//														Date
//														
// 

  void CAstronomicalCalc::calculateTime()
  {
    static std::time_t currentTime;
    static struct tm *stUTC;

    currentTime = std::time();
    stUTC = std::gmtime(currentTime);

    EnterCriticalSection(&csCJD);
    CJD.SetJD(*stUTC);  // Set the julian day
    T = (CJD.JD0() - CJD(J2000)) / DC;  // Calculate T
    LeaveCriticalSection(&csCJD);

    EnterCriticalSection(&csGMSTo);
    GMSTo = 100.46061837 + (36000.770053608 * T) + (0.000387933 * T * T) - (T * T * T/38710000); // degrees.
    GMSTo -= std::floor(GMSTo/360)*360; // Normalise
    EnterCriticalSection(&csGASTo);
    GASTo = (GMSTo + (DELTA_PSI * cos(epsilon * DD2R)));  // degrees.
    GASTo -= std::floor(GASTo/360)*360; // Normalise
    LeaveCriticalSection(&csGMSTo);
    LeaveCriticalSection(&csGASTo);

    EnterCriticalSection(&csUTC);
    EnterCriticalSection(&csdUT1);
    EnterCriticalSection(&csUT1);
    UT1 = UTC + (dUT1/3600);	// dUT1 in seconds
    LeaveCriticalSection(&csdUT1);
    LeaveCriticalSection(&csUTC);
    EnterCriticalSection(&csGMST);
    EnterCriticalSection(&csGMSTo);
    GMST = (1.0027379093 * static_cast<FP_t>(UT1) * 15) + GMSTo; // Degrees
    LeaveCriticalSection(&csUT1);
    LeaveCriticalSection(&csGMSTo);
    GMST -= std::floor(GMST/360)*360; // Normalise
    LeaveCriticalSection(&csGMST);

    EnterCriticalSection(&csGAST);
    EnterCriticalSection(&csUT1);
    EnterCriticalSection(&csGASTo);
    GAST = (1.0027379093 * static_cast<FP_t>(UT1) * 15) + GASTo; // Degrees
    LeaveCriticalSection(&csUT1);
    LeaveCriticalSection(&csGASTo);
    GAST -= std::floor(GAST/360)*360;  // Normalise
    LeaveCriticalSection(&csGAST);

    EnterCriticalSection(&csLocation);
    EnterCriticalSection(&csLAST);
    EnterCriticalSection(&csGAST);
    LAST = GAST + Location.Longitude(); // Degrees
    LeaveCriticalSection(&csLocation);
    LeaveCriticalSection(&csGAST);
    LAST -= floor(LAST/360)*360; // Degrees
    LeaveCriticalSection(&csLAST);
  }

/// @brief Realtime calculation function.
/// This procedure calculates the time and hour angle.
/// @note The application that "owns" the CAstronomicalCalc instance must arrrange for this function to be called 100times/second.
// 
// 2016-01-14/GGB - Function created.

  void CAstronomicalCalc::realTimeFunction()
  {  
    calculateTime();
    calculate_h();
  }

/// @brief High priority calculation function.
/// Function for calculating the refraction
/// @note The application that "owns" the CAstronomicalCalc instance must arrange for this function to be called 20times/second.
//
/// @version 2016-01-14/GGB - Function created.

  void CAstronomicalCalc::highPriorityFunction()
  { 
    AC->Calculate_Refraction();
  }

// Medium priority thread procedure 
// Thread procedure for calculating the time
/// @note The application that "owns" the CAstronomicalCalc instance must arrange for this function to be called every 5 seconds.
//

  void CAstronomicalCalc::mediumPriorityFunction()
  {
    static int nLoopCount = 1; // Counts the number of times the routine has run through (1-2);
	  
    switch (nLoopCount)
    {
      case 1:
        AC->Calculate_DiurnalAbberation();
        nLoopCount = 2;
        break;
      case 2:
        AC->Calculate_PlanetaryParallax();
        nLoopCount = 1;
        break;
      default:
        nLoopCount = 1;
        break;
    };
  }

// Low frequency thread procedure 
// Thread procedure called every 1 minute.
// A different set of calculations is performed every minute, rather than performing all the calculations every five minutes.
// This will help keep processor loading more constant.
// Calculations Performed:	Annual Abberation + Stellar Parallax
//													Precession	
//													Nutation
/// @version 2016-01-14/GGB - Function created.

  void CAstronomicalCalc::lowPriorityFunction()
  {
    static int nLoopCount = 1; // Counts the number of times the routine has run through (1-5);
	  
    switch (nLoopCount)
    {
      case 1:
        AC->Calculate_AnnualAbberation();
        nLoopCount++;
        break;
      case 2:
        AC->Calculate_Precession();
        nLoopCount++;
        break;
      case 3:
        AC->Calculate_Nutation();
        nLoopCount++;
        break;
      case 4:
        AC->Calculate_OrbitalMotion();
        nLoopCount++;
        break;
      case 5:
        AC->Calculate_ProperMotion();
        nLoopCount = 1;
        break;
      default:
        nLoopCount = 1;
        break;
    };
  }

// Calculates Greenwich apparent sidereal time at 0h - GASTo
// Equation: 5.14c Page 77
//
// Input: None
// Output: None
// Class variables accessed:	DPhi - 
//														eta - Mean obliquity of the ecliptic (degrees)
//														GMSTo - Greenwich mean sidereal time at 0h. (degrees)
// Class variables changed:		csGMSTo - Critical section for GMSTo
//														csGASTo - Critical secion for GASTo
//														GASTo - Greenwich apparent sidereal time (H:M:S)
// Multi-thread safe.

  void TAstronomicalCalc::Calculate_GASTo()
  {
    EnterCriticalSection(&csGMSTo);
    EnterCriticalSection(&csGASTo);
    GASTo = (GMSTo + (DELTA_PSI*cos(epsilon * DD2R)))/15;
    LeaveCriticalSection(&csGMSTo);
    LeaveCriticalSection(&csGASTo);
  }

// Calculates h - hour angle
// Equation: 5.13a Page 76
//
// Input: None
// Output: None
// Class variables accessed:	csLAST - Critical section for the LAST
//														LAST - Local apparent sidereal time (H:M:S) (normalised)
//														SC.RA - Right Ascention (H:M:S)
// 
// Multi-thread safe

  inline void TAstronomicalCalc::Calculate_h()
  {
    EnterCriticalSection(&csLAST);
    EnterCriticalSection(&csCoords_t);
    EnterCriticalSection(&csh);
    h = LAST - Coords_t.RA() * 15;
    LeaveCriticalSection(&csCoords_t);
    LeaveCriticalSection(&csLAST);
    h -= std::floorfloor(h/360)*360;  // Normalise
    LeaveCriticalSection(&csh);
  }

// Sets the location
// Multi-thread safe

  void TAstronomicalCalc::Set_Location(TLocation const &newLocation)
  {
    EnterCriticalSection(&csLocation);
    Location = newLocation;
    LeaveCriticalSection(&csLocation);
  }

// Returns the current location
// 
// Multi-thread safe
//
// 2005-09-02/GGB - Function created.

  void TAstronomicalCalc::Get_Location(TLocation &toCopy)
  {
    EnterCriticalSection(&csLocation);
    toCopy = Location;
    LeaveCriticalSection(&csLocation);
  }

// Calculates the apparent place of the current object.
//
// 2005-09-02/GGB - Function created.

  void TAstronomicalCalc::Calculate_ApparentPlace()
  {
    Calculate_AnnualAbberation();
    Calculate_Precession();
    Calculate_Nutation();
    Calculate_OrbitalMotion();
    Calculate_ProperMotion();
    Calculate_DiurnalAbberation();
    Calculate_PlanetaryParallax();
    Calculate_Refraction();
  }

/// @brief Sets the stellar coordinates of a new object.
/// Forces a recalculation of the apparent place of the object.
/// @note Multi-thread safe
//
/// @version 2016-01-14/GGB - Updated code to enable use with embedded PIC32 system.
/// @version 2005-08-20/GGB - Function created.

  void CAstronomicalCalc::setTarget(CTargetAstronomy *newTarget)
  {
    enterCriticalSection();
    currentTarget = newTarget;
    leaveCriticalSection();
    calculateApparentPlace();
  }

// Called to set the value of dUT1
//
// Multi-thread safe
//
// Written by:	Gavin Blakeman
// Date:				20 August 2005
// Revision:		1.00

  void TAstronomicalCalc::Set_dUT1(FP_t const dNewdUT1)
  {
    EnterCriticalSection(&csdUT1);
    dUT1 = dNewdUT1;
    LeaveCriticalSection(&csdUT1);
  }

// Returns the value of dUT1.
//
// Multi-thread safe.
//
// Written by:	Gavin Blakeman
// Date:				20 August 2005
// Revision:		1.00

  FP_t TAstronomicalCalc::Get_dUT1()
  {
    FP_t dRetVal;
    EnterCriticalSection(&csdUT1);
    dRetVal = dUT1;
    LeaveCriticalSection(&csdUT1);
    return dRetVal;
  }

// Input:		Coords_3
// Output:	Coords_4

  void TAstronomicalCalc::Calculate_OrbitalMotion()
  {
    Coords_4 = Coords_3;
  }

// Input:		Coords_4
// Output:	Coords_5

  void TAstronomicalCalc::Calculate_ProperMotion()
  {
    EnterCriticalSection(&csCoords_5);
    Coords_5 = Coords_4;
    LeaveCriticalSection(&csCoords_5);
  }

/// @brief This procedure obtains the current value for dUT1. This is achieved by reading through a IERS BULLETIN - A file to obtain
/// the value of dUT1 from the predictions made in the Bulletin. If no bulletin A is found in the data directory an error is
/// displayed and the user prompted to enter a value for dUT1.
//
// INPUTS:			char *szBullA - Path and Name of the Bulletin A file. This is to enable the dUT1 value to be obtained
//															from the Bulletin A file.
//
// 2005-07-31/GGB - Function created.

  void TAstronomicalCalc::Obtain_dUT1(char *szBulletinA)
  {
    FILE *fFile = NULL;
    char szData[1024];
    TJD MJD;
    SYSTEMTIME stUTC;
    double dFJD;
    double ddUT1 = 0.0;
    double dTT = 0.0;
    double dTAI = 0.0;

    GetSystemTime(&stUTC);
    MJD.SetJD(&stUTC);
    MJD = MJD - 2400000.5;
    MJD.floor();

    if (fopen_s(&fFile, szBulletinA, "r") != 0)
    {
      // an error has occurred as the file was not found.
    }
    else
    {
      // Start reading through the file.
      // The first information that will be come across is "TT = TAI + 32.184 seconds"
      // The second information will be TAI-UTC(BIPM) = xx.000 000 seconds
      // Third informaiton will be dUT1.
      do
        fscanf_s(fFile, "%s", szData, sizeof(szData));
      while ( (strcmp(szData, "TT") != 0) );
      fscanf_s(fFile, " = TAI + %lf", &TT_TAI, sizeof(TT_TAI));
				
      do
        fscanf_s(fFile, "%s", szData, sizeof(szData));
      while ( (strcmp(szData, "TAI-UTC(BIPM)") != 0) );
      fscanf_s(fFile, " = %lf", &TAI_UTC, sizeof(TAI_UTC));

  //29/8/2009				Ephemeris->Set_deltaAt(dTAI + dTT);

		do
			fscanf_s(fFile, "%s", szData, sizeof(szData));
		while ( (strcmp(szData, "UT1-UTC(sec)") != 0) && (feof(fFile) == 0) );
		if (feof(fFile) != 0)
		{
			// Error has occurred
		}
		else
		{
			do
			{
				fscanf_s(fFile, "%s", szData, sizeof(szData));	// year
				fscanf_s(fFile, "%s", szData, sizeof(szData));  // month
				fscanf_s(fFile, "%s", szData, sizeof(szData));	// day
				fscanf_s(fFile, "%lf", &dFJD, sizeof(dFJD));			// MJD
				fscanf_s(fFile, "%s", szData, sizeof(szData));	// x
				fscanf_s(fFile, "%s", szData, sizeof(szData));	// y
				fscanf_s(fFile, "%lf", &ddUT1, sizeof(ddUT1));
			}
			while ( (dFJD != MJD) && (feof(fFile) == 0) );
			if (feof(fFile) != 0)
			{
				// Error
			}
			else
			{
				dUT1 = ddUT1;
			};
		};
	};
	
	if (fFile != NULL)
		fclose(fFile);
  }

}   // namespace ACL
