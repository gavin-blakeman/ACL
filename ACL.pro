#-------------------------------------------------
#
# Project created by QtCreator 2013-06-15T19:19:08
#
#-------------------------------------------------

TARGET = ACL
TEMPLATE = lib
CONFIG += staticlib

QT       -= core gui

QMAKE_CXXFLAGS += -std=c++17
DEFINES += BOOST_THREAD_USE_LIB

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
  "../boost 1.62" \
  "../cfitsio" \
  "../dlib-19.4" \
  "../GCL" \
  "../libWCS/wcstools-3.8.7/libwcs" \
  "../MCL" \
  "../novasc3.1" \
  "../PCL" \
  "../SBIG" \
  "../SCL" \
  "../SOFA/src" \
  "../" \
  "../GeographicLib/GeographicLib-1.48/include/GeographicLib"

SOURCES += \
    Source/ImageStack.cpp \
    Source/ImageRegister.cpp \
    Source/ImagePlane.cpp \
    Source/HDBPrimary.cpp \
    Source/HDBPhotometry.cpp \
    Source/HDBImage.cpp \
    Source/HDBBinTable.cpp \
    Source/HDBAstrometry.cpp \
    Source/HDBAsciiTable.cpp \
    Source/HDB.cpp \
    Source/FWHM.cpp \
    Source/AstroImagePoly.cpp \
    Source/AstroImageMono.cpp \
    Source/AstroImage.cpp \
    Source/AstroFile.cpp \
    Source/Observation.cpp \
    Source/AstroClass.cpp \
    Source/AstroFunctions.cpp \
    Source/PhotometryObservation.cpp \
    Source/PhotometryApertureCircular.cpp \
    Source/PhotometryAperture.cpp \
    Source/AstrometryObservation.cpp \
    Source/AstroImageCalibration.cpp \
    Source/common.cpp \
    Source/findstar.cpp \
    Source/SourceExtraction.cpp \
    Source/AstronomicalTime.cpp \
    Source/AAVSO.cpp \
    Source/config.cpp \
    Source/FITSInspector.cpp \
    Source/FITSUtilities.cpp \
    Source/FITS.cpp \
    Source/FITSException.cpp \
    Source/Photometry.cpp \
    Source/AstronomicalCoordinates.cpp \
    Source/TargetAstronomy.cpp \
    Source/TargetSolar.cpp \
    Source/TargetStellar.cpp \
    Source/TargetMinorPlanet.cpp \
    Source/TargetPlanet.cpp \
    Source/FITSKeyword.cpp \
    Source/FITSKeywordString.cpp \
    Source/FITSKeywordFloat.cpp \
    Source/FITSKeywordDouble.cpp \
    Source/FITSKeywordComplex.cpp \
    Source/FITSKeywordDoubleComplex.cpp \
    Source/FITSKeywordInt08.cpp \
    Source/FITSKeywordBool.cpp \
    Source/FITSKeywordUInt08.cpp \
    Source/FITSKeywordInt16.cpp \
    Source/FITSKeywordInt32.cpp \
    Source/FITSKeywordUInt16.cpp \
    Source/FITSKeywordInt64.cpp \
    Source/FITSKeywordUInt32.cpp \
    Source/FITSKeywordDateTime.cpp \
    Source/FITSKeywordDate.cpp \
    Source/SIMBAD.cpp \
    Source/geographicLocation.cpp \
    Source/observatoryInformation.cpp \
    Source/photometryFilters.cpp \
    Source/julianDay.cpp \
    Source/telescope.cpp \
    Source/FITSMemoryFileArray.cpp \
    Source/FITSMemoryFile.cpp

HEADERS += \
    Include/wcs.h \
    Include/PhotometryObservation.h \
    Include/PhotometryApertureEliptical.h \
    Include/PhotometryApertureCircular.h \
    Include/PhotometryAperture.h \
    Include/Observation.h \
    Include/ObserClass.h \
    Include/ImageStack.h \
    Include/ImageRegister.h \
    Include/ImagePlane.h \
    Include/HDBPrimary.h \
    Include/HDBPhotometry.h \
    Include/HDBImage.h \
    Include/HDBBinTable.h \
    Include/HDBAstrometry.h \
    Include/HDBAsciiTable.h \
    Include/HDB.h \
    Include/FWHM.h \
    Include/config.h \
    Include/common.h \
    Include/AstroRT.h \
    Include/AstrometryObservation.h \
    Include/Astrometry.h \
    Include/AstroImagePoly.h \
    Include/AstroImageMono.h \
    Include/AstroImageFunctions.hpp \
    Include/AstroImageCalibration.h \
    Include/AstroImage.h \
    Include/AstroFunctions.h \
    Include/AstroFile.h \
    Include/AstroClass.h \
    Include/AstroCatalogue.h \
    Include/AAVSO.h \
    Include/findstar.h \
    Include/SourceExtraction.h \
    Include/constants.h \
    Include/AstronomicalTime.h \
    Include/FITSInspector.h \
    Include/FITSStrings.h \
    Include/FITSUtilities.h \
    Include/FITS.h \
    Include/FITSException.h \
    Include/Photometry.h \
    Include/AstronomicalCoordinates.h \
    Include/AstronomicalObject.h \
    Include/TargetAstronomy.h \
    Include/TargetSolar.h \
    Include/TargetStellar.h \
    Include/TargetMinorPlanet.h \
    Include/TargetPlanet.h \
    Include/FITSKeyword.h \
    Include/FITSKeywordString.h \
    Include/FITSKeywordFloat.h \
    Include/FITSKeywordDouble.h \
    Include/FITSKeywordComplex.h \
    Include/FITSKeywordDoubleComplex.h \
    Include/FITSKeywordDateTime.h \
    Include/FITSKeywordInt16.h \
    Include/FITSKeywordInt32.h \
    Include/FITSKeywordInt08.h \
    Include/FITSKeywordInt64.h \
    Include/FITSKeywordUInt08.h \
    Include/FITSKeywordUInt16.h \
    Include/FITSKeywordUInt32.h \
    Include/FITSKeywordBool.h \
    Include/FITSKeywordDate.h \
    Include/SIMBAD.h \
    Include/geographicLocation.h \
    Include/observatoryInformation.h \
    Include/photometryFilters.h \
    Include/julianDay.h \
    Include/telescope.h \
    Include/FITSMemoryFileArray.h \
    Include/FITSMemoryFile.h

OTHER_FILES += \
    ACL \
    license.txt \
    README \
    changelog.txt
