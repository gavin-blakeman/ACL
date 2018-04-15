//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FWHM
// SUBSYSTEM:						Calculation of FWHM value
// LANGUAGE:						C++
// TARGET OS:						None.
// LIBRARY DEPENDANCE:	boost, dlib.
// NAMESPACE:						ACL
// AUTHORS:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2013-2016 Gavin Blakeman.
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
// OVERVIEW:						Functions to calculate the FWHM of a point source. The method used is to fit a gaussian to the data.
//                      A 2D gaussian non-linear least squares fit is perfomed.
//
//
// CLASSES INCLUDED:		None.
//
// FUNCTIONS INCLUDED:  FWHM
//
// CLASS HIERARCHY:     None
//
// HISTORY:             2015-09-22 GGB - astroManager 2015.09 release
//                      2013-09-30 GGB - astroManager 2013.09 release.
//                      2013-04-25 GGB - Development of classes for astroManager
//
//*********************************************************************************************************************************

#include "../Include/FWHM.h"

#include <MCL>

namespace ACL
{
  typedef dlib::matrix<FP_t,4,1> parameter_vector;

  /// @brief Function for the model.
  /// @throws None.
  /// @version 2013-04-25/GGB - Function created.

  FP_t model(input_vector const &input, parameter_vector const &params)
  {
    FP_t const A = params(0);
    FP_t const xo = params(1);
    FP_t const yo = params(2);
    FP_t const w = params(3);

    FP_t const x = input(0);
    FP_t const y = input(1);

      // Follows in the generic 2D gaussian function.

    return (A * std::exp( - (MCL::pow2(x-xo) + MCL::pow2(y-yo)) / (2 * MCL::pow2(w))));
  }

  /// @brief Function to compute the residuals for the FWHM.
  /// @throws None.
  /// @version 2013-04-25/GGB - Function created.

  FP_t residual(std::pair<input_vector, FP_t> const &data, parameter_vector const &params)
  {
    return (model(data.first, params) - data.second);
  }

  parameter_vector residual_derivative(std::pair<input_vector, FP_t> const &data,
                                       parameter_vector const &params)
  {
    parameter_vector derivatives;

    FP_t const A = params(0);
    FP_t const xo = params(1);
    FP_t const yo = params(2);
    FP_t const w = params(3);

    FP_t const x = data.first(0);
    FP_t const y = data.first(1);

    derivatives(0) = 1;

    return derivatives;
  }

  /// @brief Determine the FWHM of a list of data samples.
  /// @details Makes use of the dlib function to solve the non-linear least squared.
  /// @version 2013-04-25/GGB - Function created.

  FP_t FWHM(std::vector<std::pair<input_vector, FP_t> > const dataSamples)
  {
    parameter_vector x;
    x = 1;

    dlib::solve_least_squares(dlib::objective_delta_stop_strategy(1e-7, 1000)/*.be_verbose()*/,
                              &residual,
                              dlib::derivative(&residual),
                              dataSamples,
                              x);
    return (2.35482 * std::abs(x(3)) );      // abs() as valid solution is w = negative.
  }

}  // namespace ACL
