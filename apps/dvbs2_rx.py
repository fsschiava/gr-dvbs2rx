#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# DVB-S2 Rx
#
# Full DVB-S2 receiver. Processes input IQ samples and outputs MPEG TS packets.

import ctypes
import os
import signal
import sys
from argparse import ArgumentParser
from distutils.version import StrictVersion

import sip
from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio import blocks
from gnuradio import gr
from gnuradio.fft import window

from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation

sys.path.append(
    os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))
from dvbs2rx_rx_hier import dvbs2rx_rx_hier  # noqa: E402


class dvbs2_rx(gr.top_block, Qt.QWidget):
    def __init__(self,
                 agc_gain=1,
                 agc_rate=1e-5,
                 agc_ref=1,
                 debug=0,
                 frame_size='normal',
                 gold_code=0,
                 gui=False,
                 in_fd=0,
                 modcod='QPSK1/4',
                 out_fd='1',
                 out_filename="",
                 pl_freq_est_period=20,
                 rolloff=0.2,
                 rrc_delay=50,
                 rrc_nfilts=32,
                 sink="fd",
                 source="fd",
                 sps=2,
                 sym_rate=1000000,
                 sym_sync_damping=1.0,
                 sym_sync_loop_bw=0.0045):
        gr.top_block.__init__(self, "DVB-S2 Rx", catch_exceptions=True)

        ##################################################
        # Parameters
        ##################################################
        self.agc_gain = agc_gain
        self.agc_rate = agc_rate
        self.agc_ref = agc_ref
        self.debug = debug
        self.frame_size = frame_size
        self.gold_code = gold_code
        self.gui = gui
        self.in_fd = in_fd
        self.modcod = modcod
        self.out_fd = out_fd
        self.out_filename = out_filename
        self.pl_freq_est_period = pl_freq_est_period
        self.rolloff = rolloff
        self.rrc_delay = rrc_delay
        self.rrc_nfilts = rrc_nfilts
        self.sink = sink
        self.source = source
        self.sps = sps
        self.sym_rate = sym_rate
        self.sym_sync_damping = sym_sync_damping
        self.sym_sync_loop_bw = sym_sync_loop_bw

        ##################################################
        # Variables
        ##################################################
        code_rate = modcod.upper().replace("8PSK", "").replace("QPSK", "")
        self.code_rate = code_rate
        self.samp_rate = sym_rate * sps
        self.plheader_len = 90
        self.plframe_len = 33282
        self.pilot_len = int((360 - 1) / 16) * 36
        self.n_rrc_taps = int(rrc_delay * sps) + 1
        self.constellation = modcod.replace(code_rate, "")

        if (self.gui):
            self.setup_gui()

        self.setup_flowgraph()

    def setup_gui(self):
        Qt.QWidget.__init__(self)
        self.setWindowTitle("DVB-S2 Rx")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "dvbs2_rx")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(
                    self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Blocks
        ##################################################
        self.tabs = Qt.QTabWidget()
        self.tabs_widget_0 = Qt.QWidget()
        self.tabs_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom,
                                           self.tabs_widget_0)
        self.tabs_grid_layout_0 = Qt.QGridLayout()
        self.tabs_layout_0.addLayout(self.tabs_grid_layout_0)
        self.tabs.addTab(self.tabs_widget_0, 'Input')
        self.tabs_widget_1 = Qt.QWidget()
        self.tabs_layout_1 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom,
                                           self.tabs_widget_1)
        self.tabs_grid_layout_1 = Qt.QGridLayout()
        self.tabs_layout_1.addLayout(self.tabs_grid_layout_1)
        self.tabs.addTab(self.tabs_widget_1, 'Frequency Correction')
        self.tabs_widget_2 = Qt.QWidget()
        self.tabs_layout_2 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom,
                                           self.tabs_widget_2)
        self.tabs_grid_layout_2 = Qt.QGridLayout()
        self.tabs_layout_2.addLayout(self.tabs_grid_layout_2)
        self.tabs.addTab(self.tabs_widget_2, 'Symbol Sync')
        self.tabs_widget_3 = Qt.QWidget()
        self.tabs_layout_3 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom,
                                           self.tabs_widget_3)
        self.tabs_grid_layout_3 = Qt.QGridLayout()
        self.tabs_layout_3.addLayout(self.tabs_grid_layout_3)
        self.tabs.addTab(self.tabs_widget_3, 'Frame Recovery')
        self.tabs_widget_4 = Qt.QWidget()
        self.tabs_layout_4 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom,
                                           self.tabs_widget_4)
        self.tabs_grid_layout_4 = Qt.QGridLayout()
        self.tabs_layout_4.addLayout(self.tabs_grid_layout_4)
        self.tabs.addTab(self.tabs_widget_4, 'Phase Recovery')
        self.top_grid_layout.addWidget(self.tabs, 0, 0, 1, 1)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_waterfall_sink_x_0 = qtgui.waterfall_sink_c(
            1024,  # size
            window.WIN_BLACKMAN_hARRIS,  # wintype
            0,  # fc
            self.samp_rate,  # bw
            "Spectrogram",  # name
            1,  # number of inputs
            None  # parent
        )
        self.qtgui_waterfall_sink_x_0.set_update_time(0.10)
        self.qtgui_waterfall_sink_x_0.enable_grid(False)
        self.qtgui_waterfall_sink_x_0.enable_axis_labels(True)

        labels = ['', '', '', '', '', '', '', '', '', '']
        colors = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_waterfall_sink_x_0.set_line_label(
                    i, "Data {0}".format(i))
            else:
                self.qtgui_waterfall_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_waterfall_sink_x_0.set_color_map(i, colors[i])
            self.qtgui_waterfall_sink_x_0.set_line_alpha(i, alphas[i])

        self.qtgui_waterfall_sink_x_0.set_intensity_range(-140, 10)

        self._qtgui_waterfall_sink_x_0_win = sip.wrapinstance(
            self.qtgui_waterfall_sink_x_0.pyqwidget(), Qt.QWidget)
        self.tabs_grid_layout_0.addWidget(self._qtgui_waterfall_sink_x_0_win,
                                          0, 0, 1, 3)
        for r in range(0, 1):
            self.tabs_grid_layout_0.setRowStretch(r, 1)
        for c in range(0, 3):
            self.tabs_grid_layout_0.setColumnStretch(c, 1)
        self.qtgui_time_sink_x_1 = qtgui.time_sink_c(
            self.plframe_len - self.pilot_len - self.plheader_len,  # size
            self.sym_rate,  # samp_rate
            "",  # name
            1,  # number of inputs
            None  # parent
        )
        self.qtgui_time_sink_x_1.set_update_time(0.10)
        self.qtgui_time_sink_x_1.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_1.set_y_label('PLFRAME Symbols', "")

        self.qtgui_time_sink_x_1.enable_tags(True)
        self.qtgui_time_sink_x_1.set_trigger_mode(qtgui.TRIG_MODE_FREE,
                                                  qtgui.TRIG_SLOPE_POS, 0.0, 0,
                                                  0, "")
        self.qtgui_time_sink_x_1.enable_autoscale(True)
        self.qtgui_time_sink_x_1.enable_grid(True)
        self.qtgui_time_sink_x_1.enable_axis_labels(True)
        self.qtgui_time_sink_x_1.enable_control_panel(False)
        self.qtgui_time_sink_x_1.enable_stem_plot(False)

        labels = [
            'I', 'Q', 'Signal 3', 'Signal 4', 'Signal 5', 'Signal 6',
            'Signal 7', 'Signal 8', 'Signal 9', 'Signal 10'
        ]
        widths = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        colors = [
            'blue', 'red', 'green', 'black', 'cyan', 'magenta', 'yellow',
            'dark red', 'dark green', 'dark blue'
        ]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
        styles = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1, -1, -1, -1, -1, -1]

        for i in range(2):
            if len(labels[i]) == 0:
                if (i % 2 == 0):
                    self.qtgui_time_sink_x_1.set_line_label(
                        i, "Re{{Data {0}}}".format(i / 2))
                else:
                    self.qtgui_time_sink_x_1.set_line_label(
                        i, "Im{{Data {0}}}".format(i / 2))
            else:
                self.qtgui_time_sink_x_1.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_1_win = sip.wrapinstance(
            self.qtgui_time_sink_x_1.pyqwidget(), Qt.QWidget)
        self.tabs_layout_3.addWidget(self._qtgui_time_sink_x_1_win)
        self.qtgui_number_sink_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_VERT,
            1,
            None  # parent
        )
        self.qtgui_number_sink_0.set_update_time(0.10)
        self.qtgui_number_sink_0.set_title("RMS Level")

        labels = ['', '', '', '', '', '', '', '', '', '']
        units = ['', '', '', '', '', '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black")]
        factor = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]

        for i in range(1):
            self.qtgui_number_sink_0.set_min(i, -1)
            self.qtgui_number_sink_0.set_max(i, 1)
            self.qtgui_number_sink_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0.set_label(i, labels[i])
            self.qtgui_number_sink_0.set_unit(i, units[i])
            self.qtgui_number_sink_0.set_factor(i, factor[i])

        self.qtgui_number_sink_0.enable_autoscale(False)
        self._qtgui_number_sink_0_win = sip.wrapinstance(
            self.qtgui_number_sink_0.pyqwidget(), Qt.QWidget)
        self.tabs_grid_layout_0.addWidget(self._qtgui_number_sink_0_win, 0, 3,
                                          1, 1)
        for r in range(0, 1):
            self.tabs_grid_layout_0.setRowStretch(r, 1)
        for c in range(3, 4):
            self.tabs_grid_layout_0.setColumnStretch(c, 1)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
            1024,  # size
            window.WIN_BLACKMAN_hARRIS,  # wintype
            0,  # fc
            self.samp_rate,  # bw
            "Frequency Correction",  # name
            2,
            None  # parent
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0,
                                                  "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(True)
        self.qtgui_freq_sink_x_0.set_fft_average(0.05)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)
        self.qtgui_freq_sink_x_0.set_fft_window_normalized(False)

        labels = ['Before', 'After', '', '', '', '', '', '', '', '']
        widths = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        colors = [
            "blue", "red", "green", "black", "cyan", "magenta", "yellow",
            "dark red", "dark green", "dark blue"
        ]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(2):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(
                    i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(
            self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.tabs_layout_1.addWidget(self._qtgui_freq_sink_x_0_win)
        self.qtgui_const_sink_x_1_0 = qtgui.const_sink_c(
            1024,  # size
            "Symbol Sync Output",  # name
            1,  # number of inputs
            None  # parent
        )
        self.qtgui_const_sink_x_1_0.set_update_time(0.10)
        self.qtgui_const_sink_x_1_0.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_1_0.set_x_axis(-2, 2)
        self.qtgui_const_sink_x_1_0.set_trigger_mode(qtgui.TRIG_MODE_FREE,
                                                     qtgui.TRIG_SLOPE_POS, 0.0,
                                                     0, "")
        self.qtgui_const_sink_x_1_0.enable_autoscale(False)
        self.qtgui_const_sink_x_1_0.enable_grid(False)
        self.qtgui_const_sink_x_1_0.enable_axis_labels(True)

        self.qtgui_const_sink_x_1_0.disable_legend()

        labels = ['', '', '', '', '', '', '', '', '', '']
        widths = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        colors = [
            "blue", "red", "red", "red", "red", "red", "red", "red", "red",
            "red"
        ]
        styles = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_1_0.set_line_label(
                    i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_1_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_1_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_1_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_1_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_1_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_1_0.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_1_0_win = sip.wrapinstance(
            self.qtgui_const_sink_x_1_0.pyqwidget(), Qt.QWidget)
        self.tabs_layout_2.addWidget(self._qtgui_const_sink_x_1_0_win)
        self.qtgui_const_sink_x_1 = qtgui.const_sink_c(
            1024,  # size
            "PL Sync Output",  # name
            1,  # number of inputs
            None  # parent
        )
        self.qtgui_const_sink_x_1.set_update_time(0.10)
        self.qtgui_const_sink_x_1.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_1.set_x_axis(-2, 2)
        self.qtgui_const_sink_x_1.set_trigger_mode(qtgui.TRIG_MODE_FREE,
                                                   qtgui.TRIG_SLOPE_POS, 0.0,
                                                   0, "")
        self.qtgui_const_sink_x_1.enable_autoscale(False)
        self.qtgui_const_sink_x_1.enable_grid(False)
        self.qtgui_const_sink_x_1.enable_axis_labels(True)

        self.qtgui_const_sink_x_1.disable_legend()

        labels = ['', '', '', '', '', '', '', '', '', '']
        widths = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        colors = [
            "blue", "red", "red", "red", "red", "red", "red", "red", "red",
            "red"
        ]
        styles = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_1.set_line_label(
                    i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_1.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_1.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_1.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_1.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_1.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_1.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_1_win = sip.wrapinstance(
            self.qtgui_const_sink_x_1.pyqwidget(), Qt.QWidget)
        self.tabs_layout_4.addWidget(self._qtgui_const_sink_x_1_win)

    def setup_flowgraph(self):
        # IQ Sample Source
        if (self.source == "fd"):
            # Read IQ stream from a file descriptor while assuming the stream
            # is composed of uint8_t I and Q values. Convert the independent
            # uint8_t streams into a single complex stream.
            self.blocks_fd_source = blocks.file_descriptor_source(
                gr.sizeof_char, self.in_fd, False)
            self.blocks_deinterleave = blocks.deinterleave(gr.sizeof_char, 1)
            self.blocks_uchar_to_float_0 = blocks.uchar_to_float()
            self.blocks_uchar_to_float_1 = blocks.uchar_to_float()
            self.blocks_add_const_ff_0 = blocks.add_const_ff(-127)
            self.blocks_add_const_ff_1 = blocks.add_const_ff(-127)
            self.blocks_multiply_const_ff_1 = blocks.multiply_const_ff(1 / 128)
            self.blocks_multiply_const_ff_0 = blocks.multiply_const_ff(1 / 128)
            self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
            self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex,
                                                     self.samp_rate, True)
            self.connect((self.blocks_fd_source, 0),
                         (self.blocks_deinterleave, 0))
            self.connect((self.blocks_deinterleave, 0),
                         (self.blocks_uchar_to_float_0, 0),
                         (self.blocks_add_const_ff_0, 0),
                         (self.blocks_multiply_const_ff_0, 0),
                         (self.blocks_float_to_complex_0, 0))
            self.connect((self.blocks_deinterleave, 1),
                         (self.blocks_uchar_to_float_1, 0),
                         (self.blocks_add_const_ff_1, 0),
                         (self.blocks_multiply_const_ff_1, 0),
                         (self.blocks_float_to_complex_0, 1))
            self.connect((self.blocks_float_to_complex_0, 0),
                         (self.blocks_throttle_0, 0))
            # Alias to connect to the IQ source
            self.source_block = self.blocks_throttle_0

        # DVB-S2 Rx
        self.dvbs2rx_rx_hier_0 = dvbs2rx_rx_hier(
            agc_gain=self.agc_gain,
            agc_rate=self.agc_rate,
            agc_ref=self.agc_ref,
            debug=self.debug,
            frame_size=self.frame_size,
            gold_code=self.gold_code,
            modcod=self.modcod,
            pl_freq_est_period=self.pl_freq_est_period,
            rolloff=self.rolloff,
            rrc_delay=self.rrc_delay,
            rrc_nfilts=self.rrc_nfilts,
            sps=self.sps,
            sym_rate=self.sym_rate,
            sym_sync_damping=self.sym_sync_damping,
            sym_sync_loop_bw=self.sym_sync_loop_bw,
        )
        self.connect((self.source_block, 0), (self.dvbs2rx_rx_hier_0, 0))

        # Sink
        if (self.sink == "fd"):
            self.blocks_fd_sink = blocks.file_descriptor_sink(
                gr.sizeof_char, self.out_fd)
            self.connect((self.dvbs2rx_rx_hier_0, 0), (self.blocks_fd_sink, 0))
        elif (self.sink == "file"):
            self.blocks_file_sink = blocks.file_sink(gr.sizeof_char,
                                                     self.out_filename)
            self.connect((self.dvbs2rx_rx_hier_0, 0), (self.blocks_fd_sink, 0))

        if (self.gui):
            self.blocks_rms_xx_0 = blocks.rms_cf(0.0001)
            self.connect((self.source_block, 0), (self.blocks_rms_xx_0, 0))
            self.connect((self.source_block, 0),
                         (self.qtgui_waterfall_sink_x_0, 0))
            self.connect((self.blocks_rms_xx_0, 0),
                         (self.qtgui_number_sink_0, 0))
            self.connect((self.dvbs2rx_rx_hier_0, 1),
                         (self.qtgui_const_sink_x_1_0, 0))
            self.connect((self.dvbs2rx_rx_hier_0, 2),
                         (self.qtgui_const_sink_x_1, 0))
            self.connect((self.dvbs2rx_rx_hier_0, 2),
                         (self.qtgui_time_sink_x_1, 0))
            self.connect((self.dvbs2rx_rx_hier_0, 3),
                         (self.qtgui_freq_sink_x_0, 1))
            self.connect((self.dvbs2rx_rx_hier_0, 4),
                         (self.qtgui_freq_sink_x_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "dvbs2_rx")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()
        event.accept()


def argument_parser():
    description = 'DVB-S2 receiver'
    parser = ArgumentParser(description=description)
    parser.add_argument("--source",
                        choices=["fd"],
                        default="fd",
                        help="Source of the input IQ sample stream")
    parser.add_argument("--sink",
                        choices=["fd", "file"],
                        default="fd",
                        help="Sink for the output MPEG transport stream")
    parser.add_argument("--gui",
                        action='store_true',
                        default=False,
                        help="Launch a graphical user interface (GUI).")
    parser.add_argument("--agc-gain",
                        dest="agc_gain",
                        type=eng_float,
                        default=eng_notation.num_to_str(float(1)),
                        help="AGC gain [default=%(default)r]")
    parser.add_argument("--agc-rate",
                        dest="agc_rate",
                        type=eng_float,
                        default=eng_notation.num_to_str(float(1e-5)),
                        help="AGC update rate [default=%(default)r]")
    parser.add_argument("--agc-ref",
                        dest="agc_ref",
                        type=eng_float,
                        default=eng_notation.num_to_str(float(1)),
                        help="AGC's reference value [default=%(default)r]")
    parser.add_argument("-d",
                        "--debug",
                        dest="debug",
                        type=intx,
                        default=0,
                        help="Debugging level [default=%(default)r]")
    parser.add_argument("-f",
                        "--frame-size",
                        dest="frame_size",
                        type=str,
                        default='normal',
                        help="FECFRAME size [default=%(default)r]")
    parser.add_argument("--gold-code",
                        dest="gold_code",
                        type=intx,
                        default=0,
                        help="Gold code [default=%(default)r]")
    parser.add_argument("-I",
                        "--in-fd",
                        dest="in_fd",
                        type=intx,
                        default=0,
                        help="Input file descriptor [default=%(default)r]")
    parser.add_argument("-m",
                        "--modcod",
                        dest="modcod",
                        type=str,
                        default='QPSK1/4',
                        help="Target MODCOD [default=%(default)r]")
    parser.add_argument(
        "-O",
        "--out-fd",
        dest="out_fd",
        type=intx,
        default=1,
        help="Output file descriptor if \"--sink=fd\" [default=%(default)r]")
    parser.add_argument(
        "--out-filename",
        type=str,
        help="Output file name if \"--sink=file\" [default=%(default)r]")
    parser.add_argument(
        "--pl-freq-est-period",
        dest="pl_freq_est_period",
        type=intx,
        default=20,
        help="Coarse frequency offset estimation period in frames "
        "[default=%(default)r]")
    parser.add_argument("-r",
                        "--rolloff",
                        dest="rolloff",
                        type=eng_float,
                        default=eng_notation.num_to_str(float(0.2)),
                        help="Rolloff factor [default=%(default)r]")
    parser.add_argument(
        "--rrc-delay",
        dest="rrc_delay",
        type=intx,
        default=50,
        help="RRC filter delay in symbol periods [default=%(default)r]")
    parser.add_argument("--rrc-nfilts",
                        dest="rrc_nfilts",
                        type=intx,
                        default=32,
                        help="Number of branches on the polyphase RRC filter "
                        "[default=%(default)r]")
    parser.add_argument(
        "-o",
        "--sps",
        dest="sps",
        type=eng_float,
        default=eng_notation.num_to_str(float(2)),
        help="Oversampling ratio in samples per symbol [default=%(default)r]")
    parser.add_argument("-s",
                        "--sym-rate",
                        dest="sym_rate",
                        type=intx,
                        default=1000000,
                        help="Symbol rate in bauds [default=%(default)r]")
    parser.add_argument(
        "--sym-sync-damping",
        dest="sym_sync_damping",
        type=eng_float,
        default=eng_notation.num_to_str(float(1.0)),
        help="Symbol synchronizer's damping factor [default=%(default)r]")
    parser.add_argument(
        "--sym-sync-loop-bw",
        dest="sym_sync_loop_bw",
        type=eng_float,
        default=eng_notation.num_to_str(float(0.0045)),
        help="Symbol synchronizer's loop bandwidth [default=%(default)r]")
    return parser


def main():
    options = argument_parser().parse_args()

    gui_mode = options.gui
    if (gui_mode):
        if sys.platform.startswith('linux'):
            try:
                x11 = ctypes.cdll.LoadLibrary('libX11.so')
                x11.XInitThreads()
            except:
                print("Warning: failed to XInitThreads()")

        if StrictVersion("4.5.0") <= StrictVersion(
                Qt.qVersion()) < StrictVersion("5.0.0"):
            style = gr.prefs().get_string('qtgui', 'style', 'raster')
            Qt.QApplication.setGraphicsSystem(style)
        qapp = Qt.QApplication(sys.argv)

    tb = dvbs2_rx(agc_gain=options.agc_gain,
                  agc_rate=options.agc_rate,
                  agc_ref=options.agc_ref,
                  debug=options.debug,
                  frame_size=options.frame_size,
                  gold_code=options.gold_code,
                  gui=options.gui,
                  in_fd=options.in_fd,
                  modcod=options.modcod,
                  out_fd=options.out_fd,
                  out_filename=options.out_filename,
                  pl_freq_est_period=options.pl_freq_est_period,
                  rolloff=options.rolloff,
                  rrc_delay=options.rrc_delay,
                  rrc_nfilts=options.rrc_nfilts,
                  sink=options.sink,
                  source=options.source,
                  sps=options.sps,
                  sym_rate=options.sym_rate,
                  sym_sync_damping=options.sym_sync_damping,
                  sym_sync_loop_bw=options.sym_sync_loop_bw)

    tb.start()
    if (gui_mode):
        tb.show()
    else:
        tb.wait()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()
        if (gui_mode):
            Qt.QApplication.quit()
        else:
            sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    if (gui_mode):
        timer = Qt.QTimer()
        timer.start(500)
        timer.timeout.connect(lambda: None)
        qapp.exec_()


if __name__ == '__main__':
    main()
