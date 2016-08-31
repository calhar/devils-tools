#include "audio_player.h"

#include <iostream>

namespace dtools {
audio_player::audio_player() 
  : _format({})
  , _device(nullptr)
  , _playing(false) {
  ao_initialize();

  _driver = ao_default_driver_id();

}

audio_player::~audio_player() {
  if (_device) {
    ao_close(_device);
  }
  ao_shutdown();
}

void audio_player::audio_stream(std::vector<char> const & stream) {
  _audio_stream = stream;
  _start_offset = 0;
  _end_offset = stream.size();
}

void audio_player::audio_stream(std::vector<char> const & stream,
                                const unsigned int start,
                                const unsigned int end) {
  _audio_stream = stream;
  _start_offset = start;
  _end_offset = end;
}

void audio_player::driver(const int driver) {
  _driver = driver;
  if (_device) {
    ao_close(_device);
    _device = nullptr;
  }
}

void audio_player::init_device() {
  if (_device) {
    ao_close(_device);
    _device = nullptr;
  }

  _device = ao_open_live(_driver, &_format, nullptr);
  if (!_device) {
    std::cout << "failed to load audio driver" << std::endl;
  }
}

void audio_player::play() {
  _playing = true;
  if (!_device) {
    init_device();
    if (!_device) {
      _playing = false;
    }
  }

  unsigned int played = _start_offset;
  if (_audio_stream.empty()) {
    _playing = false;
  }
  while (_playing) {
    unsigned int to_play = (_format.rate / 10) * // 0.1 seconds playtime
                           (_format.bits / 8) *
                           _format.channels;
    if (played + to_play > _end_offset) {
      to_play = _end_offset - played;
    }
    ao_play(_device, _audio_stream.data() + played, to_play);
    played += to_play;
    if (played == _end_offset) {
      _playing = false;
    }
  }
}

void audio_player::stop() {
  _playing = false;
}
}

