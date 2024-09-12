#include "router.hh"
#include "address.hh"

#include <iostream>
#include <limits>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  // Your code here.
#ifndef _ROUTER_TRIE
  _route_table.emplace_back( route_prefix, prefix_length, next_hop, interface_num );
#else
  route_table_.add(route_prefix, prefix_length, next_hop, interface_num);
#endif
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{
  // Your code here.
  for ( auto interface : _interfaces ) {
    auto& queue = interface->datagrams_received();

    while ( queue.size() ) {
      _route_one_datagram( queue.front() );
      queue.pop();
    }
  }
}

void Router::_route_one_datagram( InternetDatagram& dgram )
{
  const auto dst_ip = dgram.header.dst;
#ifndef _ROUTER_TRIE
  auto result_it = _route_table.end();
  for ( auto it = _route_table.begin(); it != _route_table.end(); ++it ) {
    if ( it->prefix_length == 0 || ( ( it->route_prefix ^ dst_ip ) >> ( 32 - it->prefix_length ) ) == 0 ) {
      if ( result_it == _route_table.end() || it->prefix_length > result_it->prefix_length )
        result_it = it;
    }
  }

  if ( result_it != _route_table.end() && dgram.header.ttl > 1 ) {
    --dgram.header.ttl;
    dgram.header.compute_checksum();
    auto next_interface = interface(result_it->interface_num);
    next_interface->send_datagram( dgram, result_it->next_hop.value_or( Address::from_ipv4_numeric( dst_ip ) ) );
  }
#else
  auto route_result = route_table_.find(dst_ip);
  if(route_result.has_value() && dgram.header.ttl > 1){
    --dgram.header.ttl;
    dgram.header.compute_checksum();
    auto next_interface = interface(route_result->interface_num);
    next_interface->send_datagram(dgram, route_result->next_hop.value_or(Address::from_ipv4_numeric(dst_ip)));
  }
#endif
}