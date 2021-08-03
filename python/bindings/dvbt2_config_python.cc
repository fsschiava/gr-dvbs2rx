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
/* BINDTOOL_HEADER_FILE(dvbt2_config.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(a3e7e6ceec8c7e260952bec70c4a6e6d)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <dvbs2rx/dvbt2_config.h>
// pydoc.h is automatically generated in the build directory
#include <dvbt2_config_pydoc.h>

void
bind_dvbt2_config(py::module &m)
{


  py::enum_<::gr::dvbs2rx::dvbt2_rotation_t>(m, "dvbt2_rotation_t")
      .value("ROTATION_OFF", ::gr::dvbs2rx::ROTATION_OFF) // 0
      .value("ROTATION_ON", ::gr::dvbs2rx::ROTATION_ON)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_streamtype_t>(m, "dvbt2_streamtype_t")
      .value("STREAMTYPE_TS", ::gr::dvbs2rx::STREAMTYPE_TS)     // 0
      .value("STREAMTYPE_GS", ::gr::dvbs2rx::STREAMTYPE_GS)     // 1
      .value("STREAMTYPE_BOTH", ::gr::dvbs2rx::STREAMTYPE_BOTH) // 2
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_inputmode_t>(m, "dvbt2_inputmode_t")
      .value("INPUTMODE_NORMAL", ::gr::dvbs2rx::INPUTMODE_NORMAL) // 0
      .value("INPUTMODE_HIEFF", ::gr::dvbs2rx::INPUTMODE_HIEFF)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_extended_carrier_t>(m, "dvbt2_extended_carrier_t")
      .value("CARRIERS_NORMAL", ::gr::dvbs2rx::CARRIERS_NORMAL)     // 0
      .value("CARRIERS_EXTENDED", ::gr::dvbs2rx::CARRIERS_EXTENDED) // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_preamble_t>(m, "dvbt2_preamble_t")
      .value("PREAMBLE_T2_SISO", ::gr::dvbs2rx::PREAMBLE_T2_SISO)           // 0
      .value("PREAMBLE_T2_MISO", ::gr::dvbs2rx::PREAMBLE_T2_MISO)           // 1
      .value("PREAMBLE_NON_T2", ::gr::dvbs2rx::PREAMBLE_NON_T2)             // 2
      .value("PREAMBLE_T2_LITE_SISO", ::gr::dvbs2rx::PREAMBLE_T2_LITE_SISO) // 3
      .value("PREAMBLE_T2_LITE_MISO", ::gr::dvbs2rx::PREAMBLE_T2_LITE_MISO) // 4
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_fftsize_t>(m, "dvbt2_fftsize_t")
      .value("FFTSIZE_2K", ::gr::dvbs2rx::FFTSIZE_2K)             // 0
      .value("FFTSIZE_8K", ::gr::dvbs2rx::FFTSIZE_8K)             // 1
      .value("FFTSIZE_4K", ::gr::dvbs2rx::FFTSIZE_4K)             // 2
      .value("FFTSIZE_1K", ::gr::dvbs2rx::FFTSIZE_1K)             // 3
      .value("FFTSIZE_16K", ::gr::dvbs2rx::FFTSIZE_16K)           // 4
      .value("FFTSIZE_32K", ::gr::dvbs2rx::FFTSIZE_32K)           // 5
      .value("FFTSIZE_8K_T2GI", ::gr::dvbs2rx::FFTSIZE_8K_T2GI)   // 6
      .value("FFTSIZE_32K_T2GI", ::gr::dvbs2rx::FFTSIZE_32K_T2GI) // 7
      .value("FFTSIZE_16K_T2GI", ::gr::dvbs2rx::FFTSIZE_16K_T2GI) // 11
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_papr_t>(m, "dvbt2_papr_t")
      .value("PAPR_OFF", ::gr::dvbs2rx::PAPR_OFF)   // 0
      .value("PAPR_ACE", ::gr::dvbs2rx::PAPR_ACE)   // 1
      .value("PAPR_TR", ::gr::dvbs2rx::PAPR_TR)     // 2
      .value("PAPR_BOTH", ::gr::dvbs2rx::PAPR_BOTH) // 3
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_l1constellation_t>(m, "dvbt2_l1constellation_t")
      .value("L1_MOD_BPSK", ::gr::dvbs2rx::L1_MOD_BPSK)   // 0
      .value("L1_MOD_QPSK", ::gr::dvbs2rx::L1_MOD_QPSK)   // 1
      .value("L1_MOD_16QAM", ::gr::dvbs2rx::L1_MOD_16QAM) // 2
      .value("L1_MOD_64QAM", ::gr::dvbs2rx::L1_MOD_64QAM) // 3
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_pilotpattern_t>(m, "dvbt2_pilotpattern_t")
      .value("PILOT_PP1", ::gr::dvbs2rx::PILOT_PP1) // 0
      .value("PILOT_PP2", ::gr::dvbs2rx::PILOT_PP2) // 1
      .value("PILOT_PP3", ::gr::dvbs2rx::PILOT_PP3) // 2
      .value("PILOT_PP4", ::gr::dvbs2rx::PILOT_PP4) // 3
      .value("PILOT_PP5", ::gr::dvbs2rx::PILOT_PP5) // 4
      .value("PILOT_PP6", ::gr::dvbs2rx::PILOT_PP6) // 5
      .value("PILOT_PP7", ::gr::dvbs2rx::PILOT_PP7) // 6
      .value("PILOT_PP8", ::gr::dvbs2rx::PILOT_PP8) // 7
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_version_t>(m, "dvbt2_version_t")
      .value("VERSION_111", ::gr::dvbs2rx::VERSION_111) // 0
      .value("VERSION_121", ::gr::dvbs2rx::VERSION_121) // 1
      .value("VERSION_131", ::gr::dvbs2rx::VERSION_131) // 2
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_reservedbiasbits_t>(m, "dvbt2_reservedbiasbits_t")
      .value("RESERVED_OFF", ::gr::dvbs2rx::RESERVED_OFF) // 0
      .value("RESERVED_ON", ::gr::dvbs2rx::RESERVED_ON)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_l1scrambled_t>(m, "dvbt2_l1scrambled_t")
      .value("L1_SCRAMBLED_OFF", ::gr::dvbs2rx::L1_SCRAMBLED_OFF) // 0
      .value("L1_SCRAMBLED_ON", ::gr::dvbs2rx::L1_SCRAMBLED_ON)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_misogroup_t>(m, "dvbt2_misogroup_t")
      .value("MISO_TX1", ::gr::dvbs2rx::MISO_TX1) // 0
      .value("MISO_TX2", ::gr::dvbs2rx::MISO_TX2) // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_showlevels_t>(m, "dvbt2_showlevels_t")
      .value("SHOWLEVELS_OFF", ::gr::dvbs2rx::SHOWLEVELS_OFF) // 0
      .value("SHOWLEVELS_ON", ::gr::dvbs2rx::SHOWLEVELS_ON)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_inband_t>(m, "dvbt2_inband_t")
      .value("INBAND_OFF", ::gr::dvbs2rx::INBAND_OFF) // 0
      .value("INBAND_ON", ::gr::dvbs2rx::INBAND_ON)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_equalization_t>(m, "dvbt2_equalization_t")
      .value("EQUALIZATION_OFF", ::gr::dvbs2rx::EQUALIZATION_OFF) // 0
      .value("EQUALIZATION_ON", ::gr::dvbs2rx::EQUALIZATION_ON)   // 1
      .export_values();
  py::enum_<::gr::dvbs2rx::dvbt2_bandwidth_t>(m, "dvbt2_bandwidth_t")
      .value("BANDWIDTH_1_7_MHZ", ::gr::dvbs2rx::BANDWIDTH_1_7_MHZ)   // 0
      .value("BANDWIDTH_5_0_MHZ", ::gr::dvbs2rx::BANDWIDTH_5_0_MHZ)   // 1
      .value("BANDWIDTH_6_0_MHZ", ::gr::dvbs2rx::BANDWIDTH_6_0_MHZ)   // 2
      .value("BANDWIDTH_7_0_MHZ", ::gr::dvbs2rx::BANDWIDTH_7_0_MHZ)   // 3
      .value("BANDWIDTH_8_0_MHZ", ::gr::dvbs2rx::BANDWIDTH_8_0_MHZ)   // 4
      .value("BANDWIDTH_10_0_MHZ", ::gr::dvbs2rx::BANDWIDTH_10_0_MHZ) // 5
      .export_values();
}
