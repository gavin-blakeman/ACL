//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								findstar
// SUBSYSTEM:						Source Extraction Routines
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	Boost, GCL
// NAMESPACE:						ACL
// AUTHORS:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 1996-2007 Smithsonian Astrophysical Observatory, Cambridge, MA, USA
//                      Copyright 2014-2018 Gavin Blakeman.
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
// OVERVIEW:						This file is a port of some of the routines in findstar.c to C++. findstar.c is part of the libWCS suite of
//                      routines.
//                      This port has been done to support the astroManager project.
//                      All the functions have also been ported to support x64 processors.
//
// FUNCTIONS INCLUDED:  None
//
// CLASSES INCLUDED:		CFindStars
//
// CLASS HIERARCHY:     CFindStars
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2014-01-31 GGB - Ported to C++
//
//*********************************************************************************************************************************

/*** File libwcs/findstar.c
 *** October 19, 2007
 *** By Jessica Mink, after Elwood Downey
 *** Copyright (C) 1996-2007
 *** Smithsonian Astrophysical Observatory, Cambridge, MA, USA

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Correspondence concerning WCSTools should be addressed as follows:
           Internet email: jmink@cfa.harvard.edu
           Postal address: Jessica Mink
                           Smithsonian Astrophysical Observatory
                           60 Garden St.
                           Cambridge, MA 02138 USA
 */

#include "../include/findstar.h"

  // Standard C++ library header files

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <omp.h>

  // Miscellaneous library header files.

#include "boost/format.hpp"
#include <GCL>

namespace ACL
{
  AXIS_t const NSTATPIX	= 25;     // Stats are computed for +- this many pixels
  AXIS_t const ISTATPIX	= 10;     // Stats are computed every this many pixels
  AXIS_t const MAXWALK  = 20;     // Farthest distance to walk from seed
  FP_t const BURNEDOUT	= 0;      // Clamp pixels brighter than this, if > 0
  int const NITERATE    = 5;      // Number of iterations for sigma clipping
  AXIS_t const RNOISE	 	= 50;     // Mean noise is from center +- this many pixels

  FP_t const STARSIGMA	= 5.0;    // Stars must be this many sigmas above mean
  AXIS_t const BORDER		= 10;     // Ignore this much of the edge
  AXIS_t const MAXRAD		= 20;     // Maximum radius for a star
  AXIS_t const MINRAD		= 1;      // Minimum radius for a star
  AXIS_t const MINPEAK  = 10;     // Minimum peak for a star
  AXIS_t const MINSEP		= 10;     // Minimum separations for stars

  /// @brief Defaults the structure to reasonable values.
  /// @throws None.
  /// @version 2014-02-09/GGB - Function created.

  SFindSources::SFindSources()
  {
    nspix = NSTATPIX;
    ispix = ISTATPIX;
    maxw = MAXWALK;
    burnedout = BURNEDOUT;
    niterate = NITERATE;
    starsig = STARSIGMA;
    fsborder = BORDER;
    rnoise = RNOISE;
    maxrad = MAXRAD;
    minrad = MINRAD;
    bmin = MINPEAK;
    minsep = MINSEP;
  }

  /// @brief Constructor for the defaults structure.
  /// @throws None.
  /// @note Defaults are set using the constants.
  /// @version 2014-01-31/GGB - Function created.

  CFindSources::CFindSources(double *newImage, AXIS_t xdim, AXIS_t ydim) : image(newImage), nx(xdim), ny(ydim)
  {
    nspix = NSTATPIX;
    ispix = ISTATPIX;
    maxw = MAXWALK;
    burnedout = BURNEDOUT;
    niterate = NITERATE;
    starsig = STARSIGMA;
    fsborder = BORDER;
    rnoise = RNOISE;
    maxrad = MAXRAD;
    minrad = MINRAD;
    bmin = MINPEAK;
    minsep = MINSEP;
  }

  /// @brief Gets a partial row into the array passed.
  /// @param[in] x1 - Starting index in  the row
  /// @param[in] x2 - Ending index in the row
  /// @param[in] y - The row number to copy
  /// @param[out] row - The row values.
  /// @throws None.
  /// @version 2014-01-31/GGB - Moved into file findstar.cpp and converted to standalone function.
  /// @version 2012-07-22/GGB - Function created.

  void CFindSources::getRow(AXIS_t x1, AXIS_t x2, AXIS_t y, std::valarray<FP_t> &row) const
  {
    AXIS_t offset =  x1 + y * nx;
    AXIS_t index;

    for (index = x1; index <= x2; index++)
    {
      row[index] = image[offset++];
    };
  }

  /// @brief Returns the value of a specified pixel.
  /// With some debug error checking.
  //
  // 2014-02-01/GGB - Function created.

  FP_t CFindSources::getValue(AXIS_t x, AXIS_t y) const
  {
    if ( (x >= 0) && (x < nx) && (y >= 0) && (y < ny))
    {
      return image[x + y * nx];
    }
    else
    {
      return 0;
    };
  }

  /// @brief Find the location and brightest pixel of stars in the given image.
  /// @note Pixels outside fsborder are ignored.
  /// @note Isolated hot pixels are ignored.
  /// @note Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  /// @version 2014-12-26/GGB - Changed all uses of std::clog to GCL::logger classes and functions.
  /// @version 2013-01-31/GGB - Moved into file findstar.cpp and fully ported to class CFindSources
  /// @version 2014-01-30/GGB - Added check that the return from brightWalk falls within the image.
  /// @version 2012-07-20/GGB - Function ported to C++

  void CFindSources::findStars(TImageSourceContainer &imageObjectList)
  {
    FP_t noise, nsigma;
    FP_t minll;
    AXIS_t ilp, irp, idx, idy;
    INDEX_t i;
    AXIS_t x, y, x1, x2, y1, y2;
    FP_t minsig, sigma;
    std::valarray<FP_t> svec(nx);        // Allocate a buffer to hold one image line.
    AXIS_t rmax;
    AXIS_t xborder1, xborder2, yborder1, yborder2;
    INDEX_t index;
    int ipix;
    std::vector<MCL::TPoint2D<AXIS_t> > potentialStars;
    boost::optional<MCL::TPoint2D<FP_t> > centroid;
    boost::optional<MCL::TPoint2D<AXIS_t> > bw;
    FP_t b = bmin;
    AXIS_t sx, sy, r;
    TImageSourceContainer::iterator iter;

      /* Set trim section for star searching */

    xborder1 = fsborder;
    xborder2 = fsborder;
    yborder1 = fsborder;
    yborder2 = fsborder;

      /* Compute image noise from a central swath */

    x1 = std::max(AXIS_t(0), (nx / 2) - rnoise);
    x2 = std::min((nx / 2) + rnoise, nx -1);
    y1 = std::max(AXIS_t(0), (ny / 2) - rnoise);
    y2 = std::min((ny / 2) + rnoise, ny -1);

    LOGMESSAGE(info, "Calculating image noise level... ");

    mean2D(MCL::TPoint2D<AXIS_t>(x1, y1), MCL::TPoint2D<AXIS_t>(x2, y2), noise, nsigma);

    LOGMESSAGE(info, "Mean is " + boost::str(boost::format("%.2f") % noise) + ", sigma is " +
               boost::str(boost::format("%.2f") % nsigma));

      /* Fill in borders of the image line buffer with noise */

    for (index = 0; index < static_cast<INDEX_t>(xborder1); index++)
    {
      svec[index] = noise;
      svec[nx - index - 1] = noise;
    };

    LOGMESSAGE(info, "x=[0, " + std::to_string(xborder1) + "] and [" + std::to_string(nx - xborder2 - 1) + ", " +
               std::to_string(nx - 1) + "] set to noise...");
    LOGMESSAGE(info, "y=[0, " + std::to_string(yborder1) + "] and [" + std::to_string(ny - yborder2 - 1) +
               ", " + std::to_string(ny - 1) + "] set to noise...");

    if (bmin > 0)
    {
      minll = noise + bmin;
    }
    else
    {
      minll = noise + (starsig * nsigma);
    };

    sigma = std::sqrt(minll);

    minsig = std::max(nsigma, sigma);

      /* Scan for stars based on surrounding local noise figure */

    LOGMESSAGE(info, "Searching for objects in image...");

    x1 = xborder1;
    y1 = yborder1;
    x2 = nx - xborder2;
    y2 = ny - yborder2;

    for (y = y1; y < y2; ++y)
    {
      ipix = 0;

        /* Get one line of the image minus the noise-filled borders */

      getRow(x1, x2, y, svec);

        /* Search row for bright pixels */

      for (x = x1; x < x2; ++x)
      {
          /* Redo stats once for every several pixels */

        if ( (ispix > 0) && (nspix > 0) && (ipix++ % ispix == 0)  )
        {

            /* Find stats to the left */

          ilp = std::max(AXIS_t(0), x - (nspix / 2));
          irp = std::min(ilp + nspix, nx);

          minsig = 0.0;
          if (irp > ilp)
          {
            mean1d(svec, ilp, irp, noise, minsig);
          };

          sigma = sqrt(noise);

          minsig = std::max(minsig, sigma);
          minll = noise + (starsig * minsig);
        };

        /* Pixel is a candidate if above the noise */

        if (svec[x] > minll)
        {
            /* Ignore faint stars */

          if (svec[x] > bmin)
          {

              /* Ignore hot pixels */

            if (!hotPixel(x, y, minll))
            {

                /* Walkabout to find brightest pixel in neighborhood. Brightwalk checks that found pixel falls within the image. */

              if ((bw = brightWalk(MCL::TPoint2D<AXIS_t>(x, y), maxw, b)) )
              {

                  /* Ignore really bright stars */

                if ( (burnedout > 0 && b <= burnedout) || (burnedout == 0) )
                {

                    /* Skip star if already in list */

                  for (i = 0; i < potentialStars.size(); ++i)
                  {
                    idy = potentialStars[i].y() - (*bw).y();
                    idy = std::fabs(idy);
                    if (idy <= minsep)
                    {
                      idx = potentialStars[i].x() - (*bw).x();
                      idx = std::fabs(idx);;
                      if (idx <= minsep)
                      {
                        break;
                      };
                    };
                  };

                  if (i == potentialStars.size())
                   {

                      /* Keep it if it is within the size range for stars */

                    rmax = maxrad;

                    r = starRadius( (*bw), rmax, minsig, noise );

                    if (r > minrad && r <= maxrad)
                    {

                        /* Centroid star */

                      centroid = starCentroid(*bw);

                        // Check that the return pixel is a valid coordinate. (IE falls within the image)

                      if ( ((*centroid).x() > x1) && ((*centroid).y() > y1) && ((*centroid).x() < x2) && ((*centroid).y() < y2) )
                      {
                        PImageSource imageStar(new SImageSource);
                        imageStar->center = (*centroid);
                        imageStar->peak = b;

                          /* Find radius of star for photometry */
                          /* Outermost 1-pixel radial band is one sigma above background */

                        sx = static_cast<AXIS_t>((*centroid).x() + 0.5);
                        sy = static_cast<AXIS_t>((*centroid).y() + 0.5);

                        potentialStars.push_back(MCL::TPoint2D<AXIS_t>( sx, sy ));

                        rmax = 2.0 * static_cast<FP_t>(maxrad);
                        imageStar->radius = starRadius(MCL::TPoint2D<AXIS_t>(sx, sy), rmax, minsig, noise);

                          /* Find flux from star */

                        imageStar->flux = findFlux(sx, sy, imageStar->radius, noise);

                        imageObjectList.push_back(imageStar);

                        LOGMESSAGE(info, "Number: " + std::to_string(imageObjectList.size()) + " x: " + std::to_string(sx) + " y: " + std::to_string(sy) +
                                   " Peak: " + std::to_string(imageStar->peak) + " Radius: " +std::to_string(imageStar->radius) +
                                   " Flux: " + std::to_string(imageStar->flux));
                      };
                    };
                  };
                };
              };
            };
          };
        };
      };
    };

      /* Turn fluxes into instrument magnitudes */

//    LOGMESSAGE(info, "Starting sorting by flux...");
//    imageObjectList.sort( [] (PImageSource const &l, PImageSource const &r)
//                          {
//                            if (l.get() != nullptr && r.get() != nullptr)
//                              return ((*l.get()) < (*r.get()) );
//                            else
//                              return false;
//                          });
//    LOGMESSAGE(info, "Sorting complete.");

    LOGMESSAGE(info, "Computing magnitudes...");
    for (iter = imageObjectList.begin(); iter != imageObjectList.end(); iter++)
    {
      if ( (*iter)->flux <= 0 )
      {
        (*iter)->flux = 0;
      }
      else
      {
        (*iter)->flux = -2.5 * log10((*iter)->flux);
      };
    };
    LOGMESSAGE(info, "Magnitudes complete.");

    LOGMESSAGE(info, "Total objects found: " + std::to_string(imageObjectList.size()));
  }


  /// @brief Checks if the current pixel is a hot pixel, ie A pixel surrounded by noise.
  /// @returns true - Pixel is hot, IE the pixel is high value surrounded by noise.
  /// @returns false - The pixel is not hot.
  /// @details Uses the algorithm from findstar.c (By Doug Mink, after Elwood Downey)
  //
  // 2012-07-20/GGB - Function created.

  bool CFindSources::hotPixel(AXIS_t x, AXIS_t y, FP_t limit)
  {
    FP_t pix1, pix2, pix3;

    /* Check for hot row */

    pix1 = getValue(x-1,y-1);
    pix2 = getValue(x,y-1);
    pix3 = getValue(x+1,y-1);
    if ( (pix1 > limit) && (pix2 > limit) && (pix3 > limit) )
    {
      return false;
    };

    pix1 = getValue(x-1,y+1);
    pix2 = getValue(x,y+1);
    pix3 = getValue(x+1,y+1);
    if ( (pix1 > limit) && (pix2 > limit) && (pix3 > limit) )
    {
      return false;
    };

      /* Check for hot column */

    pix1 = getValue(x-1,y-1);
    pix2 = getValue(x-1,y);
    pix3 = getValue(x-1,y+1);
    if (pix1 > limit && pix2 > limit && pix3 > limit)
    {
      return false;
    };

    pix1 = getValue(x+1,y-1);
    pix2 = getValue(x+1,y);
    pix3 = getValue(x+1,y+1);
    if (pix1 > limit && pix2 > limit && pix3 > limit)
    {
      return false;
    };

    /* Check for hot pixel */

    pix1 = getValue(x-1,y);
    pix3 = getValue(x+1,y);
    if (pix1 > limit && pix3 > limit)
    {
      return false;
    };

    pix1 = getValue(x,y-1);
    pix3 = getValue(x,y+1);
    if (pix1 > limit && pix3 > limit)
    {
      return false;
    };

    return true;
  }


  /// @brief Compute and return the radius of the star centered at (x0, y0).
  // A guard band is assumed to exist on the image. Calling program is assumed to reject object if r > rmax.
  // Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  //
  // 2014-01-31/GGB - Ported to C++ and moved to file findstar.cpp.
  // 2012-07-20/GGB - Function adopted.

  AXIS_t CFindSources::starRadius(MCL::TPoint2D<AXIS_t> const &c0, AXIS_t rmax, FP_t minsig, FP_t background)
  {
    AXIS_t r, irmax;
    FP_t dp, sum, mean;
    AXIS_t xyrr, yrr, np;
    AXIS_t inrr, outrr;
    AXIS_t x, y, xt, yt;

    assert(c0.x() >= 0);
    assert(c0.x() < nx);
    assert(c0.y() >= 0);
    assert(c0.y() < ny);
    assert(rmax > 0);

    irmax = rmax;

      // Compute star's radius. Scan in ever-greater circles until find one such that the mean at
      // that radius is less than one sigma above the background level.

    for (r = 2; r <= irmax; ++r)
    {
      inrr = MCL::pow2(r);
      outrr = MCL::pow2(r+1);
      np = 0;
      sum = 0.0;

      for (y = -r; y <= r; ++y)
      {
        yrr = MCL::pow2(y);
        for (x = -r; x <= r; ++x)
        {
          xyrr = MCL::pow2(x) + yrr;
          if (xyrr >= inrr && xyrr < outrr)
          {
            if (x >= 0)
            {
              xt = std::min(c0.x() + x, nx);
            }
            else
            {
              xt = std::max(AXIS_t(0), c0.x() + x);
            };

            if (y >= 0)
            {
              xt = std::min(c0.y() + y, ny);
            }
            else
            {
              yt = std::max(AXIS_t(0), c0.y() + y);
            };

            dp = getValue(xt, yt);
            sum += dp;
            np++;
          };
        };
      };

      mean = (sum / np) - background;
      if (mean < minsig)
      {
        break;
      };
    };

    return (r);
  }

  // Compute the fine location of the star peaking at [x0,y0]
  // Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  //
  // 2014-01-31/GGB - Ported to C++ and moved to findstar.cpp
  // 2012-07-20/GGB - Function converted to C++.

  MCL::TPoint2D<FP_t> CFindSources::starCentroid(MCL::TPoint2D<AXIS_t> const &guess)
  {
    FP_t p1, p2, p22, p3, d;
    MCL::TPoint2D<FP_t> returnValue;

      /* Find maximum of best-fit parabola in each direction. see Bevington, page 210 */

    p1 = getValue(guess.x() - 1, guess.y());
    p2 = getValue(guess.x() , guess.y());
    p22 = 2*p2;
    p3 = getValue(guess.x() + 1, guess.y());
    d = p3 - p22 + p1;
    returnValue.x() = (d == 0) ? guess.x() : guess.x() + 0.5 - (p3 - p2)/d;
    returnValue.x() += 1.0;

    p1 = getValue(guess.x(), guess.y() - 1);
    p3 = getValue(guess.x(), guess.y() + 1);
    d = p3 - p22 + p1;
    returnValue.y() = (d == 0) ? guess.y() : guess.y() + 0.5 - (p3 - p2)/d;
    returnValue.y() += 1.0;

    return returnValue;
  }

  /// Given an image and a starting point, walk the gradient to the brightest pixel and return its location, never going more than
  /// maxrad away.
  /// Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  /// PARAMETERS: c0 - Center point to search from.
  ///             rmax - The maximum search radius
  /// RETURNS:    Centroid if found.
  ///             peak - The peak value found.
  //
  // 2013-08-02/GGB - Changed bp parameter to sensitivity.
  // 2012-07-20/GGB - Function converted to C++

  boost::optional<MCL::TPoint2D<AXIS_t> > CFindSources::brightWalk(MCL::TPoint2D<AXIS_t> const &guess, AXIS_t rmax,
                                                                   FP_t &peak) const
  {
    assert(guess.x() > 0);
    assert(guess.x() < (nx - 1));
    assert(guess.y() > 0);
    assert(guess.y() < (ny - 1));
    assert(rmax > 0);

    static AXIS_t dx[8] = {1,0,-1,1,-1, 1, 0,-1};
    static AXIS_t dy[8] = {1,1, 1,0, 0,-1,-1,-1};

    FP_t b, tmpb, newb;
    AXIS_t x, y, x1, y1, i, xa, ya;
    AXIS_t newx = 0;
    AXIS_t newy = 0;
    boost::optional<MCL::TPoint2D<AXIS_t> > returnValue;

    i = 0;

      /* start by assuming seed point is brightest */

    b = getValue(guess.x(), guess.y());
    x = xa = guess.x();
    y = ya = guess.y();

      /* walk towards any brighter pixel */

    for (;;)
    {
      newx = newy = 0;

        /* Find brightest pixel in 3x3 region */

      newb = b;
      for(i = 0; i < 8; ++i)
      {
        x1 = x + dx[i];
        y1 = y + dy[i];
        tmpb = getValue(x1, y1);
        if (tmpb >= newb)
        {
          if (x1 == xa && y1 == ya)
          {
            break;
          };
          xa = x;
          ya = y;
          newx = x1;
          newy = y1;
          newb = tmpb;
        }
      }

        /* If brightest pixel is one in center of region, quit */

      if (newb == b)
      {
        break;
      };

        /* Otherwise, set brightest pixel to new center */

      x = newx;
      y = newy;
      b = newb;
      if ( (abs(x-guess.x()) > rmax) || (abs(y-guess.y()) > rmax) || (x < 1) || (y < 1) || (x >= nx - 1) || (y >= ny - 1) )
      {
        break;
      };
    }
    if (abs(x-guess.x()) <= rmax && abs(y-guess.y()) <= rmax && (x >= 0) && (y >=0) && (x < nx) && (y < ny))
    {
      returnValue = MCL::TPoint2D<AXIS_t>(x, y);
      peak = b;
    };

    return returnValue;
  }

  // Compute stats in the give region of the image of width w pixels.
  // Bounds are not checked.
  // Adapted from findstar.c (By Doug Mink, after Elwood Downey)
  // EXCEPTIONS:  0x2208 - "ASTROIMAGE: Function mean2D coordinates incorrect or out of range."
  //
  // 2014-12-29/GGB - Test's for mean computation combined into one for performance improvement.
  // 2014-01-31/GGB - Ported fully to C++ standalone function and moved to findstar.cpp.
  // 2012-07-20/GGB - Function adopted.

  void CFindSources::mean2D(MCL::TPoint2D<AXIS_t> p1,
                            MCL::TPoint2D<AXIS_t> p2,
                            FP_t &mean,
                            FP_t &stDev)
  {
    TRACEENTER;

    FP_t p, pmin = std::numeric_limits<FP_t>::min(), pmax = std::numeric_limits<FP_t>::max();
    FP_t pmean = 0.0;
    FP_t sd = 0.0;
    AXIS_t x, y;
    int i;
    FP_t sum;
    INDEX_t npix;

    assert(p1.x() >= 0);
    assert(p1.x() < nx);
    assert(p1.y() >= 0);
    assert(p1.y() < ny);
    assert(p2.x() >= 0);
    assert(p2.x() < nx);
    assert(p2.y() >= 0);
    assert(p2.y() < ny);
    assert(p2.x() > p1.x());
    assert(p2.y() > p1.y());

    for (i = 0; i < niterate; i++ )
    {
      sum = 0.0;
      npix = 0;

        /* Compute mean */

      for (y = p1.y(); y < p2.y(); ++y)
      {
        for (x = p1.x(); x < p2.x(); ++x)
        {
//          p = image[x + y * nx];
          if ( (p > pmin) && (p < pmax) )
          {
            sum += p;
            npix++;
          };
        };
      };

      pmean = sum / static_cast<FP_t>(npix);

        /* Compute average deviation */

      npix = 0;
      sum = 0.0;

      for (y = p1.y(); y < p2.y(); ++y)
      {
        for (x = p1.x(); x < p2.x(); ++x)
        {
//          p = image[x + y * nx];
          if ( (p > pmin) && (p < pmax) )
          {
            sum += std::fabs(p - pmean);
            npix++;
          };
        };
      };

      if (npix > 0)
      {
        sd = sum / static_cast<FP_t>(npix);
      }
      else
      {
        sd = 0.0;
      };

      pmin = pmean - sd * starsig;
      pmax = pmean + sd * starsig;
    };

    mean = pmean;
    stDev = sd;

    TRACEEXIT;
  }


  // Function to determine the mean and sigma for a small portion of a line.
  // This function is used by the findStars function.
  // Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  //
  // 2014-01-31/GGB - Fully ported to C++ and moved to file findstar.cpp
  // 2012-07-20/GGB - Function adopted to C++

  void CFindSources::mean1d(std::valarray<FP_t> const &svec, AXIS_t sv1, AXIS_t sv2,
                            FP_t &mean,
                            FP_t &sigma)
  {
    INDEX_t index;
    FP_t p, pmin = std::numeric_limits<FP_t>::min(), pmax = std::numeric_limits<FP_t>::max();
    FP_t pmean = 0.0;
    FP_t sd = 0.0;
    int i;
    INDEX_t npix;
    FP_t sum;

      /* Iterate with sigma-clipping */

    for (i = 0; i < niterate; ++i )
    {
      npix = 0;
      sum = 0.0;

        /* Compute mean */

      for (index = sv1; index < sv2; ++index)
      {
        p = svec[index];
        if ( (p > pmin) && (p < pmax) )
        {
          sum += p;
          npix++;
        };
      }

      if (npix != 0)
      {
        pmean = sum / static_cast<FP_t>(npix);
      }
      else
      {
        pmean = 0.0;
      };

        /* Compute average deviation */

      npix = 0;
      sum = 0.0;
      for (index = sv1; index < sv2; ++index)
      {
        p = svec[index];
        if ( (p > pmin) && (p < pmax) )
        {
          sum += std::fabs(p - pmean);
          npix++;
        }
      }

      if (npix != 0)
      {
        sd = sum / static_cast<FP_t>(npix);
      }
      else
      {
        sd = 0.0;
      };

      pmin = pmean - (sd * starsig);
      pmax = pmean + (sd * starsig);
    }

    mean = pmean;
    sigma = sd;
  }


  // Find total flux within a circular region minus a mean background level
  // Adopted from findstar.c (By Doug Mink, after Elwood Downey)
  //
  // 2013-01-30/GGB - Added try block around getValue(...)
  // 2012-07-20/GGB - Function ported to C++.

  FP_t CFindSources::findFlux(AXIS_t x0, AXIS_t y0, AXIS_t r, FP_t background)
  {
    FP_t  returnValue = 0.0;
    AXIS_t  x, y, x1, x2, y1, y2, yy, xxyy;
    AXIS_t rr = MCL::pow2(r);
    FP_t dp;
    AXIS_t xi, yi;

      /* Keep X within image */

    x1 = -r;
    if (x0-r < 0)
    {
      x1 = 0;
    };

    x2 = r;
    if (x0+r > nx)
    {
      x2 = nx - x0;   // Must be within image.
    };

      /* Keep Y within image */

    y1 = -r;
    if (y0-r < 0)
    {
      y1 = 0;
    };
    y2 = r;
    if (y0+r > ny)
    {
      y2 = ny - y0;   // Must be within image.
    };

      /* Integrate circular region around a star */

    for (y = y1; y <= y2; y++)
    {
      yy = y*y;
      for (x = x1; x <= x2; x++)
      {
        xxyy = x*x + yy;
        if (xxyy <= rr)
        {
          xi = x0 + x;
          yi = y0 + y;
          try
          {
            dp = getValue(xi, yi);
          }
          catch(...)
          {
            dp = 0;
          }

          if (dp > background)
          {
            returnValue += dp - background;
          };
        };
      };
    };

    return returnValue;
  }

  /// Sets the parameter values.
  //
  // 2014-02-09/GGB - Function created.

  void CFindSources::setParameters(SFindSources const &parameters)
  {
    nspix = parameters.nspix;
    ispix = parameters.ispix;
    maxw = parameters.maxw;
    burnedout = parameters.burnedout;
    niterate = parameters.niterate;
    starsig = parameters.starsig;
    fsborder = parameters.fsborder;
    rnoise = parameters.rnoise;
    maxrad = parameters.maxrad;
    minrad = parameters.minrad;
    bmin = parameters.bmin;
    minsep = parameters.minsep;
  }

} // namespace ACL
