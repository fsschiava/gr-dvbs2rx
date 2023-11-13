/* -*- c++ -*- */
/*
 * Copyright 2018,2019 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cpu_features_macros.h"
#include "debug_level.h"
#include "fec_params.h"
#include "ldpc_decoder_cb_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <boost/format.hpp>
#include <cmath>

#ifdef CPU_FEATURES_ARCH_ARM
#include "cpuinfo_arm.h"
using namespace cpu_features;
static const ArmFeatures features = GetArmInfo().features;
static const bool has_neon = features.neon;
#endif

#ifdef CPU_FEATURES_ARCH_AARCH64
static const bool has_neon = true; // always available on aarch64
#endif

#ifdef CPU_FEATURES_ARCH_X86
#include "cpuinfo_x86.h"
using namespace cpu_features;
static const X86Features features = GetX86Info().features;
#endif

namespace ldpc_neon {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_neon

namespace ldpc_avx2 {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_avx2

namespace ldpc_sse41 {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_sse41

namespace ldpc_generic {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_generic

constexpr int DVB_S2_TABLE_B1::DEG[];
constexpr int DVB_S2_TABLE_B1::LEN[];
constexpr int DVB_S2_TABLE_B1::POS[];

constexpr int DVB_S2_TABLE_B2::DEG[];
constexpr int DVB_S2_TABLE_B2::LEN[];
constexpr int DVB_S2_TABLE_B2::POS[];

constexpr int DVB_S2_TABLE_B3::DEG[];
constexpr int DVB_S2_TABLE_B3::LEN[];
constexpr int DVB_S2_TABLE_B3::POS[];

constexpr int DVB_S2_TABLE_B4::DEG[];
constexpr int DVB_S2_TABLE_B4::LEN[];
constexpr int DVB_S2_TABLE_B4::POS[];

constexpr int DVB_S2_TABLE_B5::DEG[];
constexpr int DVB_S2_TABLE_B5::LEN[];
constexpr int DVB_S2_TABLE_B5::POS[];

constexpr int DVB_S2_TABLE_B6::DEG[];
constexpr int DVB_S2_TABLE_B6::LEN[];
constexpr int DVB_S2_TABLE_B6::POS[];

constexpr int DVB_S2_TABLE_B7::DEG[];
constexpr int DVB_S2_TABLE_B7::LEN[];
constexpr int DVB_S2_TABLE_B7::POS[];

constexpr int DVB_S2_TABLE_B8::DEG[];
constexpr int DVB_S2_TABLE_B8::LEN[];
constexpr int DVB_S2_TABLE_B8::POS[];

constexpr int DVB_S2_TABLE_B9::DEG[];
constexpr int DVB_S2_TABLE_B9::LEN[];
constexpr int DVB_S2_TABLE_B9::POS[];

constexpr int DVB_S2_TABLE_B10::DEG[];
constexpr int DVB_S2_TABLE_B10::LEN[];
constexpr int DVB_S2_TABLE_B10::POS[];

constexpr int DVB_S2_TABLE_B11::DEG[];
constexpr int DVB_S2_TABLE_B11::LEN[];
constexpr int DVB_S2_TABLE_B11::POS[];

constexpr int DVB_S2X_TABLE_B1::DEG[];
constexpr int DVB_S2X_TABLE_B1::LEN[];
constexpr int DVB_S2X_TABLE_B1::POS[];

constexpr int DVB_S2X_TABLE_B2::DEG[];
constexpr int DVB_S2X_TABLE_B2::LEN[];
constexpr int DVB_S2X_TABLE_B2::POS[];

constexpr int DVB_S2X_TABLE_B3::DEG[];
constexpr int DVB_S2X_TABLE_B3::LEN[];
constexpr int DVB_S2X_TABLE_B3::POS[];

constexpr int DVB_S2X_TABLE_B4::DEG[];
constexpr int DVB_S2X_TABLE_B4::LEN[];
constexpr int DVB_S2X_TABLE_B4::POS[];

constexpr int DVB_S2X_TABLE_B5::DEG[];
constexpr int DVB_S2X_TABLE_B5::LEN[];
constexpr int DVB_S2X_TABLE_B5::POS[];

constexpr int DVB_S2X_TABLE_B6::DEG[];
constexpr int DVB_S2X_TABLE_B6::LEN[];
constexpr int DVB_S2X_TABLE_B6::POS[];

constexpr int DVB_S2X_TABLE_B7::DEG[];
constexpr int DVB_S2X_TABLE_B7::LEN[];
constexpr int DVB_S2X_TABLE_B7::POS[];

constexpr int DVB_S2X_TABLE_B8::DEG[];
constexpr int DVB_S2X_TABLE_B8::LEN[];
constexpr int DVB_S2X_TABLE_B8::POS[];

constexpr int DVB_S2X_TABLE_B9::DEG[];
constexpr int DVB_S2X_TABLE_B9::LEN[];
constexpr int DVB_S2X_TABLE_B9::POS[];

constexpr int DVB_S2X_TABLE_B10::DEG[];
constexpr int DVB_S2X_TABLE_B10::LEN[];
constexpr int DVB_S2X_TABLE_B10::POS[];

constexpr int DVB_S2X_TABLE_B11::DEG[];
constexpr int DVB_S2X_TABLE_B11::LEN[];
constexpr int DVB_S2X_TABLE_B11::POS[];

constexpr int DVB_S2X_TABLE_B12::DEG[];
constexpr int DVB_S2X_TABLE_B12::LEN[];
constexpr int DVB_S2X_TABLE_B12::POS[];

constexpr int DVB_S2X_TABLE_B13::DEG[];
constexpr int DVB_S2X_TABLE_B13::LEN[];
constexpr int DVB_S2X_TABLE_B13::POS[];

constexpr int DVB_S2X_TABLE_B14::DEG[];
constexpr int DVB_S2X_TABLE_B14::LEN[];
constexpr int DVB_S2X_TABLE_B14::POS[];

constexpr int DVB_S2X_TABLE_B15::DEG[];
constexpr int DVB_S2X_TABLE_B15::LEN[];
constexpr int DVB_S2X_TABLE_B15::POS[];

constexpr int DVB_S2X_TABLE_B16::DEG[];
constexpr int DVB_S2X_TABLE_B16::LEN[];
constexpr int DVB_S2X_TABLE_B16::POS[];

constexpr int DVB_S2X_TABLE_B17::DEG[];
constexpr int DVB_S2X_TABLE_B17::LEN[];
constexpr int DVB_S2X_TABLE_B17::POS[];

constexpr int DVB_S2X_TABLE_B18::DEG[];
constexpr int DVB_S2X_TABLE_B18::LEN[];
constexpr int DVB_S2X_TABLE_B18::POS[];

constexpr int DVB_S2X_TABLE_B19::DEG[];
constexpr int DVB_S2X_TABLE_B19::LEN[];
constexpr int DVB_S2X_TABLE_B19::POS[];

constexpr int DVB_S2X_TABLE_B20::DEG[];
constexpr int DVB_S2X_TABLE_B20::LEN[];
constexpr int DVB_S2X_TABLE_B20::POS[];

constexpr int DVB_S2X_TABLE_B21::DEG[];
constexpr int DVB_S2X_TABLE_B21::LEN[];
constexpr int DVB_S2X_TABLE_B21::POS[];

constexpr int DVB_S2X_TABLE_B22::DEG[];
constexpr int DVB_S2X_TABLE_B22::LEN[];
constexpr int DVB_S2X_TABLE_B22::POS[];

constexpr int DVB_S2X_TABLE_B23::DEG[];
constexpr int DVB_S2X_TABLE_B23::LEN[];
constexpr int DVB_S2X_TABLE_B23::POS[];

constexpr int DVB_S2X_TABLE_B24::DEG[];
constexpr int DVB_S2X_TABLE_B24::LEN[];
constexpr int DVB_S2X_TABLE_B24::POS[];

constexpr int DVB_S2_TABLE_C1::DEG[];
constexpr int DVB_S2_TABLE_C1::LEN[];
constexpr int DVB_S2_TABLE_C1::POS[];

constexpr int DVB_S2_TABLE_C2::DEG[];
constexpr int DVB_S2_TABLE_C2::LEN[];
constexpr int DVB_S2_TABLE_C2::POS[];

constexpr int DVB_S2_TABLE_C3::DEG[];
constexpr int DVB_S2_TABLE_C3::LEN[];
constexpr int DVB_S2_TABLE_C3::POS[];

constexpr int DVB_S2_TABLE_C4::DEG[];
constexpr int DVB_S2_TABLE_C4::LEN[];
constexpr int DVB_S2_TABLE_C4::POS[];

constexpr int DVB_S2_TABLE_C5::DEG[];
constexpr int DVB_S2_TABLE_C5::LEN[];
constexpr int DVB_S2_TABLE_C5::POS[];

constexpr int DVB_S2_TABLE_C6::DEG[];
constexpr int DVB_S2_TABLE_C6::LEN[];
constexpr int DVB_S2_TABLE_C6::POS[];

constexpr int DVB_S2_TABLE_C7::DEG[];
constexpr int DVB_S2_TABLE_C7::LEN[];
constexpr int DVB_S2_TABLE_C7::POS[];

constexpr int DVB_S2_TABLE_C8::DEG[];
constexpr int DVB_S2_TABLE_C8::LEN[];
constexpr int DVB_S2_TABLE_C8::POS[];

constexpr int DVB_S2_TABLE_C9::DEG[];
constexpr int DVB_S2_TABLE_C9::LEN[];
constexpr int DVB_S2_TABLE_C9::POS[];

constexpr int DVB_S2_TABLE_C10::DEG[];
constexpr int DVB_S2_TABLE_C10::LEN[];
constexpr int DVB_S2_TABLE_C10::POS[];

constexpr int DVB_S2X_TABLE_C1::DEG[];
constexpr int DVB_S2X_TABLE_C1::LEN[];
constexpr int DVB_S2X_TABLE_C1::POS[];

constexpr int DVB_S2X_TABLE_C2::DEG[];
constexpr int DVB_S2X_TABLE_C2::LEN[];
constexpr int DVB_S2X_TABLE_C2::POS[];

constexpr int DVB_S2X_TABLE_C3::DEG[];
constexpr int DVB_S2X_TABLE_C3::LEN[];
constexpr int DVB_S2X_TABLE_C3::POS[];

constexpr int DVB_S2X_TABLE_C4::DEG[];
constexpr int DVB_S2X_TABLE_C4::LEN[];
constexpr int DVB_S2X_TABLE_C4::POS[];

constexpr int DVB_S2X_TABLE_C5::DEG[];
constexpr int DVB_S2X_TABLE_C5::LEN[];
constexpr int DVB_S2X_TABLE_C5::POS[];

constexpr int DVB_S2X_TABLE_C6::DEG[];
constexpr int DVB_S2X_TABLE_C6::LEN[];
constexpr int DVB_S2X_TABLE_C6::POS[];

constexpr int DVB_S2X_TABLE_C7::DEG[];
constexpr int DVB_S2X_TABLE_C7::LEN[];
constexpr int DVB_S2X_TABLE_C7::POS[];

constexpr int DVB_S2X_TABLE_C8::DEG[];
constexpr int DVB_S2X_TABLE_C8::LEN[];
constexpr int DVB_S2X_TABLE_C8::POS[];

constexpr int DVB_S2X_TABLE_C9::DEG[];
constexpr int DVB_S2X_TABLE_C9::LEN[];
constexpr int DVB_S2X_TABLE_C9::POS[];

constexpr int DVB_S2X_TABLE_C10::DEG[];
constexpr int DVB_S2X_TABLE_C10::LEN[];
constexpr int DVB_S2X_TABLE_C10::POS[];

constexpr int DVB_T2_TABLE_A3::DEG[];
constexpr int DVB_T2_TABLE_A3::LEN[];
constexpr int DVB_T2_TABLE_A3::POS[];

constexpr int DVB_T2_TABLE_B3::DEG[];
constexpr int DVB_T2_TABLE_B3::LEN[];
constexpr int DVB_T2_TABLE_B3::POS[];

namespace gr {
namespace dvbs2rx {

ldpc_decoder_cb::sptr ldpc_decoder_cb::make(dvb_standard_t standard,
                                            dvb_framesize_t framesize,
                                            dvb_code_rate_t rate,
                                            dvb_constellation_t constellation,
                                            dvb_outputmode_t outputmode,
                                            dvb_infomode_t infomode,
                                            int max_trials,
                                            int debug_level)
{
    return gnuradio::get_initial_sptr(new ldpc_decoder_cb_impl(standard,
                                                               framesize,
                                                               rate,
                                                               constellation,
                                                               outputmode,
                                                               infomode,
                                                               max_trials,
                                                               debug_level));
}

/*
 * The private constructor
 */
ldpc_decoder_cb_impl::ldpc_decoder_cb_impl(dvb_standard_t standard,
                                           dvb_framesize_t framesize,
                                           dvb_code_rate_t rate,
                                           dvb_constellation_t constellation,
                                           dvb_outputmode_t outputmode,
                                           dvb_infomode_t infomode,
                                           int max_trials,
                                           int debug_level)
    : gr::block("ldpc_decoder_cb",
                gr::io_signature::make(1, 1, sizeof(gr_complex)),
                gr::io_signature::make(1, 1, sizeof(unsigned char))),
      d_debug_level(debug_level)
{
    fec_info_t fec_info;
    get_fec_info(standard, framesize, rate, fec_info);
    kldpc = fec_info.ldpc.k;
    nldpc = fec_info.ldpc.n;
    kldpc_bytes = kldpc / 8;
    nldpc_bytes = nldpc / 8;

    if (framesize == FECFRAME_NORMAL) {
        switch (rate) {
        case C1_4:
            ldpc = new LDPC<DVB_S2_TABLE_B1>();
            break;
        case C1_3:
            ldpc = new LDPC<DVB_S2_TABLE_B2>();
            break;
        case C2_5:
            ldpc = new LDPC<DVB_S2_TABLE_B3>();
            break;
        case C1_2:
            q_val = 90;
            ldpc = new LDPC<DVB_S2_TABLE_B4>();
            break;
        case C3_5:
            q_val = 72;
            ldpc = new LDPC<DVB_S2_TABLE_B5>();
            break;
        case C2_3:
            q_val = 60;
            if (standard == STANDARD_DVBS2) {
                ldpc = new LDPC<DVB_S2_TABLE_B6>();
            } else {
                ldpc = new LDPC<DVB_T2_TABLE_A3>();
            }
            break;
        case C3_4:
            q_val = 45;
            ldpc = new LDPC<DVB_S2_TABLE_B7>();
            break;
        case C4_5:
            q_val = 36;
            ldpc = new LDPC<DVB_S2_TABLE_B8>();
            break;
        case C5_6:
            q_val = 30;
            ldpc = new LDPC<DVB_S2_TABLE_B9>();
            break;
        case C8_9:
            ldpc = new LDPC<DVB_S2_TABLE_B10>();
            break;
        case C9_10:
            ldpc = new LDPC<DVB_S2_TABLE_B11>();
            break;
        case C2_9_VLSNR:
            ldpc = new LDPC<DVB_S2X_TABLE_B1>();
            break;
        case C13_45:
            ldpc = new LDPC<DVB_S2X_TABLE_B2>();
            break;
        case C9_20:
            ldpc = new LDPC<DVB_S2X_TABLE_B3>();
            break;
        case C90_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B11>();
            break;
        case C96_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B12>();
            break;
        case C11_20:
            ldpc = new LDPC<DVB_S2X_TABLE_B4>();
            break;
        case C100_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B13>();
            break;
        case C104_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B14>();
            break;
        case C26_45:
            ldpc = new LDPC<DVB_S2X_TABLE_B5>();
            break;
        case C18_30:
            ldpc = new LDPC<DVB_S2X_TABLE_B22>();
            break;
        case C28_45:
            ldpc = new LDPC<DVB_S2X_TABLE_B6>();
            break;
        case C23_36:
            ldpc = new LDPC<DVB_S2X_TABLE_B7>();
            break;
        case C116_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B15>();
            break;
        case C20_30:
            ldpc = new LDPC<DVB_S2X_TABLE_B23>();
            break;
        case C124_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B16>();
            break;
        case C25_36:
            ldpc = new LDPC<DVB_S2X_TABLE_B8>();
            break;
        case C128_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B17>();
            break;
        case C13_18:
            ldpc = new LDPC<DVB_S2X_TABLE_B9>();
            break;
        case C132_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B18>();
            break;
        case C22_30:
            ldpc = new LDPC<DVB_S2X_TABLE_B24>();
            break;
        case C135_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B19>();
            break;
        case C140_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B20>();
            break;
        case C7_9:
            ldpc = new LDPC<DVB_S2X_TABLE_B10>();
            break;
        case C154_180:
            ldpc = new LDPC<DVB_S2X_TABLE_B21>();
            break;
        default:
            break;
        }
    } else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
        case C1_4:
            ldpc = new LDPC<DVB_S2_TABLE_C1>();
            break;
        case C1_3:
            q_val = 30;
            ldpc = new LDPC<DVB_S2_TABLE_C2>();
            break;
        case C2_5:
            q_val = 27;
            ldpc = new LDPC<DVB_S2_TABLE_C3>();
            break;
        case C1_2:
            q_val = 25;
            ldpc = new LDPC<DVB_S2_TABLE_C4>();
            break;
        case C3_5:
            q_val = 18;
            if (standard == STANDARD_DVBS2) {
                ldpc = new LDPC<DVB_S2_TABLE_C5>();
            } else {
                ldpc = new LDPC<DVB_T2_TABLE_B3>();
            }
            break;
        case C2_3:
            q_val = 15;
            ldpc = new LDPC<DVB_S2_TABLE_C6>();
            break;
        case C3_4:
            q_val = 12;
            ldpc = new LDPC<DVB_S2_TABLE_C7>();
            break;
        case C4_5:
            q_val = 10;
            ldpc = new LDPC<DVB_S2_TABLE_C8>();
            break;
        case C5_6:
            q_val = 8;
            ldpc = new LDPC<DVB_S2_TABLE_C9>();
            break;
        case C8_9:
            ldpc = new LDPC<DVB_S2_TABLE_C10>();
            break;
        case C11_45:
            ldpc = new LDPC<DVB_S2X_TABLE_C1>();
            break;
        case C4_15:
            ldpc = new LDPC<DVB_S2X_TABLE_C2>();
            break;
        case C14_45:
            ldpc = new LDPC<DVB_S2X_TABLE_C3>();
            break;
        case C7_15:
            ldpc = new LDPC<DVB_S2X_TABLE_C4>();
            break;
        case C8_15:
            ldpc = new LDPC<DVB_S2X_TABLE_C5>();
            break;
        case C26_45:
            ldpc = new LDPC<DVB_S2X_TABLE_C6>();
            break;
        case C32_45:
            ldpc = new LDPC<DVB_S2X_TABLE_C7>();
            break;
        case C1_5_VLSNR_SF2:
            ldpc = new LDPC<DVB_S2_TABLE_C1>();
            break;
        case C11_45_VLSNR_SF2:
            ldpc = new LDPC<DVB_S2X_TABLE_C1>();
            break;
        case C1_5_VLSNR:
            ldpc = new LDPC<DVB_S2_TABLE_C1>();
            break;
        case C4_15_VLSNR:
            ldpc = new LDPC<DVB_S2X_TABLE_C2>();
            break;
        case C1_3_VLSNR:
            ldpc = new LDPC<DVB_S2_TABLE_C2>();
            break;
        default:
            break;
        }
    } else {
        switch (rate) {
        case C1_5_MEDIUM:
            ldpc = new LDPC<DVB_S2X_TABLE_C8>();
            break;
        case C11_45_MEDIUM:
            ldpc = new LDPC<DVB_S2X_TABLE_C9>();
            break;
        case C1_3_MEDIUM:
            ldpc = new LDPC<DVB_S2X_TABLE_C10>();
            break;
        default:
            break;
        }
    }

    unsigned int rows;
    switch (constellation) {
    case MOD_QPSK:
        mod = new PhaseShiftKeying<4, gr_complex, int8_t>();
        d_qpsk = new QpskConstellation();
        break;
    case MOD_8PSK:
        mod = new PhaseShiftKeying<8, gr_complex, int8_t>();
        rows = ldpc->code_len() / mod->bits();
        /* 210 */
        if (rate == C3_5) {
            rowaddr0 = rows * 2;
            rowaddr1 = rows;
            rowaddr2 = 0;
        }
        /* 102 */
        else if (rate == C25_36 || rate == C13_18 || rate == C7_15 || rate == C8_15 ||
                 rate == C26_45) {
            rowaddr0 = rows;
            rowaddr1 = 0;
            rowaddr2 = rows * 2;
        }
        /* 012 */
        else {
            rowaddr0 = 0;
            rowaddr1 = rows;
            rowaddr2 = rows * 2;
        }
        break;
    case MOD_16QAM:
        mod = new QuadratureAmplitudeModulation<16, gr_complex, int8_t>();
        break;
    case MOD_64QAM:
        mod = new QuadratureAmplitudeModulation<64, gr_complex, int8_t>();
        break;
    case MOD_256QAM:
        mod = new QuadratureAmplitudeModulation<256, gr_complex, int8_t>();
        break;
    default:
        break;
    }

    frame = 0;
    chunk = 0;
    d_max_trials = max_trials;
    total_trials = 0;
    total_snr = 0.0;
    signal_constellation = constellation;
    code_rate = rate;
    dvb_standard = standard;
    output_mode = outputmode;
    info_mode = infomode;

    decode = nullptr;
    std::string impl = "generic";
#ifdef CPU_FEATURES_ARCH_ANY_ARM
    d_simd_size = 16;
    if (has_neon) {
        ldpc_neon::ldpc_dec_init(ldpc);
        decode = &ldpc_neon::ldpc_dec_decode;
        impl = "neon";
    } else {
        ldpc_generic::ldpc_dec_init(ldpc);
        decode = &ldpc_generic::ldpc_dec_decode;
    }
#else
#ifdef CPU_FEATURES_ARCH_X86
    d_simd_size = features.avx2 ? 32 : 16;
    if (features.avx2) {
        ldpc_avx2::ldpc_dec_init(ldpc);
        decode = &ldpc_avx2::ldpc_dec_decode;
        impl = "avx2";
    } else if (features.sse4_1) {
        ldpc_sse41::ldpc_dec_init(ldpc);
        decode = &ldpc_sse41::ldpc_dec_decode;
        impl = "sse4_1";
    } else {
        ldpc_generic::ldpc_dec_init(ldpc);
        decode = &ldpc_generic::ldpc_dec_decode;
    }
#else
    // Not ARM, nor x86. Use generic implementation.
    d_simd_size = 16;
    ldpc_generic::ldpc_dec_init(ldpc);
    decode = &ldpc_generic::ldpc_dec_decode;
#endif
#endif
    assert(decode != nullptr);
    d_debug_logger->debug("LDPC decoder implementation: {:s}", impl);

    soft = new int8_t[ldpc->code_len() * d_simd_size];
    dint = new int8_t[ldpc->code_len() * d_simd_size];
    tempu = new int8_t[ldpc->code_len()];
    tempv = new int8_t[ldpc->code_len()];
    aligned_buffer = aligned_alloc(d_simd_size, d_simd_size * ldpc->code_len());
    generate_interleave_lookup();
    generate_deinterleave_lookup();
    unsigned int xfecframe_len = nldpc / mod->bits();
    if (outputmode == OM_MESSAGE) {
        set_output_multiple(kldpc_bytes * d_simd_size);
        set_relative_rate((double)kldpc_bytes / xfecframe_len);
    } else {
        set_output_multiple(nldpc_bytes * d_simd_size);
        set_relative_rate((double)nldpc_bytes / xfecframe_len);
    }
}

inline void ldpc_decoder_cb_impl::interleave_parity_bits(int* tempu, const int*& in)
{
    for (unsigned int k = 0; k < kldpc; k++) {
        tempu[k] = *in++;
    }
    for (unsigned int t = 0; t < q_val; t++) {
        for (int s = 0; s < 360; s++) {
            tempu[kldpc + (360 * t) + s] = in[(q_val * s) + t];
        }
    }
    in = in + (q_val * 360);
}

inline void ldpc_decoder_cb_impl::twist_interleave_columns(
    int* tempv, int* tempu, int rows, int mod, const int* twist)
{
    int index = 0, offset;

    for (int col = 0; col < mod; col++) {
        offset = twist[col];
        for (int row = 0; row < rows; row++) {
            tempv[offset + (rows * col)] = tempu[index++];
            offset++;
            if (offset == rows) {
                offset = 0;
            }
        }
    }
}

inline void ldpc_decoder_cb_impl::twist_deinterleave_columns(
    int* tempv, int* tempu, int rows, int mod, const int* twist)
{
    int index = 0, offset;

    for (int col = 0; col < mod; col++) {
        offset = twist[col];
        for (int row = 0; row < rows; row++) {
            tempv[index++] = tempu[offset + (rows * col)];
            offset++;
            if (offset == rows) {
                offset = 0;
            }
        }
    }
}

void ldpc_decoder_cb_impl::generate_interleave_lookup()
{
    int rows, index = 0;
    int tempv[FRAME_SIZE_NORMAL];
    int tempu[FRAME_SIZE_NORMAL];
    const int* twist;
    const int *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
    const int *c9, *c10, *c11, *c12, *c13, *c14, *c15, *c16;
    const int* in = &interleave_lookup_table[0];
    const int MOD_BITS = mod->bits();

    for (int i = 0; i < FRAME_SIZE_NORMAL; i++) {
        interleave_lookup_table[i] = i;
    }
    switch (signal_constellation) {
    case MOD_QPSK:
        break;
    case MOD_16QAM:
        rows = nldpc / (MOD_BITS * 2);
        if (nldpc == FRAME_SIZE_NORMAL) {
            twist = &twist16n[0];
        } else {
            twist = &twist16s[0];
        }
        interleave_parity_bits(tempu, in);
        c1 = &tempv[0];
        c2 = &tempv[rows];
        c3 = &tempv[rows * 2];
        c4 = &tempv[rows * 3];
        c5 = &tempv[rows * 4];
        c6 = &tempv[rows * 5];
        c7 = &tempv[rows * 6];
        c8 = &tempv[rows * 7];
        twist_interleave_columns(tempv, tempu, rows, (MOD_BITS * 2), twist);
        for (int j = 0; j < rows; j++) {
            tempu[index++] = c1[j];
            tempu[index++] = c2[j];
            tempu[index++] = c3[j];
            tempu[index++] = c4[j];
            tempu[index++] = c5[j];
            tempu[index++] = c6[j];
            tempu[index++] = c7[j];
            tempu[index++] = c8[j];
        }
        break;
    case MOD_64QAM:
        rows = nldpc / (MOD_BITS * 2);
        if (nldpc == FRAME_SIZE_NORMAL) {
            twist = twist64n;
        } else {
            twist = twist64s;
        }
        interleave_parity_bits(tempu, in);
        c1 = &tempv[0];
        c2 = &tempv[rows];
        c3 = &tempv[rows * 2];
        c4 = &tempv[rows * 3];
        c5 = &tempv[rows * 4];
        c6 = &tempv[rows * 5];
        c7 = &tempv[rows * 6];
        c8 = &tempv[rows * 7];
        c9 = &tempv[rows * 8];
        c10 = &tempv[rows * 9];
        c11 = &tempv[rows * 10];
        c12 = &tempv[rows * 11];
        twist_interleave_columns(tempv, tempu, rows, (MOD_BITS * 2), twist);
        for (int j = 0; j < rows; j++) {
            tempu[index++] = c1[j];
            tempu[index++] = c2[j];
            tempu[index++] = c3[j];
            tempu[index++] = c4[j];
            tempu[index++] = c5[j];
            tempu[index++] = c6[j];
            tempu[index++] = c7[j];
            tempu[index++] = c8[j];
            tempu[index++] = c9[j];
            tempu[index++] = c10[j];
            tempu[index++] = c11[j];
            tempu[index++] = c12[j];
        }
        break;
    case MOD_256QAM:
        if (nldpc == FRAME_SIZE_NORMAL) {
            rows = nldpc / (MOD_BITS * 2);
            interleave_parity_bits(tempu, in);
            c1 = &tempv[0];
            c2 = &tempv[rows];
            c3 = &tempv[rows * 2];
            c4 = &tempv[rows * 3];
            c5 = &tempv[rows * 4];
            c6 = &tempv[rows * 5];
            c7 = &tempv[rows * 6];
            c8 = &tempv[rows * 7];
            c9 = &tempv[rows * 8];
            c10 = &tempv[rows * 9];
            c11 = &tempv[rows * 10];
            c12 = &tempv[rows * 11];
            c13 = &tempv[rows * 12];
            c14 = &tempv[rows * 13];
            c15 = &tempv[rows * 14];
            c16 = &tempv[rows * 15];
            twist_interleave_columns(tempv, tempu, rows, (MOD_BITS * 2), twist256n);
            for (int j = 0; j < rows; j++) {
                tempu[index++] = c1[j];
                tempu[index++] = c2[j];
                tempu[index++] = c3[j];
                tempu[index++] = c4[j];
                tempu[index++] = c5[j];
                tempu[index++] = c6[j];
                tempu[index++] = c7[j];
                tempu[index++] = c8[j];
                tempu[index++] = c9[j];
                tempu[index++] = c10[j];
                tempu[index++] = c11[j];
                tempu[index++] = c12[j];
                tempu[index++] = c13[j];
                tempu[index++] = c14[j];
                tempu[index++] = c15[j];
                tempu[index++] = c16[j];
            }
        } else {
            rows = nldpc / MOD_BITS;
            interleave_parity_bits(tempu, in);
            c1 = &tempv[0];
            c2 = &tempv[rows];
            c3 = &tempv[rows * 2];
            c4 = &tempv[rows * 3];
            c5 = &tempv[rows * 4];
            c6 = &tempv[rows * 5];
            c7 = &tempv[rows * 6];
            c8 = &tempv[rows * 7];
            twist_interleave_columns(tempv, tempu, rows, MOD_BITS, twist256s);
            for (int j = 0; j < rows; j++) {
                tempu[index++] = c1[j];
                tempu[index++] = c2[j];
                tempu[index++] = c3[j];
                tempu[index++] = c4[j];
                tempu[index++] = c5[j];
                tempu[index++] = c6[j];
                tempu[index++] = c7[j];
                tempu[index++] = c8[j];
            }
        }
        break;
    }
    memcpy(interleave_lookup_table, tempu, nldpc * sizeof(int));
}

void ldpc_decoder_cb_impl::generate_deinterleave_lookup()
{
    int rows, index = 0;
    int tempv[FRAME_SIZE_NORMAL];
    int tempu[FRAME_SIZE_NORMAL];
    const int* twist;
    int *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
    int *c9, *c10, *c11, *c12, *c13, *c14, *c15, *c16;
    const int* in = &deinterleave_lookup_table[0];
    const int MOD_BITS = mod->bits();

    for (int i = 0; i < FRAME_SIZE_NORMAL; i++) {
        deinterleave_lookup_table[i] = i;
    }
    switch (signal_constellation) {
    case MOD_QPSK:
        break;
    case MOD_16QAM:
        rows = nldpc / (MOD_BITS * 2);
        if (nldpc == FRAME_SIZE_NORMAL) {
            twist = &twist16n[0];
        } else {
            twist = &twist16s[0];
        }
        c1 = &tempu[0];
        c2 = &tempu[rows];
        c3 = &tempu[rows * 2];
        c4 = &tempu[rows * 3];
        c5 = &tempu[rows * 4];
        c6 = &tempu[rows * 5];
        c7 = &tempu[rows * 6];
        c8 = &tempu[rows * 7];
        for (int j = 0; j < rows; j++) {
            c1[j] = in[index++];
            c2[j] = in[index++];
            c3[j] = in[index++];
            c4[j] = in[index++];
            c5[j] = in[index++];
            c6[j] = in[index++];
            c7[j] = in[index++];
            c8[j] = in[index++];
        }
        twist_deinterleave_columns(tempv, tempu, rows, (MOD_BITS * 2), twist);
        break;
    case MOD_64QAM:
        rows = nldpc / (MOD_BITS * 2);
        if (nldpc == FRAME_SIZE_NORMAL) {
            twist = twist64n;
        } else {
            twist = twist64s;
        }
        c1 = &tempu[0];
        c2 = &tempu[rows];
        c3 = &tempu[rows * 2];
        c4 = &tempu[rows * 3];
        c5 = &tempu[rows * 4];
        c6 = &tempu[rows * 5];
        c7 = &tempu[rows * 6];
        c8 = &tempu[rows * 7];
        c9 = &tempu[rows * 8];
        c10 = &tempu[rows * 9];
        c11 = &tempu[rows * 10];
        c12 = &tempu[rows * 11];
        for (int j = 0; j < rows; j++) {
            c1[j] = in[index++];
            c2[j] = in[index++];
            c3[j] = in[index++];
            c4[j] = in[index++];
            c5[j] = in[index++];
            c6[j] = in[index++];
            c7[j] = in[index++];
            c8[j] = in[index++];
            c9[j] = in[index++];
            c10[j] = in[index++];
            c11[j] = in[index++];
            c12[j] = in[index++];
        }
        twist_deinterleave_columns(tempv, tempu, rows, (MOD_BITS * 2), twist);
        break;
    case MOD_256QAM:
        if (nldpc == FRAME_SIZE_NORMAL) {
            rows = nldpc / (MOD_BITS * 2);
            c1 = &tempu[0];
            c2 = &tempu[rows];
            c3 = &tempu[rows * 2];
            c4 = &tempu[rows * 3];
            c5 = &tempu[rows * 4];
            c6 = &tempu[rows * 5];
            c7 = &tempu[rows * 6];
            c8 = &tempu[rows * 7];
            c9 = &tempu[rows * 8];
            c10 = &tempu[rows * 9];
            c11 = &tempu[rows * 10];
            c12 = &tempu[rows * 11];
            c13 = &tempu[rows * 12];
            c14 = &tempu[rows * 13];
            c15 = &tempu[rows * 14];
            c16 = &tempu[rows * 15];
            for (int j = 0; j < rows; j++) {
                c1[j] = in[index++];
                c2[j] = in[index++];
                c3[j] = in[index++];
                c4[j] = in[index++];
                c5[j] = in[index++];
                c6[j] = in[index++];
                c7[j] = in[index++];
                c8[j] = in[index++];
                c9[j] = in[index++];
                c10[j] = in[index++];
                c11[j] = in[index++];
                c12[j] = in[index++];
                c13[j] = in[index++];
                c14[j] = in[index++];
                c15[j] = in[index++];
                c16[j] = in[index++];
            }
            twist_deinterleave_columns(tempv, tempu, rows, (MOD_BITS * 2), twist256n);
        } else {
            rows = nldpc / MOD_BITS;
            c1 = &tempu[0];
            c2 = &tempu[rows];
            c3 = &tempu[rows * 2];
            c4 = &tempu[rows * 3];
            c5 = &tempu[rows * 4];
            c6 = &tempu[rows * 5];
            c7 = &tempu[rows * 6];
            c8 = &tempu[rows * 7];
            for (int j = 0; j < rows; j++) {
                c1[j] = in[index++];
                c2[j] = in[index++];
                c3[j] = in[index++];
                c4[j] = in[index++];
                c5[j] = in[index++];
                c6[j] = in[index++];
                c7[j] = in[index++];
                c8[j] = in[index++];
            }
            twist_deinterleave_columns(tempv, tempu, rows, MOD_BITS, twist256s);
        }
        break;
    }
    memcpy(deinterleave_lookup_table, tempv, nldpc * sizeof(int));
}

/*
 * Our virtual destructor.
 */
ldpc_decoder_cb_impl::~ldpc_decoder_cb_impl()
{
    free(aligned_buffer);
    delete[] tempv;
    delete[] tempu;
    delete[] dint;
    delete[] soft;
    delete mod;
    delete d_qpsk;
    delete ldpc;
}

void ldpc_decoder_cb_impl::forecast(int noutput_items,
                                    gr_vector_int& ninput_items_required)
{
    if (output_mode == OM_MESSAGE) {
        unsigned int n_frames = noutput_items / kldpc_bytes;
        ninput_items_required[0] = n_frames * (nldpc / mod->bits());
    } else {
        ninput_items_required[0] = 8 * noutput_items / mod->bits();
    }
}

const int DEFAULT_TRIALS = 25;
#define FACTOR 2 // same factor used on the decoder implementation

int ldpc_decoder_cb_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    const gr_complex* insnr = (const gr_complex*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];
    const int CODE_LEN = ldpc->code_len();
    const int MOD_BITS = mod->bits();
    int8_t tmp[MOD_BITS];
    int8_t* code = nullptr;
    gr_complex s, e;
    const int SYMBOLS = CODE_LEN / MOD_BITS;
    const int trials = (d_max_trials == 0) ? DEFAULT_TRIALS : d_max_trials;
    int consumed = 0;
    int rows, offset, indexin, indexout;
    const int* mux;
    int8_t *c1, *c2, *c3;
    int output_size = output_mode ? kldpc_bytes : nldpc_bytes;
    static constexpr float Es = 1.0; // assume unitary symbol energy

    for (int i = 0; i < noutput_items; i += output_size * d_simd_size) {
        for (int blk = 0; blk < d_simd_size; blk++) {
            // Compute an initial SNR estimate if this is the first batch of frames ever.
            if (chunk == 0) {
                if (signal_constellation == MOD_QPSK) {
                    d_snr_lin = d_qpsk->estimate_snr(in, SYMBOLS);
                } else {
                    float sp = 0;
                    float np = 0;
                    for (int j = 0; j < SYMBOLS; j++) {
                        mod->hard(tmp, in[j]);
                        s = mod->map(tmp);
                        e = in[j] - s;
                        sp += std::norm(s);
                        np += std::norm(e);
                    }

                    if (!(np > 0)) {
                        np = 1e-12;
                    }
                    d_snr_lin = sp / np;
                }
                snr = 10 * std::log10(d_snr_lin);
                d_N0 = Es / d_snr_lin;
                precision = FACTOR / (d_N0 / 2);
            }

            // Soft constellation demapping
            if (signal_constellation == MOD_QPSK) {
                d_qpsk->demap_soft(soft + (blk * CODE_LEN), in, SYMBOLS, d_N0);
            } else {
                for (int j = 0; j < SYMBOLS; j++) {
                    mod->soft(soft + (j * MOD_BITS + (blk * CODE_LEN)), in[j], precision);
                }
            }

            // Deinterleave
            switch (signal_constellation) {
            case MOD_QPSK:
                if (dvb_standard == STANDARD_DVBT2) {
                    if (code_rate == C1_3 || code_rate == C2_5) {
                        for (unsigned int t = 0; t < q_val; t++) {
                            for (int s = 0; s < 360; s++) {
                                dint[(kldpc + (q_val * s) + t) + (blk * CODE_LEN)] =
                                    soft[(kldpc + (360 * t) + s) + (blk * CODE_LEN)];
                            }
                        }
                        for (unsigned int k = 0; k < kldpc; k++) {
                            dint[k + (blk * CODE_LEN)] = soft[k + (blk * CODE_LEN)];
                        }
                        code = dint;
                    } else {
                        code = soft;
                    }
                } else {
                    code = soft;
                }
                break;
            case MOD_8PSK:
                rows = nldpc / MOD_BITS;
                c1 = &dint[rowaddr0 + (blk * CODE_LEN)];
                c2 = &dint[rowaddr1 + (blk * CODE_LEN)];
                c3 = &dint[rowaddr2 + (blk * CODE_LEN)];
                indexin = 0;
                for (int j = 0; j < rows; j++) {
                    c1[j] = soft[indexin++ + (blk * CODE_LEN)];
                    c2[j] = soft[indexin++ + (blk * CODE_LEN)];
                    c3[j] = soft[indexin++ + (blk * CODE_LEN)];
                }
                code = dint;
                break;
            case MOD_16QAM:
                if (code_rate == C3_5 && nldpc == FRAME_SIZE_NORMAL) {
                    mux = &mux16_35[0];
                } else if (code_rate == C1_3 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux16_13[0];
                } else if (code_rate == C2_5 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux16_25[0];
                } else {
                    mux = &mux16[0];
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < nldpc / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempu[indexout++] = soft[(offset + indexin) + (blk * CODE_LEN)];
                    }
                    indexin += MOD_BITS * 2;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int j = 0; j < nldpc; j++) {
                    tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                }
                for (unsigned int t = 0; t < q_val; t++) {
                    for (int s = 0; s < 360; s++) {
                        dint[(kldpc + (q_val * s) + t) + (blk * CODE_LEN)] =
                            tempv[(kldpc + (360 * t) + s)];
                    }
                }
                for (unsigned int k = 0; k < kldpc; k++) {
                    dint[k + (blk * CODE_LEN)] = tempv[k];
                }
                code = dint;
                break;
            case MOD_64QAM:
                if (code_rate == C3_5 && nldpc == FRAME_SIZE_NORMAL) {
                    mux = &mux64_35[0];
                } else if (code_rate == C1_3 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux64_13[0];
                } else if (code_rate == C2_5 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux64_25[0];
                } else {
                    mux = &mux64[0];
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < nldpc / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempu[indexout++] = soft[(offset + indexin) + (blk * CODE_LEN)];
                    }
                    indexin += MOD_BITS * 2;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int j = 0; j < nldpc; j++) {
                    tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                }
                for (unsigned int t = 0; t < q_val; t++) {
                    for (int s = 0; s < 360; s++) {
                        dint[(kldpc + (q_val * s) + t) + (blk * CODE_LEN)] =
                            tempv[(kldpc + (360 * t) + s)];
                    }
                }
                for (unsigned int k = 0; k < kldpc; k++) {
                    dint[k + (blk * CODE_LEN)] = tempv[k];
                }
                code = dint;
                break;
            case MOD_256QAM:
                if (nldpc == FRAME_SIZE_NORMAL) {
                    if (code_rate == C3_5) {
                        mux = &mux256_35[0];
                    } else if (code_rate == C2_3) {
                        mux = &mux256_23[0];
                    } else {
                        mux = &mux256[0];
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < nldpc / (MOD_BITS * 2); d++) {
                        for (int e = 0; e < (MOD_BITS * 2); e++) {
                            offset = mux[e];
                            tempu[indexout++] =
                                soft[(offset + indexin) + (blk * CODE_LEN)];
                        }
                        indexin += MOD_BITS * 2;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int j = 0; j < nldpc; j++) {
                        tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                    }
                    for (unsigned int t = 0; t < q_val; t++) {
                        for (int s = 0; s < 360; s++) {
                            dint[(kldpc + (q_val * s) + t) + (blk * CODE_LEN)] =
                                tempv[(kldpc + (360 * t) + s)];
                        }
                    }
                    for (unsigned int k = 0; k < kldpc; k++) {
                        dint[k + (blk * CODE_LEN)] = tempv[k];
                    }
                    code = dint;
                } else {
                    if (code_rate == C1_3) {
                        mux = &mux256s_13[0];
                    } else if (code_rate == C2_5) {
                        mux = &mux256s_25[0];
                    } else {
                        mux = &mux256s[0];
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < nldpc / MOD_BITS; d++) {
                        for (int e = 0; e < MOD_BITS; e++) {
                            offset = mux[e];
                            tempu[indexout++] =
                                soft[(offset + indexin) + (blk * CODE_LEN)];
                        }
                        indexin += MOD_BITS;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int j = 0; j < nldpc; j++) {
                        tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                    }
                    for (unsigned int t = 0; t < q_val; t++) {
                        for (int s = 0; s < 360; s++) {
                            dint[(kldpc + (q_val * s) + t) + (blk * CODE_LEN)] =
                                tempv[(kldpc + (360 * t) + s)];
                        }
                    }
                    for (unsigned int k = 0; k < kldpc; k++) {
                        dint[k + (blk * CODE_LEN)] = tempv[k];
                    }
                    code = dint;
                }
                break;
            default:
                code = soft;
                break;
            }
            in += nldpc / MOD_BITS;
            consumed += nldpc / MOD_BITS;
        }

        // LDPC Decoding
        int count = decode(aligned_buffer, code, trials);
        if (count < 0) {
            total_trials += trials;
            GR_LOG_DEBUG_LEVEL(1,
                               "frame = {:d}, snr = {:.2f}, trials = {:d} (max)",
                               (chunk * d_simd_size),
                               snr,
                               trials);
        } else {
            total_trials += (trials - count);
            GR_LOG_DEBUG_LEVEL(1,
                               "frame = {:d}, snr = {:.2f}, trials = {:d}",
                               (chunk * d_simd_size),
                               snr,
                               (trials - count));
        }
        chunk++;

        // Map the soft LDPC-decoded output to +-1 and use those to remap into the
        // corresponding constellation symbols. Then, refine the SNR estimate.
        float N0_accum = 0;
        for (int blk = 0; blk < d_simd_size; blk++) {
            switch (signal_constellation) {
            case MOD_QPSK:
                // The QPSK case has an optimization using the QpskConstellation's
                // estimate_snr_llr_ref() method. No need to map to +-1 here.
                break;
            case MOD_8PSK:
                for (int j = 0; j < CODE_LEN; j++) {
                    tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                }
                rows = nldpc / MOD_BITS;
                c1 = &tempu[rowaddr0];
                c2 = &tempu[rowaddr1];
                c3 = &tempu[rowaddr2];
                indexout = 0;
                for (int j = 0; j < rows; j++) {
                    tempv[indexout++] = c1[j];
                    tempv[indexout++] = c2[j];
                    tempv[indexout++] = c3[j];
                }
                break;
            case MOD_16QAM:
                if (code_rate == C3_5 && nldpc == FRAME_SIZE_NORMAL) {
                    mux = &mux16_35[0];
                } else if (code_rate == C1_3 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux16_13[0];
                } else if (code_rate == C2_5 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux16_25[0];
                } else {
                    mux = &mux16[0];
                }
                for (int j = 0; j < CODE_LEN; j++) {
                    tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < nldpc / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempv[offset + indexout] =
                            tempu[interleave_lookup_table[indexin++]];
                    }
                    indexout += MOD_BITS * 2;
                }
                break;
            case MOD_64QAM:
                if (code_rate == C3_5 && nldpc == FRAME_SIZE_NORMAL) {
                    mux = &mux64_35[0];
                } else if (code_rate == C1_3 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux64_13[0];
                } else if (code_rate == C2_5 && nldpc == FRAME_SIZE_SHORT) {
                    mux = &mux64_25[0];
                } else {
                    mux = &mux64[0];
                }
                for (int j = 0; j < CODE_LEN; j++) {
                    tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < nldpc / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempv[offset + indexout] =
                            tempu[interleave_lookup_table[indexin++]];
                    }
                    indexout += MOD_BITS * 2;
                }
                break;
            case MOD_256QAM:
                if (nldpc == FRAME_SIZE_NORMAL) {
                    if (code_rate == C3_5) {
                        mux = &mux256_35[0];
                    } else if (code_rate == C2_3) {
                        mux = &mux256_23[0];
                    } else {
                        mux = &mux256[0];
                    }
                    for (int j = 0; j < CODE_LEN; j++) {
                        tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < nldpc / (MOD_BITS * 2); d++) {
                        for (int e = 0; e < (MOD_BITS * 2); e++) {
                            offset = mux[e];
                            tempv[offset + indexout] =
                                tempu[interleave_lookup_table[indexin++]];
                        }
                        indexout += MOD_BITS * 2;
                    }
                } else {
                    if (code_rate == C1_3) {
                        mux = &mux256s_13[0];
                    } else if (code_rate == C2_5) {
                        mux = &mux256s_25[0];
                    } else {
                        mux = &mux256s[0];
                    }
                    for (int j = 0; j < CODE_LEN; j++) {
                        tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < nldpc / MOD_BITS; d++) {
                        for (int e = 0; e < MOD_BITS; e++) {
                            offset = mux[e];
                            tempv[offset + indexout] =
                                tempu[interleave_lookup_table[indexin++]];
                        }
                        indexout += MOD_BITS;
                    }
                }
                break;
            default:
                break;
            }

            // Refine the SNR estimate using the LDPC-decoded output
            if (signal_constellation == MOD_QPSK) {
                d_snr_lin = d_qpsk->estimate_snr(insnr, &code[blk * CODE_LEN], SYMBOLS);
            } else {
                float sp = 0;
                float np = 0;
                for (int j = 0; j < SYMBOLS; j++) {
                    s = mod->map(&tempv[(j * MOD_BITS)]);
                    e = insnr[j] - s;
                    sp += std::norm(s);
                    np += std::norm(e);
                }
                if (!(np > 0)) {
                    np = 1e-12;
                }
                d_snr_lin = sp / np;
            }
            snr = 10 * std::log10(d_snr_lin);
            total_snr += snr;
            N0_accum += Es / d_snr_lin;
            insnr += nldpc / MOD_BITS;
            frame++;
            if (info_mode) {
                GR_LOG_DEBUG_LEVEL(1,
                                   "frame = {:d}, snr = {:.2f}, average trials = {:d}, "
                                   "average snr =,.2f",
                                   frame,
                                   snr,
                                   (total_trials / chunk),
                                   (total_snr / frame));
            }
        }
        d_N0 = N0_accum / d_simd_size;
        precision = FACTOR / (d_N0 / 2);

        // Output bit-packed bytes with the hard decisions and with the MSB first
        for (int blk = 0; blk < d_simd_size; blk++) {
            for (int j = 0; j < output_size; j++) {
                *out = 0;
                for (int k = 0; k < 8; k++) {
                    if (code[(j * 8) + k + (blk * CODE_LEN)] < 0) {
                        *out |= 1 << (7 - k);
                    }
                }
                out++;
            }
        }
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(consumed);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

const int ldpc_decoder_cb_impl::twist16n[8] = { 0, 0, 2, 4, 4, 5, 7, 7 };

const int ldpc_decoder_cb_impl::twist64n[12] = { 0, 0, 2, 2, 3, 4, 4, 5, 5, 7, 8, 9 };

const int ldpc_decoder_cb_impl::twist256n[16] = { 0,  2,  2,  2,  2,  3,  7,  15,
                                                  16, 20, 22, 22, 27, 27, 28, 32 };

const int ldpc_decoder_cb_impl::twist16s[8] = { 0, 0, 0, 1, 7, 20, 20, 21 };

const int ldpc_decoder_cb_impl::twist64s[12] = { 0, 0, 0, 2, 2, 2, 3, 3, 3, 6, 7, 7 };

const int ldpc_decoder_cb_impl::twist256s[8] = { 0, 0, 0, 1, 7, 20, 20, 21 };

const int ldpc_decoder_cb_impl::mux16[8] = { 7, 1, 4, 2, 5, 3, 6, 0 };

const int ldpc_decoder_cb_impl::mux64[12] = { 11, 7, 3, 10, 6, 2, 9, 5, 1, 8, 4, 0 };

const int ldpc_decoder_cb_impl::mux256[16] = { 15, 1, 13, 3, 8,  11, 9,  5,
                                               10, 6, 4,  7, 12, 2,  14, 0 };

const int ldpc_decoder_cb_impl::mux16_35[8] = { 0, 5, 1, 2, 4, 7, 3, 6 };

const int ldpc_decoder_cb_impl::mux16_13[8] = { 6, 0, 3, 4, 5, 2, 1, 7 };

const int ldpc_decoder_cb_impl::mux16_25[8] = { 7, 5, 4, 0, 3, 1, 2, 6 };

const int ldpc_decoder_cb_impl::mux64_35[12] = { 2, 7, 6, 9, 0, 3, 1, 8, 4, 11, 5, 10 };

const int ldpc_decoder_cb_impl::mux64_13[12] = { 4, 2, 0, 5, 6, 1, 3, 7, 8, 9, 10, 11 };

const int ldpc_decoder_cb_impl::mux64_25[12] = { 4, 0, 1, 6, 2, 3, 5, 8, 7, 10, 9, 11 };

const int ldpc_decoder_cb_impl::mux256_35[16] = { 2,  11, 3, 4,  0, 9,  1, 8,
                                                  10, 13, 7, 14, 6, 15, 5, 12 };

const int ldpc_decoder_cb_impl::mux256_23[16] = { 7,  2,  9,  0, 4, 6,  13, 3,
                                                  14, 10, 15, 5, 8, 12, 11, 1 };

const int ldpc_decoder_cb_impl::mux256s[8] = { 7, 3, 1, 5, 2, 6, 4, 0 };

const int ldpc_decoder_cb_impl::mux256s_13[8] = { 4, 0, 1, 2, 5, 3, 6, 7 };

const int ldpc_decoder_cb_impl::mux256s_25[8] = { 4, 0, 5, 1, 2, 3, 6, 7 };

} /* namespace dvbs2rx */
} /* namespace gr */
