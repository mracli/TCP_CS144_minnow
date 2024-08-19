#include "wrapping_integers.hh"
#include <climits>
#include <cstdint>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32 { static_cast<uint32_t>( n ) + zero_point.raw_value_ };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  auto diff = raw_value_ - zero_point.raw_value_ - static_cast<uint32_t>( checkpoint );
  uint64_t result;
  if(diff <= (1u << 31)){
    result = checkpoint + diff;
  }else{
    result = checkpoint - ((1ul << 32) - diff);
    if(result > checkpoint){
      result = checkpoint + diff;
    }
  }
  return result;
}
