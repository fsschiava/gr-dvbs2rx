/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(dvbs2_config.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(1693754357c0059dddba29e257e5c526)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <dvbs2rx/dvbs2_config.h>
// pydoc.h is automatically generated in the build directory
#include <dvbs2_config_pydoc.h>

void
bind_dvbs2_config(py::module &m)
{


  py::enum_<::gr::dvbs2rx::dvbs2_rolloff_factor_t>(m, "dvbs2_rolloff_factor_t")
      .value("RO_0_35", ::gr::dvbs2rx::RO_0_35)         // 0
      .value("RO_0_25", ::gr::dvbs2rx::RO_0_25)         // 1
      .value("RO_0_20", ::gr::dvbs2rx::RO_0_20)         // 2
      .value("RO_RESERVED", ::gr::dvbs2rx::RO_RESERVED) // 3
      .value("RO_0_15", ::gr::dvbs2rx::RO_0_15)         // 4
      .value("RO_0_10", ::gr::dvbs2rx::RO_0_10)         // 5
      .value("RO_0_05", ::gr::dvbs2rx::RO_0_05)         // 6
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbs2_pilots_t>(m, "dvbs2_pilots_t")
      .value("PILOTS_OFF", ::gr::dvbs2rx::PILOTS_OFF) // 0
      .value("PILOTS_ON", ::gr::dvbs2rx::PILOTS_ON)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbs2_interpolation_t>(m, "dvbs2_interpolation_t")
      .value("INTERPOLATION_OFF", ::gr::dvbs2rx::INTERPOLATION_OFF) // 0
      .value("INTERPOLATION_ON", ::gr::dvbs2rx::INTERPOLATION_ON)   // 1
      .export_values();
}
