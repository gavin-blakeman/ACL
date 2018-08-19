//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								Astrometry.h
// SUBSYSTEM:						Astronometry Classes
// LANGUAGE:						C++
// TARGET OS:						None. 
// LIBRARY DEPENDANCE:	MCL, Boost, gsl
// NAMESPACE:						libAstroClass
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
// Copyright 2011-2013 Gavin Blakeman.
// This file is part of the Astronomy Class Library (libAstroClass)
//
// libAstroClass is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
//
// libAstroClass is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with libAstroClass.  If not, 
// see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:						Classes supporting astrometric reductions.
//
//                      PLATE CONSTANTS:
//                      When calculating plate constants, only stellar objects can be used as reference points. These stellar
//                      objects should also be stars and not extended objects.
//
// CLASSES INCLUDED:		CAstrometry					    - A class to perform astrometric reductions.
//                      CAstrometryObservation  - Class to store astrometry observation data. This includes object information,
//                                                ccd coordinates.
//                      SPlateData - 
//
// CLASS HIERARCHY:
//											CAstrometry
//                      SPlateData
//                      CAstrometryObservation
//
// HISTORY:             2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-21 GGB - AIRDAS 0000.00 release.
//                      2011-06-26 GGB - Start Development of classes in file libAstroClass::Astrometry
//
//*********************************************************************************************************************************

#include "../Include/Astrometry.h"

  // ACL Library

#include "../Include/AstroFunctions.h"
#include "../Include/AstroImageFunctions.hpp"
#include "../Include/FITS.h"
#include "../Include/StellarObjects.h"

#include "math.h"

#include "boost/scoped_array.hpp"

  // gsl library

//#include "linalg/gsl_linalg.h"
//#include "statistics/gsl_statistics.h"

#include "sofam.h"

namespace libAstroClass
{

#define MAX_OBJECTS 512

   // Data structures

  struct asciiTableColumns
  {
    std::string name;
    CCfits::ValueType type;
    int width;
    std::string unit;
    int decimals;
  };

    // Astrometry HDU related constants

    // Column Names

#define ASTROMETRY_COLUMNS  13
  asciiTableColumns const astrometryColumnDetails[ASTROMETRY_COLUMNS] = 
    { { std::string("SEQ"), CCfits::Tint, 10, std::string(""), 0 },
      { std::string("OBJECT NAME"), CCfits::Tstring, 26, std::string(""), 0 },
      { std::string("R/T"), CCfits::Tstring, 1, std::string(""), 0 },
      { std::string("RA"), CCfits::Tdouble, 20, std::string("Catalog Coordinates (hh.mmss)"), 8 },
      { std::string("DEC"), CCfits::Tdouble, 20, std::string("Catalog coordinates (-dd.dddd"), 7 },
      { std::string("pmRA"), CCfits::Tfloat, 16, std::string(""), 2 },
      { std::string("pmDEC"), CCfits::Tfloat, 16, std::string(""), 2 },
      { std::string("Parallax"), CCfits::Tfloat, 16, std::string(""), 2 },
      { std::string("radialVelocity"), CCfits::Tfloat, 16, std::string(""), 2 },
      { std::string("EPOCH"), CCfits::Tdouble, 10, std::string("JD"), 1 },
      { std::string("Coord System"), CCfits::Tstring, 20, std::string(""), 0 },
      { std::string("PPX"), CCfits::Tfloat, 16, std::string(""), 2 },
      { std::string("PPY"), CCfits::Tfloat, 16, std::string(""), 2 } 
    };  

  //*******************************************************************************************************************************
  //
  // CAstrometry
  //
  //*******************************************************************************************************************************

  // Class constructor.
  //
  // 2011-06-30/GGB - Function created.

  CAstrometry::CAstrometry() : _references(), _targets(), observationLocation(), observationTime()
  {
  };

  // Class destructor.
  //
  // 2011-06-30/GGB - Function created.

  CAstrometry::~CAstrometry()
  {
  };

  /// Calculates the plate constants based on the objects that have been added to the vector.
  /// Performs a number of checks and conversions before commencing with the plate constants.
  /// See Tatum, The Measurement of Comet Positions (Journal of the Royal Astronomical Society of Canada, Vol 76, 1982)
  /// 1. Are there sufficient object (>3) to be able to calculate the plate constants.
  /// 2. Convert each of the (RA,Dec) pairs to observed coordinates.
  /// 3. Determine the focal length (F)
  /// 4. Convert all (x,y) to ratio F
  /// 5. Estimate the center of the plate.
  /// 6. Determine the standard coordinates for each object (zeta, eta)
  /// 4. Linear regress the plate constants (objects > 3)
  /// EXCEPTIONS: 
  //
  // R/D/A: 1.00/
  

  void CAstrometry::calculatePlateConstants()
  {
    //DAstrometryStore::iterator iter;
    //unsigned int numComparisons;
    //unsigned int index1, index2, index;
    //unsigned int offset = 0;
    //double distance, omega;
    //double maxRA = 0, minRA = D2PI, maxDec = -DPI, minDec = DPI;
    //double Aa, cosAa;
    //double zeta, eta;

    //if (checkRequisites())
    //{
    //    // Correct the pairs to observed coordinates.

    //  for (iter = _references.begin(); iter != _references.end(); iter++)
    //  {
    //    /*(*iter)->astroObject->setObservationTime(observationTime);
    //    (*iter)->astroObject->setObservationLocation(observationLocation);
    //    (*iter)->astroObject->setObservationWeather(observationWeather);
    //    (*iter)->astroObject->observedPlace();*/
    //  };

    //    // Determine the focal length of the image.
    //    // There are n(n-1)/2 combinations to check to determine the focal length. We take the average of all these combinaitons.
    //    // Each combination takes 8 bytes. 131072 combinations takes 1MB to store.
    //    // Evaluation time is not linear with an increasing number of objects. Limit to MAX_OBJECTS = 512

    //  numComparisons = _references.size() * (_references.size() - 1) / 2;
    //  boost::scoped_array<double> FocalLengthArray(new double[numComparisons]);

    //  for (index1 = 0; index1 < _references.size(); index1++)
    //    for (index2 = index1 + 1; index2 < _references.size(); index2++)
    //    {
    //        // Compare object index1 to object index2 and record the F.

    //      distance = hypot(_references[index1]->CCDCoordinates().x() - _references[index2]->CCDCoordinates().x(),
    //        _references[index1]->CCDCoordinates().y() - _references[index2]->CCDCoordinates().y());

    //      omega = sin(_references[index1]->observedPlace().Dec()) * sin(_references[index2]->observedPlace().Dec()) +
    //        cos(_references[index1]->observedPlace().Dec()) * cos(_references[index2]->observedPlace().Dec()) *
    //        cos(_references[index1]->observedPlace().RA() - _references[index1]->observedPlace().Dec());

    //      FocalLengthArray[offset++] = distance / omega;
    //    };

    //  plateData->F = gsl_stats_mean(FocalLengthArray.get(), 1, numComparisons);

    //    // Divide all the (x,y) pairs by the focal Length (F)

    // /* for (iter = _references.begin(); iter != _references.end(); iter++)
    //    (*iter)->standardisedCCD = (*iter)->_CCDCoordinates / plateData->F;*/

    //    // Determine an approximate plate center.

    //  if (!plateCenterDefined())
    //  {
    //      // Try to choose some coordinates near the center.
    //      // Get the maximum and minimum values at the same time.

    //    /*for (iter = _references.begin(); iter != _references.end(); iter++)
    //    {
    //      if ( (*iter)->astroObject->observedPlace().RA() < minRA)
    //        minRA = (*iter)->astroObject->observedPlace().RA();

    //      if ( (*iter)->astroObject->observedPlace().RA() > maxRA)
    //        maxRA = (*iter)->astroObject->observedPlace().RA();

    //      if ( (*iter)->astroObject->observedPlace().Dec() < minDec)
    //        minDec = (*iter)->astroObject->observedPlace().Dec();

    //      if ( (*iter)->astroObject->observedPlace().Dec() > maxDec)
    //        maxDec = (*iter)->astroObject->observedPlace().Dec();
    //    };*/

    //    plateCenter.setCoordinates( (maxRA - minRA) / 2 + minRA, (maxDec - minDec) / 2 + minDec, false);
    //  };

    //    // Calculate (zeta, eta) for each coordinate.

    //  for (iter = _references.begin(); iter != _references.end(); iter++)
    //  {
    //    Aa = (*iter)->astroObject->observedPlace().RA() - plateCenter.RA();
    //    cosAa = cos(Aa);

    //    zeta = sin(Aa) / (sin(plateCenter.Dec()) * tan((*iter)->astroObject->observedPlace().Dec()) + cos(plateCenter.Dec()) * cosAa);

    //    eta = tan((*iter)->astroObject->observedPlace().Dec()) - tan(plateCenter.Dec()) * cosAa /
    //      (tan(plateCenter.Dec()) * tan((*iter)->astroObject->observedPlace().Dec()) + cosAa);

    //    (*iter)->standardCoordinates.set(zeta, eta);
    //  };

    //  if (_references.size() == 3)
    //  {
    //      // Perform a direct solution.

    //    boost::scoped_array<double> Adata(new double[3,3]);
    //    boost::scoped_array<double> bdata(new double[3]);

    //    for (index = 0; index < 2; index++)
    //    {
    //      Adata[index, 0] = _references[index]->standardisedCCD.x();
    //      Adata[index, 1] = _references[index]->standardisedCCD.y();
    //      Adata[index, 2] = 1;

    //      bdata[index] = _references[index]->standardCoordinates.x() - _references[index]->standardisedCCD.x();
    //    };

    //    gsl_matrix_view m = gsl_matrix_view_array(Adata.get(), 3, 3);
    //    gsl_vector_view b = gsl_vector_view_array(bdata.get(), 3);

    //    gsl_vector *x = gsl_vector_alloc (4);

    //    int s;

    //    gsl_permutation * p = gsl_permutation_alloc (4);

    //    gsl_linalg_LU_decomp (&m.matrix, p, &s);

    //    gsl_linalg_LU_solve (&m.matrix, p, &b.vector, x);

    //    plateData->a = gsl_vector_get(x, 0);
    //    plateData->b = gsl_vector_get(x, 1);
    //    plateData->c = gsl_vector_get(x, 2);

    //    for (index = 0; index < 2; index++)
    //    {
    //      Adata[index, 0] = _references[index]->standardisedCCD.x();
    //      Adata[index, 1] = _references[index]->standardisedCCD.y();
    //      Adata[index, 2] = 1;

    //      bdata[index] = _references[index]->standardCoordinates.y() - _references[index]->standardisedCCD.y();
    //    };

    //    gsl_linalg_LU_decomp (&m.matrix, p, &s);

    //    gsl_linalg_LU_solve (&m.matrix, p, &b.vector, x);

    //    plateData->d = gsl_vector_get(x, 0);
    //    plateData->e = gsl_vector_get(x, 1);
    //    plateData->f = gsl_vector_get(x, 2);

    //    gsl_permutation_free (p);
    //    gsl_vector_free (x);
    //  }
    //  else
    //  {

    //      // Have the actual locations of each of the objects. Now perform the least squares linear fit


    //  };
    //};
  };

  /// Checks that the requisites to perform a plate constants calculation are met. 
  /// The exception thrown is dependant on the reason that the requisites are not met.
  /// RETURNS:    true - requisites are met
  ///             false - requisites are not met
  /// EXCEPTIONS: 0x3000 - ASTROMETRY: Need at least 3 reference objects to calculate plate constants.
  ///             0x3001 - ASTROMETRY: Observation Location must be defined to reduce positions to calculate plate constants.
  ///             0x3002 - ASTROMETRY: Observation Time must be defined to reduce positions to calculate plate constants.
  //
  // R/D/A: 1.00/2012-01-13/GGB

  bool CAstrometry::checkRequisites() const 
  {
  //  if (_references.size() < 3)
  //    ACL_ERROR(0x3000);        // ASTROMETRY: Need at least 3 reference objects to calculate plate constants.
  //  else if (!observationLocation)
  //    ACL_ERROR(0x3001);        // ASTROMETRY: Observation Location must be defined to reduce positions to calculate plate constants.
  //  else if (!observationTime)
  //    ACL_ERROR(0x3002);        // ASTROMETRY: Observation Time must be defined to reduce positions to calculate plate constants.
  //  else 
  //    return true;
  //};

  ///// Mirrors the image around the y-axis
  ////
  //// R/D/A: 1.00/2012-01-21

  //void CAstrometry::imageFlip(long ymax)
  //{
  //  DAstrometryStore::iterator iter;

  //  for (iter = _references.begin(); iter != _references.end(); iter++)
  //  {
  //    (*iter)->_CCDCoordinates = libAstroClass::imageFlip((*iter)->_CCDCoordinates, ymax);
  //  };

  //  for (iter = _targets.begin(); iter != _targets.end(); iter++)
  //  {
  //    (*iter)->_CCDCoordinates = libAstroClass::imageFlip((*iter)->_CCDCoordinates, ymax);
  //  };
  //};

  ///// Mirrors the image around the x-axis
  ////
  ////  1.00/2012-01-21

  //void CAstrometry::imageFlop(long xmax)
  //{
  //  DAstrometryStore::iterator iter;

  //  for (iter = _references.begin(); iter != _references.end(); iter++)
  //  {
  //    (*iter)->_CCDCoordinates = libAstroClass::imageFlop((*iter)->_CCDCoordinates, xmax);
  //  };

  //  for (iter = _targets.begin(); iter != _targets.end(); iter++)
  //  {
  //    (*iter)->_CCDCoordinates = libAstroClass::imageFlop((*iter)->_CCDCoordinates, xmax);
  //  };
    return false;
  };

  /// Rotates the image by a number of radians.
  //
  // R/D/A: 1.00/2012-01-15

  void CAstrometry::imageRotate(double theta)
  {
    /*DAstrometryStore::iterator iter;

    for (iter = _references.begin(); iter != _references.end(); iter++)
    {
      (*iter)->_CCDCoordinates = libAstroClass::imageRotate(MCL::TPoint2D<double>(0, 0), (*iter)->_CCDCoordinates, theta);
    };

    for (iter = _targets.begin(); iter != _targets.end(); iter++)
    {
      (*iter)->_CCDCoordinates = libAstroClass::imageRotate(MCL::TPoint2D<double>(0, 0), (*iter)->_CCDCoordinates, theta);
    };*/
  };

  /// Loads the astrometry data from the FITS file.
  /// EXCEPTIONS: 0x3003 - ASTROMETRY: Error while reading from FITS, unknown coordinate system specified.
  //
  // 2012-01-22/GGB - Function created.

  void CAstrometry::loadFromFITS(CCfits::AsciiTable &ahdu) 
  {
    //int column = 1;
    //int index;
    //std::vector<int> seq;
    //std::vector<std::string> objects;
    //std::vector<std::string> TR;
    //std::vector<double> RA;
    //std::vector<double> DEC;
    //std::vector<double> pmRA;
    //std::vector<double> pmDec;
    //std::vector<double> parallax;
    //std::vector<double> radialVelocity;
    //std::vector<double> Epoch;
    //std::vector<std::string> coordinateSystem;
    //std::vector<float> PPX;
    //std::vector<float> PPY;
    //PAstroObject ao;
    //CStellarObject::EReferenceSystem rs;
    //CEC catCoords;
    //CAstrometryObservation_Ptr aobs;

    //long objectCount = ahdu.rows();    // Get the number of rows.

    //ahdu.column(column++).read(seq, 1, objectCount);
    //ahdu.column(column++).read(objects, 1, objectCount);
    //ahdu.column(column++).read(TR, 1, objectCount);
    //ahdu.column(column++).read(RA, 1, objectCount);
    //ahdu.column(column++).read(DEC, 1, objectCount);
    //ahdu.column(column++).read(pmRA, 1, objectCount);
    //ahdu.column(column++).read(pmDec, 1, objectCount);
    //ahdu.column(column++).read(parallax, 1, objectCount);
    //ahdu.column(column++).read(radialVelocity, 1, objectCount);
    //ahdu.column(column++).read(Epoch, 1, objectCount);
    //ahdu.column(column++).read(coordinateSystem, 1, objectCount);
    //ahdu.column(column++).read(PPX, 1, objectCount);
    //ahdu.column(column++).read(PPY, 1, objectCount);

    //  // Now create the objects and add them to the list.

    //for (index = 0; index < objectCount; index++)
    //{
    //  if (TR[index] == "R")
    //  {
    //      // Create a reference.

    //    if (coordinateSystem[index] == COORDINATESYSTEM_ICRS)
    //      rs = CStellarObject::RS_ICRS;
    //    else if (coordinateSystem[index] == COORDINATESYSTEM_FK4)
    //      rs = CStellarObject::RS_FK4;
    //    else if (coordinateSystem[index] == COORDINATESYSTEM_FK5)
    //      rs = CStellarObject::RS_FK5;
    //    else if (coordinateSystem[index] == COORDINATESYSTEM_NONE)
    //      rs = CStellarObject::RS_NONE;
    //    else
    //      ACL_ERROR(0x3003);    // ASTROMETRY: Error while reading from FITS, unknown coordinate system specified.

    //    catCoords.setCoordinates(RA[index], DEC[index]);

    //    ao.reset(new CStellarObject(objects[index], catCoords, rs, Epoch[index], pmRA[index], pmDec[index], radialVelocity[index],
    //      parallax[index]));

    //    aobs.reset(new CAstrometryObservation(ao, MCL::TPoint2D<double>(PPX[index], PPY[index])));

    //    _references.push_back(aobs);
    //    ao.reset();
    //    aobs.reset();

    //  }
    //  else
    //  {
    //      // Create a target
    //  };
    //};
  };

  /// Returns the plate constants. If they are not valid, they will be calculated.
  //
  // R/D/A: 1.00/2012-01-13/GGB

  SPlateData_Ptr CAstrometry::plateConstants() 
  {
    if (!plateDataValid())
      calculatePlateConstants();

    return plateData;
  };

  /// Adds a reference object into the list of references.
  /// A check is conducted for duplicates. Duplicate names are not allowed.
  //
  // R/D/A: 1.10/2011-12-24/GGB - Converted to smart pointers
  // R/D/A: 1.00/2011-06-30/GGB

  bool CAstrometry::referenceAdd(PAstrometryObservation toAdd)
  {
    //DAstrometryStore::const_iterator iter;
    //bool bFound = false;

    //  // Check for duplicates. 

    //for (iter = _references.begin(); (iter != _references.end()) && !bFound; iter++)
    //  if ( (*(*iter)) == (*toAdd) )
    //    bFound = true;

    //if (bFound)
    //  return false;
    //else
    //{
    //  _references.push_back(toAdd);
    //  plateDataValid(false);
    //  requisitesMet(false);
    //  return true;
    //};
    return false;
  };

  /// Returns the number of reference objects.
  //
  // R/D/A: 1.00/2012-01-12/GGB

  DAstrometryStore::size_type CAstrometry::referenceCount() const
  {
    return _references.size();
  };

  /// Moves the iterator to the first item and returns the item. If there is no items in the list, then a null
  /// pointer is returned.
  //
  // 2012-01-21/GGB - Function created.

  PAstrometryObservation CAstrometry::referenceFirst()
  {
    PAstrometryObservation nullPtr;

    referenceIterator = _references.begin();

    if (referenceIterator == _references.end())
      return nullPtr;
    else
      return (*referenceIterator);
  };

  /// Moves the iterator to the next item and returns the item. If there are not items, then a null pointer is returned.
  //
  // 2012-01-21/GGB - Function created.

  PAstrometryObservation CAstrometry::referenceNext()
  {
    PAstrometryObservation nullPtr;

    if (referenceIterator != _references.end())
      referenceIterator++;
      
    if (referenceIterator == _references.end())
      return nullPtr;
    else
      return (*referenceIterator);
  };
 
  bool CAstrometry::referenceRemove(std::string const &)
  {
    return false;
  };

  /// Function to set the observing conditions.
  //
  // R/D/A: 1.00/2011-06-30/GGB

  void CAstrometry::setObservationParameters(CAstroTime_Ptr JD, double temperature, double pressure)
  {
    observationTime = JD;

    observationWeather->setWeather(temperature, pressure);
  };

  // Sets the observing conditions.
  //
  // R/D/A: 1.00/2011-07-11/GGB

  void CAstrometry::setObservationParameters(CAstroTime_Ptr JD, CWeather_Ptr weather)
  {
    observationTime = JD;

    observationWeather = weather;
  };

  // Sets the plate center to the specified value.
  //
  // R/D/A: 1.00/2011-07-11/GGB

  void CAstrometry::setPlateCenter(CEC const &pltCenter)
  {
    plateCenter = pltCenter;
    plateCenterDefined(true);
  };

  /// Sets the site information.
  //
  // R/D/A: 1.01/2012-01-12/GGB - Converted to use smart pointers.
  // R/D/A: 1.00/2011-06-30/GGB

  void CAstrometry::setObservationLocation(double lat, double lon, double alt, double timeZone)
  {
    observationLocation.reset(new TLocation(lat, lon, (int) alt, timeZone));
  };

  //Sets the site information to the new object.
  //
  // R/D/A: 1.00/2011-07-11/GGB

  void CAstrometry::setObservationLocation(TLocation_Ptr const location)
  {
    observationLocation = location;
  };

  // Sets the weather data.
  //
  // R/D/A: 1.00/2011-07-11/GGB

  void CAstrometry::setWeather(CWeather_Ptr const weather)
  {
    observationWeather = weather;
  };

  bool CAstrometry::targetAdd(PAstrometryObservation)
  {
    return false;
  };

  /// Returns the number of target objects stored.
  //
  // R/D/A: 1.00/2012-01-12/GGB

  DAstrometryStore::size_type CAstrometry::targetCount() const
  {
    return _targets.size();
  };

   /// Moves the iterator to the first item and returns the item. If there is no items in the list, then a null
  /// pointer is returned.
  //
  // 2012-01-21/GGB - Function created.

  PAstrometryObservation CAstrometry::targetFirst()
  {
     PAstrometryObservation nullPtr;

    targetIterator = _targets.begin();

    if (targetIterator == _targets.end())
      return nullPtr;
    else
      return (*targetIterator);
  };
    
  /// Moves the iterator to the next item and returns the item. If there are not items, then a null pointer is returned.
  //
  // R/D/A: 1.00/2012-01-21/GGB

  PAstrometryObservation CAstrometry::targetNext()
  {
    PAstrometryObservation nullPtr;

    if (targetIterator != _targets.end())
      targetIterator++;
      
    if (targetIterator == _targets.end())
      return nullPtr;
    else
      return (*targetIterator);
  };
    
  bool CAstrometry::targetRemove(std::string const &)
  {
    return false;
  };

  /// Writes the stored data to a FITS Ascii Table HDU.
  //
  // R/D/A: 1.00/2012-01-22/GGB

  void CAstrometry::writeToFITS(CCfits::AsciiTable *ahdu) const 
  {
    //int index;
    //int column = 1;
    //DAstrometryStore::const_iterator iter;
    //long itemCount = _references.size() + _targets.size();    // Total number of rows for the table.

    //std::vector<int> seq(itemCount);
    //std::vector<std::string> objects(itemCount);
    //std::vector<std::string> TR(itemCount);
    //std::vector<double> RA(itemCount);
    //std::vector<double> DEC(itemCount);
    //std::vector<double> pmRA(itemCount);
    //std::vector<double> pmDec(itemCount);
    //std::vector<double> parallax(itemCount);
    //std::vector<double> radialVelocity(itemCount);
    //std::vector<double> Epoch(itemCount);
    //std::vector<std::string> coordinateSystem(itemCount);
    //std::vector<float> PPX(itemCount);
    //std::vector<float> PPY(itemCount);

    //  // Write the Ascii table data. Plate data is written as keywords. 
    //  // Need to set up the column details when we begin.

    //for (index = 0; index < ASTROMETRY_COLUMNS; index++)
    //  ahdu->addColumn(astrometryColumnDetails[index].type, astrometryColumnDetails[index].name,
    //                  astrometryColumnDetails[index].width, "", astrometryColumnDetails[index].decimals);

    //index = 0;

    //  // Create the column data for the references.

    //for (iter = _references.begin(); iter != _references.end(); iter++)
    //{
    //  seq[index] = index + 1;
    //  objects[index] = (*iter)->astroObject->objectName();
    //  TR[index] = std::string("R");
    //  RA[index] = (*iter)->astroObject->catalogPlace().RA(true);
    //  DEC[index] = (*iter)->astroObject->catalogPlace().Dec(true);
    //  pmRA[index] = (*iter)->astroObject->catalogPMRA();
    //  pmDec[index] = (*iter)->astroObject->catalogPMDEC();
    //  parallax[index] = (*iter)->astroObject->catalogParallax();
    //  radialVelocity[index] = (*iter)->astroObject->catalogRadialVelocity();
    //  Epoch[index] = (double) ((*iter)->astroObject->catalogEpoch());
    //  coordinateSystem[index] = (*iter)->astroObject->catalogSystemString();
    //  PPX[index] = (*iter)->CCDCoordinates().x();
    //  PPY[index] = (*iter)->CCDCoordinates().y();

    //  index++;
    //};

    //      // Create the column data for the targets.

    //for (iter = _targets.begin(); iter != _targets.end(); iter++)
    //{
    //  seq[index] = index + 1;
    //  objects[index] = (*iter)->astroObject->objectName();
    //  TR[index] = std::string("T");
    //  RA[index] = (*iter)->astroObject->catalogPlace().RA(true);
    //  DEC[index] = (*iter)->astroObject->catalogPlace().Dec(true);
    //  pmRA[index] = (*iter)->astroObject->catalogPMRA();
    //  pmDec[index] = (*iter)->astroObject->catalogPMDEC();
    //  parallax[index] = (*iter)->astroObject->catalogParallax();
    //  radialVelocity[index] = (*iter)->astroObject->catalogRadialVelocity();
    //  Epoch[index] = (double) ((*iter)->astroObject->catalogEpoch());
    //  coordinateSystem[index] = (*iter)->astroObject->catalogSystemString();
    //  PPX[index] = (*iter)->CCDCoordinates().x();
    //  PPY[index] = (*iter)->CCDCoordinates().y();

    //  index++;
    //};

    //  // Now write the column data to disk.

    //ahdu->column(column++).write(seq, 1);
    //ahdu->column(column++).write(objects, 1);
    //ahdu->column(column++).write(TR, 1);
    //ahdu->column(column++).write(RA, 1);
    //ahdu->column(column++).write(DEC, 1);
    //ahdu->column(column++).write(pmRA, 1);
    //ahdu->column(column++).write(pmDec, 1);
    //ahdu->column(column++).write(parallax, 1);
    //ahdu->column(column++).write(radialVelocity, 1);
    //ahdu->column(column++).write(Epoch, 1);
    //ahdu->column(column++).write(coordinateSystem, 1);
    //ahdu->column(column++).write(PPX, 1);
    //ahdu->column(column++).write(PPY, 1);
  };

};  // namespace libAstroClass
