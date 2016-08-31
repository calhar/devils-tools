#include <vector>

#include "ao/ao.h"

namespace dtools {
class audio_player {
  private:
    int _driver;
    ao_sample_format _format;
    ao_device *_device;

    std::vector<char> _audio_stream;
    unsigned int _start_offset;
    unsigned int _end_offset;

    bool _playing;

  public:
    audio_player();
    ~audio_player();

    void bits(const int bits) { _format.bits = bits; }
    void channels(const int channels) { _format.channels = channels; }
    void rate(const int rate) { _format.rate = rate; }
    void byte_format(const int byte_format) { _format.byte_format = byte_format; }

    bool playing() const { return _playing; };

    void driver(const int driver);

    void init_device();

    void audio_stream(std::vector<char> const & stream);
    void audio_stream(std::vector<char> const & stream,
                      const unsigned int start,
                      const unsigned int end);

    void start(const unsigned int start) { _start_offset = start; }
    void end(const unsigned int end) { _end_offset = end; }

    void play();
    void stop();
};
}
