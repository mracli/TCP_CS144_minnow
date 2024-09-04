#include <iostream>

#include "arp_message.hh"
#include "ethernet_frame.hh"
#include "ethernet_header.hh"
#include "exception.hh"
#include "ipv4_datagram.hh"
#include "network_interface.hh"
#include "parser.hh"

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  // Your code here.
  const auto next_hop_ip = next_hop.ipv4_numeric();
  auto it = arp_table_.find( next_hop_ip );
  if ( it == arp_table_.end() ) {
    // arp 表中没找到，那就发送ARP请求给邻居，找一下是否存在需要的mac地址
    if ( waiting_arp_response_table_.find( next_hop_ip ) == waiting_arp_response_table_.end() ) {
      // 构建一个 arp payload
      ARPMessage arp_msg;
      arp_msg.sender_ip_address = ip_address_.ipv4_numeric();
      arp_msg.sender_ethernet_address = ethernet_address_;
      arp_msg.target_ethernet_address = {};
      arp_msg.target_ip_address = next_hop_ip;
      arp_msg.opcode = ARPMessage::OPCODE_REQUEST;
      _send_frame( ETHERNET_BROADCAST, EthernetHeader::TYPE_ARP, serialize( arp_msg ) );
      waiting_arp_response_table_.emplace( next_hop_ip, ARP_RESPONSE_TIME_MS );
    }
    buffered_ip_table_[next_hop_ip].emplace_back( dgram );
  } else {
    _send_frame(it->second.addr, EthernetHeader::TYPE_IPv4, serialize(dgram));
  }
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( const EthernetFrame& frame )
{
  // Your code here.
  auto&& header = frame.header;
  // 发送的帧目标不是本机
  if ( header.dst != ETHERNET_BROADCAST && header.dst != ethernet_address_ )
    return;
  if ( header.type == EthernetHeader::TYPE_IPv4 ) {
    InternetDatagram dgram;
    parse( dgram, frame.payload );
    datagrams_received().emplace( std::move( dgram ) );
  }
  if ( header.type == EthernetHeader::TYPE_ARP ) {
    ARPMessage msg;
    parse( msg, frame.payload );
    if ( msg.opcode == ARPMessage::OPCODE_REQUEST && msg.target_ip_address == ip_address_.ipv4_numeric() ) {
      ARPMessage reply_msg;
      reply_msg.sender_ethernet_address = ethernet_address_;
      reply_msg.sender_ip_address = ip_address_.ipv4_numeric();
      reply_msg.target_ethernet_address = msg.sender_ethernet_address;
      reply_msg.target_ip_address = msg.sender_ip_address;
      reply_msg.opcode = ARPMessage::OPCODE_REPLY;
      _send_frame(msg.sender_ethernet_address, EthernetHeader::TYPE_ARP, serialize(reply_msg));
    }
    // 不管是哪种ARPMessage 我们都用来更新 ARP table
    arp_table_[msg.sender_ip_address] = { ARP_TIME_MS, msg.sender_ethernet_address };
    // 检查一下是否有对应的next_hop_ip datagram 需要发送
    auto it = buffered_ip_table_.find( msg.sender_ip_address );
    if ( it != buffered_ip_table_.end() ) {
      for ( auto dgram_it = it->second.begin(); dgram_it != it->second.end(); dgram_it++ ) {
        _send_frame(msg.sender_ethernet_address, EthernetHeader::TYPE_IPv4, serialize(std::move(*dgram_it)));
      }
      buffered_ip_table_.erase( it );
    }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  // Your code here.
  for ( auto it = waiting_arp_response_table_.begin(); it != waiting_arp_response_table_.end(); ) {
    if ( it->second > ms_since_last_tick ) {
      it->second -= ms_since_last_tick;
      ++it;
    } else {
      it = waiting_arp_response_table_.erase( it );
    }
  }

  for ( auto it = arp_table_.begin(); it != arp_table_.end(); ) {
    if ( it->second.ttl > ms_since_last_tick ) {
      it->second.ttl -= ms_since_last_tick;
      ++it;
    } else {
      it = arp_table_.erase( it );
    }
  }
}

void NetworkInterface::_send_frame( const EthernetAddress dst,
                                    const uint16_t type,
                                    std::vector<std::string>&& payload )
{
  EthernetFrame frame;
  frame.header.src = ethernet_address_;
  frame.header.dst = dst;
  frame.header.type = type;
  frame.payload = std::move( payload );
  transmit( std::move( frame ) );
}