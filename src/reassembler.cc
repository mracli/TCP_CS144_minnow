#include "reassembler.hh"
#include <cstdint>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  if ( writer().available_capacity() == 0 ) {
    return;
  }
  if ( data.empty() ) {
    if ( is_last_substring ) {
      output_.writer().close();
    }
    return;
  }

  if ( is_last_substring ) {
    has_last_ = true;
    last_index = first_index + data.size();
  }

  // data's indices: [first_index, end_index)
  // Bytestream's need to pushed indices: [0, bytes_pushed)
  //    which means first unpushed index is `bytes_pushed`
  //    first unacceptable index is bytes_pushed + available_capacity
  uint64_t end_index = first_index + data.size();
  uint64_t availible_first_index = writer().bytes_pushed();
  uint64_t unavailible_first_index = writer().bytes_pushed() + writer().available_capacity();

  // discard all data which totally beyond situations reassebler could handled.
  if ( end_index <= availible_first_index || first_index >= unavailible_first_index )
    return;

  // discard last bytes of unavailible data
  if ( end_index > unavailible_first_index ) {
    data = data.substr( 0, data.size() - end_index + unavailible_first_index );
    end_index = unavailible_first_index;
  }
  // discard prev bytes of data which already pushed
  if ( first_index < availible_first_index ) {
    data = data.substr( availible_first_index - first_index );
    first_index = availible_first_index;
  }
  insert_into_storage( first_index, std::move( data ) );
  if(availible_first_index == storage_.front().first){
    pop_from_storage();
    availible_first_index = writer().bytes_pushed();
  }
  // check if all data pushed into bytestream
  if ( storage_.empty() && has_last_ && availible_first_index == last_index ) {
    output_.writer().close();
  }
}

void Reassembler::insert_into_storage( const uint64_t first_index, std::string&& data )
{

  auto begin_index = first_index;
  const auto end_index = begin_index + data.size();
  // find position to insert data in order
  for ( auto it = storage_.begin(); it != storage_.end() && begin_index < end_index; ) {
    if ( begin_index >= it->first ) {
      begin_index = std::max( begin_index, it->first + it->second.size() );
      it++;
      continue;
    }

    if ( begin_index == first_index && end_index <= it->first ) {
      bytes_pending_ += ( end_index - begin_index );
      storage_.emplace( it, begin_index, std::move( data ) );
      return;
    }

    // begin_index < it->first
    const auto right_index = std::min( end_index, it->first );
    const auto len = right_index - begin_index;
    it = storage_.emplace( it, begin_index, data.substr( begin_index - first_index, len ) );
    bytes_pending_ += len;
    begin_index = right_index;
  }

  // handle situation when storage is empty
  if ( begin_index < end_index ) {
    bytes_pending_ += ( end_index - begin_index );
    storage_.emplace_back( begin_index, data.substr( begin_index - first_index ) );
  }
}

void Reassembler::pop_from_storage()
{
  for ( auto it = storage_.begin(); it != storage_.end(); ) {
    if ( it->first > writer().bytes_pushed() )
      break;
    output_.writer().push( it->second );
    bytes_pending_ -= it->second.size();
    it = storage_.erase( it );
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_;
}
