#-----------------------------------------------------------------------------------------------------------------------------------
#
# PROJECT:            Astronomy Class Library
# FILE:								ACL.pro
# SUBSYSTEM:          Project File
# LANGUAGE:						C++
# TARGET OS:          WINDOWS/UNIX/LINUX/MAC
# LIBRARY DEPENDANCE:	None.
# NAMESPACE:          N/A
# AUTHOR:							Gavin Blakeman.
# LICENSE:            GPLv2
#
#                     Copyright 2013-2018 Gavin Blakeman.
#                     This file is part of the Astronomy Class Library (ACL).
#
#                     ACL is free software: you can redistribute it and/or modify it under the terms of the GNU General
#                     Public License as published by the Free Software Foundation, either version 2 of the License, or (at your
#                     option) any later version.
#
#                     ACL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
#                     implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
#                     for more details.
#
#                     You should have received a copy of the GNU General Public License along with ACL.  If not, see
#                     <http://www.gnu.org/licenses/>.
#
# OVERVIEW:						Project file for compiling the project
#
# HISTORY:            2013-06-15/GGB - Development of classes for astroManager
#
#-----------------------------------------------------------------------------------------------------------------------------------

TARGET    = ACL
TEMPLATE  = lib
CONFIG    += staticlib

QT        -= core gui

QMAKE_CXXFLAGS += -std=c++17
DEFINES   += BOOST_THREAD_USE_LIB

win32:CONFIG(release, debug|release) {
  DESTDIR = "../Library/win32/release"
  OBJECTS_DIR = "../Library/win32/release/object/ACL"
}
else:win32:CONFIG(debug, debug|release) {
  DESTDIR = "../Library/win32/debug"
  OBJECTS_DIR = "../Library/win32/debug/object/ACL"
}
else:unix:CONFIG(release, debug|release) {
  DESTDIR = ""
  OBJECTS_DIR = "objects"
}
else:unix:CONFIG(debug, debug|release) {
  DESTDIR = ""
  OBJECTS_DIR = "objects"
}

INCLUDEPATH += \
  "../cfitsio" \
  "../dlib-19.4" \
  "../GCL" \
  "../libWCS/wcstools-3.8.7/libwcs" \
  "../MCL" \
  "../NOVAS" \
  "../PCL" \
  "../SBIG" \
  "../SCL" \
  "../SOFA/src" \
  "../" \
  "../GeographicLib/GeographicLib-1.48/include/GeographicLib" \
  "/home/gavin/Documents/Projects/software/Library/Boost/boost_1_71_0" \
  "../LibRaw"

SOURCES += \
    source/ImageStack.cpp \
    source/ImageRegister.cpp \
    source/ImagePlane.cpp \
    source/HDBPrimary.cpp \
    source/HDBPhotometry.cpp \
    source/HDBImage.cpp \
    source/HDBAstrometry.cpp \
    source/HDBAsciiTable.cpp \
    source/HDB.cpp \
    source/FWHM.cpp \
    source/AstroImagePoly.cpp \
    source/AstroImageMono.cpp \
    source/AstroImage.cpp \
    source/AstroFile.cpp \
    source/Observation.cpp \
    source/AstroClass.cpp \
    source/AstroFunctions.cpp \
    source/PhotometryObservation.cpp \
    source/PhotometryApertureCircular.cpp \
    source/PhotometryAperture.cpp \
    source/AstrometryObservation.cpp \
    source/AstroImageCalibration.cpp \
    source/common.cpp \
    source/findstar.cpp \
    source/SourceExtraction.cpp \
    source/AstronomicalTime.cpp \
    source/AAVSO.cpp \
    source/config.cpp \
    source/FITSInspector.cpp \
    source/FITSUtilities.cpp \
    source/FITS.cpp \
    source/FITSException.cpp \
    source/AstronomicalCoordinates.cpp \
    source/FITSKeyword.cpp \
    source/FITSKeywordString.cpp \
    source/FITSKeywordFloat.cpp \
    source/FITSKeywordDouble.cpp \
    source/FITSKeywordComplex.cpp \
    source/FITSKeywordDoubleComplex.cpp \
    source/FITSKeywordInt08.cpp \
    source/FITSKeywordBool.cpp \
    source/FITSKeywordUInt08.cpp \
    source/FITSKeywordInt16.cpp \
    source/FITSKeywordInt32.cpp \
    source/FITSKeywordUInt16.cpp \
    source/FITSKeywordInt64.cpp \
    source/FITSKeywordUInt32.cpp \
    source/FITSKeywordDateTime.cpp \
    source/FITSKeywordDate.cpp \
    source/SIMBAD.cpp \
    source/geographicLocation.cpp \
    source/observatoryInformation.cpp \
    source/photometryFilters.cpp \
    source/julianDay.cpp \
    source/telescope.cpp \
    source/FITSMemoryFileArray.cpp \
    source/FITSMemoryFile.cpp \
    source/error.cpp \
    source/MPCORB.cpp \
    source/targetComet.cpp \
    source/CometEls.cpp \
    source/IERS.cpp \
    source/HDBBinaryTable.cpp \
    source/photometryFunctions.cpp \
    source/photometry.cpp \
    source/targetAstronomy.cpp \
    source/targetMajorPlanet.cpp \
    source/targetMinorPlanet.cpp \
    source/targetStellar.cpp

HEADERS += \
    include/wcs.h \
    include/PhotometryObservation.h \
    include/PhotometryApertureEliptical.h \
    include/PhotometryApertureCircular.h \
    include/PhotometryAperture.h \
    include/Observation.h \
    include/ObserClass.h \
    include/ImageStack.h \
    include/ImageRegister.h \
    include/ImagePlane.h \
    include/HDBPrimary.h \
    include/HDBPhotometry.h \
    include/HDBImage.h \
    include/HDBAstrometry.h \
    include/HDBAsciiTable.h \
    include/HDB.h \
    include/FWHM.h \
    include/config.h \
    include/common.h \
    include/AstroRT.h \
    include/AstrometryObservation.h \
    include/Astrometry.h \
    include/AstroImagePoly.h \
    include/AstroImageMono.h \
    include/AstroImageFunctions.hpp \
    include/AstroImageCalibration.h \
    include/AstroImage.h \
    include/AstroFunctions.h \
    include/AstroFile.h \
    include/AstroClass.h \
    include/AstroCatalogue.h \
    include/AAVSO.h \
    include/findstar.h \
    include/SourceExtraction.h \
    include/constants.h \
    include/AstronomicalTime.h \
    include/FITSInspector.h \
    include/FITSStrings.h \
    include/FITSUtilities.h \
    include/FITS.h \
    include/FITSException.h \
    include/AstronomicalCoordinates.h \
    include/AstronomicalObject.h \
    include/FITSKeyword.h \
    include/FITSKeywordString.h \
    include/FITSKeywordFloat.h \
    include/FITSKeywordDouble.h \
    include/FITSKeywordComplex.h \
    include/FITSKeywordDoubleComplex.h \
    include/FITSKeywordDateTime.h \
    include/FITSKeywordInt16.h \
    include/FITSKeywordInt32.h \
    include/FITSKeywordInt08.h \
    include/FITSKeywordInt64.h \
    include/FITSKeywordUInt08.h \
    include/FITSKeywordUInt16.h \
    include/FITSKeywordUInt32.h \
    include/FITSKeywordBool.h \
    include/FITSKeywordDate.h \
    include/SIMBAD.h \
    include/geographicLocation.h \
    include/observatoryInformation.h \
    include/photometryFilters.h \
    include/julianDay.h \
    include/telescope.h \
    include/FITSMemoryFileArray.h \
    include/FITSMemoryFile.h \
    include/error.h \
    include/RST.h \
    include/MPCORB.h \
    include/targetComet.h \
    include/CometEls.h \
    include/IERS.h \
    include/HDBBinaryTable.h \
    include/photometryFunctions.h \
    include/photometry.h \
    include/targetAstronomy.h \
    include/targetMajorPlanet.h \
    include/targetStellar.h \
    include/targetMinorPlanet.h

OTHER_FILES += \
    ACL \
    license.txt \
    README \
    changelog.txt
