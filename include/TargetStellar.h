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
//                      Copyright 2005-2018 Gavin Blakeman.
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
// CLASS HIERARCHY:     CTargetAstronomy
//                        - CTargetStellar
//                        - CTargetSolar
//                          - CTargetMinorPlanet
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-21 GGB - astroManager 0000.00 release.
//                      2011-06-30 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_TARGETSTELLAR_H
#define ACL_TARGETSTELLAR_H

  // Standard C++ library header files

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

  // ACL Library header files.

#include "AstronomicalCoordinates.h"
#include "TargetAstronomy.h"
#include "AstronomicalTime.h"
#include "config.h"
#include "Photometry.h"

  // Miscellaneous library header files.

#include <MCL>
#include <PCL>

namespace ACL
{

  // Define the library to use for determining stellar object positions.

#if !defined(USE_SOFA) && !defined(USE_NOVAS)
# define USE_SOFA
#endif

  /// @brief This class is used to store stellar objects and the associated data.
  /// The class also handles the reduction of the objects position to an observed position.
  /// Stellar objects are extra-solar (outside the solar system) and that generally do not display large amount of motion. This
  /// includes extra-galactic objects, but does not include exo-planets.
  ///
  /// Determination of the observed position of stellar objects is done by applying corrections for observation epoch, space
  /// motion, refraction etc. This is in contrast to solar objects where the position is determined by using an Ephemeris. The
  /// stellar object class includes the code necessary to reduce the catalog position to a observed position.


  ////*******************************************************************************************************************************
  //
  // Chain of astrometric transformations (Referenced from sofa_ast_c.pdf)
  //
  //                  ***********************************************************
  //                  *                                                         *
  //                  *  ICRS (RA, DEC, pmRA, pmDEC, parallax, radialVelocity)  *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                           space motion
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                     BCRS (RA, DEC, r)                   *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                            parallax
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                   astrometric (RA, DEC)                 *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                          light deflection
  //                                            aberration
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                       GCRS (RA, DEC)                    *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                            frame bias
  //                                          precession nutation
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                           CIRS                          *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                        Earth rotation angle
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                           TIRS                          *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                           polar motion
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                           ITRS                          *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                            longitude
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                        local apparent                   *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                  diurnal abberation and parallax
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                     topocentric (h, dec)                *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                            latitude
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                     topocentric (alt, az)               *
  //                  *                                                         *
  //                  ***********************************************************
  //                                                |
  //                                            refraction
  //                                                |
  //                  ***********************************************************
  //                  *                                                         *
  //                  *                        observed (alt, az)               *
  //                  *                                                         *
  //                  ***********************************************************
  //
  //*******************************************************************************************************************************
  //
  // Catalog Coordinates: ICRS      (ICRS, FK5, FK4 etc)
  // Current Coordinates:
  // Observed Coordinates:  Direction to point a telescope.
  // SOFA function iauAtco13: ICRS->observed
  //
  // iauApco13: Prepare star-independant astrometry parameters for transformations between ICRS and observed coordinates.
  //      Inputs: UTC:  use function iadDtf2d to convert from calendar date to julian date.
  //              dUT1: UT1-UTC (from IERS bulletins)
  //              elong: longitude
  //              phi: latitude
  //              hm: height above ellipsoid
  //              xp, yp: Polar motion (IERS)
  //              phpa: Pressure at observer
  //              tc: Temperature at observer
  //              rh: Relative humidity at observer
  //              wl: wavelength
  //      Returned:
  //              astrom: star-independant astronomy parameters.
  //              pmt: PM time interval
  //              eb: SSB to observer
  //              eh: Sun to observer
  //              em: Distance from sun to observer
  //              v: barycentric observer velocity
  //              bm1: Reciprical of lorenz factor.
  //              bpn: Bias nutation matrix.
  //              along: longiture+s'
  //              xpl: polar motion
  //              ypl: polar motion
  //              sphi: sine of latitude
  //              cphi: cosine of latitude
  //              diurab: magnitude of diurnal abberation vector
  //              eral: local earth rotation angle
  //              refa: refraction constant A
  //              refb: refraction constant B
  //              eo: Equation of origins.

  class CTargetStellar : public CTargetAstronomy
  {
  public:
      // The possible reference systems that may be used for specifying a stellar objects position.

    enum EReferenceSystem
    {
      RS_NONE,      ///< No reference system specified
      RS_ICRS,      ///< Using the ICRS reference system. This is the system in use today.
      RS_FK4,       ///< Using the FK4 reference system. This is only used in older catalogs.
      RS_FK5        ///< Using the FK5 reference system. This is used in more modern catalogs.
    };

  private:
  protected:
    CAstronomicalCoordinates catalogCoordinates_;     ///< Catalog Coordinates of the object. (Stored as ICRS)
    TJD catalogEquinox_;                              ///< JD of coordinate equinox.
    std::optional<FP_t> pmRA_;                        ///< Proper Motion RA (mas)
    std::optional<FP_t> pmDec_;                       ///< Proper Motion DEC (mas)
    std::optional<FP_t> radialVelocity_;              ///< Radial velocity (km/s)
    std::optional<FP_t> parallax_;                    ///< The parallax (distance) of the object
    TJD epoch_;                                       ///< JD of epoch
    std::string spectralType_;                        ///< Spectral type of the object.
    std::vector<std::string> identifiers_;            ///< Alternate identifiers for the object.
    std::vector<CPhotometryMeasurement> photometry_;  ///< Any photometry measurements associated with the object
    std::uint64_t oid_;                               ///< SIMBAD OID
    std::string objectType_;

  public:
    explicit CTargetStellar();        // void form of the constructor.
    CTargetStellar(const CTargetStellar &);
    CTargetStellar(std::string const &, CAstronomicalCoordinates const &);
    CTargetStellar(std::string const &, CAstronomicalCoordinates const &, FP_t, FP_t, FP_t, FP_t, FP_t);

    virtual ~CTargetStellar();

      // Factory functions

    virtual std::unique_ptr<CTargetAstronomy> createCopy() const;

      // Information functions

    virtual std::string objectType() const;

      // Operator functions

    virtual CTargetStellar &operator=(const CTargetStellar &);

      // Setting functions

    virtual void objectType(std::string const &ot) { objectType_ = ot; }
    virtual void catalogueCoordinates(CAstronomicalCoordinates, EReferenceSystem = RS_ICRS);
    virtual void setEpoch(std::string const &);
    inline virtual void setEpoch(FP_t ne) {epoch_ = ne;}  // Use a JD to set the epoch
    virtual void setEpoch(TJD const &);
    inline virtual void pm(FP_t dPMRA, FP_t dPMDEC) {pmRA_ = dPMRA; pmDec_ = dPMDEC;}
    inline virtual void pmRA(FP_t pmra) {pmRA_ = pmra;}
    inline virtual void pmDec(FP_t pmdec) { pmDec_ = pmdec;}
    inline void radialVelocity(FP_t RV) {radialVelocity_ = RV;}
    inline void parallax(FP_t dP) { parallax_ = dP;}

      // Getting functions

    virtual CAstronomicalCoordinates catalogueCoordinates() const { return catalogCoordinates_; }
    inline virtual FP_t pmRA() const { return *pmRA_; }
    inline virtual FP_t pmDec() const { return *pmDec_; }
    inline virtual FP_t radialVelocity() const { return *radialVelocity_; }
    inline FP_t parallax() const { return (*parallax_); }
    FP_t getDistance();

    CAstronomicalCoordinates properMotion(TJD const &) const;
    CAstronomicalCoordinates properMotion(FP_t) const;

    virtual std::string catalogSystemString();

      // Catalog functions

    virtual TJD const &catalogEpoch() { return epoch_; }
    virtual CAstronomicalCoordinates &catalogPlace() { return catalogCoordinates_; }
    virtual FP_t catalogPMRA() { return *pmRA_; }
    virtual FP_t catalogPMDEC() { return *pmDec_; }
    virtual FP_t catalogRadialVelocity() { return *radialVelocity_; }
    virtual FP_t catalogParallax() { return *parallax_; }

      // Posotion Functions

    virtual CAstronomicalCoordinates positionICRS(CAstroTime const &) const {}
    virtual SObservedPlace positionObserved(CAstroTime const &, CGeographicLocation const &, CWeather const *);
  };

}  // namespace ACL

#endif // ACL_TARGETSTELLAR_H
