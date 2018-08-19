//*************************************************************************************************
//
// PROJECT:   Astronomy Class Library (libAstroClass)
// FILE:			AAVSO.h
// SUBSYSTEM: AAVSO related classes.
// TRAGET OS:	Standard C++
// AUTHOR:    Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2010-2018 Gavin Blakeman.
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
//						AAVSO - Extended Format ©AAVSO
//								The extended format has two components: parameters and data. Each component is
//								discussed in detail below.
//								The format is not case sensitive.
//							PARAMETERS
//								The Parameters are specified at the top of the file and are used to describe the
//								data that follows. Parameters must begin with a pound sign (#) at the start of
//								the line. There are six specific parameters that we require to exist at the top
//								of the file. Personal comments may also be added as long as they follow a pound
//								sign (#). These comments will be ignored by the software and not loaded into
//								the database. However, they will be retained when the complete file is stored in
//								the AAVSO permanent archives.
//								The six parameters that we require are:
//									#TYPE=Extended
//									#OBSCODE=
//									#SOFTWARE=
//									#DELIM=
//									#DATE=
//									#OBSTYPE=
//								The six parameters explained:
//									?TYPE: Should always say Extended for this format.
//									?OBSCODE: The official AAVSO Observer Code for the observer which was
//										previously assigned by the AAVSO. Request AAVSO Observer Initials here.
//									?SOFTWARE: Name and version of software used to create the format. If it is
//										private software, put some type of description here.
//										For example: "#SOFTWARE=AIP4Win Version 2.2". This is limited to 30
//										characters.
//									?DELIM: The delimiter used to separate fields in the report. Any ASCII
//										character or UNICODE number that corresponds to ascii code 32-126 is
//										acceptable as long as it is not used in any field. Suggested delimiters are:
//										comma (,) semi-colon(;), exclamation point(!), and pipe(|). The only
//										character that cannot be used is the pound (#) and the " " (space). If you
//										want to use a tab, use the word "tab" instead of an actual tab character.
//										Note: Excel users who want to use a comma will have to type "comma" here
//										instead of a ",". Otherwise Excel will export the field incorrectly.
//									?DATE: The format of the date used in the report. Convert all times from UT to
//										one of the following formats:
//											?JD: Julian Day (Ex: 2454101.7563)
//											?HJD: Heliocentric Julian Day
//											?EXCEL: the format created by Excel's NOW() function
//												(Ex: 12/31/2007 12:59:59 a.m )
//									?OBSTYPE: The type of observation in the data file. It can be CCD or PEP (for
//										Photoelectric Photometry). If absent, it is assumed to be CCD.
//
//									The OBSCODE and DATE parameters may also be included elsewhere in the data.
//									Our data processing software will read these parameters and will expect all
//									following data to adhere to them. (For example, you can add "#OBSCODE=TST01"
//									to the report and all subsequent observations will be attributed to observer TST01.)
//
//							DATA
//								After the parameters comes the actual variable star observations. There should be
//								one observation per line and the fields should be separated by the same character
//								that is defined in the DELIM parameter field. If you do not have data for one of
//								the optional fields, you must put "na" as a place holder. The list of fields are:
//									?NAME: The star's identifier. It can be the AAVSO Designation, the AAVSO Name,
//										or the AAVSO Unique Identifier
//									?DATE: The date of the observation, in the format specified in the DATE parameter.
//									?MAGNITUDE: The magnitude of the observation. Prepend with < if a fainter-than.
//									?MAGERR: Photometric uncertainty associated with the variable star magnitude.
//										If not available put "na".
//									?FILTER: The filter used for the observation. This can be one of the following
//										letters (in bold):
//											?U: Johnson U
//											?B: Johnson B
//											?V: Johnson V
//											?R: Cousins R
//											?I: Cousins I
//											?J: NIR 1.2 micron
//											?H: NIR 1.6 micron
//											?K: NIR 2.2 micron
//											?TG: Green Filter
//											?Z: Sloan Z
//											?CV: Clear (unfiltered), V-band comp star magnitudes (this is more common than CR)
//											?CR: Clear (unfiltered), R-band comp star magnitudes
//										These additional filters were added on 29 July 2009.
//											?SZ: Sloan z
//											?SU: Sloan u
//											?SG: Sloan g
//											?SR: Sloan r
//											?SI: Sloan i
//											?STU: Stromgren u
//											?STV: Stromgren v
//											?STB: Stromgren b
//											?STY: Stromgren y
//											?STHBW: Stromgren Hbw
//											?STHBN: Stromgren Hbn
//									?TRANS: YES if transformed, NO if not.
//									?MTYPE: Magnitude type. DIF if differential, STD if standardized. If you are
//										currently using ABS for 'absolute' we will still accept it. Differential
//										requires the use of CNAME.
//									?CNAME: Comparison star name or label such as the chart label or the AUID for
//										the comparison star used. If not present, use "na".
//									?CMAG: Instrumental magnitude of the comparison star. If not present, use "na".
//									?KNAME: Check star name or label such as the chart label or AUID for the check
//										star. If not present, use "na".
//									?KMAG: Instrumental magnitude of the check star. If not present, use "na".
//									?AIRMASS: Airmass of observation. If not present, use "na".
//									?GROUP: Grouping identifier (maximum 5 characters). It is used for grouping
//										multiple observations together, usually an observation set that was taken
//										through multiple filters. It makes it easier to retrieve all magnitudes from
//										a given set in the database, say, if someone wanted to form color indices
//										such as (B-V) with them. If you are just doing time series, or using the
//										same filter for multiple stars, etc., just set GROUP to "na." For cases
//										where you want to group observations, GROUP should be an integer, identical
//										for all observations in a group, and unique for a given observer for a given
//										star on a given Julian Date.
//									?CHART: This should be the latest date you see anywhere on the chart, entered
//										as YYMMDD. If you do not see a date, use the copyright year (Ex: "Copyright
//										2007" would be 070101). If no chart used, enter "na".
//									?NOTES: Comments or notes about the observation. This field has a maximum length
//										of 100 characters. If no comments, use "na".
//
// HISTORY:         2015-09-22 GGB - astroManager 2015.09 release
//
//*************************************************************************************************

#include "../include/AAVSO.h"

namespace ACL
{

  char szFilters[25][MAX_FILTERSTRING+1] =
  {
    "U",
    "B",
    "V",
    "R",
    "I",
    "J",
    "H",
    "K",
    "TG",
    "Z",
    "CV",
    "CR",
    "SZ",
    "SU",
    "SG",
    "SR",
    "SI",
    "STU",
    "STV",
    "STB",
    "STY",
    "STHBW",
    "STHBN",
    "VIS"
  };

  //***********************************************************************************************
  //
  // Class AAVSOEntendedFormat
  //
  //***********************************************************************************************


} // namespace ACL
