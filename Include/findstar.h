//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								findstar
// SUBSYSTEM:						Source Extraction Routines
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None.
// NAMESPACE:						ACL
// AUTHORS:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 1996-2007 Smithsonian Astrophysical Observatory, Cambridge, MA, USA
//                      Copyright 2014-2016 Gavin Blakeman.
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
// FUNCTIONS INCLUDED:  findstar(...)
//
// CLASSES INCLUDED:		None
//
// CLASS HIERARCHY:     None
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2014-01-31 GGB - Ported to C++
//
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

#ifndef ACL_FINDSTAR_H
#define ACL_FINDSTAR_H

#include "config.h"
#include "SourceExtraction.h"

  // Miscellaneous Libraries

#include <MCL>
#include <SCL>

namespace ACL
{
  struct SFindSources
  {
  public:
    AXIS_t nspix;
    AXIS_t ispix;                     // Stats are computed every this many pixels
    AXIS_t maxw;                      // Farthest distance to walk from seed
    FP_t burnedout;                   // Clamp pixels brighter than this
    int niterate ;                    // Number of iterations for sigma clipping
    FP_t starsig;                     // Stars must be at least this many standard deviations above the mean
    AXIS_t fsborder;                  // Ignore this much of the edge
    AXIS_t rnoise;                    // Mean noise is from center +- this many pixels
    AXIS_t maxrad;                    // Maximum radius for a star
    AXIS_t minrad;                    // Minimum radius for a star
    FP_t bmin;                        // Minimum peak for a star
    AXIS_t minsep;                    // Minimum separation for stars

    SFindSources();
  };

  class CFindSources
  {
  private:
    double *image;                    ///< The image
    AXIS_t nx;                        ///< x-dimension of the image
    AXIS_t ny;                        ///< y-dimension of the image
    AXIS_t nspix;                     ///< Stats are computed for +- this many pixels
    AXIS_t ispix;                     ///< Stats are computed every this many pixels
    AXIS_t maxw;                      ///< Farthest distance to walk from seed
    FP_t burnedout;                   ///< Clamp pixels brighter than this
    int niterate ;                    ///< Number of iterations for sigma clipping
    FP_t starsig;                     ///< Stars must be at least this many standard deviations above the mean
    AXIS_t fsborder;                  ///< Ignore this much of the edge
    AXIS_t rnoise;                    ///< Mean noise is from center +- this many pixels
    AXIS_t maxrad;                    ///< Maximum radius for a star
    AXIS_t minrad;                    ///< Minimum radius for a star
    FP_t bmin;                        ///< Minimum peak for a star
    AXIS_t minsep;                    ///< Minimum separation for stars

    FP_t getValue(AXIS_t x, AXIS_t y) const;

  protected:
    void getRow(AXIS_t x1, AXIS_t x2, AXIS_t y, std::valarray<FP_t> &row) const;
    AXIS_t starRadius(const MCL::TPoint2D<AXIS_t> &c0, AXIS_t rmax, FP_t minsig, FP_t background);
    MCL::TPoint2D<FP_t> starCentroid(MCL::TPoint2D<AXIS_t> const &guess);
    void mean1d(std::valarray<FP_t> const &svec, AXIS_t sv1, AXIS_t sv2, FP_t &mean, FP_t &sigma);
    void mean2D(MCL::TPoint2D<AXIS_t> p1, MCL::TPoint2D<AXIS_t> p2, FP_t &mean, FP_t &stDev);
    boost::optional<MCL::TPoint2D<AXIS_t> > brightWalk(MCL::TPoint2D<AXIS_t> const &guess, AXIS_t rmax, FP_t &) const;
    FP_t findFlux(AXIS_t x0, AXIS_t y0, AXIS_t r, FP_t background);
    bool hotPixel(AXIS_t x, AXIS_t y, FP_t limit);

  public:
    CFindSources(double *, AXIS_t, AXIS_t);

    void setParameters(SFindSources const &parameters);

    void findStars(TImageSourceContainer &imageObjectList);

  };

}   // namespace ACL

#endif // ACL_FINDSTAR_H
