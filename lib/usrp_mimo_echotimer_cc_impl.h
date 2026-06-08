/* -*- c++ -*- */
/*
 * Copyright 2014 Communications Engineering Lab, KIT.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_RADAR_USRP_MIMO_ECHOTIMER_CC_IMPL_H
#define INCLUDED_RADAR_USRP_MIMO_ECHOTIMER_CC_IMPL_H

#include <radar/usrp_mimo_echotimer_cc.h>

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/thread.hpp>

namespace gr {
namespace radar {

class usrp_mimo_echotimer_cc_impl : public usrp_mimo_echotimer_cc
{
private:
    // Nothing to declare in this block.

protected:
    int calculate_output_stream_length(const gr_vector_int& ninput_items);

public:
    usrp_mimo_echotimer_cc_impl(int samp_rate,
                           float center_freq,
                           int num_delay_samps,
                           std::string args_tx,
                           int channel_tx0,
                           int channel_tx1,
                           std::string wire_tx,
                           std::string clock_source_tx,
                           std::string time_source_tx,
                           std::string antenna_tx0,
                           std::string antenna_tx1,
                           float gain_tx0,
                           float gain_tx1,
                           float timeout_tx,
                           float wait_tx,
                           float lo_offset_tx0,
                           float lo_offset_tx1,
                           std::string args_rx,
                           int channel_rx0,
                           int channel_rx1,
                           std::string wire_rx,
                           std::string clock_source_rx,
                           std::string time_source_rx,
                           std::string antenna_rx0,
                           std::string antenna_rx1,
                           float gain_rx0,
                           float gain_rx1,
                           float timeout_rx,
                           float wait_rx,
                           float lo_offset_rx0,
                           float lo_offset_rx1,
                           const std::string& len_key);
    ~usrp_mimo_echotimer_cc_impl();
    void send();
    void receive();
    void set_num_delay_samps(int num_samps);
    void set_rx_gain(float gain);
    void set_tx_gain(float gain);

    int d_samp_rate;
    float d_center_freq;
    int d_num_delay_samps;
    std::vector<gr_complex> d_out_buffer0;
    std::vector<gr_complex> d_out_buffer1;

    std::string d_args_tx, d_args_rx;
    std::string d_clock_source_tx, d_clock_source_rx;
    std::string d_wire_tx, d_wire_rx;
    std::string d_antenna_tx0, d_antenna_tx1; 
    std::string d_antenna_rx0, d_antenna_rx1;
    std::string d_time_source_tx, d_time_source_rx;
    int d_channel_tx0, d_channel_tx1;
    int d_channel_rx0, d_channel_rx1;
    uhd::usrp::multi_usrp::sptr d_usrp_tx, d_usrp_rx;
    uhd::tune_request_t d_tune_request_tx, d_tune_request_rx;
    uhd::tx_streamer::sptr d_tx_stream;
    uhd::rx_streamer::sptr d_rx_stream;
    uhd::tx_metadata_t d_metadata_tx;
    uhd::rx_metadata_t d_metadata_rx;
    double d_lo_offset_tx0, d_lo_offset_tx1;
    double d_lo_offset_rx0, d_lo_offset_rx1;
    float d_timeout_tx, d_timeout_rx;
    float d_wait_tx, d_wait_rx;
    float d_gain_tx0, d_gain_tx1;
    float d_gain_rx0, d_gain_rx1;

    uhd::time_spec_t d_time_now_tx, d_time_now_rx;

   gr::thread::thread d_thread_recv;

    gr_complex* d_out_recv0;
    gr_complex* d_out_recv1;

    int d_noutput_items_recv;
    pmt::pmt_t d_time_key, d_time_val, d_srcid;

    gr::thread::thread d_thread_send;

    const gr_complex* d_in_send0;
    const gr_complex* d_in_send1;

    int d_noutput_items_send;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace radar
} // namespace gr

#endif /* INCLUDED_RADAR_USRP_MIMO_ECHOTIMER_CC_IMPL_H */
