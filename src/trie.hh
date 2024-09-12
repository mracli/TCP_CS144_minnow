#pragma once

#include "address.hh"
#include <array>
#include <cstdint>
#include <optional>

using std::array;
using std::nullopt;
using std::optional;

struct Trie
{
public:
  struct Entry
  {
    optional<Address> next_hop;
    size_t interface_num;
  };

  Trie() {}

  void add( uint32_t route_prefix, uint8_t prefix_length, optional<Address> next_hop, size_t interface_num )
  {
    Trie* p = this;
    for ( size_t i = 0; i < prefix_length; i++ ) {
      size_t num = ( route_prefix >> ( 31 - i ) ) & 1;
      if ( p->node_[num] == nullptr )
        p->node_[num] = new Trie();
      p = p->node_[num];
    }
    p->entry_ = { next_hop, interface_num };
  }

  [[nodiscard]] optional<Entry> find( uint32_t route_ip )
  {
    Trie* p = this;
    auto result = p->entry_;
    for ( size_t i = 0; i < 32; i++ ) {
      size_t num = ( route_ip >> ( 31 - i ) ) & 1;
      if ( p->node_[num] == nullptr )
        return entry_.value();
      p = p->node_[num];
      if ( p->entry_.has_value() )
        result = p->entry_;
    }

    return entry_;
  }

  ~Trie()
  {
    for ( auto elem : node_ ) {
      if ( elem == nullptr )
        continue;
      delete elem;
    }
  }

private:
  array<Trie*, 2> node_ { nullptr };

  optional<Entry> entry_ {};
};