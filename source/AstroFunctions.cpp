//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								AstroFunctions.h
// SUBSYSTEM:						Standalone astronomy related functions.
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	None
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2009-2020 Gavin Blakeman.
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
// OVERVIEW:						Standalone functions for Astronomy. These are functions that should not be included in classes.
//
// NOTES:               1. All new code additions to this library should be written in C++. (Using the latest standard of C++ that
//                          is available.
//                      2. This library may reference other C libraries.
//                      3. This file has been modified to allow compilation to C++03 standard for use with embedded systems.
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-01-20 GGB - astroManager 0000.00 release.
//                      2009-09-06 GGB  - Start Development of classes in file ACL::Astrometry
//
//*********************************************************************************************************************************

#include "include/AstroFunctions.h"

  // Standard C++ library header files.

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <locale>

  // Miscellaneous library header files.

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/format.hpp"
#include "boost/locale.hpp"
#include <GCL>
#include "sofa.h"

  // ACL Library header files

#include "include/common.h"
#include "include/error.h"

namespace ACL
{

  typedef struct row { FP_t ral, rau, del; char cst[4]; } ROW ;
#define ITEMS(a)  (sizeof(a)/sizeof(a[0]))


  /* The Constellation Boundaries as extracted from Cat.#6042,
   paper by N.G. Roman, 1987PASP...99..695R
*/

  static ROW ConstellationBoundaries[] = {
    { 0, 360, 90, "" },			/* North Pole */
    {  0.0000,360.0000, 88.0000,"UMi"},
    {120.0000,217.5000, 86.5000,"UMi"},
    {315.0000,345.0000, 86.1667,"UMi"},
    {270.0000,315.0000, 86.0000,"UMi"},
    {  0.0000,120.0000, 85.0000,"Cep"},
    {137.5000,160.0000, 82.0000,"Cam"},
    {  0.0000, 75.0000, 80.0000,"Cep"},
    {160.0000,217.5000, 80.0000,"Cam"},
    {262.5000,270.0000, 80.0000,"UMi"},
    {302.5000,315.0000, 80.0000,"Dra"},
    {  0.0000, 52.6250, 77.0000,"Cep"},
    {172.5000,203.7500, 77.0000,"Cam"},
    {248.0000,262.5000, 75.0000,"UMi"},
    {302.5000,310.0000, 75.0000,"Cep"},
    {119.5000,137.5000, 73.5000,"Cam"},
    {137.5000,170.0000, 73.5000,"Dra"},
    {195.0000,248.0000, 70.0000,"UMi"},
    { 46.5000, 51.2500, 68.0000,"Cas"},
    {306.2500,310.0000, 67.0000,"Dra"},
    {170.0000,180.0000, 66.5000,"Dra"},
    {  0.0000,  5.0000, 66.0000,"Cep"},
    {210.0000,235.0000, 66.0000,"UMi"},
    {353.7500,360.0000, 66.0000,"Cep"},
    {180.0000,202.5000, 64.0000,"Dra"},
    {202.5000,216.2500, 63.0000,"Dra"},
    {347.5000,353.7500, 63.0000,"Cep"},
    { 91.5000,105.0000, 62.0000,"Cam"},
    {300.0000,306.2500, 61.5000,"Dra"},
    {308.0500,309.0000, 60.9167,"Cep"},
    {105.0000,119.5000, 60.0000,"Cam"},
    {119.5000,126.2500, 60.0000,"UMa"},
    {296.5000,300.0000, 59.5000,"Dra"},
    {300.0000,308.0500, 59.5000,"Cep"},
    {343.0000,347.5000, 59.0833,"Cep"},
    {  0.0000, 36.5000, 58.5000,"Cas"},
    {291.2500,296.5000, 58.0000,"Dra"},
    { 25.5000, 28.6250, 57.5000,"Cas"},
    { 36.5000, 46.5000, 57.0000,"Cas"},
    { 46.5000, 47.5000, 57.0000,"Cam"},
    {334.7500,343.0000, 56.2500,"Cep"},
    { 75.0000, 91.5000, 56.0000,"Cam"},
    {210.5000,216.2500, 55.5000,"UMa"},
    {216.2500,291.2500, 55.5000,"Dra"},
    { 47.5000, 50.0000, 55.0000,"Cam"},
    {332.0000,334.7500, 55.0000,"Cep"},
    {309.0000,329.5000, 54.8333,"Cep"},
    {  0.0000, 25.5000, 54.0000,"Cas"},
    { 91.5000, 97.5000, 54.0000,"Lyn"},
    {181.2500,202.5000, 53.0000,"UMa"},
    {228.7500,236.2500, 53.0000,"Dra"},
    {329.5000,332.0000, 52.7500,"Cep"},
    { 50.0000, 75.0000, 52.5000,"Cam"},
    {343.0000,350.0000, 52.5000,"Cas"},
    {236.2500,255.0000, 51.5000,"Dra"},
    { 30.6250, 37.7500, 50.5000,"Per"},
    {255.0000,273.5000, 50.5000,"Dra"},
    {  0.0000, 20.5000, 50.0000,"Cas"},
    { 20.5000, 25.0000, 50.0000,"Per"},
    { 97.5000,102.0000, 50.0000,"Lyn"},
    {350.0000,360.0000, 50.0000,"Cas"},
    {202.5000,210.5000, 48.5000,"UMa"},
    {  0.0000, 16.7500, 48.0000,"Cas"},
    {353.7500,360.0000, 48.0000,"Cas"},
    {272.6250,273.5000, 47.5000,"Her"},
    {273.5000,286.2500, 47.5000,"Dra"},
    {286.2500,287.5000, 47.5000,"Cyg"},
    { 25.0000, 30.6250, 47.0000,"Per"},
    {126.2500,137.5000, 47.0000,"UMa"},
    {  2.5000, 13.0000, 46.0000,"Cas"},
    {180.0000,181.2500, 45.0000,"UMa"},
    {102.0000,110.5000, 44.5000,"Lyn"},
    {328.6250,329.5000, 44.0000,"Cyg"},
    {328.1250,328.6250, 43.7500,"Cyg"},
    {287.5000,291.0000, 43.5000,"Cyg"},
    {137.5000,152.5000, 42.0000,"UMa"},
    {152.5000,161.7500, 40.0000,"UMa"},
    {231.5000,236.2500, 40.0000,"Boo"},
    {236.2500,245.0000, 40.0000,"Her"},
    {138.7500,143.7500, 39.7500,"Lyn"},
    {  0.0000, 37.7500, 36.7500,"And"},
    { 37.7500, 38.5000, 36.7500,"Per"},
    {290.3750,291.0000, 36.5000,"Lyr"},
    { 67.5000, 70.3750, 36.0000,"Per"},
    {326.0000,328.1250, 36.0000,"Cyg"},
    {328.1250,330.0000, 36.0000, "Acl"},
    { 98.0000,110.5000, 35.5000,"Aur"},
    {110.5000,116.2500, 35.5000,"Lyn"},
    {  0.0000, 30.0000, 35.0000,"And"},
    {330.0000,342.2500, 35.0000,"Acl"},
    {342.2500,343.0000, 34.5000,"Acl"},
    {343.0000,352.5000, 34.5000,"And"},
    { 38.5000, 40.7500, 34.0000,"Per"},
    {161.7500,165.0000, 34.0000,"UMa"},
    {180.0000,185.0000, 34.0000,"CVn"},
    {116.2500,138.7500, 33.5000,"Lyn"},
    {138.7500,148.2500, 33.5000,"LMi"},
    { 10.7500, 21.1250, 33.0000,"And"},
    {227.7500,231.5000, 33.0000,"Boo"},
    {352.5000,356.2500, 32.0833,"And"},
    {185.0000,198.7500, 32.0000,"CVn"},
    {356.2500,360.0000, 31.3333,"And"},
    {209.3750,210.5000, 30.7500,"CVn"},
    { 36.2500, 40.7500, 30.6667,"Tri"},
    { 40.7500, 67.5000, 30.6667,"Per"},
    { 67.5000, 71.2500, 30.0000,"Aur"},
    {272.6250,290.3750, 30.0000,"Lyr"},
    {165.0000,180.0000, 29.0000,"UMa"},
    {295.0000,313.7500, 29.0000,"Cyg"},
    { 71.2500, 88.2500, 28.5000,"Aur"},
    {148.2500,157.5000, 28.5000,"LMi"},
    {198.7500,209.3750, 28.5000,"CVn"},
    {  0.0000,  1.0000, 28.0000,"And"},
    { 21.1250, 25.0000, 28.0000,"Tri"},
    { 88.2500, 98.0000, 28.0000,"Aur"},
    {118.2500,120.0000, 28.0000,"Gem"},
    {313.7500,326.0000, 28.0000,"Cyg"},
    {288.8750,295.0000, 27.5000,"Cyg"},
    { 28.7500, 36.2500, 27.2500,"Tri"},
    {242.5000,245.0000, 27.0000,"CrB"},
    {226.2500,227.7500, 26.0000,"Boo"},
    {227.7500,242.5000, 26.0000,"CrB"},
    {275.5000,283.0000, 26.0000,"Lyr"},
    {161.2500,165.0000, 25.5000,"LMi"},
    {283.0000,288.8750, 25.5000,"Lyr"},
    { 25.0000, 28.7500, 25.0000,"Tri"},
    { 10.7500, 12.7500, 23.7500,"Psc"},
    {157.5000,161.2500, 23.5000,"LMi"},
    {318.7500,321.2500, 23.5000,"Vul"},
    { 85.5000, 88.2500, 22.8333,"Tau"},
    {  1.0000,  2.1250, 22.0000,"And"},
    {238.7500,240.5000, 22.0000,"Ser"},
    { 88.2500, 93.2500, 21.5000,"Gem"},
    {297.5000,303.7500, 21.2500,"Vul"},
    {283.0000,288.7500, 21.0833,"Vul"},
    {  2.1250, 12.7500, 21.0000,"And"},
    {303.7500,308.5000, 20.5000,"Vul"},
    {117.1250,118.2500, 20.0000,"Gem"},
    {308.5000,318.7500, 19.5000,"Vul"},
    {288.7500,297.5000, 19.1667,"Vul"},
    { 49.2500, 50.5000, 19.0000,"Ari"},
    {283.0000,285.0000, 18.5000,"Sge"},
    { 85.5000, 86.5000, 18.0000,"Ori"},
    { 93.2500, 94.6250, 17.5000,"Gem"},
    {285.0000,297.5000, 16.1667,"Sge"},
    { 74.5000, 80.0000, 16.0000,"Tau"},
    {238.7500,241.2500, 16.0000,"Her"},
    {297.5000,303.7500, 15.7500,"Sge"},
    { 69.2500, 74.5000, 15.5000,"Tau"},
    { 80.0000, 84.0000, 15.5000,"Tau"},
    {192.5000,202.5000, 15.0000,"Com"},
    {258.7500,273.7500, 14.3333,"Her"},
    {178.0000,192.5000, 14.0000,"Com"},
    {112.5000,117.1250, 13.5000,"Gem"},
    {251.2500,258.7500, 12.8333,"Her"},
    {  0.0000,  2.1250, 12.5000,"Peg"},
    { 84.0000, 86.5000, 12.5000,"Tau"},
    {105.0000,112.5000, 12.5000,"Gem"},
    {316.7500,320.0000, 12.5000,"Peg"},
    { 94.6250,104.0000, 12.0000,"Gem"},
    {273.7500,283.0000, 12.0000,"Her"},
    {313.1250,315.7500, 11.8333,"Del"},
    {315.7500,316.7500, 11.8333,"Peg"},
    {172.7500,178.0000, 11.0000,"Leo"},
    { 93.6250, 94.6250, 10.0000,"Ori"},
    {104.0000,105.0000, 10.0000,"Gem"},
    {117.1250,118.8750, 10.0000,"Cnc"},
    {357.5000,360.0000, 10.0000,"Peg"},
    { 25.0000, 49.2500,  9.9167,"Ari"},
    {302.1250,304.5000,  8.5000,"Del"},
    {202.5000,226.2500,  8.0000,"Boo"},
    {341.2500,357.5000,  7.5000,"Peg"},
    {118.8750,138.7500,  7.0000,"Cnc"},
    {138.7500,161.2500,  7.0000,"Leo"},
    {273.7500,279.9333,  6.2500,"Oph"},
    {279.9333,283.0000,  6.2500,"Aql"},
    {312.5000,313.1250,  6.0000,"Del"},
    {105.0000,105.2500,  5.5000,"CMi"},
    {273.7500,276.3750,  4.5000,"Ser"},
    {241.2500,251.2500,  4.0000,"Her"},
    {273.7500,276.3750,  3.0000,"Oph"},
    {322.0000,325.0000,  2.7500,"Peg"},
    {  0.0000, 30.0000,  2.0000,"Psc"},
    {278.7500,283.0000,  2.0000,"Ser"},
    {304.5000,312.5000,  2.0000,"Del"},
    {312.5000,320.0000,  2.0000,"Equ"},
    {320.0000,322.0000,  2.0000,"Peg"},
    {330.0000,341.2500,  2.0000,"Peg"},
    {325.0000,330.0000,  1.7500,"Peg"},
    {105.2500,108.0000,  1.5000,"CMi"},
    { 53.7500, 69.2500,  0.0000,"Tau"},
    { 69.2500, 70.0000,  0.0000,"Ori"},
    {108.0000,121.2500,  0.0000,"CMi"},
    {220.0000,226.2500,  0.0000,"Vir"},
    {267.5000,273.7500,  0.0000,"Oph"},
    { 39.7500, 49.2500, -1.7500,"Cet"},
    { 49.2500, 53.7500, -1.7500,"Tau"},
    {226.2500,244.0000, -3.2500,"Ser"},
    { 70.0000, 76.2500, -4.0000,"Ori"},
    { 87.5000, 93.6250, -4.0000,"Ori"},
    {267.5000,269.5000, -4.0000,"Ser"},
    {273.7500,278.7500, -4.0000,"Ser"},
    {278.7500,283.0000, -4.0000,"Aql"},
    {341.2500,357.5000, -4.0000,"Psc"},
    {161.2500,172.7500, -6.0000,"Leo"},
    {172.7500,177.5000, -6.0000,"Vir"},
    {  0.0000,  5.0000, -7.0000,"Psc"},
    {357.5000,360.0000, -7.0000,"Psc"},
    {213.7500,220.0000, -8.0000,"Vir"},
    {238.7500,244.0000, -8.0000,"Oph"},
    {300.0000,308.0000, -9.0000,"Aql"},
    {320.0000,328.0000, -9.0000,"Aqr"},
    {257.5000,269.5000,-10.0000,"Oph"},
    { 87.5000,121.2500,-11.0000,"Mon"},
    { 73.7500, 76.2500,-11.0000,"Eri"},
    { 76.2500, 87.5000,-11.0000,"Ori"},
    {121.2500,125.5000,-11.0000,"Hya"},
    {143.7500,161.2500,-11.0000,"Sex"},
    {177.5000,192.5000,-11.0000,"Vir"},
    {263.7500,265.0000,-11.6667,"Oph"},
    {283.0000,300.0000,-12.0333,"Aql"},
    { 72.5000, 73.7500,-14.5000,"Eri"},
    {308.0000,320.0000,-15.0000,"Aqr"},
    {257.5000,273.7500,-16.0000,"Ser"},
    {273.7500,283.0000,-16.0000,"Sct"},
    {125.5000,128.7500,-17.0000,"Hya"},
    {244.0000,245.6250,-18.2500,"Oph"},
    {128.7500,136.2500,-19.0000,"Hya"},
    {161.2500,162.5000,-19.0000,"Crt"},
    {244.0000,245.6250,-19.2500,"Sco"},
    {235.0000,238.7500,-20.0000,"Lib"},
    {188.7500,192.5000,-22.0000,"Crv"},
    {192.5000,213.7500,-22.0000,"Vir"},
    {136.2500,146.2500,-24.0000,"Hya"},
    { 25.0000, 39.7500,-24.3833,"Cet"},
    { 39.7500, 56.2500,-24.3833,"Eri"},
    {162.5000,177.5000,-24.5000,"Crt"},
    {177.5000,188.7500,-24.5000,"Crv"},
    {213.7500,223.7500,-24.5000,"Lib"},
    {244.0000,251.2500,-24.5833,"Oph"},
    {  0.0000, 25.0000,-25.5000,"Cet"},
    {320.0000,328.0000,-25.5000,"Cap"},
    {328.0000,357.5000,-25.5000,"Aqr"},
    {357.5000,360.0000,-25.5000,"Cet"},
    {146.2500,153.7500,-26.5000,"Hya"},
    { 70.5000, 72.5000,-27.2500,"Eri"},
    { 72.5000, 91.7500,-27.2500,"Lep"},
    {300.0000,320.0000,-28.0000,"Cap"},
    {153.7500,158.7500,-29.1667,"Hya"},
    {188.7500,223.7500,-29.5000,"Hya"},
    {223.7500,235.0000,-29.5000,"Lib"},
    {235.0000,240.0000,-29.5000,"Sco"},
    { 68.7500, 70.5000,-30.0000,"Eri"},
    {251.2500,264.0000,-30.0000,"Oph"},
    {264.0000,267.5000,-30.0000,"Sgr"},
    {158.7500,162.5000,-31.1667,"Hya"},
    { 91.7500,110.5000,-33.0000,"CMa"},
    {183.7500,188.7500,-33.0000,"Hya"},
    {162.5000,183.7500,-35.0000,"Hya"},
    { 52.5000, 56.2500,-36.0000,"For"},
    {125.5000,140.5000,-36.7500,"Pyx"},
    { 64.0000, 68.7500,-37.0000,"Eri"},
    {267.5000,287.5000,-37.0000,"Sgr"},
    {320.0000,345.0000,-37.0000,"PsA"},
    {345.0000,350.0000,-37.0000,"Scl"},
    { 45.0000, 52.5000,-39.5833,"For"},
    {140.5000,165.0000,-39.7500,"Ant"},
    {  0.0000, 25.0000,-40.0000,"Scl"},
    { 25.0000, 45.0000,-40.0000,"For"},
    { 58.0000, 64.0000,-40.0000,"Eri"},
    {350.0000,360.0000,-40.0000,"Scl"},
    {212.5000,223.7500,-42.0000,"Cen"},
    {235.0000,240.0000,-42.0000,"Lup"},
    {240.0000,246.3125,-42.0000,"Sco"},
    { 72.5000, 75.0000,-43.0000,"Cae"},
    { 75.0000, 98.7500,-43.0000,"Col"},
    {120.0000,125.5000,-43.0000,"Pup"},
    { 51.2500, 58.0000,-44.0000,"Eri"},
    {246.3125,267.5000,-45.5000,"Sco"},
    {267.5000,287.5000,-45.5000,"CrA"},
    {287.5000,305.0000,-45.5000,"Sgr"},
    {305.0000,320.0000,-45.5000,"Mic"},
    { 45.0000, 51.2500,-46.0000,"Eri"},
    { 67.5000, 72.5000,-46.5000,"Cae"},
    {230.0000,235.0000,-48.0000,"Lup"},
    {  0.0000, 35.0000,-48.1667,"Phe"},
    { 40.0000, 45.0000,-49.0000,"Eri"},
    { 61.2500, 64.0000,-49.0000,"Hor"},
    { 64.0000, 67.5000,-49.0000,"Cae"},
    {320.0000,330.0000,-50.0000,"Gru"},
    { 90.0000,120.0000,-50.7500,"Pup"},
    {120.0000,122.5000,-50.7500,"Vel"},
    { 36.2500, 40.0000,-51.0000,"Eri"},
    { 57.5000, 61.2500,-51.0000,"Hor"},
    {  0.0000, 27.5000,-51.5000,"Phe"},
    { 90.0000, 92.5000,-52.5000,"Car"},
    {122.5000,126.7500,-53.0000,"Vel"},
    { 52.5000, 57.5000,-53.1667,"Hor"},
    { 57.5000, 60.0000,-53.1667,"Dor"},
    {  0.0000, 23.7500,-53.5000,"Phe"},
    { 32.5000, 36.2500,-54.0000,"Eri"},
    { 67.5000, 75.0000,-54.0000,"Pic"},
    {225.7500,230.0000,-54.0000,"Lup"},
    {126.7500,132.5000,-54.5000,"Vel"},
    { 92.5000, 97.5000,-55.0000,"Car"},
    {177.5000,192.5000,-55.0000,"Cen"},
    {212.5000,225.7500,-55.0000,"Lup"},
    {225.7500,230.0000,-55.0000,"Nor"},
    { 60.0000, 65.0000,-56.5000,"Dor"},
    {132.5000,165.0000,-56.5000,"Vel"},
    {165.0000,168.7500,-56.5000,"Cen"},
    {262.5000,270.0000,-57.0000,"Ara"},
    {270.0000,305.0000,-57.0000,"Tel"},
    {330.0000,350.0000,-57.0000,"Gru"},
    { 48.0000, 52.5000,-57.5000,"Hor"},
    { 75.0000, 82.5000,-57.5000,"Pic"},
    { 97.5000,102.5000,-58.0000,"Car"},
    {  0.0000, 20.0000,-58.5000,"Phe"},
    { 20.0000, 32.5000,-58.5000,"Eri"},
    {350.0000,360.0000,-58.5000,"Phe"},
    { 65.0000, 68.7500,-59.0000,"Dor"},
    {230.0000,246.3125,-60.0000,"Nor"},
    {305.0000,320.0000,-60.0000,"Ind"},
    { 82.5000, 90.0000,-61.0000,"Pic"},
    {227.5000,230.0000,-61.0000,"Cir"},
    {246.3125,248.7500,-61.0000,"Ara"},
    {223.7500,227.5000,-63.5833,"Cir"},
    {248.7500,251.2500,-63.5833,"Ara"},
    { 90.0000,102.5000,-64.0000,"Pic"},
    {102.5000,135.5000,-64.0000,"Car"},
    {168.7500,177.5000,-64.0000,"Cen"},
    {177.5000,192.5000,-64.0000,"Cru"},
    {192.5000,218.0000,-64.0000,"Cen"},
    {202.5000,205.0000,-65.0000,"Cir"},
    {251.2500,252.5000,-65.0000,"Ara"},
    { 32.5000, 48.0000,-67.5000,"Hor"},
    { 48.0000, 68.7500,-67.5000,"Ret"},
    {221.2500,223.7500,-67.5000,"Cir"},
    {252.5000,262.5000,-67.5000,"Ara"},
    {262.5000,270.0000,-67.5000,"Pav"},
    {330.0000,350.0000,-67.5000,"Tuc"},
    { 68.7500, 98.7500,-70.0000,"Dor"},
    {205.0000,221.2500,-70.0000,"Cir"},
    {221.2500,255.0000,-70.0000,"TrA"},
    {  0.0000, 20.0000,-75.0000,"Tuc"},
    { 52.5000, 68.7500,-75.0000,"Hyi"},
    { 98.7500,135.5000,-75.0000,"Vol"},
    {135.5000,168.7500,-75.0000,"Car"},
    {168.7500,205.0000,-75.0000,"Mus"},
    {270.0000,320.0000,-75.0000,"Pav"},
    {320.0000,350.0000,-75.0000,"Ind"},
    {350.0000,360.0000,-75.0000,"Tuc"},
    { 11.2500, 20.0000,-76.0000,"Tuc"},
    {  0.0000, 52.5000,-82.5000,"Hyi"},
    {115.0000,205.0000,-82.5000,"Cha"},
    {205.0000,270.0000,-82.5000,"Aps"},
    { 52.5000,115.0000,-85.0000,"Men"},
    {  0.0000,360.0000,-90.0000,"Oct"},
  } ;

#define X1(i)	x1[i-1]
#define X2(i)	x2[i-1]
#define R(i,j)	r[i-1][j-1]

  /*------------------------------------------
C      HERGET PRECESSION, SEE P. 9 OF PUBL. CINCINNATI OBS. NO. 24
C INPUT=  RA1 AND DEC1 MEAN PLACE, IN RADIANS, FOR EPOCH1, IN YEARS A.D.
C OUTPUT= RA2 AND DEC2 MEAN PLACE, IN RADIANS, FOR EPOCH2, IN YEARS A.D.
--------------------------------------------*/

  void HGTPRC(double *RA1,double *DEC1, double *EPOCH1, double *EPOCH2, double *RA2, double *DEC2)
  {
    static double CDR = 0.17453292519943e-01;
    static double EP1=0, EP2=0;
    static double x1[3],x2[3],r[3][3], T,ST,A,B,C,CSR,SINA,SINB,SINC,COSA,COSB,COSC;
    int i, j;

    /* Compute input direction cosines */

    A=cos(*DEC1);
    X1(1)=A*cos(*RA1);
    X1(2)=A*sin(*RA1);
    X1(3)=sin(*DEC1);

    /* Set up rotation matrix (R) */

    if(EP1 == *EPOCH1 &&  EP2 == *EPOCH2)
      ;
    else
    {
      EP1 = *EPOCH1;
      EP2 = *EPOCH2;
      CSR=CDR/3600;
      T=0.001*(EP2-EP1);
      ST=0.001*(EP1-1900.);
      A=CSR*T*(23042.53+ST*(139.75+0.06*ST)+T*(30.23-0.27*ST+18.0*T));
      B=CSR*T*T*(79.27+0.66*ST+0.32*T)+A;
      C=CSR*T*(20046.85-ST*(85.33+0.37*ST)+T*(-42.67-0.37*ST-41.8*T));
      SINA=sin(A);
      SINB=sin(B);
      SINC=sin(C);
      COSA=cos(A);
      COSB=cos(B);
      COSC=cos(C);

      R(1,1)=COSA*COSB*COSC-SINA*SINB;
      R(1,2)=-COSA*SINB-SINA*COSB*COSC;
      R(1,3)=-COSB*SINC;
      R(2,1)=SINA*COSB+COSA*SINB*COSC;
      R(2,2)=COSA*COSB-SINA*SINB*COSC;
      R(2,3)=-SINB*SINC;
      R(3,1)=COSA*SINC;
      R(3,2)=-SINA*SINC;
      R(3,3)=COSC;
    }
    /* Perform the rotation to get the direction cosines at epoch2 */

    for (i=1; i<=3; i++)
    {
      X2(i)=0;
      for (j=1; j<=3; j++) X2(i)+=R(i,j)*X1(j);
    }

    *RA2=atan2(X2(2),X2(1));

    if(*RA2 <  0)
      *RA2 = 6.28318530717948 + *RA2;
    *DEC2=asin(X2(3));
    return;
  }

  /// @brief Calculates airmass using the Bemporad method.
  /// @param[in] z - true zenith angle.
  /// @returns 0 - for z >= PI/2 && z < 0
  /// @returns Airmass for all other values.
  /// @note 1) Bemporad's formula only gives good result to +- 85degrees.
  /// @note 2) Subtracts the refraction value from the Zenith Distance.
  /// @throws None.
  /// @version 2010-07-23/GGB - Function created.

  FP_t airmass(FP_t  z)
  {
    FP_t secz;
    FP_t secx;

    if ( (z >= MCL::PI_DIV_2) || (z < 0) )
    {
      return 0;		// Invalid number
    }
    else
    {
      z = z - refraction(z);		// Correct to apparent zenith distance.
      secz = MCL::sec(z);
      secx = secz - 1;

      return ( secz - (0.0018167 * secx)-(0.002875 * pow(secx, 2)) - (0.0008083 * pow(secx, 3)) );
    };
  }

  /// @brief        Converts the passed string into a Julian day epoch
  /// @param[in]    newEpoch: The epoch to convert.
  /// @returns      Julian day value.
  /// @throws       std::out_of_range
  /// @throws       std::invalid_argument
  /// @throws       GCL::CError(ACL, 0x0301)
  /// @version      2017-08-05/GGB - Updated to a C++ function. (Bug #56)
  /// @version      2009-12-18/GGB - Function created.

  TJD convertEpoch(std::string const &newEpoch)
  {
    TJD returnValue;
    FP_t ep;								// Epoch decimal.

    if (newEpoch[0] == 'B')
    {
      ep = std::stod(newEpoch.substr(1));   // This will throw if invalid number passed.
      iauEpb2jd(ep, &returnValue(0), &returnValue(1));
    }
    else if (newEpoch[0] == 'J')
    {
      ep = std::stod(newEpoch.substr(1));   // This will throw if invalid number passed.
      iauEpj2jd(ep, &returnValue(0), &returnValue(1));
    }
    else if (std::isdigit(newEpoch[0]))
    {
      ep = std::stod(newEpoch.substr(0));   // This will throw if invalid number passed.
      iauEpj2jd(ep, &returnValue(0), &returnValue(1));
    }
    else
    {
      RUNTIME_ERROR(boost::locale::translate("ASTROFUNCTIONS: Incorrect format of Epoch."), E_ASTROFUNCTIONS_EPOCHERROR,
                    LIBRARYNAME);
    };

    return returnValue;
  }

  // Returns the constellation name for a posision and epoch.
  // The position does not have to be the position of a star.
  // Inputs:	dEpoch = Epoch in years AD.
  //					dRA - Right Ascension in Degrees
  //					dDec - Declination in degrees
  //
  //
  //
  char *ConstellationName(double dEpoch, double dRA, double dDec)
  {
    //double djs0, djs;
    //double djf0, djf;
    //double zeta, z, theta;
    double dRA2, dDec2;
    double dEpoch2 = 1875;
    ROW *pr, *pe;
    pe = ConstellationBoundaries + ITEMS(ConstellationBoundaries) ;	/* End-of-data sentinel */


    /* Convert to radians. */

    dRA *= D_D2R;
    dDec *= D_D2R;

    /* Convert the B1875 Epoch to a Julian Date */

    //iauEpb2jd(1875.0, &djf0, &djf);	// Use Sofa routine.
    //iauEpj2jd(dEpoch, &djs0, &djs);

    /* Precess the position  to B1875.0 */

    //iauPrec76(djs0, djs, djf0, djf, &zeta, &z, &theta);

    HGTPRC(&dRA, &dDec, &dEpoch, &dEpoch2, &dRA2, &dDec2);

    /* Normalise angles. */

    dRA2 = iauAnp(dRA2);

    /* Convert back to degrees */

    dRA2 *= D_R2D;
    dDec2 *= D_R2D;

    /* Now search through the array. */

    for (pr=ConstellationBoundaries + 1; pr<pe; pr++)
    {
      if ((dRA2 >= pr->ral) && (dRA2 < pr->rau) && (dDec2 >= pr->del))
        break;
    }
    if (pr < pe)
    {
      return (pr->cst);
    }
    else
    {
      return "???";
    }
  }

  /// @brief        Solve Kepler's equation.
  /// @param[in]    meanAnomoly: The mean anomoly, M.
  /// @param[in]    eccentricity: The eccentric anomoly, e.
  /// @returns      The eccentricAnomoly, E.
  /// @throws       0x0302: AstroFunctions: Keplers equation failed to converge.
  /// @version      2018-08-25/GGB - Function created.

  FP_t keplersEquation(FP_t M, FP_t e)
  {

      // Modulus the mean anomoly so that -180degrees <= M <= +180degrees and then obtain the eccentric anomoly
      // E, from the solution of Kepler's equation M = e*sinE, where e* = 180/PIe = 57.29578e.

    M = MCL::moduloR(M, 180);

    double estar = e * D_R2D;
    double const tol = 1e-6;
    double deltaM, deltaE;
    std::uint_fast16_t loopCounter = 0;
    double E = M + estar * std::sin(MCL::D2R(M));

    do
    {
      deltaM = M - (E - estar * std::sin(MCL::D2R(E)));
      deltaE = deltaM / (1 - e * std::cos(MCL::D2R(E)));
      E = E + deltaE;
      loopCounter ++;
      if (loopCounter >= 255)       // Bailout if not converging.
      {
        RUNTIME_ERROR(boost::locale::translate("AstroFunctions: Keplers equation failed to converge."), E_ASTROFUNCTIONS_KEPLER,
                      LIBRARYNAME);
      }
    }
    while (deltaE > tol);

    return E;
  }


  // Formats a double in Latitude format with the required number of decimal places.
  // Uses the secure form of sprintf.
  // INPUTS:	nLatLong: 0 = N/S, 1=E/W
  //
  // 2009-10-25/GGB - Function created.

  int sprintfLMS(char *szBuffer, FP_t dNumber, int nDecimal, int nLatLong)
  {
    FP_t deg, min, sec;
    char szFormat1[50];
    char szFormat2[10];

    if (dNumber < 0)
    {
      if (nLatLong == 0)
      {
        strcpy(szFormat1, "S%02.0f°%02.0f'%0");
      }
      else
      {
        strcpy(szFormat1, "W%02.0f°%02.0f'%0");
      };

      sec = std::fabs(dNumber);  // Make positive.
    }
    else
    {
      if (nLatLong == 0)
      {
        strcpy(szFormat1, "N%02.0f°%02.0f'%0");
      }
      else
      {
        strcpy(szFormat1, "E%02.0f°%02.0f'%0");
      };

      sec = dNumber;
    };

    if (nDecimal == 0)
    {
      strcpy(szFormat2, "2.0");
    }
    else
    {
      sprintf(szFormat2, "%i.%i", nDecimal+3, nDecimal);
    };

    strcat(szFormat1, szFormat2);
    strcat(szFormat1, "f\"");
    deg = std::floor(sec);
    sec -= deg;
    sec *= 60;
    min = std::floor(sec);
    sec -= min;
    sec *= 60;

    return ( sprintf(szBuffer, szFormat1, deg, min, sec) );
  }

  /// @brief C++ version of the sprintfLMS function. Calls the C version for the actual formatting, but provides a better
  ///        interface when using with C++ applications.
  /// @param[in] latlon - The latitude or longitude to convert to a string.
  /// @param[in] nDecimal - The number of decimal places.
  /// @param[in] nLatLon -
  /// @returns A string with the latitude/longiture
  /// @throws
  /// @version 2011-12-31/GGB - Function created

  std::string sprintfLMS(FP_t latlon, int nDecimal, int nLatLon)
  {
    char szNumber[50];

    if (sprintfLMS(szNumber, latlon, nDecimal, nLatLon) > 0)
    {
      return std::string(szNumber);
    }
    else
    {
      ERROR(ACL, 0x0300);      // ASTROFUNCTIONS: Error while formatting Latitude (sprintfLMS())
    };
  }

  /// Formats a double in TMS format with the required number of decimal places.
  //
  // 2009-10-19/GGB - Function created.

  int sprintfTMS(char *szBuffer, FP_t dNumber, int nDecimal, int iNormalise)
  {
    FP_t hr, min, sec;
    char szFormat1[50] = "%02.0f:%02.0f:%0";
    char szFormat2[10];

    if (nDecimal == 0)
    {
      strcpy(szFormat2, "2.0");
    }
    else
    {
      sprintf(szFormat2, "%i.%i", nDecimal+3, nDecimal);
    };
    strcat(szFormat1, szFormat2);
    strcat(szFormat1, "f");

    dNumber = std::abs(dNumber);		// 2010-03-25/GGB
    hr = std::floor(dNumber);
    sec = dNumber - hr;
    if (iNormalise)
    {
      hr -= std::floor(hr/24)*24;	// Normalise to 0-24. GGB 25/3/2010
    };
    sec *= 60;
    min = std::floor(sec);
    sec -= min;
    sec *= 60;

    return ( sprintf(szBuffer, szFormat1, hr, min, sec) );
  }

  /// @brief Converts a string into hh.hhh decimal format.
  /// @param[in] szConvert - The string to convert.
  /// @param[ou] result - The result of the conversion in ddd.ddd format.
  /// @returns 0 - No error, number of characters scanned.
  /// @returns -1 - String too long
  /// @returns -2 - Invalid character.
  /// @returns -3 - Abnormal string termination.
  /// @returns -4 - Range Error
  /// @note Formats accepted:
  ///   @li hh mm ss.ss
  ///   @li hh:mm:ss.ss
  ///   @li hhHmmM
  ///   @li hhHmm.mm
  ///   @li hh.hhh
  ///   @li ddDmmMss.ssS
  ///   @li dd.ddd
  /// @note
  ///   @li 1) The minutes and seconds values must be less than 60, otherwise an error is generated.
  ///   @li If the hours/degrees value is greater that 24/360, it will be normalised.
  /// @version 2016-05-01/GGB - Updated to use std::string.
  /// @version 2010-01-13/GGB - Function created.

  int sscanfHMS(std::string const &szConvert, FP_t &result)
  {
    FP_t dG1 = 0, dG2 = 0, dG3 = 0;
    std::string szBuffer;
    size_t nIndex = 0;
    int nRetVal = 0;
    bool bNumRead = false;
    bool bExitLoop = false;
    bool bDegrees = 0;		// Degrees = -1, Hours = 0
    bool bDecimalInNo = false;

    /* Read the first number from the string. */

    while ( (!bExitLoop) && (nIndex < szConvert.length()) && (!nRetVal) )
    {
      if (isdigit(szConvert[nIndex]) )
      {
        if (!bNumRead)
        {
          bNumRead = true;		// Number is being read.
        };
        szBuffer += szConvert[nIndex++];  // Copy the digits.
      }
      else
      {
        switch (szConvert[nIndex])
        {
          case '.':
          {
            szBuffer += szConvert[nIndex++];		// Copy the decimal point.
            bDecimalInNo = -1;			// There is a decimal in the first group. Thus there will only be 1 group.
            break;
          };
          case ' ':
          case ':':
          case 'H':
          case 'h':           /* The group is finished. Copy the number to the double. */
          {
            bDegrees = false;
            if (bNumRead)
            {
              bExitLoop = true;		// Exit the loop.
            }
            else
            {
              nRetVal = -3;
            };
            break;
          };
          case 'd': /* The group is finished. Copy the number and convert to hhh */
          case 'D':
          {
            bDegrees = true;			// Value in DMS.
            if (bNumRead)
            {
              bExitLoop = true;
            }
            else
            {
              nRetVal = -3;
            };
            break;
          };
          default:
          {
            nRetVal = -2;		// Invalid character.
            break;
          };
        };
      };
    };
    if (bExitLoop)		// Otherwise an error and finish.
    {
      dG1 = std::stod(szBuffer);
      bNumRead = false;
      if (!bDecimalInNo)		// If decimal in number, then we are finished.
      {
        /* We have finished reading the first group and determined if the values are in HMS or DMS.
         * Now we can read the second group. */

        bExitLoop = false;
        bDecimalInNo = false;
        nIndex++;
        szBuffer.clear();

        while ( (!bExitLoop) && (nIndex < szConvert.length()) && (!nRetVal) )
        {
          if (isdigit(szConvert[nIndex]) )
          {
            if (!bNumRead)
            {
              bNumRead = true;
            };
            szBuffer += szConvert[nIndex++];
          }
          else
          {
            switch (szConvert[nIndex])
            {
              case '.':  // Continue reading the number.
              {
                szBuffer += szConvert[nIndex++];
                bDecimalInNo = true;
                break;
              };
              case ' ':
              case ':':
              case 'M':
              case 'm':
              {
                if (bNumRead)
                {
                  bExitLoop = true;
                }
                else
                {
                  nRetVal = -3;
                };
                break;
              };
              default:
              {
                nRetVal = -2;
                break;
              };
            };
          };
        };
        /* String can end on a '\0' now, must also be checked for. */

        if (!nRetVal)  // No Error has occurred.
        {
          dG2 = std::stod(szBuffer);
          if (dG2 > 60)
          {
            nRetVal = -4;			// Range error.
          };
          bNumRead = false;
          if (!(bDecimalInNo || (nIndex == szConvert.length())) )  // Are we finished
          {
            // Not yet finished. Read third group.

            bDecimalInNo = false;
            bExitLoop = false;
            nIndex++;
            szBuffer.clear();

            while ( (!bExitLoop) && (nIndex < szConvert.length()) && (!nRetVal) )
            {
              if (isdigit(szConvert[nIndex]))
              {
                if (!bNumRead)
                {
                  bNumRead = true;
                }
                szBuffer += szConvert[nIndex++];
              }
              else
              {
                switch (szConvert[nIndex])
                {
                  case '.':
                  {
                    szBuffer += szConvert[nIndex++];
                    bDecimalInNo = true;
                    break;
                  };
                  case 's':
                  case 'S':
                  {
                    bExitLoop = true;
                    break;
                  };
                  case '\0':
                  {
                    if (bNumRead)
                    {
                      bExitLoop = true;
                    }
                    else
                    {
                      nRetVal = -3;
                    };
                    break;
                  };
                  default:
                  {
                    nRetVal = -2;
                    break;
                  };
                };
              };
            };
          };
          if (bExitLoop || (szConvert[nIndex] == '\0') )
          {
            dG3 = std::stod(szBuffer);
            if (dG3 > 60)
            {
              nRetVal = -4;		// Range error.
            };
          };
        };
      };
    }
    else if (nIndex == szConvert.length())
    {
      nRetVal = -3;
    };

    if (nRetVal == 0)
    {
      nRetVal = nIndex;
      if (bDegrees)
      {
        if (dG1 > 360)
        {
          dG1 = MCL::moduloN(dG1, 360);
        };
        result = (dG1 + dG2/60 + dG3/3600);		// Convert to h.hhhh
      }
      else
      {
        dG1 = MCL::moduloN(dG1, 24);
        result = (dG1 + dG2/60 + dG3/3600) * 15;
      };
    };

    return nRetVal;
  }

  /// @brief Converts a string into dd.ddd decimal format.
  /// @param[in] szConvert - The string to convert.
  /// @param[in] result - The converted value.
  /// @returns 0 - No error, number of characters scanned.
  /// @returns -1 - String too long
  /// @returns -2 - Invalid character.
  /// @returns -3 - Abnormal string termination.
  /// @returns -4 - Range Error
  /// @note Formats accepted:
  ///   @li -dd mm ss.ss
  ///   @li dd:mm:ss.ss
  ///   @li ddDmmM
  ///   @li ddDmm.mm
  ///   @li dd.dddD
  ///   @li dddDmmMss.ssS
  /// @note
  ///   @li The minutes and seconds values must be less than 60, otherwise an error is generated.
  ///   @li If the hours/degrees value is greater that 24/360, it will be normalised.
  /// @version 2016-05-01/GGB - Updated to use std::string
  /// @version 2010-01-13/GGB - Function created.

  int sscanfDMS(std::string const &szConvert, FP_t &result)
  {
    double dG1 = 0, dG2 = 0, dG3 = 0;
    std::string szBuffer;
    unsigned int nIndex = 0;
    int nRetVal = 0;
    bool bNumRead = false;
    bool bExitLoop = false;
    bool bDecimalInNo = false;

    /* Read the first number from the string. */

    while ( (!bExitLoop) && (nIndex < szConvert.length()) && (!nRetVal) )
    {
      if (std::isdigit(szConvert[nIndex]) )
      {
        if (!bNumRead)
        {
          bNumRead = true;		// Number is being read.
        };
        szBuffer += szConvert[nIndex++];  // Copy the digits.
      }
      else
      {
        switch (szConvert[nIndex])
        {
          case '-':
          case '+':
          {
            if (!bNumRead)
            {
              szBuffer += szConvert[nIndex++];
            }
            else
            {
              nRetVal = -2;
            };
            break;
          };
          case '.':
          {
            szBuffer += szConvert[nIndex++];		// Copy the decimal point.
            bDecimalInNo = true;			// There is a decimal in the first group. Thus there will only be 1 group.
            break;
          };
          case ' ':
          case ':':
          case 'd': /* The group is finished. Copy the number and convert to hhh */
          case 'D':
          {
            if (bNumRead)
            {
              bExitLoop = true;		// Exit the loop.
            }
            else
            {
              nRetVal = -3;
            };
            break;
          };
          default:
          {
            nRetVal = -2;		// Invalid character.
            break;
          };
        };
      };
    };
    if (bExitLoop)		// Otherwise an error and finish.
    {
      dG1 = stod(szBuffer);		// Convert string to double.
      bNumRead = false;

      if (!bDecimalInNo)		// If decimal in number, then we are finished.
      {
        /* We have finished reading the first group and determined if the values are in HMS or DMS.
           Now we can read the second group. */

        bExitLoop = false;
        bDecimalInNo = false;
        nIndex++;
        szBuffer.clear();

        while ( (!bExitLoop) && (nIndex < szConvert.length()) && (!nRetVal) )
        {
          if (isdigit(szConvert[nIndex]) )
          {
            if (!bNumRead)
            {
              bNumRead = true;
            }
            szBuffer += szConvert[nIndex++];
          }
          else
          {
            switch (szConvert[nIndex])
            {
              case '.':  // Continue reading the number.
              {
                szBuffer += szConvert[nIndex++];
                bDecimalInNo = true;
                break;
              };
              case ' ':
              case ':':
              case 'M':
              case 'm':
              {
                if (bNumRead)
                {
                  bExitLoop = true;
                }
                else
                {
                  nRetVal = -3;
                };
                break;
              };
              default:
              {
                nRetVal = -2;
                break;
              };
            };
          };
        };

        /* String can end on a '\0' now, must also be checked for. */

        if (!nRetVal)  // No Error has occurred.
        {
          dG2 = std::stod(szBuffer);
          if (dG2 > 60)
          {
            nRetVal = -4;			// Range error.
          };
          bNumRead = false;

          if (!(bDecimalInNo || (nIndex == szConvert.length())) )  // Are we finished
          {
            // Not yet finished. Read third group.

            bDecimalInNo = false;
            bExitLoop = false;
            nIndex++;
            szBuffer.clear();

            while ( (!bExitLoop) && (nIndex < szConvert.length()) && (!nRetVal) )
            {
              if (isdigit(szConvert[nIndex]))
              {
                if (bNumRead)
                {
                  bNumRead = true;
                };
                szBuffer += szConvert[nIndex++];
              }
              else
              {
                switch (szConvert[nIndex])
                {
                  case '.':
                  {
                    szBuffer += szConvert[nIndex++];
                    bDecimalInNo = true;
                    break;
                  };
                  case 's':
                  case 'S':
                  {
                    bExitLoop = true;
                    break;
                  };
                  case '\0':
                  {
                    if (bNumRead)
                    {
                      bExitLoop = -1;
                    }
                    else
                    {
                      nRetVal = -3;
                    }
                    break;
                  };
                  default:
                  {
                    nRetVal = -2;
                    break;
                  };
                };
              };
            };
          };
          if (bExitLoop || (szConvert[nIndex] == '\0') )
          {
            dG3 = std::stod(szBuffer);
            if (dG3 > 60)
            {
              nRetVal = -4;		// Range error.
            };
          };
        };
      };
    }
    else if (nIndex == szConvert.length())
    {
      nRetVal = -3;
    };


    if (nRetVal == 0)
    {
      nRetVal = nIndex;

      if (std::fabs(dG1) > 90)
      {
        nRetVal = -4;
      }
      else if (dG1 > 0)
      {
        result = (dG1 + dG2/60 + dG3/3600);
      }
      else
      {
        result  = (dG1 - dG2/60 - dG3/3600);
      }
    };

    return nRetVal;
  }

  /// @brief Convert a jd value to a HJD value.
  /// @param[in] jd - Julian day numbner
  /// @param[in] ra - RA in degrees
  /// @param[in] dec - Declination in degrees
  /// @returns The HJD
  /// @throws None.
  /// @version 2010-07-07/GGB - Function created.

  FP_t JD2HJD(FP_t jd, FP_t ra, FP_t dec)
  {
    FP_t dt, epsilon;	// Delta-T correction to HJD.
    FP_t X, Y;
    FP_t T, L, p, G;
    FP_t dDec, dRA;

    T = (jd - 2415020)/36525;		// Julian century
    p = (1.396041 + 0.000308 * (T + 0.5)) * (T - 0.499998);
    L = 279.696678 + 36000.76892 * T + 0.000303 * T * T - p;
    G = 358.475833 + 35999.04975 * T - 0.00015 * T * T;				// Mean solar anomoly (degrees)
    G *= DD2R;		// Convert to radians.
    L *= DD2R;		// Convert to radians.

    epsilon = 23.45 * DD2R;

    X = 0.99986 * std::cos(L) - 0.025127 * std::cos(G-L) + 0.008374 * std::cos(G+L) +
        0.000105 * std::cos(2*G+L) + 0.000063 * T * std::cos(G-L) +
        0.000035 * std::cos(2*G-L);
    Y = 0.917308 * std::sin(L) + 0.023053 * std::sin(G-L) + 0.007683 * std::sin(G+L) +
        0.000097 * std::sin(2*G+L) - 0.000057 * T * std::sin(G-L) -
        0.000032 * std::sin(2*G-L);

    dDec = dec * DD2R;
    dRA = ra * DD2R;

    dt = -0057755 * ((std::cos(dDec) * std::cos(dRA) * X) + (std::tan(epsilon) * std::sin(dDec) + std::cos(dDec) * std::sin(dRA)) * Y);

    return (jd + dt);
  }

  /// @brief Converts a dd.mmss value to a dd.ddd value.
  /// @param[in] x - Value to convert.
  /// @returns Converted value.
  /// @throws None.
  /// @version 2010-07-09/GGB - Function created.

  FP_t dms2deg(FP_t x)
  {
    FP_t dd, mm, ss;
    FP_t inter;

    inter = std::fabs(x);
    dd = std::floor(inter);
    inter -= dd;
    inter *= 100;
    mm = std::floor(inter);
    inter -= mm;
    ss = inter * 100;

    inter = dd + mm/60 + ss/3600;
#ifdef _MSC_VER
    inter = _copysign(inter, x);
#else
    inter = copysign(inter, x);
#endif

    return inter;
  }

  /// @brief Converts a sexagesimal number to a floating point number.
  /// @param[in] dd - The degrees value
  /// @param[in] mm - The minutes value
  /// @param[in] ss - The seconds value.
  /// @returns The values converted to a floating point degrees value.
  /// @throws None.
  /// @version 2016-04-30/GGB - Function created.

  FP_t dms2deg(FP_t dd, FP_t mm, FP_t ss)
  {
    FP_t returnValue = std::fabs(dd);

    returnValue += mm / 60;
    returnValue += ss / 3600;
    returnValue = copysign(returnValue, dd);

    return returnValue;
  }

  /// @brief Converts a dd.ddd value to seperate dd, mm, ss values.
  /// @param[in] deg - The value in decimal degrees. (dd.dddd)
  /// @param[out] dd - The degrees value
  /// @param[out] mm - The minutes value
  /// @param[out] ss - The seconds value.
  /// @throws None.
  /// @version 2016-04-30/GGB - Function created.

  void deg2dms(FP_t deg, int &dd, int &mm, int &ss)
  {
    FP_t iv = std::fabs(deg);
    dd = std::floor(iv);
    iv -= dd;
    iv *= 60;
    mm = std::floor(iv);
    iv -= mm;
    iv *= 60;
    ss = std::floor(iv);

    dd = std::copysign(dd, deg);
  }

  /// @brief Convert dd.ddd value to dd.mmss value.
  /// @param[in] x - Value to convert.
  /// @returns Converted value.
  /// @throws None.
  /// @version 2013-01-14/GGB - Function created.

  FP_t deg2dms(FP_t x)
  {
    FP_t dd, mm, inter;
    FP_t returnValue;

    inter = std::fabs(x);
    dd = std::floor(inter);
    inter -= dd;
    inter *= 60;
    mm = std::floor(inter);
    inter -= mm;
    inter *= 60;

    returnValue = dd + mm/100 + inter /10000;
#ifdef _MSC_VER
    returnValue = _copysign(returnValue, x);
#else
    returnValue = copysign(returnValue, x);
#endif

    return returnValue;
  }

  /// @brief Converts an hh.mmss to dd.dddd value.
  /// @param[in] x - Value to convert
  /// @returns Converted value.
  /// @throws None.
  /// @version 2010-07-09/GGB - Function created.

  FP_t hms2deg(FP_t x)
  {
    FP_t hh, mm, ss, inter;

    inter = std::fabs(x);
    hh = std::floor(inter);
    inter -= hh;
    inter *= 100;
    mm = std::floor(inter);
    inter -= mm;
    ss = inter * 100;

    inter = hh + mm / 60 + ss / 3600;
    inter *= 15;		// Convert to degrees.

    return inter;
  }

  /// @brief Converts a sexagesimal number to a floating point number.
  /// @param[in] hh - The hours value
  /// @param[in] mm - The minutes value
  /// @param[in] ss - The seconds value.
  /// @returns The values converted to a floating point degrees value.
  /// @throws None.
  /// @version 2016-04-30/GGB - Function created.
  ///
  FP_t hms2deg(FP_t hh, FP_t mm, FP_t ss)
  {
    FP_t returnValue = std::fabs(hh);
    returnValue += mm / 60;
    returnValue += ss / 3600;

    returnValue *= 15;

    return returnValue;
  }

  /// @brief Converts a RA in HHMMSS.sss format to HH.HHHH format.
  /// @param[in] hms - The value to convert.
  /// @returns The converted value.
  /// @throws None.
  /// @version 2017-08-01/GGB - Function created.

  FP_t hms2hrs(FP_t hms)
  {
    FP_t hh, mm, ss, inter;

    RUNTIME_ASSERT((hms > 0) && (hms < 235960), "hms value must fall in range (0, 235960");

    inter = std::fabs(hms);
    hh = std::floor(inter / 10000);
    inter -= hh * 10000;
    mm = std::floor(inter / 100);
    inter -= mm * 100;
    ss = inter * 100;

    inter = hh + mm / 60 + ss / 3600;

    return inter;
  }


  /// @brief Simple refraction calculation.
  /// @details This uses the method of Cassini and Bessel
  /// @param[in] z = Zenith angle (radians)
  /// @returns refraction angle (radians)
  /// @throws None.
  /// @version 2010-07-24/GGB - Function created.

  FP_t refraction(FP_t z)
  {
    return ( 0.000293 * std::tan(z) - 0.0000003242434 * std::pow(std::tan(z),3) );
  }

  /// @brief Parses a string to determine the date/time contained in the string.
  /// @param[in] szDateTime - string value to parse.
  /// @param[out] year - The year value in the string.
  /// @param[out] month - The month value in the string.
  /// @param[out] day - The day value in the string.
  /// @param[out] hour - The hour value in the string.
  /// @param[out] minute - The minute value in the string.
  /// @param[out] second - The seconds value in the string.
  /// @returns true - successfull
  /// @returns false - unsuccesfull.
  /// @throws None.
  /// @version 2013-09-17/GGB - Corrected error created when changing second to FP_t.
  /// @version 2011-07-15/GGB - Function created.

  bool sscanfDateTime(std::string const &szDateTime, unsigned int &year, unsigned int &month, unsigned int &day,
                      unsigned int &hour, unsigned int &minute, FP_t &second)
  {
    float ss = 0;
    bool returnValue;

    returnValue =  (sscanf(szDateTime.c_str(), "%u-%u-%uT%u:%u:%f", &year, &month, &day, &hour, &minute, &ss) == 6);

    if (returnValue)
    {
      second = static_cast<FP_t>(ss);
    };

    return returnValue;
  }

}	// namespace
