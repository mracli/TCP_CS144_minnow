#pragma once

#include <cstdint>
struct Timer
{
public:
  Timer( uint64_t init_timeout_ms ) : initial_RTO_ms_( init_timeout_ms ), cur_RTO_ms_( init_timeout_ms ) {}

  bool is_running() const { return running_; }

  bool is_expired() const { return running_ && time_ms_ >= cur_RTO_ms_; }

  void reset_RTO() { cur_RTO_ms_ = initial_RTO_ms_; }

  void double_RTO() { cur_RTO_ms_ <<= 1; }

  void stop() { running_ = false; }

  void start()
  {
    time_ms_ = 0;
    running_ = true;
  }

  void tick( uint64_t ms_since_last_tick )
  {
    if ( running_ )
      time_ms_ += ms_since_last_tick;
  }

private:
  uint64_t initial_RTO_ms_;
  uint64_t cur_RTO_ms_;
  uint64_t time_ms_ { 0 };
  bool running_ = false;
};