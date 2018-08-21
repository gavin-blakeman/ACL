// LICENSE:             GPLv2
//
// Copyright 2011-2016 Gavin Blakeman.
// This file is part of the Astronomy Class Library (ACL)
//
// ACL is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
//
// ACL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ACL.  If not,
// see <http://www.gnu.org/licenses/>.

#ifndef ACL_OBSERCLASS_H
#define ACL_OBSERCLASS_H

#include <astroclass.h>

#define MAXTELESCOPENAME[50]
#define MAXOBSERVERNAME	50
#define MAXOBSERVERINIT	3

class TObserver : public TLocation
{
private:
  char ObserverName[MAXOBSERVERNAME];
  char ObserverInit[MAXOBSERVERINIT];

protected:
public:
  TObserver(char *, char *, const TLocation &);
  TObsserver(void);
  void SetData(char *, char *, const TLocation &);
  const char *GetObserverName(void) {return ObserverName;}
  const char *GetObserverInit(void);
};

class TTelecope
{
private:
  char TelescopeName[MAXTELESCOPENAME];
  double dAperture;
  double dFRatio;
protected:
public:
  TTelescope(void);
  TTelescope(const char *, double, double);
  double Aperture(void);
  double FRatio(void);
  const char *TelescopeName(void);
};

class TInstrument
{
private:
  char InstrumentName[MAXINSTRUMENTNAME];
  char SerialNumber[MAXSERIALNUMBER];
  char InstrumentType[MAXINSTRUMENTTYPE];

protected:
public:
};

#endif //__OBSERCLASS_H
