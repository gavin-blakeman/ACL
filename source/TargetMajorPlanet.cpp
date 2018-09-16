//*********************************************************************************************************************************
//
// PROJECT:							ACL
// FILE:								TargetMajorPlanet
// SUBSYSTEM:						Major Planet Positions and Calculations
// LANGUAGE:						C++
// TARGET OS:						NONE
// LIBRARY DEPENDANCE:  MCL
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2016-2018 Gavin Blakeman.
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
// OVERVIEW:						These classes provide methods of handling planets
//
// CLASSES INCLUDED:    CTargetMajorPlanet - Class for managing planets.
//
// CLASS HIERARCHY:     CTargetAstronomy
//                        CTargetStellar
//                        CTargetMinorPlanet
//                        CTargetPlanet
//
// HISTORY:             2016-03-25 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../include/TargetMajorPlanet.h"

  // Standard C++ library header files.


  // ACL header files.

#include "../include/AstroFunctions.h"
#include "../include/constants.h"
#include "../include/error.h"

  // Miscellaneous library header files.

#include <MCL>

namespace ACL
{

  struct SKeplerianElements
  {
    double a0, adot;
    double e0, edot;
    double I0, Idot;
    double L0, Ldot;
    double wbar0, wbardot;
    double W0, Wdot;
  };

  static SKeplerianElements kelperianElements[] =
  {
    {  0.38709927,  0.00000037, 0.20563593,  0.00001906,  7.00497902, -0.00594749, 252.25032350, 149472.67411175,  77.45779628,  0.16047689,  48.33076593, -0.12534081},  // Mercury
    {  0.72333566,  0.00000390, 0.00677672, -0.00004107,  3.39467605, -0.00078890, 181.97909950,  58517.81538729, 131.60246718,  0.00268329,  76.67984255, -0.27769418},  // Venus
    {  1.52371034,  0.00001847, 0.09339410,  0.00007882,  1.84969142, -0.00813131,  -4.55343205,  19140.30268499, -23.94362959,  0.44441088,  49.55953891, -0.29257343},  // Mars
    {  5.20288700, -0.00011607, 0.04838624, -0.00013253,  1.30439695, -0.00183714,  34.39644051,   3034.74612775,  14.72847983,  0.21252668, 100.47390909,  0.20469106},  // Jupiter
    {  9.53667594, -0.00125060, 0.05386179, -0.00050991,  2.48599187,  0.00193609,  49.95424423,   1222.49362201,  92.59887831, -0.41897216, 113.66242448, -0.28867794},  // Saturn
    { 19.18916464, -0.00196176, 0.04725744, -0.00004397,  0.77263783, -0.00242939, 313.23810451,    428.48202785, 170.95427630,  0.40805281,  74.01692503,  0.04240589},  // Uranus
    { 30.06992276,  0.00026291, 0.00859048,  0.00005105,  1.77004347,  0.00035372, -55.12002969,    218.45945325,  44.96476227, -0.32241464, 131.78422574, -0.00508664},  // Neptune
    { 39.48211675, -0.00031596, 0.24882730,  0.00005170, 17.14001206,  0.00004818, 238.92903833,    145.20780515, 224.06891629, -0.04062942, 110.30393684, -0.01183482}   // Pluto
  };


  /// @brief Constructor for class. Automatically sets the name based on the object that is passed.
  /// @param[in] np: Planet name
  /// @throws std::bad_alloc
  /// @version 2018-08-22/GGB - Function created.

  CTargetMajorPlanet::CTargetMajorPlanet(EPlanets const &np) : CTargetAstronomy(""), planet(np)
  {
    switch (np)
    {
      case Mercury:
      {
        objectName("Mercury");
        break;
      };
      case Venus:
      {
        objectName("Venus");
        break;
      };
      case Mars:
      {
        objectName("Mars");
        break;
      };
      case Jupiter:
      {
        objectName("Jupiter");
        break;
      };
      case Saturn:
      {
        objectName("Saturn");
        break;
      };
      case Uranus:
      {
        objectName("Uranus");
        break;
      };
      case Neptune:
      {
        objectName("Neptune");
        break;
      };
      case Pluto:{
        objectName("Pluto");
        break;
      };
      default:
      {
        ACL_CODE_ERROR;
      }
    }
  }

  /// @brief Copy constructor.
  /// @param[in] toCopy: Instance to copy.
  /// @throws std::bad_alloc
  /// @version 2018-09-15/GGB - Function created.

  CTargetMajorPlanet::CTargetMajorPlanet(CTargetMajorPlanet const &toCopy) : CTargetAstronomy(toCopy), planet(toCopy.planet)
  {
  }

  /// @brief Creates a copy of this instance.
  /// @returns Pointer to a new copy.
  /// @throws std::bad_alloc
  /// @version 2018-09-15/GGB - Function created.

  std::unique_ptr<CTargetAstronomy> CTargetMajorPlanet::createCopy() const
  {
    std::unique_ptr<CTargetAstronomy> returnValue(new CTargetMajorPlanet(*this));

    return returnValue;
  }

  /// @brief Returns the coordinates of the object at the date/time specified.
  /// @param[in] Teph: The date/time to return the catalogueCoordinates.
  /// @returns
  /// @details Use the method given in aprx_pos_planets.pdf (See documentation) published on the JPL website. (E M Standish)
  ///          The values for the period 1800AD - 2050AD are used. Referenced to the mead ecliptic and equinox of J2000.0
  /// @throws ACL, 0x2600 - Keplers equation fails to converge.
  /// @version 2018-08-22/GGB - Function created.

  CAstronomicalCoordinates CTargetMajorPlanet::positionICRS(CAstroTime const &Teph) const
  {
    double T;
    double a, e, I, L, wbar, W;
    double w, M;
    double E;
    double xdash, ydash, zdash;
    double xecl, yecl, zecl;
    double xeq, yeq, zeq;

    double static eta = 23.43928 * D_D2R;
    double static sineta = std::sin(eta);
    double static coseta = std::cos(eta);

      // Step 1. Compute the value of each of the planets six elements.
      //         element = element0 + elementdot x T
      //         T = number of centuries past J2000.0

    T = (static_cast<double>(Teph.TT()) - static_cast<double>(J2000)) / 36525;

      /// @todo This may be able to be done using a valarray for speedup.

    a = kelperianElements[planet].a0 + kelperianElements[planet].adot * T;
    e = kelperianElements[planet].e0 + kelperianElements[planet].edot * T;
    I = kelperianElements[planet].I0 + kelperianElements[planet].Idot * T;
    L = kelperianElements[planet].L0 + kelperianElements[planet].Ldot * T;
    wbar = kelperianElements[planet].wbar0 + kelperianElements[planet].wbardot * T;
    W = kelperianElements[planet].W0 + kelperianElements[planet].Wdot * T;

      // Step 2. Compute the argument of perihelion, w, and the mean anamoly, M.

    w = wbar - W;
    M = L - wbar;

      // Step 3. Modulus the mean anomoly so that -180degrees <= M <= +180degrees and then obtain the eccentric anomoly
      //         E, from the solution of Kepler's equation M = e*sinE, where e* = 180/PIe = 57.29578e.

    E = keplersEquation(M, e);

      // Step 4. Compute the planet's heliocentric coordinates in it's orbital plane rdash with the xdash axis aligned from the
      //         focus to the perihelion

    xdash = a * (std::cos(E) - e);
    ydash = a * std::sqrt(1 - MCL::pow2(e)) * std::sin(E);
    zdash = 0;

      // Step 5. Compute the coordinates recl in the J2000 ecliptic plane with the x-axis aligned toward the equinox

    double cosw = std::cos(MCL::D2R(w));
    double sinw = std::sin(MCL::D2R(w));
    double cosW = std::cos(MCL::D2R(W));
    double sinW = std::sin(MCL::D2R(W));
    double cosI = std::cos(MCL::D2R(I));
    double sinI = std::sin(MCL::D2R(I));

    xecl = (cosw * cosW - sinw * sinW * cosI) * xdash;
    xecl += (-sinw * cosW - cosw * sinW * cosI) * ydash;

    yecl = (cosw * sinW - sinw * cosW * cosI) * xdash;
    yecl += (-sinw * sinW + cosw * cosW * cosI) * ydash;

    zecl = sinw * sinI * xdash + cosw * sinI * ydash;

      // Step 6. Convert to ICRF frame.

    xeq = xecl;
    yeq = coseta * yecl - sineta * zecl;
    zeq = sineta * yecl + coseta * zecl;

    return CAstronomicalCoordinates(MCL::TVector3D<FP_t>(xeq, yeq, zeq));

  }


} // namespace ACL

