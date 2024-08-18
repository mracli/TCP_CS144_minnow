#include "byte_stream.hh"
#include <cstdint>
#include <sys/types.h>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_;
}

void Writer::push( string data )
{
  // Your code here.
  // Check if still enough capacity to save enough data
  if(error_ || is_closed() || data.empty())
    return;
  if(data.size() > available_capacity())
    data = data.substr(0, available_capacity());
  buffer_.append(data);
  pushed_size_ += data.size();
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return pushed_size_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed_ && buffer_.empty();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return popped_size_;
}

string_view Reader::peek() const
{
  // Your code here.
  return buffer_;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if(buffer_.empty()) return;
  len = std::min(len, buffer_.size());
  buffer_.erase(0, len);
  popped_size_ += len;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer_.size();
}
