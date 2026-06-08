/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <radar/usrp_mimo_echotimer_cc.h>

void bind_usrp_mimo_echotimer_cc(py::module& m)
{
    using usrp_mimo_echotimer_cc = ::gr::radar::usrp_mimo_echotimer_cc;

    py::class_<usrp_mimo_echotimer_cc,
               gr::tagged_stream_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<usrp_mimo_echotimer_cc>>(
        m, "usrp_mimo_echotimer_cc", "")

        .def(py::init(&usrp_mimo_echotimer_cc::make),
             py::arg("samp_rate"),
             py::arg("center_freq"),
             py::arg("num_delay_samps"),
             py::arg("args_tx"),
             py::arg("channel_tx0"),
             py::arg("channel_tx1"),
             py::arg("wire_tx"),
             py::arg("clock_source_tx"),
             py::arg("time_source_tx"),
             py::arg("antenna_tx0"),
             py::arg("antenna_tx1"),
             py::arg("gain_tx0"),
             py::arg("gain_tx1"),
             py::arg("timeout_tx"),
             py::arg("wait_tx"),
             py::arg("lo_offset_tx0"),
             py::arg("lo_offset_tx1"),
             py::arg("args_rx"),
             py::arg("channel_rx0"),
             py::arg("channel_rx1"),
             py::arg("wire_rx"),
             py::arg("clock_source_rx"),
             py::arg("time_source_rx"),
             py::arg("antenna_rx0"),
             py::arg("antenna_rx1"),
             py::arg("gain_rx0"),
             py::arg("gain_rx1"),
             py::arg("timeout_rx"),
             py::arg("wait_rx"),
             py::arg("lo_offset_rx0"),
             py::arg("lo_offset_rx1"),
             py::arg("len_key") = "packet_len",
             "")

        .def("set_num_delay_samps",
             &usrp_mimo_echotimer_cc::set_num_delay_samps,
             py::arg("num_samps"),
             "")

        .def("set_rx_gain",
             &usrp_mimo_echotimer_cc::set_rx_gain,
             py::arg("gain"),
             "")

        .def("set_tx_gain",
             &usrp_mimo_echotimer_cc::set_tx_gain,
             py::arg("gain"),
             "");
}
