#include "tcp_receiver.hh"
#include "tcp_receiver_message.hh"
#include "wrapping_integers.hh"
#include <cstdint>
#include <net/if.h>
#include <sys/types.h>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  // check if RST segment incoming
  if ( message.RST ) {
    set_error();
  }

  if ( has_error() )
    return;

  // discard anything before the SYN segment
  if ( !isn_.has_value() ) {
    if ( !message.SYN )
      return;
    isn_ = message.seqno;
  }

  // 1. calc abs_seqno from message, seqno -> abs_seqno
  auto abs_seqno = message.seqno.unwrap( isn_.value(), writer().bytes_pushed() + 1 );
  // 2. calc stream index from abs_seqno
  auto first_index = message.SYN ? 0 : abs_seqno - 1;

  reassembler_.insert( first_index, message.payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage msg;

  if ( has_error() ) {
    msg.RST = true;
  }

  auto window_size = writer().available_capacity();
  msg.window_size = window_size > UINT16_MAX ? UINT16_MAX : window_size;
  if ( isn_.has_value() ) {
    const auto abs_seqno = writer().bytes_pushed() + 1 + writer().is_closed();
    msg.ackno = Wrap32::wrap( abs_seqno, isn_.value() );
  } else {
    msg.ackno = nullopt;
  }
  return msg;
}
