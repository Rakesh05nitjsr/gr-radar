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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "usrp_echotimer_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {
namespace radar {

usrp_echotimer_cc::sptr usrp_echotimer_cc::make(int samp_rate,
                                                float center_freq,
                                                int num_delay_samps,
                                                std::string args_tx,
                                                int channel_tx,
                                                std::string wire_tx,
                                                std::string clock_source_tx,
                                                std::string time_source_tx,
                                                std::string antenna_tx,
                                                float gain_tx,
                                                float timeout_tx,
                                                float wait_tx,
                                                float lo_offset_tx,
                                                std::string args_rx,
                                                int channel_rx,
                                                std::string wire_rx,
                                                std::string clock_source_rx,
                                                std::string time_source_rx,
                                                std::string antenna_rx,
                                                float gain_rx,
                                                float timeout_rx,
                                                float wait_rx,
                                                float lo_offset_rx,
                                                const std::string& len_key)
{
    return gnuradio::make_block_sptr<usrp_echotimer_cc_impl>(samp_rate,
                                                             center_freq,
                                                             num_delay_samps,
                                                             args_tx,
                                                             channel_tx,
                                                             wire_tx,
                                                             clock_source_tx,
                                                             time_source_tx,
                                                             antenna_tx,
                                                             gain_tx,
                                                             timeout_tx,
                                                             wait_tx,
                                                             lo_offset_tx,
                                                             args_rx,
                                                             channel_rx,
                                                             wire_rx,
                                                             clock_source_rx,
                                                             time_source_rx,
                                                             antenna_rx,
                                                             gain_rx,
                                                             timeout_rx,
                                                             wait_rx,
                                                             lo_offset_rx,
                                                             len_key);
}

/*
 * The private constructor
 */
usrp_echotimer_cc_impl::usrp_echotimer_cc_impl(int samp_rate,
                                               float center_freq,
                                               int num_delay_samps,
                                               std::string args_tx,
                                               int channel_tx,
                                               std::string wire_tx,
                                               std::string clock_source_tx,
                                               std::string time_source_tx,
                                               std::string antenna_tx,
                                               float gain_tx,
                                               float timeout_tx,
                                               float wait_tx,
                                               float lo_offset_tx,
                                               std::string args_rx,
                                               int channel_rx,
                                               std::string wire_rx,
                                               std::string clock_source_rx,
                                               std::string time_source_rx,
                                               std::string antenna_rx,
                                               float gain_rx,
                                               float timeout_rx,
                                               float wait_rx,
                                               float lo_offset_rx,
                                               const std::string& len_key)
    : gr::tagged_stream_block("usrp_echotimer_cc",
                              gr::io_signature::make(2, 2, sizeof(gr_complex)),
                              gr::io_signature::make(2, 2, sizeof(gr_complex)),
                              len_key)
{
    d_samp_rate = samp_rate;
    d_center_freq = center_freq;
    d_num_delay_samps = num_delay_samps;

    //***** Setup USRP TX *****//

    d_args_tx = args_tx;
    d_wire_tx = wire_tx;
    d_clock_source_tx = clock_source_tx;
    d_time_source_tx = time_source_tx;
    d_antenna_tx = antenna_tx;
    d_lo_offset_tx = lo_offset_tx;
    d_gain_tx = gain_tx;
    d_timeout_tx = timeout_tx; // timeout for sending
    d_wait_tx = wait_tx;       // secs to wait befor sending

    // Setup USRP TX: args (addr,...)
    d_usrp_tx = uhd::usrp::multi_usrp::make(d_args_tx);
    std::cout << "Using USRP Device (TX): " << std::endl
              << d_usrp_tx->get_pp_string() << std::endl;

    // Setup USRP TX: sample rate
    std::cout << "Setting TX Rate: " << d_samp_rate << std::endl;
    d_usrp_tx->set_tx_rate(d_samp_rate);
    std::cout << "Actual TX Rate: " << d_usrp_tx->get_tx_rate() << std::endl;

    // Setup USRP TX: gain
    set_tx_gain(d_gain_tx);

    // Setup USRP TX: tune request
    d_tune_request_tx =
        uhd::tune_request_t(d_center_freq); // FIXME: add alternative tune requests
    d_usrp_tx->set_tx_freq(d_tune_request_tx);

    // Setup USRP TX: antenna
    d_usrp_tx->set_tx_antenna(d_antenna_tx);

    // Setup USRP TX: clock source
    d_usrp_tx->set_clock_source(d_clock_source_tx); // Set TX clock, TX is master

    // Setup USRP TX: time source
    d_usrp_tx->set_time_source(d_time_source_tx); // Set TX time, TX is master

    // Setup USRP TX: timestamp
    if (d_time_source_tx != "gpsdo") {
        d_usrp_tx->set_time_now(
            uhd::time_spec_t(0.0)); // Do set time on startup if not gpsdo is activated.
    }

    // Setup transmit streamer
    uhd::stream_args_t stream_args_tx("fc32", d_wire_tx); // complex floats
    std::vector<size_t> channel_nums_tx;
    channel_nums_tx.push_back(0);
    channel_nums_tx.push_back(1);
    stream_args_tx.channels = channel_nums_tx;
    d_tx_stream = d_usrp_tx->get_tx_stream(stream_args_tx);

    //***** Setup USRP RX *****//

    d_args_rx = args_rx;
    d_wire_rx = wire_rx;
    d_clock_source_rx = clock_source_rx;
    d_time_source_rx = time_source_rx;
    d_antenna_rx = antenna_rx;
    d_lo_offset_rx = lo_offset_rx;
    d_gain_rx = gain_rx;
    d_timeout_rx = timeout_rx; // timeout for receiving
    d_wait_rx = wait_rx;       // secs to wait befor receiving

    // Setup USRP RX: args (addr,...)
    d_usrp_rx = uhd::usrp::multi_usrp::make(d_args_rx);
    std::cout << "Using USRP Device (RX): " << std::endl
              << d_usrp_rx->get_pp_string() << std::endl;

    // Setup USRP RX: sample rate
    std::cout << "Setting RX Rate: " << d_samp_rate << std::endl;
    d_usrp_rx->set_rx_rate(d_samp_rate);
    std::cout << "Actual RX Rate: " << d_usrp_rx->get_rx_rate() << std::endl;

    // Setup USRP RX: gain
    set_rx_gain(d_gain_rx);

    // Setup USRP RX: tune request
    d_tune_request_rx = uhd::tune_request_t(
        d_center_freq, d_lo_offset_rx); // FIXME: add alternative tune requests
    d_usrp_rx->set_rx_freq(d_tune_request_rx);

    // Setup USRP RX: antenna
    d_usrp_rx->set_rx_antenna(d_antenna_rx);

    // Setup USRP RX: clock source
    d_usrp_rx->set_clock_source(d_clock_source_rx); // RX is slave, clock is set on TX

    // Setup USRP RX: time source
    d_usrp_rx->set_time_source(d_time_source_rx);

    // Setup receive streamer
    uhd::stream_args_t stream_args_rx("fc32", d_wire_rx); // complex floats
    std::vector<size_t> channel_nums_rx;
    channel_nums_rx.push_back(0);
    channel_nums_rx.push_back(1);
    stream_args_rx.channels = channel_nums_rx;
    d_rx_stream = d_usrp_rx->get_rx_stream(stream_args_rx);

    //***** Misc *****//

    // Setup rx_time pmt
    d_time_key = pmt::string_to_symbol("rx_time");
    d_srcid = pmt::string_to_symbol("usrp_echotimer");

    // Setup thread priority
    // uhd::set_thread_priority_safe(); // necessary? doesnt work...

    // Sleep to get sync done
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000)); // FIXME: necessary?
}

/*
 * Our virtual destructor.
 */
usrp_echotimer_cc_impl::~usrp_echotimer_cc_impl() {}

int usrp_echotimer_cc_impl::calculate_output_stream_length(
    const gr_vector_int& ninput_items)
{
    int noutput_items = ninput_items[0];
    return noutput_items;
}

void usrp_echotimer_cc_impl::set_num_delay_samps(int num_samps)
{
    d_num_delay_samps = num_samps;
}

void usrp_echotimer_cc_impl::set_rx_gain(float gain) { d_usrp_rx->set_rx_gain(gain); }

void usrp_echotimer_cc_impl::set_tx_gain(float gain) { d_usrp_tx->set_tx_gain(gain); }

void usrp_echotimer_cc_impl::send()
{
    d_metadata_tx.start_of_burst = true;
    d_metadata_tx.end_of_burst = false;
    d_metadata_tx.has_time_spec = true;
    d_metadata_tx.time_spec = d_time_now_tx + uhd::time_spec_t(d_wait_tx);

    size_t total_num_samps = d_noutput_items_send;

    std::vector<const void*> tx_buffs(2);
    tx_buffs[0] = d_in_send0;
    tx_buffs[1] = d_in_send1;

    double timeout = total_num_samps / (float)d_samp_rate + d_timeout_tx;

    size_t num_tx_samps = d_tx_stream->send(
        tx_buffs,
        total_num_samps,
        d_metadata_tx,
        timeout
    );

    if (num_tx_samps < total_num_samps)
        std::cerr << "Send timeout..." << std::endl;

    d_metadata_tx.start_of_burst = false;
    d_metadata_tx.end_of_burst = true;
    d_metadata_tx.has_time_spec = false;

    d_tx_stream->send("", 0, d_metadata_tx);
}

void usrp_echotimer_cc_impl::receive()
{
    size_t total_num_samps = d_noutput_items_recv;

    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
    stream_cmd.num_samps = total_num_samps;
    stream_cmd.stream_now = false;
    stream_cmd.time_spec = d_time_now_rx + uhd::time_spec_t(d_wait_rx);

    d_rx_stream->issue_stream_cmd(stream_cmd);

    std::vector<void*> rx_buffs(2);
    rx_buffs[0] = d_out_recv0;
    rx_buffs[1] = d_out_recv1;

    double timeout = total_num_samps / (float)d_samp_rate + d_timeout_rx;

    size_t num_rx_samps = d_rx_stream->recv(
        rx_buffs,
        total_num_samps,
        d_metadata_rx,
        timeout
    );

    d_time_val =
        pmt::make_tuple(pmt::from_uint64(d_metadata_rx.time_spec.get_full_secs()),
                        pmt::from_double(d_metadata_rx.time_spec.get_frac_secs()));

    if (d_metadata_rx.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
        throw std::runtime_error(
            str(boost::format("Receiver error %s") % d_metadata_rx.strerror()));
    }

    if (num_rx_samps < total_num_samps)
        std::cerr << "Receive timeout before all samples received..." << std::endl;
}

int usrp_echotimer_cc_impl::work(int noutput_items,
                                 gr_vector_int& ninput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    gr_complex* in0 = (gr_complex*)input_items[0]; // remove const
    gr_complex* in1 = (gr_complex*)input_items[1];

    gr_complex* out0 = (gr_complex*)output_items[0];
    gr_complex* out1 = (gr_complex*)output_items[1];

    // Set output items on packet length
    noutput_items = ninput_items[0];

    // Resize MIMO output buffers
    d_out_buffer0.resize(noutput_items);
    d_out_buffer1.resize(noutput_items);

  if (noutput_items <= 0)
    return 0;

if (d_num_delay_samps >= noutput_items)
    d_num_delay_samps = noutput_items - 1;

    // Get time from USRP TX
    d_time_now_tx = d_usrp_tx->get_time_now();
    d_time_now_rx = d_time_now_tx;

    // Send thread
    d_in_send0 = in0;
    d_in_send1 = in1;
    d_noutput_items_send = noutput_items;
    d_thread_send = gr::thread::thread(boost::bind(&usrp_echotimer_cc_impl::send, this));

    // Receive thread
    d_out_recv0 = &d_out_buffer0[0];
    d_out_recv1 = &d_out_buffer1[0];
    d_noutput_items_recv = noutput_items;
    d_thread_recv =
        gr::thread::thread(boost::bind(&usrp_echotimer_cc_impl::receive, this));

    // Wait for threads to complete
    d_thread_send.join();
    d_thread_recv.join();

    // Shift of number delay samples (fill with zeros)
   memcpy(out0,
       &d_out_buffer0[0] + d_num_delay_samps,
       (noutput_items - d_num_delay_samps) * sizeof(gr_complex));

   memset(out0 + (noutput_items - d_num_delay_samps),
       0,
       d_num_delay_samps * sizeof(gr_complex));

   memcpy(out1,
       &d_out_buffer1[0] + d_num_delay_samps,
       (noutput_items - d_num_delay_samps) * sizeof(gr_complex));

   memset(out1 + (noutput_items - d_num_delay_samps),
       0,
       d_num_delay_samps * sizeof(gr_complex));

    // Tell runtime system how many output items we produced.
    add_item_tag(0, nitems_written(0), d_time_key, d_time_val, d_srcid);
    add_item_tag(1, nitems_written(1), d_time_key, d_time_val, d_srcid);
    return noutput_items;
}

} /* namespace radar */
} /* namespace gr */
