//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSStrings
// SUBSYSTEM:						FITS strings and helper classes
// LANGUAGE:						C++
// TARGET OS:						none
// LIBRARY DEPENDANCE:	none
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2011-2017 Gavin Blakeman.
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
// OVERVIEW:						Implements classes for interfacing to FITS files. This file does not overlap the FITSIO
//                      library, but provides additional support for these libraries.
//                      This file includes strings for FITS headerss. This can be keywords as well as standard comments.
//
// CLASSES INCLUDED:    none
//
// CLASS HIERARCHY:     none
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-03-22 GGB - astroManager 2013.03 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2011-07-14 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#ifndef ACL_FITSSTRINGS_H
#define ACL_FITSSTRINGS_H

  // Standard C++ library header files

#include <string>

/// @file
/// This file contains string constants used for writing and reading data from FITS files.
/// This includes the standard defined keywords, as well as keywords that have been identified as informal standards from an
/// information survey.

namespace ACL
{

  //****************************************************************************

    // Some defines

  int const FITSLEN_DATETIME(22);         /* The length of a date time keyword */
  char const FITS_STRINGQUOTE(0x27);      /* The single quote character */

  //***************************************************************************

    // FITS standard keywords (keywords defined in the FITS standard.

    // Header Information

  std::string const FITS_SIMPLE                   ("SIMPLE");
  std::string const FITS_XTENSION                 ("XTENSION");
  std::string const FITS_EXTNAME                  ("EXTNAME");
  std::string const FITS_EXTVER                   ("EXTVER");
  std::string const FITS_EXTLEVEL                 ("EXTLEVEL");

    // Standard information

  std::string const FITS_NAXIS                    ("NAXIS");
  std::string const FITS_NAXIS1                   ("NAXIS1");
  std::string const FITS_NAXIS2                   ("NAXIS2");
  std::string const FITS_BITPIX                   ("BITPIX");
  std::string const FITS_GCOUNT                   ("GCOUNT");
  std::string const FITS_PCOUNT                   ("PCOUNT");

    // General descriptive keywords

  std::string const FITS_DATE                     ("DATE");
  std::string const FITS_ORIGIN                   ("ORIGIN");
  std::string const FITS_EXTEND                   ("EXTEND");
  std::string const FITS_BLOCKED                  ("BLOCKED");

    // Keywords describing observations

  std::string const FITS_DATEOBS                  ("DATE-OBS");
  std::string const FITS_MJDOBS                   ("MJD_OBS");
  std::string const FITS_TELESCOPE                ("TELESCOP");
  std::string const FITS_INSTRUMENT               ("INSTRUME");
  std::string const FITS_OBSERVER                 ("OBSERVER");
  std::string const FITS_OBJECT                   ("OBJECT");
  std::string const FITS_TIMESYS                  ("TIMESYS");
  std::string const FITS_EQUINOX                  ("EQUINOX");
  std::string const FITS_RADECSYS                 ("RADECSYS");
  std::string const FITS_EPOCH                    ("EPOCH");

    // Bibliographic keywords

  std::string const FITS_AUTHOR                   ("AUTHOR");
  std::string const FITS_REFERENCE                ("REFERENC");

    // Commentary keywords

  std::string const FITS_COMMENT                  ("COMMENT");
  std::string const FITS_HISTORY                  ("HISTORY");

    // Keywords that describe arrays

  std::string const FITS_BSCALE                   ("BSCALE");
  std::string const FITS_BZERO                    ("BZERO");
  std::string const FITS_BUNIT                    ("BUNIT");
  std::string const FITS_BLANK                    ("BLANK");
  std::string const FITS_DATAMAX                  ("DATAMAX");
  std::string const FITS_DATAMIN                  ("DATAMIN");

    // Keywords defining WCS

  std::string const FITS_WCS_CTYPE1         ("CTYPE1");
  std::string const FITS_WCS_CTYPE2         ("CTYPE2");
  std::string const FITS_WCS_CRPIX1         ("CRPIX1");
  std::string const FITS_WCS_CRPIX2         ("CRPIX2");
  std::string const FITS_WCS_CRVAL1         ("CRVAL1");
  std::string const FITS_WCS_CRVAL2         ("CRVAL2");
  std::string const FITS_WCS_CDELT1         ("CDELT1");
  std::string const FITS_WCS_CDELT2         ("CDELT2");
  std::string const FITS_WCS_CROTA          ("CROTA");
  std::string const FITS_WCS_CD             ("CD");
  std::string const FITS_WCS_CD1_1          ("CD1_1");
  std::string const FITS_WCS_CD1_2          ("CD1_2");
  std::string const FITS_WCS_CD2_1          ("CD2_1");
  std::string const FITS_WCS_CD2_2          ("CD2_2");
  std::string const FITS_WCS_PC             ("PC");
  std::string const FITS_WCS_PV             ("PV");
  std::string const FITS_WCS_LONPOLE        ("LONPOLE");
  std::string const FITS_WCS_LATPOLE        ("LATPOLE");
  std::string const FITS_WCS_WCSAXES        ("WCSAXES");
  std::string const FITS_WCS_CUNIT          ("CUNIT");
  std::string const FITS_WCS_CRDER          ("CRDER");
  std::string const FITS_WCS_CSYER          ("CSYER");
  std::string const FITS_WCS_WCSNAME        ("WCSNAME");

  //********************************************************************************************************************************
  // Other keywords
  //
  // Additional keywords defined that do not appear in FITS standard.
  //********************************************************************************************************************************

    // Location information

  std::string const NOAO_OBSERVATORY                    ("OBSERVAT");

  std::string const UCOLICK_LATITUDE                    ("LATITUDE");
  std::string const SBIG_LATITUDE                       ("SITELAT");
  std::string const MAXIM_LATITUDE                      ("LAT-OBS");

  std::string const UCOLICK_LONGITUDE                   ("LONGITUD");
  std::string const SBIG_LONGITUDE                      ("SITELONG");
  std::string const MAXIM_LONGITUDE                     ("LONG-OBS");

  std::string const ASTROMANAGER_ALTITUDE               ("ALTITUDE");
  std::string const MAXIM_ALTITUDE                      ("ALT-OBS");

    // Date/Time information

  std::string const ASTROMANAGER_JD                     ("JD");
  std::string const ASTROMANAGER_DATEHP                 ("DATE-HP");
  std::string const ASTROMANAGER_TIME                   ("TIME-OBS");
  std::string const ASTROMANAGER_UT                     ("UT");

    // Weather information

  std::string const NOAO_ENVTIME                        ("ENVMJD");
  std::string const NOAO_ENVTEM                         ("ENVTEM");
  std::string const NOAO_ENVPRE                         ("ENVPRE");
  std::string const NOAO_ENVHUM                         ("ENVHUM");

  std::string const MAXIM_BOLTAMBT                      ("BOLTAMBT");     // Boltwood Ambient temperature degrees C
  std::string const MAXIM_BOLTHUM                       ("BOLTHUM");      // Boltwod Humidity %

  std::string const MAXIM_DAVAMBT                       ("DAVAMBT");      // Davis Ambient temperature deg C
  std::string const MAXIM_DAVHUM                        ("DAVHUM");       // Davis Humidity in percent
  std::string const MAXIM_DAVBAROM                      ("DAVBAROM");     // Davis Ambient Pressure in hPa

  std::string const ASTROMANAGER_AMBTEM                 ("AMBTEM");       // Ambient temperature deg C
  std::string const ASTROMANAGER_AMBPRE                 ("AMBPRE");       // Ambient pressure hPa
  std::string const ASTROMANAGER_AMBHUM                 ("AMBHUM");       // Ambient humidity %

    // Pixel information

  std::string const SBIG_PIXELSIZEX                     ("XPIXSZ");
  std::string const SBIG_PIXELSIZEY                     ("YPIXSZ");
  std::string const SBIG_XBINNING                       ("XBINNING");
  std::string const SBIG_YBINNING                       ("YBINNING");

    // Exposure information

  std::string const NOAO_EXPTIME                        ("EXPTIME");
  std::string const HEASARC_EXPOSURE                    ("EXPOSURE");
  std::string const SBIG_CCDTEMP                        ("CCD-TEMP");
  std::string const HEASARC_FILTER                      ("FILTER");
  std::string const SBIG_EGAIN                          ("EGAIN");

    // Telescope information

  std::string const SBIG_FOCALLEN                       ("FOCALLEN");
  std::string const SBIG_APTDIA                         ("APTDIA");
  std::string const SBIG_APTAREA                        ("APTAREA");
  std::string const ITELESCOPE_TELESCOPE                ("iTELESCOPE");

    // General information

  std::string const HEASARC_CREATOR                     ("CREATOR");
  std::string const ASTROMANAGER_UUID                   ("UUID");

    // Display information

  std::string const SBIG_CWHITE                         ("CWHITE");
  std::string const SBIG_CBLACK                         ("CBLACK");

    // Other known and used keywords

  std::string const ASTROMANAGER_ZMAG                   ("ZMAG");
  std::string const SBIG_PEDESTAL                       ("PEDESTAL");

    // libAstrometry information

  std::string const LA_PPMXL                            ("PPMXL");
  std::string const LA_ANFILE                           ("AN-FILE");
  std::string const LA_HEALPIX                          ("HEALPIX");
  std::string const LA_NSTART                           ("NSTARS");
  std::string const LA_HPNSIDE                          ("HPNSIDE");
  std::string const LA_NSIDE                            ("NSIDE");
  std::string const LA_ALLSKY                           ("ALLSKY");

    // Photometry

  std::string const astroManager_FWHM                   ("FWHM");

    // Maxim keywords

  std::string const MAXIM_FLIPSTAT                      ("FLIPSTAT");
  std::string const MAXIM_SET_TEMP                      ("SET-TEMP");
  std::string const MAXIM_CCD_TEMP                      ("CCD-TEMP");
  std::string const MAXIM_CALSTAT                       ("CALSTAT");
  std::string const MAXIM_RA                            ("RA");
  std::string const MAXIM_OBJECTRA                      ("OBJCTRA");
  std::string const MAXIM_DEC                           ("DEC");
  std::string const MAXIM_OBJECTDEC                     ("OBJDCDEC");
  std::string const MAXIM_SWMODIFY                      ("SWMODIFY");
  std::string const MAXIM_SWCREATE                      ("SWCREATE");
  std::string const MAXIM_SWOWNER                       ("SWOWNER");
  std::string const MAXIM_XORGSUBF                      ("XORGSUBF");
  std::string const MAXIM_YORGSUBF                      ("YORGSUBF");
  std::string const MAXIM_POSITIONANGLE                 ("PA");
  std::string const MAXIM_PIERSIDE                      ("PIERSIDE");
  std::string const MAXIM_CLRBAND                       ("CLRBAND");

    // SBIG Keywords

  std::string const SBIG_SBSTDVER                       ("SBSTDVER");
  std::string const SBIG_IMAGETYP                       ("IMAGETYP");
  std::string const SBIG_AIRMASS                        ("AIRMASS");

  //*******************************************************************************************************************************

    // Pre-determined comments

  std::string const FITS_COMMENT_SIMPLE_TRUE            ("File does conform to FITS standard");
  std::string const FITS_COMMENT_SIMPLE_FALSE           ("File does not conform to FITS standard");

  std::string const FITS_COMMENT_XTENSION               ("The type of extension");

  std::string const FITS_COMMENT_DATEOBS                ("Date/Time of the observation");
  std::string const FITS_COMMENT_DATE                   ("The date on which the HDU was created.");

  std::string const FITS_COMMENT_BITPIX                 ("8,16,32,64 - Integer; -32,-64 - Floating");
  std::string const FITS_COMMENT_BITPIX_8               ("Character or unsigned binary integer");
  std::string const FITS_COMMENT_BITPIX_16              ("16-bit two's complement binary integer");
  std::string const FITS_COMMENT_BITPIX_32              ("32-bit two's complement binary integer");
  std::string const FITS_COMMENT_BITPIX_64              ("64-bit two's complement binary integer");
  std::string const FITS_COMMENT_BITPIX_N32             ("IEEE single precision floating-point");
  std::string const FITS_COMMENT_BITPIX_N64             ("IEEE double precision floating-point");

  std::string const FITS_COMMENT_NAXIS                  ("NUMBER OF AXES IN HDU");
  std::string const FITS_COMMENT_NAXISN                 ("NAXISn SIZE of AXIS n");
  std::string const FITS_COMMENT_PCOUNT                 ("");
  std::string const FITS_COMMENT_GCOUNT                 ("");

  std::string const FITS_COMMENT_BSCALE                 ("");
  std::string const FITS_COMMENT_BZERO                  ("");

  std::string const FITS_COMMENT_INSTRUMENT             ("Camera model acquiring the image.");

  std::string const FITS_COMMENT_HEALPIX                ("The healpix number of this catalog.");

  std::string const SBIG_COMMENT_CCDTEMP                ("CCD Temperature.");
  std::string const SBIG_COMMENT_EXPTIME                ("Image exposure duration");
  std::string const SBIG_COMMENT_XBINNING               ("Binning factor in width");
  std::string const SBIG_COMMENT_YBINNING               ("Binning factor in height");
  std::string const SBIG_COMMENT_EGAIN                  ("Electronic gain in e-/ADU");
  std::string const SBIG_COMMENT_FOCALLEN               ("Focal Length of the Telescope used in mm");
  std::string const SBIG_COMMENT_APTDIA                 ("Aperture Diameter of the Telescope used in mm");
  std::string const SBIG_COMMENT_APTAREA                ("Aperture Area of the telescope used in mm^2");
  std::string const SBIG_COMMENT_PEDESTAL               ("Correction to add for a zero-based ADU");
  std::string const SBIG_COMMENT_CBLACK                 ("Black point for the image.");
  std::string const SBIG_COMMENT_CWHITE                 ("White point for the image.");

  std::string const HEASARC_COMMENT_FILTER              ("Name of the filter used during the observation");
  std::string const HEASARC_COMMENT_CREATOR             ("the name of the software task that created the file");
  std::string const HEASARC_COMMENT_EXPOSURE            ("[s] Exposure time in seconds");

  std::string const NOAO_COMMENT_EXPTIME                ("[s] Exposure time in seconds");

  std::string const ASTROMANAGER_COMMENT_FWHM           ("[pixels] Mean Full-Width-Half-Max of image star");
  std::string const ASTROMANAGER_COMMENT_PLATE_FL       ("Astrometry: Plate Constant - Focal Length");
  std::string const ASTROMANAGER_COMMENT_PLATE_A        ("Astrometry: Plate Constant - a");
  std::string const ASTROMANAGER_COMMENT_PLATE_B        ("Astrometry: Plate Constant - b");
  std::string const ASTROMANAGER_COMMENT_PLATE_C        ("Astrometry: Plate Constant - c");
  std::string const ASTROMANAGER_COMMENT_PLATE_D        ("Astrometry: Plate Constant - d");
  std::string const ASTROMANAGER_COMMENT_PLATE_E        ("Astrometry: Plate Constant - e");
  std::string const ASTROMANAGER_COMMENT_PLATE_F        ("Astrometry: Plate Constant - f");

  std::string const ASTROMANAGER_COMMENT_LATITUDE       ("Geographic latitude where the observation was made");
  std::string const ASTROMANAGER_COMMENT_LONGITUDE      ("Geographic longitude where the observation was made");
  std::string const ASTROMANAGER_COMMENT_ALTITUDE       ("Geographic altitude where the observation was taken");
  std::string const ASTROMANAGER_COMMENT_UUID           ("UUID file identifier");

  std::string const ASTROMANAGER_COMMENT_AMBTEM         ("Ambient temperature deg C");
  std::string const ASTROMANAGER_COMMENT_AMBPRE         ("Ambient pressure hPa");
  std::string const ASTROMANAGER_COMMENT_AMBHUM         ("Ambient RH %");

    // ACL comments

  std::string const LA_PPMXL_COMMENT                    ("This is a PPMXL catalog.");
  std::string const LA_ANFILE_COMMENT                   ("libAstrometry file type");
  std::string const LA_ALLSKY_COMMENT                   ("All-sky catalog.");
  std::string const HEALPIX_NSIDE_COMMENT               ("The healpix resolution.");
  std::string const LA_HEALPIX_COMMENT                  ("Healpix covered by this catalog, with Nside=HPNSIDE");
  std::string const LA_NSTARS_COMMENT                   ("Number of stars.");
  std::string const LA_HPNSIDE_COMMENT                  ("Nside of HEALPIX.");

    // IMAGETYP Comments

  std::string const IMAGETYP_SYNTHETIC_COMMENT          ("Type of image");

  //*******************************************************************************************************************************

  // Defined strings

  std::string const ASTROMANAGER_HDB_PRIMARY            ("PRIMARY");
  std::string const ASTROMANAGER_HDB_ASTROMETRY         ("ASTROMETRY");
  std::string const ASTROMANAGER_HDB_PHOTOMETRY         ("PHOTOMETRY");

  // Time strings

  std::string const FITS_TIMESYS_UTC                    ("UTC");
  std::string const FITS_TIMESYS_UT                     ("UT");
  std::string const FITS_TIMESYS_TAI                    ("TAI");
  std::string const FITS_TIMESYS_AT                     ("AT");
  std::string const FITS_TIMESYS_ET                     ("ET");
  std::string const FITS_TIMESYS_TT                     ("TT");
  std::string const FITS_TIMESYS_TDT                    ("TDT");
  std::string const FITS_TIMESYS_TDB                    ("TDB");
  std::string const FITS_TIMESYS_TCG                    ("TCG");
  std::string const FITS_TIMESYS_TCB                    ("TCB");

  // XTENSION strings

#define FITS_XTENSION_IUEIMAGE      std::string("IUEIMAGE")
#define FITS_XTENSION_A3DTABLE      std::string("A3DTABLE")
#define FITS_XTENSION_FOREIGN       std::string("FOREIGN ")
#define FITS_XTENSION_DUMP          std::string("DUMP    ")
#define FITS_XTENSION_IMAGE         std::string("IMAGE   ")
#define FITS_XTENSION_TABLE         std::string("TABLE   ")
#define FITS_XTENSION_BINTABLE      std::string("BINTABLE")

  // Coordinate systems

#define COORDINATESYSTEM_NONE       std::string("NONE")
#define COORDINATESYSTEM_ICRS       std::string("ICRS")
#define COORDINATESYSTEM_FK4        std::string("FK4")
#define COORDINATESYSTEM_FK5        std::string("FK5")

    // Reserved Celestial Coordinate Algorithm Codes

  std::string const FITS_WCS_AZP                        ("AZP");
  std::string const FITS_WCS_SZP                        ("SZP");
  std::string const FITS_WCS_TAN    ("TAN");
  std::string const FITS_WCS_STG    ("STG");
  std::string const FITS_WCS_SIN    ("SIN");
  std::string const FITS_WCS_ARC    ("ARC");
  std::string const FITS_WCS_ZPN    ("ZPN");
  std::string const FITS_WCS_ZEA    ("ZEA");
  std::string const FITS_WCS_AIR    ("AIR");
  std::string const FITS_WCS_CYP    ("CYP");
  std::string const FITS_WCS_CEA    ("CEA");
  std::string const FITS_WCS_CAR    ("CAR");
  std::string const FITS_WCS_MER    ("MER");
  std::string const FITS_WCS_SFL    ("SFL");
  std::string const FITS_WCS_PAR    ("PAR");
  std::string const FITS_WCS_MOL    ("MOL");
  std::string const FITS_WCS_AIT    ("AIT");
  std::string const FITS_WCS_COP    ("COP");
  std::string const FITS_WCS_COE    ("COE");
  std::string const FITS_WCS_COD    ("COD");
  std::string const FITS_WCS_COO    ("COO");
  std::string const FITS_WCS_BON    ("BON");
  std::string const FITS_WCS_PCO    ("PCO");
  std::string const FITS_WCS_TSC    ("TSC");
  std::string const FITS_WCS_CSC    ("CSC");
  std::string const FITS_WCS_QSC    ("QCS");
  std::string const FITS_WCS_HPX    ("HPX");

    // IMAGETYP

  std::string const IMAGETYP_SYNTHETIC            ("SYNTHETIC");
}

#endif // ACL_FITSSTRINGS_H
