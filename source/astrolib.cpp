//*****************************************************************************
//
// PROJECT:   Common Files (TCS, VSOP)
// FILE:			AstroLib.cpp
// SUBSYSTEM: Astro functions library
// All rights reserved.
//
// OVERVIEW:
// This library provides a set of functions to be used for astro-calculation purposes.
//
// CLASSES INCLUDED:
//
//
// Revision:	1.20
// By:				Gavin Blakeman
// Date:			25 October 2009
// Changes:		1) Stripped out all functions used in CEphemeris.
//						2) Added function sprintfTMS_s.
//						3) Added function sprintfLMS_s
//
// REVISION:	1.10
// BY:				Gavin Blakeman
// DATE:			6 September 2009
// CHANGES:		1) Added two functions to library. sprintfDMS_s, sprintfHMS_s
//						2) Removed global variables obsr_lat, obsr_lon, obsr_ele
//						3) Used class TLocation to replace the above global variables.
//
// ORIGINAL FILE:
//					Project:  fecsoftc
//					Filename: astrolib.c
//					Author:   Joe Heafner
//					Purpose:  Library routines header file.
//					Thanks to Charles Gamble for extensive modifications.
//
//*****************************************************************************

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../Include/astrolib.h"
#include "../Include/support.h"


/* Globals **********************************************************/

/* Temporary data used when reading in binary file */

int tmpInt;
long tmpLong;

/*********************************************************************
Name:    Aberrate
Purpose: Function to correct an input vector for aberration.
Inputs:  p1    - Zero-offset geocentric state vector of object.
         EBdot - Zero-offset barycentric velocity of Earth.
Outputs: p2 - Zero-offset geocentric state vector of object corrected
              for aberration.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Aberrate(double p1[], double EBdot[], double p2[]) {

  double v[3], up[3], p[3], pdot[3], magP, magV, p1dotv, beta;
  int i;

  /* Extract the pos. portion of the state vector */
  SplitStateVector(p1, p, pdot);
  magP = Vecmag(p);

  /* Need to make Ppos() a unit vector */
  Uvector(p, up);

  for (i=0; i<3; i++) {
    v[i] = EBdot[i] / DC;						/* CAUD changed to DC to reflect use of Sofam.h  GGB 2009-11-03 */
  }

  Vdot(3, up, v, &p1dotv);
  magV = Vecmag(v);

  beta = 1.0 / sqrt(1.0 - magV * magV);

  for (i=0; i<3; i++) {
    p2[i] = ((up[i] / beta) +
      (1.0 + p1dotv / (1.0 + (1.0 / beta))) * v[i]) / (1.0 + p1dotv);
    /* Make p2[] a non-unit vector */
    p2[i] = magP * p2[i];
    p2[i+3] = p1[i+3];
  }

  return;
}

/*********************************************************************
Name:    amodulo
Purpose: Function to reduce a to the range 0 <= a < b.
Inputs:  a - See above.
         b - See above.
Outputs: None.
Returns: See above.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double amodulo(double a, double b) {

  double x;

  x = a - b * floor(a/b);
  return (x);
}

/*********************************************************************
Name:    Cal2JED
Purpose: Function to compute the Julian date on the specified time scale.
         The Julian day algorithm of Meeus is used, and the algorithm for
         converting to TDB is due to Hirayama et al. and can be found in
         REFERENCE FRAMES IN ASTRONOMY AND GEOPHYSICS ed. by Kovalevsky
         et al., 1989, pp. 439-442.
Inputs:  m       - Month.
         d       - Day.
         y       - Year.
         utc     - UTC as hh.mmssss.
         s       - 1 for UT1, 2 for UT2, 3 for TDT,
                   4 for TDB, 5 for UTC.
         tai_utc - TAI-UTC in seconds.
         ut1_utc - UT1-UTC in seconds.
         w       - 1 for MJD, 0 otherwise.
Outputs: jed - Appropriate JED.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Cal2JED(int m, int d, int y, double utc, int s,
  double tai_utc, double ut1_utc, int w, double *jed) {
  double time, datnum, a, b, term1, corr, T;

  /* Convert to decimal hours */
  time = deg(utc);

  if ((m == 1) || (m == 2)) {
    y = y - 1;
    m = m + 12;
  }

  /*
      Test to see if date is in Gregorian calendar.
      Converts date to a number of the form YYYY.MMDD.
  */
  datnum = (double) y + 0.01 * (double) m + 0.0001 * (double) d;

  if (datnum >= 1582.1015) {
    /* Gregorian calendar */
    a = fix(0.01 * (double) y);
    b = 2.0 - a + fix(0.25 * (double) a);
  } else {
    /* Julian calendar */
    a = 0.0;
    b = 0.0;
  }

  if (y < 0) {
    /* Handles negative years */
    term1 = fix(365.25 * (double) y - 0.75); /* change here */
  } else {
    term1 = fix(365.25 * (double) y);
  }

  *jed = term1 + fix(30.6001 * ((double) m + 1.0)) +
    (double) d + time / 24.0 + 1720994.5 + (double) b;

  switch (s) {
    case (1):
      corr = ut1_utc / 86400.0;
      break;
    case (2):
      corr = ut1_utc / 86400.0;
      *jed = *jed + corr;
      /* Compute date in Besselian years */
      T = 2000.0 + (*jed - 2451544.5333981) / 365.242198781;
      corr = 0.022 * sin(D2PI * T);
      corr += -0.012 * cos(D2PI * T);
      corr += -0.006 * sin(2.0 * D2PI * T);
      corr += 0.007 * cos(2.0 * D2PI * T);
      corr = corr / 86400.0;
      break;
    case (3):
      corr = (tai_utc + 32.184) / 86400.0;
      break;
    case (4):
      /* First convert to TDT */
      corr = (tai_utc + 32.184) / 86400.0;
      *jed = *jed + corr;
      T = (*jed - J2000) / DJC;
      /* Now compute the new correction in microseconds */
      corr = 1656.675     * sin((35999.3729 * T + 357.5387) * DD2R);
      corr +=  22.418     * sin((32964.467  * T + 246.199)  * DD2R);
      corr +=  13.84      * sin((71998.746  * T + 355.057)  * DD2R);
      corr +=   4.77      * sin(( 3034.906  * T +  25.463)  * DD2R);
      corr +=   4.67      * sin((34777.259  * T + 230.394)  * DD2R);
      corr +=  10.216 * T * sin((35999.373  * T + 243.451)  * DD2R);
      corr +=   0.171 * T * sin((71998.746  * T + 240.98 )  * DD2R);
      corr +=   0.027 * T * sin(( 1222.114  * T + 194.661)  * DD2R);
      corr +=   0.027 * T * sin(( 3034.906  * T + 336.061)  * DD2R);
      corr +=   0.026 * T * sin((  -20.186  * T +   9.382)  * DD2R);
      corr +=   0.007 * T * sin((29929.562  * T + 264.911)  * DD2R);
      corr +=   0.006 * T * sin((  150.678  * T +  59.775)  * DD2R);
      corr +=   0.005 * T * sin(( 9037.513  * T + 256.025)  * DD2R);
      corr +=   0.043 * T * sin((35999.373  * T + 151.121)  * DD2R);
      /* Convert from microseconds to seconds */
      corr = corr * 0.000001;
      /* Convert to days */
      corr = corr / 86400.0;
      break;
    case (5):
      corr = 0.0;
      break;
    default:
      corr = 0.0;
  }

  *jed += corr;

  /* Return modified JED if requested */
  if (w == 1) *jed -= 2400000.5;
}



/*********************************************************************
Name:    Conway
Purpose: Function to solve Kepler's equation using the method of
         Conway-Laguerre for universal variables.
Inputs:  uele - Array of universal orbital elements.
         mu   - Gravitational constant for the body.
         jed  - Time for which computations are to be performed.
Outputs: r_cos_nu - r*cos(true anomaly).
         r_sin_nu - r*sin(true anomaly).
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Conway(double uele[], double mu, double jed, double *r_cos_nu,
  double *r_sin_nu) {

  double tspp, alpha, s, x, c1, c2, c3, f, fp, fpp, term, ds;
  int niter;

  /* Compute time since perihelion passage in days */
  tspp = jed - uele[5];

  /* Compute alpha */
  alpha = mu * (1.0 - uele[1]) / uele[0];

  /* Initial guess for s */
  s = 0.0;

  /* Initialize iteration counter */
  niter = 0;

  do {
    /* Compute Stumpff functions */
    x = alpha * s * s;
    c1 = StumpffN(x, 1);
    c2 = StumpffN(x, 2);
    c3 = StumpffN(x, 3);

    f = uele[0] * s + mu * uele[1] * s * s * s * c3 - tspp;
    fp = uele[0] + mu * uele[1] * s * s * c2;
    fpp = mu * uele[1] * s * c1;

    /* Evaluate Laguerre's method */
    term = 16.0 * fp * fp - 20.0 * f * fpp;
    ds = -5.0 * f / (fp + fp/fabs(fp) * sqrt(fabs(term)));

    s = s + ds;

    niter = niter + 1;

    /* Check for convergence or more than ten iterations */
    if (niter > 10)
    {
      LogMsg(stderr, "Conway: more than ten iterations.\n");
      exit(1);
    }

    if (fabs(f) < 1e-12) break;

  } while(1);

  /* Now compute r_sin_nu and r_cos_nu */
  *r_sin_nu = sqrt(mu * uele[0] * (1.0 + uele[1])) * s * c1;
  *r_cos_nu = uele[0] - mu * s * s * c2;
}

/*********************************************************************
Name:    createDMatrix
Purpose: Creates an n x n double matrix.
Inputs:  n - Dimension of matrix to create.
Outputs: None.
Returns: DMatrix.
Status:  Finished.
Errors:  None known.
*********************************************************************/
DMatrix createDMatrix(int n) {

  DMatrix m;
  int i;

  m = (DMatrix )calloc(n, sizeof(double*));
  if (m == NULL) {
    return NULL;
  }

  for (i=0; i<n; i++) {
    m[i] = (double *) calloc(n, sizeof(double));
    if (m[i] == NULL) {
      freeDMatrix(m, i); /* Avoids garbage */
      return NULL;
    }
  }

  return m;
};

/*********************************************************************
Name:    deg
Purpose: Converts dd.mmssss to dd.dddddd.
         Converts hh.mmssss to hh.hhhhhh.
Inputs:  x - Value to convert.
Outputs: None.
Returns: Converted value.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double deg(double x) 
{

  double dd, d, fixdd, ddfixdd;

  if (x == 0.0) return (0.0);

  dd = fabs(x);
  fixdd = floor(dd);
  ddfixdd = dd - fixdd + 5.0e-10;  /* fudge factor */
  d = fixdd + floor(100.0 * ddfixdd) / 60.0;
  d = d + (10000.0 * ddfixdd - 100.0 * floor(100.0 * ddfixdd)) / 3600.0;

  return ((x / dd) * d);
}

/*********************************************************************
Name:    dms
Purpose: Converts dd.dddddd to dd.mmssss.
         Converts hh.hhhhhh to hh.mmssss.
Inputs:  x - Value to convert.
Outputs: None.
Returns: Converted value.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double dms(double x) {

  double dd, fdd, dmfd, d;

  if (x == 0.0) return (0.0);

  dd = fabs(x);
  fdd = floor(dd);
  dmfd = dd - fdd + 5.0e-10;  /* fudge factor */
  d = fdd + floor(60.0 * dmfd) / 100.0 + 0.006 *
    (60.0 * dmfd - floor(60.0 * dmfd));

  return ((x / dd) * d);
}

/*********************************************************************
Name:    DRound
Purpose: Function to round a number to a given number of decimal places.
Inputs:  x - Value to round.
         n - Number of decimal places.
Outputs: None.
Returns: Rounded number.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double DRound(double x, int n) {

  double a, y;
  int sgn;

  if (x > 0) {
    sgn = 1;
  }
  else if (x < 0) {
    sgn = -1;
  } else {
    sgn = 0;
  }

  a = pow(10.0, (double) n);
  y = floor((a * x) + ((double) sgn) * 0.5) / a;

  return y;
}

/*********************************************************************
Name:    Epoch2JED
Purpose: Function to convert an epoch to its corresponding JED.
Inputs:  epoch - J or B epoch (e.g. J2000).
Outputs: jed - Appropriate JED.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Epoch2JED(char *epoch, double *jed) {

  double date;
  char buffer[80];

  strcpy(buffer, epoch);

  ucase(buffer);

  sscanf(buffer, "%*c %lf", &date);

  if (epoch[0] == 'J') {
    /* Julian epoch */
    *jed = (date - 2000.0) * 365.25 + 2451545.0;
  } else {
    /* Besselian epoch */
    *jed = (date - 1900.0) * 365.242198781731 + 2415020.31352;
  }

  return;
}

/*********************************************************************
Name:    Eq2Ecl
Purpose: Subprogram to convert an equatorial state vector to an
         ecliptic state vector or the reverse transformation.
Inputs:  a   - Zero-offset input vector.
         s   - 0 for eq  -> ecl, 1 for ecl -> eq.
         eps - Mean or apparent obliquity.
Outputs: b - Zero-offset output vector.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Eq2Ecl(double a[], int s, double eps, double b[]) {

  DMatrix r, dr;

  r  = createDMatrix(3);
  dr = createDMatrix(3);

  if (s == 0) {
    /* Equatorial to ecliptic */
    RotMat(1,  eps, r, dr);
  } else {
    /* Ecliptic to equatorial */
    RotMat(1, -eps, r, dr);
  }

  MatXVec(r, a, b, 3, 3);

  freeDMatrix(r, 3);
  freeDMatrix(dr,3);

  return;
}

//*************************************************************************************************
// Name:    Eq2Hor
// Purpose: Function to convert an equatorial state vector to a
//          horizon state vector or the reverse transformation.
// Inputs:  a - Zero-offset input vector.
//          s - 0 for eq -> hor, 1 for hor -> eq.
// Outputs: b - Zero-offset output vector.
// Returns: Nothing.
// Status:  Finished.
// Errors:  None known.
//
// Revision:			2.00
// Revision by:		Gavin Blakeman
// Date:					14 October 2009
// Modification:	1) Included extra parameter TLocation *
//
//*************************************************************************************************
void Eq2Hor(TLocation *Location, double a[], int s, double b[]) {

  DMatrix r2, r3, r, dr2, dr3, dr;

  r2 = createDMatrix(3);
  r3 = createDMatrix(3);
  r  = createDMatrix(3);
  dr2 = createDMatrix(3);
  dr3 = createDMatrix(3);
  dr = createDMatrix(3);

  if (s == 0) {
    /* Equatorial to horizon */
    RotMat(2, PIDIV2 - Location->latitude() * DD2R, r2, dr2);
    RotMat(3, -DPI, r3, dr3);
    MatXMat(r3, r2, r, 3);
  } else {
    /* Horizon to equatorial */
    RotMat(3, DPI, r3, dr3);
    RotMat(2, Location->latitude() * DD2R - PIDIV2, r2, dr2);
    MatXMat(r2, r3, r, 3);
  }

  MatXVec(r, a, b, 3, 3);

  freeDMatrix(r2, 3);
  freeDMatrix(r3, 3);
  freeDMatrix(r, 3);
  freeDMatrix(dr2, 3);
  freeDMatrix(dr3, 3);
  freeDMatrix(dr, 3);

  return;
}

/*********************************************************************
Name:    errprt
Purpose: Function to print error messages.
Inputs:  group   - Error code number.
         message - Error message.
Outputs: None.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void errprt(int group, char *message) {

  LogMsg(stderr, "\nERROR #%d %s\n", group, message);
  exit(1);
}

/*********************************************************************
Name:    fix
Purpose: Function to return the integer part of a number. Basically,
         this function emulates the PowerBasic fix() function.
Inputs:  x - Decimal value.
Outputs: None.
Returns: Integral part of a number.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double fix(double x) {

  if (x < 0) {
    return ceil(x);
  } else {
    return floor(x);
  }
}

/*********************************************************************
Name:    FmtDms
Purpose: Function to format angular and time quantities. Basically,
         this function is a pretty-print version of dms().
         Modified with permission from Rex Shudde's code by Joe Heafner.
Inputs:  x - Decimal value.
         n - Number of seconds decimal digits.
         m - 0 convert decimal degrees to:
               ddd mm ss      if n = 0
               ddd mm ss.f    if n = 1
               ddd mm ss.ff   if n = 2
               ddd mm ss.fff  if n = 3
               ddd mm ss.ffff if n = 4
         m - 1 converts decimal hours to:
               hh mm ss       if n = 0
               hh mm ss.f     if n = 1
               hh mm ss.ff    if n = 2
               hh mm ss.fff   if n = 3
               hh mm ss.ffff  if n = 4
Outputs: s - Destination for output string.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
//
// Revision:			2.00
// Revision by:		Gavin Blakeman
// Date:					14 October 2009
// Modification:	1) Converted to use strcpy_s.
//								2) Added parameter size_t numberOfElements.
//
*********************************************************************/
void FmtDms(double x, int n, int m, char *s, size_t numberOfElements) 
{

  double absx, deg, min, sec;
  int nf;
  unsigned int fig;
  static int defd;
  static char dh[3];
  static char mm[3];
  static char ss[3];
  char buffer[16] = "              ";
  char right_buffer[16], left_buffer[16];

  strcpy(s, "              ");

  if (!defd) 
	{
    defd++;
    dh[0] = '\xf8'; /* f8 is the hex code for degree symbol */
    dh[1] = 'h';
    dh[2] = '\0';
    mm[0] = '\x27'; /* 27 is the hex code for arcmin symbol */
    mm[1] = 'm';
    mm[2] = '\0';
    ss[0] = '\x22'; /* 22 is the hex code for arcsec symbol */
    ss[1] = 's';
    ss[2] = '\0';
  };

  absx = fabs(x);

  /* determine how many digits before the decimal */
  if (absx < 100) 
	{
    fig = 2;
  } 
	else 
	{
    fig = 3;
  };

  deg = floor(absx);
  absx = 60.0 * (absx - deg);
  min = floor(absx);
  sec = 60.0 * (absx - min);
  sec = DRound(sec, n);

  if (sec >= 60.0) 
	{
    sec = 0.0;
    min = min + 1.0;
  };

  if (min >= 60.0) 
	{
    min = 0.0;
    deg = deg + 1.0;
  };

  if (((deg == 24.0) && m == 1) || (deg == 360.0)) 
	{
    deg = 0.0;
  };

  strcpy(s, " ");
  if (x < 0) 
	{
    strcpy(s, "-");
  }
  else if (m == 0) 
	{
    strcpy(s, "+");
  };

  /* begin building up the return string in buffer */
  sprintf(buffer, "%.0f", (1000.0+deg));
  right(buffer, fig, right_buffer);
  strcat(s, right_buffer);
  left(dh, m+1, left_buffer);
  right(left_buffer, 1, right_buffer);
  strcat(s, right_buffer);

  sprintf(buffer, "%.0f", (100.0+min));
  right(buffer, 2, right_buffer);
  strcat(s, right_buffer);
  left(mm, m+1, left_buffer);
  right(left_buffer, 1, right_buffer);
  strcat(s, right_buffer);

  switch (n) 
	{
    case 0:
      sprintf(buffer, "%.0f", (100.0+sec+0.0000001));
      break;
    case 1:
      sprintf(buffer, "%.1f", (100.0+sec+0.0000001));
      break;
    case 2:
      sprintf(buffer, "%.2f", (100.0+sec+0.0000001));
      break;
    case 3:
      sprintf(buffer, "%.3f", (100.0+sec+0.0000001));
      break;
    case 4:
      sprintf(buffer, "%.4f", (100.0+sec+0.0000001));
  };

  if (n == 0) 
	{
    nf = 2;
  } 
	else 
	{
    nf = n + 3;
  };

  right(buffer, strlen(buffer)-1, right_buffer);
  left(right_buffer, nf, left_buffer);
  strcat(s, left_buffer);

  left(ss, m+1, left_buffer);
  right(left_buffer, 1, right_buffer);
  strcat(s, right_buffer);
};

/*********************************************************************
Name:    free_matrix
Purpose: Free a double matrix allocated by matrix().
Inputs:  m    - Pointer to matrix.
         nrow - Number of rows.
         ncol - Number of columns.
Outputs: None.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void free_matrix(double **m, int nrow, int ncol)
{

  int i;

  for (i = 0;i < nrow;i++)
  {
    free(m[i]);
  };

  free(m);
};

/*********************************************************************
Name:    freeDMatrix
Purpose: Free all storage associated with matrix m.
Inputs:  m - Matrix to free.
         n - Dimension of matrix.
Outputs: None.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void freeDMatrix(DMatrix m, int n)
{
  int i;

  for (i=0; i<n; i++)
  {
    free(m[i]); /* storage for doubles in row i */
  }
  free(m); /* storage for pointers to rows */

  return;
}

/*********************************************************************
Name:    FunArgIAU
Purpose: Function to compute the fundamental arguments using the IAU
         expressions.
Inputs:  jed - JED on TDB scale.
Outputs: funarg[0]   - Mean anomaly of Moon.
         funarg[1]   - Mean anomaly of Sun.
         funarg[2]   - Argument of lat. of Moon.
         funarg[3]   - Mean elongation of Moon.
         funarg[4]   - Mean longitude of Moon's ascending node.
         funarg[5]   - Mean longitude of Moon.
         funarg[i+6] - Derivative of ith fundamental argument.
         NOTE: All derivatives are in units of rad/day.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void FunArgIAU(double jed, double *funarg) {

  static double jedz;
  double T, T2, T3, L, Ldot, lp, lpdot, F, Fdot, D, Ddot;
  double N, Ndot, LL, LLdot;

  if (jed == jedz) return;
  jedz = jed;

  T = (jed - J2000) / DC;				/* Changed JulCty to DC to reflect use of Sofam.h GGB 2009-11-03 */
  T2 = T * T;
  T3 = T2 * T;

  /* Compute fundamental arguments */
  L = 485866.733 + (1325. * 1296000. + 715922.633) * T
    + 31.31 * T2 + 0.064 * T3;
  Ldot = (1325. * 1296000. + 715922.633) + 2. * 31.31 * T
    + 3. * 0.064 * T2;
    L = amodulo(L * DAS2R, D2PI);			/* A2R changed to DAS2R to relect use of Saofam.h GGB 2009-11-03 */
    Ldot = amodulo(Ldot * DAS2R / 36525., D2PI);
  lp = 1287099.804 + (99. * 1296000. + 1292581.224) * T
    - 0.577 * T2 - 0.012 * T3;
  lpdot = (99. * 1296000. + 1292581.224) - 2. * 0.577 * T
    - 3. * 0.012 * T2;
    lp = amodulo(lp * DAS2R, D2PI);
    lpdot = amodulo(lpdot * DAS2R / 36525., D2PI);		// A2R changed to DAS2R to relect sofam.h GGB 2009-11-03 */
  F = 335778.877 + (1342. * 1296000. + 295263.137) * T
    - 13.257 * T2 + 0.011 * T3;
  Fdot = (1342. * 1296000. + 295263.137) - 2. * 13.257 * T
    + 3. * 0.011 * T2;
    F = amodulo(F * DAS2R, D2PI);
    Fdot = amodulo(Fdot * DAS2R / 36525., D2PI);
  D = 1072261.307 + (1236. * 1296000. + 1105601.328) * T
    - 6.891 * T2 + 0.019 * T3;
  Ddot = (1236. * 1296000. + 1105601.328) - 2. * 6.891 * T
    + 3. * 0.019 * T2;
    D = amodulo(D * DAS2R, D2PI);
    Ddot = amodulo(Ddot * DAS2R / 36525., D2PI);
  N = 450160.28 - (5. * 1296000. + 482890.539) * T
    + 7.455 * T2 + 0.008 * T3;
  Ndot = (5. * 1296000. + 482890.539) + 2. * 7.455 * T
    + 3. * 0.008 * T2;
    N = amodulo(N * DAS2R, D2PI);
    Ndot = amodulo(Ndot * DAS2R / 36525., D2PI);
  LL = 785939.157 + (1336. * 1296000. + 1108372.598) * T
    - 5.802 * T2 + 0.019 * T3;
  LLdot = (1336. * 1296000. + 1108372.598) - 2. * 5.802 * T
    + 3. * 0.019 * T2;
    LL = amodulo(LL * DAS2R, D2PI);
    LLdot = amodulo(LLdot * DAS2R / 36525., D2PI);

  funarg[0]  = L;
  funarg[6]  = Ldot;
  funarg[1]  = lp;
  funarg[7]  = lpdot;
  funarg[2]  = F;
  funarg[8]  = Fdot;
  funarg[3]  = D;
  funarg[9]  = Ddot;
  funarg[4]  = N;
  funarg[10] = Ndot;
  funarg[5]  = LL;
  funarg[11] = LLdot;
};

/*********************************************************************
Name:    GetInvQMatrix
Purpose: Function to compute the elements of the inverse of a
         given Q-matrix.
Inputs:  QMatrix - Zero-offset 6X6 Q-matrix.
Outputs: InvQMatrix - Zero-offset inverse of QMatrix.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void GetInvQMatrix(DMatrix QMatrix, DMatrix InvQMatrix) {

  int i, j;

  for (i= 0; i<3; i++) {
    for (j=0; j<3; j++) {
      InvQMatrix[i][j] = QMatrix[j][i];
      InvQMatrix[i][j+3] = 0.0;
      InvQMatrix[i+3][j] = QMatrix[j+3][i];
      InvQMatrix[i+3][j+3] = QMatrix[j+3][i+3];
    }
  }
}

/*********************************************************************
Name:    GetPrecessParams
Purpose: Function that computes the general precession parameters and their
         derivatives for precessing equatorial rectangular coordinates and
         velocities from jed1 to jed2. Lieske's formulae are used.
Inputs:  jed1 - Initial Julian Date.
         jed2 - Final Julian Date.
Outputs: zeta     - equatorial precession parameter.
         z        - equatorial precession parameter.
         theta    - equatorial precession parameter.
         zetadot  - derivative in rad/day.
         zdot     - derivative in rad/day.
         thetadot - derivative in rad/day.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void GetPrecessParams(double jed1, double jed2, double *zeta, double *z,
  double *theta, double *zetadot, double *zdot, double *thetadot) {

  double T1, T2, c1, c2, c3, c4, c5, c6, p1, p2, x, xdot;

  T1 = (jed1 - J2000) / DJC;   /* JulCty changed to DJC to reflect use of sofam.h GGB 2009-11-03 */
  T2 = (jed2 - jed1) / DJC;

  /* compute zeta, z, theta, zetadot, zdot, thetadot */
  c1 = 2306.2181;
  c2 =    1.39656;
  c3 =   -0.000139;
  c4 =    0.30188;
  c5 =   -0.000344;
  c6 =    0.017998;
  p1 = c1 + c2 * T1 + c3 * T1 * T1;
  p2 = c4 + c5 * T1;
  x = p1 * T2 + p2 * T2 * T2 + c6 * T2 * T2 * T2;
  xdot = p1 + 2.0 * p2 * T2 + 3.0 * c6 * T2 * T2;
  *zeta = x * DAS2R;
  *zetadot = xdot * DAS2R / DJC;

  c1 = 2306.2181;
  c2 =    1.39656;
  c3 =   -0.000139;
  c4 =    1.09468;
  c5 =    0.000066;
  c6 =    0.018203;
  p1 = c1 + c2 * T1 + c3 * T1 * T1;
  p2 = c4 + c5 * T1;
  x = p1 * T2 + p2 * T2 * T2 + c6 * T2 * T2 * T2;
  xdot = p1 + 2.0 * p2 * T2 + 3.0 * c6 * T2 * T2;
  *z = x * DAS2R;
  *zdot = xdot * DAS2R / DJC;

  c1 = 2004.3109;
  c2 =   -0.85330;
  c3 =   -0.000217;
  c4 =   -0.42665;
  c5 =   -0.000217;
  c6 =   -0.041833;
  p1 = c1 + c2 * T1 + c3 * T1 * T1;
  p2 = c4 + c5 * T1;
  x = p1 * T2 + p2 * T2 * T2 + c6 * T2 * T2 * T2;
  xdot = p1 + 2.0 * p2 * T2 + 3.0 * c6 * T2 * T2;
  *theta = x * DAS2R;
  *thetadot = xdot * DAS2R / DJC;
}

/*********************************************************************
Name:    GetQMatrix
Purpose: Function to compute the elements of the Q-matrix for a
         given angle and its time derivative.
Inputs:  phi    - Angle in radians
         phidot - Time der. of phi.
         axis   - 1 for x-axis,
                  2 for y-axis,
                  3 for z-axis.
         s      - 0 for inertial to inertial,
                  1 for inertial to rotating.
Outputs: QMatrix - Zero-offset 6X6 Q-matrix.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void GetQMatrix(double phi, double phidot, int axis,
  int s, DMatrix QMatrix) {

  DMatrix r, dr;
  int i, j;

  r  = createDMatrix(3);
  dr = createDMatrix(3);

  /* form the 3X3 r[] and dr[] matrices */
  RotMat(axis, phi, r, dr);

  /* form the 6X6 Q-matrix */
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      QMatrix[i][j] = r[i][j];
      QMatrix[i][j+3] = 0.0;
      QMatrix[i+3][j] = phidot * dr[i][j] * s;
      QMatrix[i+3][j+3] = r[i][j];
    }
  }

  freeDMatrix(r, 3);
  freeDMatrix(dr, 3);
}

/*********************************************************************
Name:    GetRPNmat
Purpose: Function to compute the precession matrix, the nutation
         matrix, or the combined R matrix for equatorial coordinates.
Inputs:  jed1 - Initial JED on TDB scale.
         jed2 - Final JED on TDB scale.
         rpn  - 1 for precession matrix,
                2 for nutation matrix,
                3 for R matrix.
         d    - 1 for zero-offset 3X3 matrix,
                2 for zero-offset 6X6 Qmatrix.
Outputs: m3 - Requested zero-offset 3X3 matrix.
         m6 - Requested zero-offset 6X6 matrix.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void GetRPNmat(double jed1, double jed2, int rpn, int d,
  DMatrix m3, DMatrix m6) {

  double dpsi, deps, dpsidot, depsdot, trueeps, trueepsdot,
    meaneps, meanepsdot, zeta, zetadot, z, zdot, theta, thetadot;
  DMatrix p1mat, p2mat, p3mat, pmat, n1mat, n2mat, n3mat,
    nmat, rmat, p, n;

  p1mat = createDMatrix(6);
  p2mat = createDMatrix(6);
  p3mat = createDMatrix(6);
  pmat  = createDMatrix(6);
  n1mat = createDMatrix(6);
  n2mat = createDMatrix(6);
  n3mat = createDMatrix(6);
  nmat  = createDMatrix(6);
  rmat  = createDMatrix(6);
  p     = createDMatrix(6);
  n     = createDMatrix(6);

  switch (rpn) {
    case 1:
      /* compute precession matrix */
      GetPrecessParams(jed1,jed2,&zeta,&z,&theta,&zetadot,&zdot,&thetadot);
      GetQMatrix(-zeta, -zetadot, 3, 0, p1mat);
      GetQMatrix(theta, thetadot, 2, 0, p2mat);
      GetQMatrix(-z, -zdot, 3, 0, p3mat);
      MatXMat(p2mat, p1mat, pmat, 6);
      MatXMat(p3mat, pmat, m3, 6);
      freeDMatrix(p3mat, 6);
      freeDMatrix(p2mat, 6);
      freeDMatrix(p1mat, 6);
      freeDMatrix(pmat, 6);
      break;
    case 2:
      /* compute nutation matrix */
      GetDpsiDeps(jed2, &dpsi, &deps, &dpsidot, &depsdot);
      Obliquity(jed1, jed2, 0, &meaneps, &meanepsdot);
      Obliquity(jed1, jed2, 1, &trueeps, &trueepsdot);
      GetQMatrix(meaneps, meanepsdot, 1, 0, n1mat);
      GetQMatrix(-dpsi, -dpsidot, 3, 0, n2mat);
      GetQMatrix(-trueeps, -trueepsdot, 1, 0, n3mat);
      MatXMat(n2mat, n1mat, nmat, 6);
      MatXMat(n3mat, nmat, m3, 6);
      freeDMatrix(n3mat, 6);
      freeDMatrix(n2mat, 6);
      freeDMatrix(n1mat, 6);
      freeDMatrix(nmat, 6);
      break;
    case 3:
      /* compute R matrix */
      GetPrecessParams(jed1,jed2,&zeta,&z,&theta,&zetadot,&zdot,&thetadot);
      GetQMatrix( -zeta, -zetadot, 3, 0, p1mat);
      GetQMatrix(theta, thetadot, 2, 0, p2mat);
      GetQMatrix(-z, -zdot, 3, 0, p3mat);
      MatXMat(p2mat, p1mat, pmat, 6);
      MatXMat(p3mat, pmat, p, 6);
      GetDpsiDeps(jed2, &dpsi, &deps, &dpsidot, &depsdot);
      Obliquity(jed1, jed2, 0, &meaneps, &meanepsdot);
      Obliquity(jed1, jed2, 1, &trueeps, &trueepsdot);
      GetQMatrix(meaneps, meanepsdot, 1, 0, n1mat);
      GetQMatrix(-dpsi, -dpsidot, 3, 0, n2mat);
      GetQMatrix(-trueeps, -trueepsdot, 1, 0, n3mat);
      MatXMat(n2mat, n1mat, nmat, 6);
      MatXMat(n3mat, nmat, n, 6);
      MatXMat(n, p, m3, 6);
      freeDMatrix(p3mat, 6);
      freeDMatrix(p2mat, 6);
      freeDMatrix(p1mat, 6);
      freeDMatrix(pmat, 6);
      freeDMatrix(p, 6);
      freeDMatrix(n3mat, 6);
      freeDMatrix(n2mat, 6);
      freeDMatrix(n1mat, 6);
      freeDMatrix(nmat, 6);
      freeDMatrix(n, 6);
      break;
  }
}


/*********************************************************************
Name:    HelEphemeris
Purpose: Function to compute an orbiting body's heliocentric
         ecliptic state vector for a single instant of time
         given the universal orbital elements and the time.
         Reference: Mansfield. 1986. AIAA Paper 86-2269-CP.
Inputs:  uelement[] - Array of universal orbital elements:
         uelement[0] = q
         uelement[1] = e
         uelement[2] = i
         uelement[3] = node
         uelement[4] = arg. peri.
         uelement[5] = T
         mu         - Gravitational constant for the body.
         jed        - Time.
Outputs: posvel[] - State vector:
         posvel[0..2] = position vector.
         posvel[3..5] = velocity vector.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void HelEphemeris(double *uelement, double mu, double jed, double *posvel) {

  int i;
  double LongPeri, RetLongPeri, cosi, sini, coslp, sinlp, cosrlp, sinrlp;
  double cosw, sinw, rcosnu, rsinnu, r, cosnu, sinnu, param, p[3], q[3];

  /* Compute longitude of perihelion */
  LongPeri = uelement[4] + uelement[3];
  /* Compute retrograde longitude of perihelion */
  RetLongPeri = uelement[4] - uelement[3];

  /* Compute the P vector */
  cosi   = cos(uelement[2]);
  sini   = sin(uelement[2]);
  coslp  = cos(LongPeri);
  sinlp  = sin(LongPeri);
  cosrlp = cos(RetLongPeri);
  sinrlp = sin(RetLongPeri);
  cosw   = cos(uelement[4]);
  sinw   = sin(uelement[4]);
  p[0] = 0.5 * (1.0 + cosi) * coslp + 0.5 * (1.0 - cosi) * cosrlp;
  p[1] = 0.5 * (1.0 + cosi) * sinlp - 0.5 * (1.0 - cosi) * sinrlp;
  p[2] = sinw * sini;

  /* Compute the Q vector */
  q[0] = -0.5 * (1.0 + cosi) * sinlp - 0.5 * (1.0 - cosi) * sinrlp;
  q[1] =  0.5 * (1.0 + cosi) * coslp - 0.5 * (1.0 - cosi) * cosrlp;
  q[2] = cosw * sini;

  /* Solve Kepler's equation */
  Conway(uelement, mu, jed, &rcosnu, &rsinnu);

  /* Compute magnitude of radius vector */
  r = sqrt(rcosnu * rcosnu + rsinnu * rsinnu);
  cosnu = rcosnu / r;
  sinnu = rsinnu / r;

  /* Compute heliocentric ecliptic position vector */
  for (i = 0; i < 3; i++) {
    posvel[i] = p[i] * rcosnu + q[i] * rsinnu;
  }

  /* Compute heliocentric ecliptic velocity vector */
  param = uelement[0] * (1.0 + uelement[1]);

  for (i = 3; i < 6; i++) {
    posvel[i]   = -p[i-3] * sqrt(mu / param) * sinnu;
    posvel[i] = posvel[i] + q[i-3] * sqrt(mu / param) *
      (uelement[1] + cosnu);
  }
};

/*********************************************************************
Name:    Interp1
Purpose: Subprogram to perform interpolation on a list
         of tabular values to find maxima/minima or
         to find the zero of a function within the limits
         of the table.  The algorithms used are those of Meeus.
Inputs:  x[] - Array of function arguments.
         y[] - Array of function values at the
               corresponding arguments.
         L   - Equals 3 for 3-pt. interpolation.
               5 for 5-pt. interpolation.
         m   - Equals 0 for zero of function.
               1 for a extremum of the function.
Outputs: arg - Argument corresponding to the
               extremum or zero of the tabular function.
         v   - The value of the function corresponding to arg.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Interp1(double *x, double *y, int L, int m, double *arg, double *v) {

  double a, b, c, d, e, f, g, h, j, k, nm, n0, n1;

  if (L == 3) {
    /* Use a 3 pt. table */
    a = y[1] - y[0];
    b = y[2] - y[1];
    c = b - a;
    if (m == 1) {
      /* Find extremum of the tabular function */
      *v = y[1] - (a + b) * (a + b) / (8.0 * c);
      nm = -(a + b) / (2.0 * c);
      *arg = x[1] + nm * (x[1] - x[0]);
    } else {
      n1 = 0.0; /* Find zero of the tabular function */
      do {
        n0 = -2.0 * y[1] / (a + b + c * n1);
        if (fabs(n0 - n1) < 0.000000000000001)
          break;
        n1 = n0;
      } while (1 == 1);
      *v = y[1] + 0.5 * n0 * (x[1] - x[0]) * (a + b + n0 * c);
      *arg = x[1] + n0 * (x[1] - x[0]);
    }
  } else {
    /* Use a 5 pt. table */
    a = y[1] - y[0];
    b = y[2] - y[1];
    c = y[3] - y[2];
    d = y[4] - y[3];
    e = b - a;
    f = c - b;
    g = d - c;
    h = f - e;
    j = g - f;
    k = j - h;
    if (m == 0) {
      /* Find extremum of tabular function */
      n1 = 0.0;
      do {
        nm = (6.0 * b + 6.0 * c - h - j + 3.0 * n1 * n1 *
          (h + j) + 2.0 * n1 * n1 * n1 * k) / (k - 12.0 * f);
        if (fabs(nm - n1) < 0.000000000000001)
          break;
        n1 = nm;
      } while (1 == 1);
      nm = nm * (x[3] - x[2]);
      *arg = x[2] + nm;
      *v = y[2] + 0.5 * nm * (b + c) + 0.5 * nm * nm * f + nm
        * (nm * nm - 1.0) * (h + j) / 12.0 + nm * nm * (nm * nm - 1.0)
        * k / 24.0;
    } else {
      /* Find zero of the tabular function */
      n1 = 0.0;
      do {
        n0 = (-24.0 * y[2] + n1 * n1 * (k - 12.0 * f) - 2.0 *
          n1 * n1 * n1 * (h + j) - n1 * n1 * n1 * n1 * k) / (2.0
          * (6.0 * b + 6.0 * c - h - j));
        if (fabs(n0 - n1) < 0.000000000000001)
          break;
        n1 = n0;
      } while (1 == 1);
      n0 = n0 * (x[3] - x[2]);
      *arg = x[2] + n0;
      *v = y[2] + 0.5 * n0 * (b + c) + 0.5 * n0 * n0 * f + n0 * (n0 * n0 - 1.0) * (h + j) / 12.0 + n0 * n0 * (n0 * n0 - 1.0) * k / 24.0;
    }
  }
}

/*********************************************************************
Name:    Interpol
Purpose: Function for 3 or 5 point interpolation
         using Meeus' algorithm.
Inputs:  x[] - Aarray of function arguments.
         y[] - Array of function values at
               the three arguments.
         i   - Switch indicating whether a
               3 or 5 point interpolation
               is performed (i=3 for 3 pt.,
               i=5 for 5 pt.).
         arg - Argument for which a function value
               is needed.
Outputs: None.
Returns: Interpolated value.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double Interpol(double *x, double *y, int i, double arg) {

  double d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, n;

  if (i == 3) {
    d1 = y[1] - y[0];
    d2 = y[2] - y[1];
    d3 = d2 - d1;
    n = (arg - x[1]) / (x[1] - x[0]);
    return (y[1] + (n / 2.0) * (d1 + d2 + n * d3));
  } else {
    d1 = y[1] - y[0];
    d2 = y[2] - y[1];
    d3 = y[3] - y[2];
    d4 = y[4] - y[3];
    d5 = d2 - d1;
    d6 = d3 - d2;
    d7 = d4 - d3;
    d8 = d6 - d5;
    d9 = d7 - d6;
    d10 = d9 - d8;
    n = (arg - x[2]) / (x[2] - x[1]);
    return (y[2] + n * ((d2 + d3) / 2.0 - (d8 + d9) / 12.0) +
      n * n * (d6 / 2.0 - d10 / 24.0) + n * n * n *
      ((d8 + d9) / 12.0) + n * n * n * n * (d10 / 24.0));
  }
}

/*********************************************************************
Name:    JED2Cal
Purpose: Function to convert a JED to an ordinary calendar date.
         The algorithm is that of Meeus.
Inputs:  jed - Julian Ephemeris Date.
Outputs: yr - Year.
         mo - Month.
         dy - Day.
         ti - Time of day in decimal hours.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void JED2Cal(double jed, int *yr, int *mo, int *dy, double *ti) {

  static double jedz;
  double z, f, a, b, c, d, e, alpha, daywtime;

  if (jed == jedz) return;
  jedz = jed;

  z = floor(jed + 0.5);
  f = (jed + 0.5) - z + 5.0e-10; /* fudge factor */
  if (z < 2299161.0) {
    a = z;
  } else {
    alpha = floor((z - 1867216.25) / 36524.25);
    a = z + 1.0 + alpha - floor(0.25 * alpha);
  }

  b = a + 1524.0;
  c = floor((b - 122.1) / 365.25);
  d = floor(365.25 * c);
  e = floor((b - d) / 30.6001);
  daywtime = b - d - floor(30.6001 * e) + f;
  *dy = (int) floor(daywtime);
  *ti = 24.0 * (daywtime - (double) *dy);

  if (e  < 13.5) *mo = (int) floor(e - 1.0);
  if (e  > 13.5) *mo = (int) floor(e - 13.0);
  if (*mo > 2.5) *yr = (int) floor(c - 4716.0);
  if (*mo < 2.5) *yr = (int) floor(c - 4715.0);
}

/*********************************************************************
Name:    JED2Epoch
Purpose: Function to convert a given JED to its corresponding Julian
         or Besselian epoch. For example, 2451545.0 becomes J2000.
Inputs:  jed - Input jed.
         s   - J or B, whichever is desired.
Outputs: epoch - J or B epoch as a string.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void JED2Epoch(double jed, char *s, char *epoch) {

  char d[80];
  double date;

  if (strcmp(s,"J") == 0) {
    date = 2000.0 + (jed - 2451545.0) / 365.25;
    sprintf(d, "%f", date);
  } else {
    date = 1900.0 + (jed - (double) 2415020.31352) /
      (double) 365.242198781731;
    sprintf(d, "%f", date);
  }

  strcpy(epoch, s);
  strcat(epoch, d);
}


/*********************************************************************
Name:    matrix
Purpose: Function that allocates a double matrix with dimensions
         m[nrow][ncol].
Inputs:  nrow - Number of rows.
         ncol - Number of columns.
Outputs: None.
Returns: Pointer to 2-D matrix of doubles.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double **matrix(int nrow, int ncol) {

  long i;
  double **m;

  /* Allocate pointer to rows */
  m = (double **) malloc(nrow * sizeof(double *));
  if (!m) {
    LogMsg(stderr, "matrix(): allocation failure 1\n");
    exit(1);
  }

  /* Allocate rows and set pointers to them */
  for (i = 0;i < nrow;i++) {
    m[i] = (double *) malloc(ncol * sizeof(double));
    if (!m[i]) {
      LogMsg(stderr, "matrix(): allocation failure 2\n");
      exit(1);
    }
  }
  /* return pointer to array of pointers to rows */
  return m;
}

/*********************************************************************
Name:    MatXMat
Purpose: Square matrix multiplication subroutine.
         NOTE: If a[][] and c[][] have the same name in the
         calling program, the original a[][] matrix is overwritten.
Inputs:  a[][] - Zero-offset matrix a.
         b[][] - Zero-offset matrix b.
         n     - Dimension of matrix.
Outputs: c[][] - Zero-offset matrix a x b.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void MatXMat(DMatrix a, DMatrix b, DMatrix c, int n) {

  int i, j, k;

  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      c[i][j] = 0.0;
      for (k=0; k<n; k++) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

/*********************************************************************
Name:    MatXVec
Purpose: Matrix/vector multiplication subroutine.
Inputs:  a[][] - Zero-offset matrix a (l rows by m columns).
         b[]   - Zero-offset vector b (m rows by 1 column).
         l     - Dimension of vector (i.e. lx1).
         m     - Dimension of matrix (i.e. mxl).
Outputs: c[]   - Zero-offset vector a x b (l rows by 1 column).
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void MatXVec (DMatrix a, double b[], double c[], int l, int m) {

  int i, j;
  double s, temp[6];

  for (i=0; i<l; i++) {
    s = 0.0;
    for (j=0; j<m; j++) {
      s += a[i][j] * b[j];
    }
    temp[i] = s;
  }

  for (i=0; i<l; i++) {
    c[i] = temp[i];
  }
}

/*********************************************************************
Name:    MRotate
Purpose: Function to perform a matrix rotation of an input vector
         through a given angle about a desired axis. A right-handed
         orthogonal coordinate system is assumed, and a 3X3
         rotation matrix is used, not a Q-matrix.
         NOTE: The vin[] and vout[] can have the same name in the
         calling program.
Inputs:  vin[] - Zero-offset input vector.
         axis  - 1 for rot. about x-axis,
                 2 for rot. about y-axis,
                 3 for rot. about z-axis.
         phi   - Rotation angle in radians.
Outputs: vout[] - Zero-offset transformed vector.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void MRotate(double vin[], int axis, double phi, double vout[]) {

  double cosphi, sinphi, T;

  cosphi = cos(phi);
  sinphi = sin(phi);

  switch (axis) {
    case 1:  /* rotate about x-axis */
      T = cosphi * vin[1] + sinphi * vin[2];
      vout[2] = -sinphi * vin[1] + cosphi * vin[2];
      vout[1] = T;
      vout[0] = vin[0];
      break;
    case 2:  /* rotate about y-axis */
      T = cosphi * vin[0] - sinphi * vin[2];
      vout[2] = sinphi * vin[0] + cosphi * vin[2];
      vout[0] = T;
      vout[1] = vin[1];
      break;
    case 3:  /* rotate about z-axis */
      T = cosphi * vin[0] + sinphi * vin[1];
      vout[1] = -sinphi * vin[0] + cosphi * vin[1];
      vout[0] = T;
      vout[2] = vin[2];
      break;
    default:
      LogMsg(stderr,"MRotate: axis not valid.\n");
      exit(1);
  }
}

/*********************************************************************
Name:    GetDpsiDeps
Purpose: Function to compute dpsi, deps, dpsidot, and depsdot.
Inputs:  jed - JED on TDB scale.
Outputs: dpsi    - Nutation in longitude.
         deps    - Nutation in obliquity.
         dpsidot - Derivative of dpsi in radians/day.
         depsdot - Derivative of deps in radians/day.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void GetDpsiDeps(double jed, double *dpsi, double *deps,
  double *dpsidot, double *depsdot) {

  double L, Ldot, LP, LPdot, F, Fdot, D, Ddot, N, Ndot, LL, LLdot;
  double T, c1, c2, c3, c4, c5, lamp, oamp, ls, os, arg, argdot;
  double funarg[12];
  int j;
  static double dpsiz, depsz, dpsidotz, depsdotz, jedz;

  /* 1980 IAU nutation coefficients */
  static double nc[106][9] = {
    {-171996., -174.2, 92025., 8.9,  0., 0., 0., 0., 1.},
    {   2062.,    0.2,  -895., 0.5,  0., 0., 0., 0., 2.},
    {     46.,     0.,   -24.,  0., -2., 0., 2., 0., 1.},
    {     11.,     0.,     0.,  0.,  2., 0.,-2., 0., 0.},
    {     -3.,     0.,     1.,  0., -2., 0., 2., 0., 2.},
    {     -3.,     0.,     0.,  0.,  1.,-1., 0.,-1., 0.},
    {     -2.,     0.,     1.,  0.,  0.,-2., 2.,-2., 1.},
    {      1.,     0.,     0.,  0.,  2., 0.,-2., 0., 1.},
    { -13187.,   -1.6,  5736., -3.1, 0., 0., 2.,-2., 2.},
    {   1426.,   -3.4,    54., -0.1, 0., 1., 0., 0., 0.},
    {   -517.,    1.2,   224., -0.6, 0., 1., 2.,-2., 2.},
    {    217.,   -0.5,   -95.,  0.3, 0.,-1., 2.,-2., 2.},
    {    129.,    0.1,   -70.,  0.,  0., 0., 2.,-2., 1.},
    {     48.,     0.,     1.,  0.,  2., 0., 0.,-2., 0.},
    {    -22.,     0.,     0.,  0.,  0., 0., 2.,-2., 0.},
    {     17.,   -0.1,     0.,  0.,  0., 2., 0., 0., 0.},
    {    -15.,     0.,     9.,  0.,  0., 1., 0., 0., 1.},
    {    -16.,    0.1,     7.,  0.,  0., 2., 2.,-2., 2.},
    {    -12.,     0.,     6.,  0.,  0.,-1., 0., 0., 1.},
    {     -6.,     0.,     3.,  0., -2., 0., 0., 2., 1.},
    {     -5.,     0.,     3.,  0.,  0.,-1., 2.,-2., 1.},
    {      4.,     0.,    -2.,  0.,  2., 0., 0.,-2., 1.},
    {      4.,     0.,    -2.,  0.,  0., 1., 2.,-2., 1.},
    {     -4.,     0.,     0.,  0.,  1., 0., 0.,-1., 0.},
    {      1.,     0.,     0.,  0.,  2., 1., 0.,-2., 0.},
    {      1.,     0.,     0.,  0.,  0., 0.,-2., 2., 1.},
    {     -1.,     0.,     0.,  0.,  0., 1.,-2., 2., 0.},
    {      1.,     0.,     0.,  0.,  0., 1., 0., 0., 2.},
    {      1.,     0.,     0.,  0., -1., 0., 0., 1., 1.},
    {     -1.,     0.,     0.,  0.,  0., 1., 2.,-2., 0.},
    {  -2274.,   -0.2,   977., -0.5, 0., 0., 2., 0., 2.},
    {    712.,    0.1,    -7.,   0., 1., 0., 0., 0., 0.},
    {   -386.,   -0.4,   200.,   0., 0., 0., 2., 0., 1.},
    {   -301.,     0.,   129., -0.1, 1., 0., 2., 0., 2.},
    {   -158.,     0.,    -1.,   0., 1., 0., 0.,-2., 0.},
    {    123.,     0.,   -53.,   0.,-1., 0., 2., 0., 2.},
    {     63.,     0.,    -2.,   0., 0., 0., 0., 2., 0.},
    {     63.,    0.1,   -33.,   0., 1., 0., 0., 0., 1.},
    {    -58.,   -0.1,    32.,   0.,-1., 0., 0., 0., 1.},
    {    -59.,     0.,    26.,   0.,-1., 0., 2., 2., 2.},
    {    -51.,     0.,    27.,   0., 1., 0., 2., 0., 1.},
    {    -38.,     0.,    16.,   0., 0., 0., 2., 2., 2.},
    {     29.,     0.,    -1.,   0., 2., 0., 0., 0., 0.},
    {     29.,     0.,   -12.,   0., 1., 0., 2.,-2., 2.},
    {    -31.,     0.,    13.,   0., 2., 0., 2., 0., 2.},
    {     26.,     0.,    -1.,   0., 0., 0., 2., 0., 0.},
    {     21.,     0.,   -10.,   0.,-1., 0., 2., 0., 1.},
    {     16.,     0.,    -8.,   0.,-1., 0., 0., 2., 1.},
    {    -13.,     0.,     7.,   0., 1., 0., 0.,-2., 1.},
    {    -10.,     0.,     5.,   0.,-1., 0., 2., 2., 1.},
    {     -7.,     0.,     0.,   0., 1., 1., 0.,-2., 0.},
    {      7.,     0.,    -3.,   0., 0., 1., 2., 0., 2.},
    {     -7.,     0.,     3.,   0., 0.,-1., 2., 0., 2.},
    {     -8.,     0.,     3.,   0., 1., 0., 2., 2., 2.},
    {      6.,     0.,     0.,   0., 1., 0., 0., 2., 0.},
    {      6.,     0.,    -3.,   0., 2., 0., 2.,-2., 2.},
    {     -6.,     0.,     3.,   0., 0., 0., 0., 2., 1.},
    {     -7.,     0.,     3.,   0., 0., 0., 2., 2., 1.},
    {      6.,     0.,    -3.,   0., 1., 0., 2.,-2., 1.},
    {     -5.,     0.,     3.,   0., 0., 0., 0.,-2., 1.},
    {      5.,     0.,     0.,   0., 1.,-1., 0., 0., 0.},
    {     -5.,     0.,     3.,   0., 2., 0., 2., 0., 1.},
    {     -4.,     0.,     0.,   0., 0., 1., 0.,-2., 0.},
    {      4.,     0.,     0.,   0., 1., 0.,-2., 0., 0.},
    {     -4.,     0.,     0.,   0., 0., 0., 0., 1., 0.},
    {     -3.,     0.,     0.,   0., 1., 1., 0., 0., 0.},
    {      3.,     0.,     0.,   0., 1., 0., 2., 0., 0.},
    {     -3.,     0.,     1.,   0., 1.,-1., 2., 0., 2.},
    {     -3.,     0.,     1.,   0.,-1.,-1., 2., 2., 2.},
    {     -2.,     0.,     1.,   0.,-2., 0., 0., 0., 1.},
    {     -3.,     0.,     1.,   0., 3., 0., 2., 0., 2.},
    {     -3.,     0.,     1.,   0., 0.,-1., 2., 2., 2.},
    {      2.,     0.,    -1.,   0., 1., 1., 2., 0., 2.},
    {     -2.,     0.,     1.,   0.,-1., 0., 2.,-2., 1.},
    {      2.,     0.,    -1.,   0., 2., 0., 0., 0., 1.},
    {     -2.,     0.,     1.,   0., 1., 0., 0., 0., 2.},
    {      2.,     0.,     0.,   0., 3., 0., 0., 0., 0.},
    {      2.,     0.,    -1.,   0., 0., 0., 2., 1., 2.},
    {      1.,     0.,    -1.,   0.,-1., 0., 0., 0., 2.},
    {     -1.,     0.,     0.,   0., 1., 0., 0.,-4., 0.},
    {      1.,     0.,    -1.,   0.,-2., 0., 2., 2., 2.},
    {     -2.,     0.,     1.,   0.,-1., 0., 2., 4., 2.},
    {     -1.,     0.,     0.,   0., 2., 0., 0.,-4., 0.},
    {      1.,     0.,    -1.,   0., 1., 1., 2.,-2., 2.},
    {     -1.,     0.,     1.,   0., 1., 0., 2., 2., 1.},
    {     -1.,     0.,     1.,   0.,-2., 0., 2., 4., 2.},
    {      1.,     0.,     0.,   0.,-1., 0., 4., 0., 2.},
    {      1.,     0.,     0.,   0., 1.,-1., 0.,-2., 0.},
    {      1.,     0.,    -1.,   0., 2., 0., 2.,-2., 1.},
    {     -1.,     0.,     0.,   0., 2., 0., 2., 2., 2.},
    {     -1.,     0.,     0.,   0., 1., 0., 0., 2., 1.},
    {      1.,     0.,     0.,   0., 0., 0., 4.,-2., 2.},
    {      1.,     0.,     0.,   0., 3., 0., 2.,-2., 2.},
    {     -1.,     0.,     0.,   0., 1., 0., 2.,-2., 0.},
    {      1.,     0.,     0.,   0., 0., 1., 2., 0., 1.},
    {      1.,     0.,     0.,   0.,-1.,-1., 0., 2., 1.},
    {     -1.,     0.,     0.,   0., 0., 0.,-2., 0., 1.},
    {     -1.,     0.,     0.,   0., 0., 0., 2.,-1., 2.},
    {     -1.,     0.,     0.,   0., 0., 1., 0., 2., 0.},
    {     -1.,     0.,     0.,   0., 1., 0.,-2.,-2., 0.},
    {     -1.,     0.,     0.,   0., 0.,-1., 2., 0., 1.},
    {     -1.,     0.,     0.,   0., 1., 1., 0.,-2., 1.},
    {     -1.,     0.,     0.,   0., 1., 0.,-2., 2., 0.},
    {      1.,     0.,     0.,   0., 2., 0., 0., 2., 0.},
    {     -1.,     0.,     0.,   0., 0., 0., 2., 4., 2.},
    {      1.,     0.,     0.,   0., 0., 1., 0., 1., 0.}
  };

  if (jedz != jed) {
    jedz = jed;

    T = (jed - J2000) / DJC;

    FunArgIAU(jed, funarg);
    L = funarg[0];
    Ldot = funarg[6];
    LP = funarg[1];
    LPdot = funarg[7];
    F  = funarg[2];
    Fdot  = funarg[8];
    D  = funarg[3];
    Ddot  = funarg[9];
    N  = funarg[4];
    Ndot  = funarg[10];
    LL = funarg[5];
    LLdot = funarg[11];

    /* evaluate the series */
    dpsiz = 0.;  /* initialize to zero */
    depsz = 0.;
    dpsidotz = 0.;
    depsdotz = 0.;
    for (j=0; j<106; j++) {
      lamp = nc[j][0];
      oamp = nc[j][2];
      ls   = nc[j][1];
      os   = nc[j][3];
      c1   = nc[j][4];
      c2   = nc[j][5];
      c3   = nc[j][6];
      c4   = nc[j][7];
      c5   = nc[j][8];
      arg  = c1 * L + c2 * LP + c3 * F + c4 * D + c5 * N;
      arg  = amodulo(arg, D2PI);
      dpsiz += (lamp + ls * T) * sin(arg);
      depsz += (oamp + os * T) * cos(arg);
      argdot = c1 * Ldot + c2 * LPdot + c3 * Fdot + c4 * Ddot + c5 * Ndot;
      argdot = amodulo(argdot, D2PI);
      dpsidotz += (lamp + ls * T) * argdot * cos(arg) +
        ls * sin(arg) / DJC;
      depsdotz -= (oamp + os * T) * argdot * sin(arg) +
        os * cos(arg) / DJC;
    }

    /* normalize and convert units */
    dpsiz *= 0.0001 * DAS2R;
    depsz *= 0.0001 * DAS2R;
    dpsidotz *= 0.0001 * DAS2R;
    depsdotz *= 0.0001 * DAS2R;
  }

  *dpsi = dpsiz;
  *deps = depsz;
  *dpsidot = dpsidotz;
  *depsdot = depsdotz;
}

/*********************************************************************
Name:    Obliquity
Purpose: Function to compute the mean obliquity of the ecliptic
         and its derivative using Lieske's formula.
Inputs:  jed1 - Initial JED on the TDB scale.
         jed2 - Final JED on the TDB scale.
         m    - 0 for mean obliquity,
                1 for true obliquity.
Outputs: obl    - Obliquity of the ecliptic in radians.
         obldot - Derivative in radians/day.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Obliquity(double jed1, double jed2, int m, double *obl,
  double *obldot) {

  double t1, t2, e0, e1, e2, e3, e4, e5, e6, epsbar;
  double dpsi, deps, dpsidot, depsdot;

  t1 = (jed1 - J2000) / DJC;
  t2 = (jed2 - jed1) / DJC;

  e0 = 84381.448;
  e1 =   -46.815;
  e2 =    -0.00059;
  e3 =     0.001813;
  epsbar = e0 + e1 * t1 + e2 * t1 * t1 + e3 * t1 * t1 * t1;
  e1 = -46.815;
  e2 =  -0.00117;
  e3 =   0.005439;
  e4 =  -0.00059;
  e5 =   0.005439;
  e6 =   0.001813;
  *obl = epsbar + (e1 + t1 * (e2 + t1 * e3)) * t2
    + (e4 + e5 * t1) * t2 * t2
    + e6 * t2 * t2 * t2;
  *obldot = e1 + t1 * (e2 + t1 * e3)
    + 2.0 * (e4 + e5 * t1) * t2
    + 3.0 * e6 * t2 * t2;

  if (m == 1) {
    /* need true obliquity */
    GetDpsiDeps(jed2, &dpsi, &deps, &dpsidot, &depsdot);
    /* Unit conversion is needed because obl is  */
    /* in arc seconds, nutations are in radians. */
    *obl = *obl + deps * DR2AS;
    *obldot = *obldot + depsdot * DR2AS;
  }

  /* convert to radians and radians/day */
  *obl = *obl * DAS2R;
  *obldot = *obldot * DAS2R / DJC;
}

/*********************************************************************
Name:    Pol2Rec
Purpose: Function to convert a polar state vector into a cartesian
         state vector.
         NOTE:  THIS ROUTINE EXPECTS THE POLAR VELOCITY VECTOR TO BE
         THE TOTAL VELOCITY CORRECTED FOR THE EFFECT OF LATITUDE.
Inputs:  a[] - Zero-offset polar state vector.
Outputs: b[] - Zero-offset cartesian state vector.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Pol2Rec(double a[], double b[]) {

  double lambda, beta, R, v_lambda, v_beta, v_r;
  double lambda_dot, beta_dot, r_dot, CosL, SinL, CosB, SinB;

  lambda   = a[0];
  beta     = a[1];
  R        = a[2];
  v_lambda = a[3];
  v_beta   = a[4];
  v_r      = a[5];

  /* separate the angluar derivatives from the total velocity components */
  CosL = cos(lambda);
  SinL = sin(lambda);
  CosB = cos(beta);
  SinB = sin(beta);

  lambda_dot = v_lambda / (R * CosB);
  beta_dot = v_beta / R;
  r_dot = v_r;

  /* position vector components */
  b[0] = R * CosL * CosB;
  b[1] = R * SinL * CosB;
  b[2] = R * SinB;

  /* velocity vector components */
  b[3] = r_dot * CosL * CosB - R * lambda_dot * SinL * CosB -
    R * beta_dot * CosL * SinB;
  b[4] = r_dot * SinL * CosB + R * lambda_dot * CosL * CosB -
    R * beta_dot * SinL * SinB;
  b[5] = r_dot * SinB + R * beta_dot * CosB;
}

/*********************************************************************
Name:    PrecessElements
Purpose: Subprogram to transform angular orbital elements
         from equinox eqnx1 to equinox eqnx2.
Inputs:  eqnx1   - String containing initial
                   eqninox.  e.g. B1950 or 2415020.5.
         element - Array containing elements
                   referred to eqnx1 as follows:
                   element[0] = inclination.
                   element[1] = long. of asc. node.
                   element[2] = arg. of peri.
         eqnx2   - String containing final
                   eqninox.  e.g. J2000 or 2451545.
Outputs: element[] - Array containing elements referred to eqnx2.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void PrecessElements(char *eqnx1, double *element, char *eqnx2) {

  double  jd1, jd2, tt, t, tt2, t2, t3, tmp;
  double  SmallPI, LargePI, pa, cosi, sini, sinisinn, sinicosn;
  double  newi, newnode, sindwsini, cosdwsini, dw, newargp;

  ucase(eqnx1);
  ucase(eqnx2);
  if ((eqnx1[0] == 'B') || (eqnx1[0] == 'J')) {
    Epoch2JED(eqnx1, &jd1);
  } else {
    jd1 = atof(eqnx1);
  }

  if ((eqnx2[0] == 'B') || (eqnx2[0] == 'J')) {
    Epoch2JED(eqnx2, &jd2);
  } else {
    jd2 = atof(eqnx2);
  }

  tt = (jd1 - 2451545.0) / 365250.0;
  t = (jd2 - jd1) / 365250.0;
  tt2 = tt * tt;
  t2 = t * t;
  t3 = t2 * t;

  /* Compute precessional quantities */
  SmallPI = (470.029 - 6.603 * tt + 0.598 * tt2) * t +
    (-3.302 + 0.598 * tt) * t2 + 0.06 * t3;
  LargePI = 174.8763838888889 + (32894.789 * tt) / 3600.0 +
    (60.622 * tt2) / 3600.0 + ((-8698.089 - 50.491 * tt) * t) /
    3600.0 + (3.536 * t2) / 3600.0;
  pa = (50290.966 + 222.226 * tt - 0.042 * tt2) * t +
    (111.113 - 0.042 * tt) * t2 - 0.006 * t3;
  SmallPI = SmallPI * DAS2R;
  LargePI = LargePI * DD2R;
  pa = pa * DAS2R;

  /* Compute new inclination and node */
  cosi = cos(element[0]) * cos(SmallPI) + sin(element[0]) *
    sin(SmallPI) * cos(LargePI - element[1]);

  sinisinn = sin(element[0]) * sin(LargePI - element[1]);
  sinicosn = -sin(SmallPI) * cos(element[0]) + cos(SmallPI) *
    sin(element[0]) * cos(LargePI - element[1]);

  sini = sqrt(sinisinn * sinisinn + sinicosn * sinicosn);
  newi = atan2(sini, cosi);
  if (newi < 0.0)
    newi += D2PI;

  tmp = atan2(sinisinn, sinicosn);
  if (tmp < 0.0)
    tmp += D2PI;
  newnode = pa + LargePI - tmp;
  newnode = amodulo(newnode, D2PI);

  /* Compute new argument of perihelion */
  sindwsini = sin(SmallPI) * sin(LargePI - element[1]);
  cosdwsini = sin(element[0]) * cos(SmallPI) - cos(element[0]) *
    sin(SmallPI) * cos(LargePI - element[1]);
  dw = atan2(sindwsini, cosdwsini);
  if (dw < 0.0)
    dw += D2PI;

  newargp = element[2] + dw;
  newargp = amodulo(newargp, D2PI);

  /* Put new elements in element[] array */
  element[0] = newi;
  element[1] = newnode;
  element[2] = newargp;
}

/*********************************************************************
Name:    QRotate
Purpose: Function to perform a matrix rotation of a state vector through
         a given angle about a desired axis.  A right-handed orthogonal
         coordinate system is assumed, and a 6X6 Q-matrix is used.
         NOTE: The vin[] and vout[] can have the same name in
         the calling program.
Inputs:  vin    - Zero-offset input state vector.
         axis   - 1 for x-axis,
                  2 for y-axis,
                  3 for z-axis.
         phi    - Rotation angle in radians.
         phidot - Derivative of phi.
         s      - 0 for inertial to inertial,
                  1 for inertial to rotating.
Outputs: vout[] - Zero-offset transformed state vector.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void QRotate(double vin[], int axis, double phi, double phidot,
  int s, double vout[]) {

  int i;
  double temp[6];
  DMatrix QMatrix;

  QMatrix = createDMatrix(6);

  GetQMatrix(phi, phidot, axis, s, QMatrix);

  for (i=0; i<6; i++) {
    temp[i] = vin[i];
  }

  MatXVec(QMatrix, temp, vout, 6, 6);

  freeDMatrix(QMatrix, 6);
}

/*********************************************************************
Name:    RayBend
Purpose: Function to correct an input vector for relativistic light
         deflection due to the Sun's gravity field.
Inputs:  earth_hel[] - Zero-offset heliocentric state vector of Earth.
         body_geo[]  - Zero-offset geocentric state vector of object.
         body_hel[]  - Zero-offset heliocentric state vector of object.
Outputs: p1[] - Zero-offset geocentric state vector of object
                corrected for light deflection.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void RayBend(double earth_hel[], double body_geo[], double body_hel[],
  double p1[6]) {

  double ue[3], up[3], uq[3], E[3], Edot[3], P[3], Pdot[3], Q[3], Qdot[3];
  double magE, magP, pdotq, edotp, qdote;
  int i;

  /* extract the pos. portions of the state vectors */
  SplitStateVector(earth_hel, E, Edot);
  SplitStateVector( body_geo, P, Pdot);
  SplitStateVector( body_hel, Q, Qdot);

  /* form unit vectors */
  Uvector(E, ue);
  Uvector(P, up);
  Uvector(Q, uq);

  /* form dot products and other quantities */
  magE = Vecmag(E);
  magP = Vecmag(P);
  Vdot(3, up, uq, &pdotq);
  Vdot(3, ue, up, &edotp);
  Vdot(3, uq, ue, &qdote);

  for (i=0; i<3; i++) {
    p1[i] = up[i] + (MUC / magE) * (pdotq * ue[i] - edotp * uq[i])
      / (1.0 + qdote);
    /* make p1[] a non-unit vector */
    p1[i] = magP * p1[i];
    p1[i+3] = body_geo[i+3];
  }
}

/*********************************************************************
Name:    Rec2Pol
Purpose: Subprogram to convert a cartesian state vector into a
         polar state vector. NOTE: THE POLAR VELOCITY VECTOR IS
         THE TOTAL VELOCITY, CORRECTED FOR THE EFFECT OF LATITUDE.
Inputs:  a[] - Zero-offset cartesian state vector.
Outputs: b[] - Zero-offset polar state vector.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Rec2Pol(double a[], double b[]) {

  double x, y, z, x_dot, y_dot, z_dot,
    rho, r, lambda, beta, lambda_dot,
    beta_dot, r_dot;

  x = a[0];
  y = a[1];
  z = a[2];
  x_dot = a[3];
  y_dot = a[4];
  z_dot = a[5];

  rho = sqrt(x * x + y * y);
    r = sqrt(rho * rho + z * z);

  lambda = atan2(y, x);
  if (lambda < 0.0)
    lambda += D2PI;

  beta   = atan2(z, rho);
  if (beta < 0.0)
    beta += D2PI;

  if (z < 0) {
    beta = beta - D2PI;
  }

  if (rho == 0) {
    lambda_dot = 0.0;
    beta_dot = 0.0;
  } else {
    lambda_dot = (x*y_dot-y*x_dot) / (rho*rho);
    beta_dot = (z_dot*rho*rho-z*(x*x_dot+y*y_dot))/(r*r*rho);
  }

  r_dot = (x * x_dot + y * y_dot + z * z_dot) / r;

  /* position vector components */
  b[0] = lambda;
  if (b[0] >= D2PI)
    b[0] = b[0] - D2PI;

  b[1] = beta;
  b[2] = r;
  /* total velocity vector components */
  b[3] = r * lambda_dot * cos(beta);
  b[4] = r * beta_dot;
  b[5] = r_dot;
};

void Reduce(TLocation *, double jed, int body, int place, double StarData[], double p3[])
  {
};

/*********************************************************************
Name:    Refract
Purpose: Subprogram to correct right ascension and declination
         for atmospheric refraction.
         Reference:  Taff. Computational Spherical Astronomy, pp. 78-80.
Inputs:  ra1   - Uncorrected RA and DEC in radians.
         dec1  - Uncorrected RA and DEC in radians.
         lha   - Local apparent hour angle in radians.
         temp  - Temperature in deg F.
         press - Air pressure in inches of Hg.
Outputs: ra2  - Corrected RA and DEC in radians.
         dec2 - Corrected RA and DEC in radians.
Returns: Nothing.

Status:  Finished.
Errors:  None known.
*********************************************************************/
void Refract(TLocation *Location, double ra1, double dec1, double lha, double temp, double press, double *ra2, double *dec2) {

  double cosz, z, r1, r2, r, tanzr, rr, diff, rlocal, denom;

  cosz = sin(Location->latitude()*DD2R) * sin(dec1) + cos(Location->latitude()*DD2R) * cos(dec1) * cos(lha);
  z = acos(cosz);

  r1 = 58.294 * DAS2R;
  r2 = -0.0668 * DAS2R;

  r = 0.0;
  do {
    tanzr = tan(z - r);
    rr = r1 * tanzr + r2 * tanzr * tanzr * tanzr;
    printf("%f\n", rr * DR2AS);
    diff = fabs(rr - r);
    r = rr;
  } while (diff > 1e-15);

  rlocal = (17.0 * press) * r / (460.0 + temp);

  denom = 1.0 / (cos(dec1) * sin(z));
  *dec2 = dec1 + (rlocal * (sin(Location->latitude()*DD2R) - sin(dec1) * cos(z))) * denom;
  denom = 1.0 / (cos(*dec2) * sin(z));
  *ra2 = ra1 + (rlocal * cos(Location->latitude()*DD2R) * sin(lha)) * denom;
}

/*********************************************************************
Name:    RotMat
Purpose: Function to compute the 3X3 rotation matrix and its partial
         derivative for a rotation about the Ith coordinate axis
         through an angle phi.
Inputs:  axis - 1 for x-axis,
                2 for y-axis,
                3 for z-axis.
         phi  - Rotation angle in radians.
Outputs: r[]  - Zero-offset 3X3 rotation matrix.
         dr[] - Zero-offset 3X3 partial derivative of the matrix r[].
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void RotMat(int axis, double phi, DMatrix r, DMatrix dr) {

  double cosphi, sinphi;

  cosphi = cos(phi);
  sinphi = sin(phi);

  switch (axis) {
    case 1:  /* rotate about x-axis */
      r[0][0] = 1.0;
      r[0][1] = 0.0;
      r[0][2] = 0.0;
      r[1][0] = 0.0;
      r[1][1] =  cosphi;
      r[1][2] =  sinphi;
      r[2][0] = 0.0;
      r[2][1] = -sinphi;
      r[2][2] =  cosphi;
      dr[0][0] = 0.0;
      dr[0][1] = 0.0;
      dr[0][2] = 0.0;
      dr[1][0] = 0.0;
      dr[1][1] = -sinphi;
      dr[1][2] =  cosphi;
      dr[2][0] = 0.0;
      dr[2][1] = -cosphi;
      dr[2][2] = -sinphi;
      break;
    case 2:  /* rotate about y-axis */
      r[0][0] =  cosphi;
      r[0][1] = 0.0;
      r[0][2] = -sinphi;
      r[1][0] = 0.0;
      r[1][1] = 1.0;
      r[1][2] = 0.0;
      r[2][0] =  sinphi;
      r[2][1] = 0.0;
      r[2][2] =  cosphi;
      dr[0][0] = -sinphi;
      dr[0][1] = 0.0;
      dr[0][2] = -cosphi;
      dr[1][0] = 0.0;
      dr[1][1] = 0.0;
      dr[1][2] = 0.0;
      dr[2][0] =  cosphi;
      dr[2][1] = 0.0;
      dr[2][2] = -sinphi;
      break;
    case 3:  /* rotate about z-axis */
      r[0][0] =  cosphi;
      r[0][1] =  sinphi;
      r[0][2] = 0.0;
      r[1][0] = -sinphi;
      r[1][1] =  cosphi;
      r[1][2] = 0.0;
      r[2][0] = 0.0;
      r[2][1] = 0.0;
      r[2][2] = 1.0;
      dr[0][0] = -sinphi;
      dr[0][1] =  cosphi;
      dr[0][2] = 0.0;
      dr[1][0] = -cosphi;
      dr[1][1] = -sinphi;
      dr[1][2] = 0.0;
      dr[2][0] = 0.0;
      dr[2][1] = 0.0;
      dr[2][2] = 0.0;
      break;
    default:
      LogMsg(stderr,"RotMat: axis not valid.\n");
      exit(1);
  }
};

/*********************************************************************
Name:    split
Purpose: Function to break a number into an integer part and a
         fractional part. For negative input numbers, 'ipart'
         contains the next more negative number and 'fpart' contains
         a positive fraction.
Inputs:  tt - Number to be split.
Outputs: ipart - Integer part.
         fpart - Fractional part.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void split(double tt, double *ipart, double *fpart) {

  *ipart = floor(tt);
  *fpart = tt - *ipart;

  if ((tt < 0) && (*fpart != 0)) {
    *ipart = *ipart - 1.0;
    *fpart = *fpart + 1.0;
  }
}

/*********************************************************************
Name:    SplitStateVector
Purpose: Subprogram to split a state vector into its position
         and velocity components.
Inputs:  pv[] - Zero-offset 6-d state vector.
Outputs: p[] - Zero-offset 3-d position vector.
         v[] - Zero-offset 3-d velocity vector.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void SplitStateVector(double pv[], double p[], double v[]) {

  int i;

  for (i=0; i<3; i++) {
    p[i] = pv[i];
    v[i] = pv[i+3];
  }
};


/*********************************************************************
Name:    Stat2Elem
Purpose: Subprogram to transform the components of a state vector into
         the universal orbital element set at a given time.
         Reference: Mansfield. 1986. AIAA Paper 86-2269-CP.
Inputs:  posvel[] - State vector.
         mu       - Gravitational const.
         jed      - Time.
Outputs: uelement[] - Array of universal elements:
                      uelement[0] = q.
                      uelement[1] = e.
                      uelement[2] = i.
                      uelement[3] = node.
                      uelement[4] = arg.peri.
                      uelement[5] = T.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Stat2Elem(double *posvel, double mu, double jed, double *uelement) {

  int i;
  double r[3], rdot[3], L[3], wvec[3], u[3], v[3], pvec[3], qvec[3];
  double magr, magL, p, vsquared, alpha, chi, psi, sini, cosi, sinn, cosn;
  double rnudot, rrdot, magrdot, esinnu, ecosnu, sinnu, cosnu;
  double z, w, h, s, x, c3, tspp;

  /* Get pos. and vel. vectors from state vector */
  r[0] = posvel[0];
  r[1] = posvel[1];
  r[2] = posvel[2];
  rdot[0] = posvel[3];
  rdot[1] = posvel[4];
  rdot[2] = posvel[5];

  /* Compute magr */
  magr = Vecmag(r);

  /* Compute angular momentum vector */
  Vcross(r, rdot, L);

  /* Compute magL */
  magL = Vecmag(L);

  /* Compute wvec[] */
  Uvector(L, wvec);

  /* Compute u[] */
  Uvector(r, u);

  /* Compute v[] */
  Vcross(wvec, u, v);

  /* Compute semilatus rectum */
  p = magL * magL / mu;

  /* Compute square of velocity */
  Vdot(3, rdot, rdot, &vsquared);

  /* Compute alpha */
  alpha = 2.0 * mu / magr - vsquared;

  /* Compute eccentricity */
  uelement[1] = sqrt(1.0 - alpha * magL * magL / (mu * mu));

  /* Compute perihelion distance */
  uelement[0] = p / (1.0 + uelement[1]);

  /* Compute node and inclination */
  /* First compute chi and psi */
  chi = wvec[0] / (1.0 + wvec[2]);
  psi = -wvec[1] / (1.0 + wvec[2]);
  /* Now get inclination */
  sini = 2.0 * sqrt(chi * chi + psi * psi) / (1.0 + chi * chi + psi * psi);
  cosi = (1.0 - chi * chi - psi * psi) / (1.0 + chi * chi + psi * psi);
  uelement[2] = atan2(sini, cosi);
  if (uelement[2] < 0.0)
    uelement[2] += D2PI;

  /* Now get node */
  sinn = chi / sqrt(chi * chi + psi * psi);
  cosn = psi / sqrt(chi * chi + psi * psi);
  uelement[3] = atan2(sinn, cosn);
  if (uelement[3] < 0.0)
    uelement[3] += D2PI;

  /* Compute arg. peri. */
  /* Compute rnudot */
  Vdot(3, rdot, v, &rnudot);
  /* Compute magrdot */
  Vdot(3, r, rdot, &rrdot);
  magrdot = rrdot / magr;
  esinnu = magrdot * sqrt(p / mu);
  ecosnu = rnudot * sqrt(p / mu) - 1.0;
  /* Proceed to compute arg. peri. */
  z = esinnu / (uelement[1] + ecosnu);
  sinnu = 2.0 * z / (1.0 + z * z);
  cosnu = (1.0 - z * z) / (1.0 + z * z);
  /* Get the pvec[] and qvec[] vectors */
  for (i = 0; i < 3; i++) {
    pvec[i] = u[i] * cosnu - v[i] * sinnu;
    qvec[i] = u[i] * sinnu + v[i] * cosnu;
  }

  /* Finally compute arg. peri. */
  uelement[4] = atan2(pvec[2], qvec[2]);
  if (uelement[4] < 0.0)
    uelement[4] += D2PI;

  /* Compute time of peri. passage */
  w = sqrt(uelement[0] / (mu * (1.0 + uelement[1]))) * z;
  h = alpha * w * w;
  s = 2.0 * ((((((h / 13.0 - 1.0 / 11.0) * h + 1.0 / 9.0) * h - 1.0 / 7.0)
    * h + 1.0 / 5.0) * h - 1.0 / 3.0) * h + 1.0) * w;
  /* Compute Stumpff functions */
  x = alpha * s * s;
  c3 = StumpffN(x, 3);
  tspp = uelement[0] * s + mu * uelement[1] * s * s * s * c3;
  uelement[5] = jed - tspp;
}
/*********************************************************************
Name:    StumpffN
Purpose: Function to compute the nth order Stumpff function for any x.
         Reference: Danby. FUN, pp. 172-174.
Inputs:  x      - Argument.
         Norder - Order desired.
Outputs: None.
Returns: nth order Stumpff function.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double StumpffN(double x, int Norder) {

  int n = 0;
  double a, b, c0, c1, c2, c3;

  do {
    n++;
    x = x / 4.0;
  } while (fabs(x) > 0.1);

  a = (1.0 - x * (1.0 - x / 182.0) / 132.0);
  b = (1.0 - x * (1.0 - x * a / 90.0) / 56.0);
  c2 = (1.0 - x * (1.0 - x * b / 30.0) / 12.0) / 2.0;
  a = (1.0 - x * (1.0 - x / 210.0) / 156.0);
  b = (1.0 - x * (1.0 - x * a / 110.0) / 72.0);
  c3 = (1.0 - x * (1.0  - x * b / 42.0)/ 20.0) / 6.0;

  c1 = 1.0 - x * c3;
  c0 = 1.0 - x * c2;

  do {
    n--;
    c3 = (c2 + c0 * c3) / 4.0;
    c2 = c1 * c1 / 2.0;
    c1 = c0 * c1;
    c0 = 2.0 * c0 * c0 - 1.0;
    x = x * 4.0;
  } while (n > 0);

  switch (Norder) {
    case 0:
      return (c0);
      case 1:
      return (c1);
    case 2:
      return (c2);
      case 3:
      return (c3);
    default:
      LogMsg(stderr, "StumpffN: Norder not 1, 2, or 3\n");
      exit(1);
  }

  return(-999);
}

/*********************************************************************
Name:    Transpose
Purpose: Function to compute the transpose of a matrix.
Inputs:  a[][] - Zero-offset matrix a (n rows by n columns).
Outputs: b[][] - Zero-offset matrix transpose (n rows by n columns).
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Transpose(DMatrix a, DMatrix b, int n) {

  int i, j;

  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      b[i][j] = a[j][i];
    }
  }
}

/*********************************************************************
Name:    Uvector
Purpose: Unit vector subroutine.
Inputs:  a[] - Zero-offset column vector (3 rows by 1 column).
Outputs: unita[] - Zero-offset unit vector (3 rows by 1 column).
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Uvector(double a[], double unita[]) {

  double maga;
  int i;

  maga = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
  for (i=0; i<3; i++) {
    if (maga != 0) {
      unita[i] = a[i]/maga;
    } else {
      unita[i] = 0.0;
    }
  }
}

/*********************************************************************
Name:    Vcross
Purpose: Vector cross product subroutine.
Inputs:  a - Zero-offset vector a (3 rows by 1 column).
         b - Zero-offset vector b (3 rows by 1 column).
Outputs: acrossb - a X b (3 rows by 1 column).
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Vcross(double a[], double b[], double acrossb[]) {

  acrossb[0] = a[1] * b[2] - a[2] * b[1];
  acrossb[1] = a[2] * b[0] - a[0] * b[2];
  acrossb[2] = a[0] * b[1] - a[1] * b[0];
}

/*********************************************************************
Name:    Vdot
Purpose: Vector dot product function.
Inputs:  n   - Number of rows.
         a[] - Zero-offset column vector with N rows.
         b[] - Zero-offset column vector with N rows.
Outputs: adotb - Dot product of a and b.
Returns: Nothing.
Status:  Finished.
Errors:  None known.
*********************************************************************/
void Vdot(int n, double a[], double b[], double *adotb) {

  int i;

  *adotb = 0.0;
  for (i = 0; i<n; i++) {
    *adotb += a[i] * b[i];
  }
}

/*********************************************************************
Name:    Vecmag
Purpose: Vector magnitude function.
Inputs:  a[] - Zero-offset column vector (3 rows by 1 column).
Outputs: None.
Returns: Magnitude of vector.
Status:  Finished.
Errors:  None known.
*********************************************************************/
double Vecmag (double a[]) {

  double x;

  x = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

  return (x);
};

/* End Of File - astrolib.c *****************************************/
