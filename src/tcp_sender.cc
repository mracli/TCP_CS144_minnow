#include "tcp_sender.hh"
#include "byte_stream.hh"
#include "tcp_config.hh"
#include "tcp_sender_message.hh"
#include "wrapping_integers.hh"
#include <cstdint>
#include <optional>

using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  return seq_no_in_flight_cnt_;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return retransmission_cnt_;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  // Your code here.
  auto const cur_window_size = window_size_ ? window_size_ : 1u;
  while ( cur_window_size > seq_no_in_flight_cnt_ ) {
    auto const available_window = cur_window_size - seq_no_in_flight_cnt_;
    auto msg = make_empty_message();
    if ( !syn_ )
      syn_ = msg.SYN = true;

    auto const payload_size = min( TCPConfig::MAX_PAYLOAD_SIZE, available_window );
    read( input_.reader(), payload_size, msg.payload );

    if ( !fin_ && reader().is_finished() && available_window )
      fin_ = msg.FIN = true;

    if(msg.RST){
      transmit(msg);
      break;
    }
    // when sequence_length == 0, which is meaningless message
    if (!msg.sequence_length() )
      break;

    // special case: FIN message cannot exceed receiver's window
    if ( msg.FIN && available_window < msg.sequence_length() )
      fin_ = msg.FIN = false;
    
    seq_no_in_flight_cnt_ += msg.sequence_length();
    outstanding_segments_.push( msg );
    next_seq_no_ += msg.sequence_length();
    transmit( msg );

    if ( msg.FIN || reader().bytes_buffered() == 0 )
      break;
  }
  if ( !timer_.is_running() ) {
    timer_.start();
  }
  if(outstanding_segments_.empty())
    timer_.stop();
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  // Your code here.
  return { Wrap32::wrap( next_seq_no_, isn_ ), {}, {}, {}, has_error()};
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  if ( msg.RST ) {
    set_error();
    timer_.stop();
  }
  // error state, don't do any thing!
  if ( has_error() )
    return;
  window_size_ = msg.window_size;
  // no any ackno is meaningless
  if ( !msg.ackno.has_value() )
    return;

  auto const received_abs_ack_no = msg.ackno->unwrap( isn_, next_seq_no_ );
  if ( received_abs_ack_no > next_seq_no_
       || received_abs_ack_no < outstanding_segments_.front().seqno.unwrap( isn_, next_seq_no_ ) )
    return;
  abs_acked_no_ = received_abs_ack_no;

  // Now we got the newest msg to update current state
  // check if any outstanding segments already received.
  bool has_newdata = [&] {
    bool flag = false;
    while ( outstanding_segments_.size() ) {
      auto& front_seg = outstanding_segments_.front();
      // front seg still not sent to peer successfully
      if ( front_seg.seqno.unwrap( isn_, next_seq_no_ ) + front_seg.sequence_length() > abs_acked_no_ )
        break;
      seq_no_in_flight_cnt_ -= front_seg.sequence_length();
      outstanding_segments_.pop();
      flag = true;
    }
    return flag;
  }();

  if ( has_newdata ) {
    timer_.reset_RTO();
    retransmission_cnt_ = 0;
    timer_.start();
  }

  if ( outstanding_segments_.empty() )
    timer_.stop();
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // Your code here.
  // (void)ms_since_last_tick;
  // (void)transmit;
  // (void)initial_RTO_ms_;
  timer_.tick( ms_since_last_tick );
  // send the oldest outstanding segment when timer is expired
  if ( timer_.is_expired() ) {
    transmit( outstanding_segments_.front() );
    if ( window_size_ != 0 ) {
      ++retransmission_cnt_;
      timer_.double_RTO();
    }
    timer_.start();
  }
}