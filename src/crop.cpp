#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <thread>

#include "ao/ao.h"
#include "audio_player.h"

int main(int argc, char *args[]) {
  std::string in_filename;
  std::string out_filename;

  int channels = 2;
  int bytes = 2;
  int rate = 44100;

  int start = 0;
  int end;

  in_filename = std::string(args[1]);
  out_filename = std::string(args[2]);
  start = std::stoi(args[3]);
  end = std::stoi(args[4]);

  // get basic_istream for filename or stdin if none provided
  std::ifstream in_filestream;
  std::istream* in_stream = &std::cin;
  if (!in_filename.empty()) {
    in_filestream = std::ifstream(in_filename, std::ios::binary);
    inStream = &in_filestream;
  }

  std::vector<char> audio((std::istreambuf_iterator<char>(in_stream)),
                          (std::istreambuf_iterator<char>()));

  int samples = audio.size() / (channels * bytes);

  int max_time = (samples / rate) * rate;

  dtools::audio_player player;
  player.bits(8 * bytes);
  player.channels(channels);
  player.rate(rate);
  player.byte_format(AO_FMT_LITTLE);

  player.init_device();

  std::thread player_thread;
  char c = '0';
  do {
    // basic command line interface using getchar()
    // a/f controls start of crop
    // h/l controls end of crop
    // p plays/stops the audio preview
    // c saves the cropped section to file
    // q quits
    if (c == 'f') {
      start = start + 1 < end ? start + 1 : start;
    } else if (c == 'a') {
      start = start - 1 >= 0 ? start - 1 : start;
    } else if (c == 'l') {
      end = end + 1 <= max_time ? end + 1 : end;
    } else if (c == 'h') {
      end = end - 1 > start ? end - 1 : end;
    } else if (c == 'p') {
      if (player.playing()) {
        player.stop();
        player_thread.join();

      } else {
        if (player_thread.joinable()) {
          player_thread.join();
        }
        player.audio_stream(audio,
                            start * bytes * channels * 44100,
                            end * bytes * channels * 44100);

        player_thread = std::thread(&dtools::audio_player::play, &player);
      }
      // bit of a hack to avoid race conditions stemming from entering commands
      // like "pq<return>"
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } else if (c == 'c') {
      // create/open file or get stdout
      std::ofstream out_filestream;
      std::ostream* out_stream = &std::cout;
      if (!out_filename.empty()) {
        out_filestream = std::ofstream(
            out_filename,
            std::ios::out | std::ios::binary);
        out_stream = &out_filestream;
      }

      std::copy(
          audio.begin() + start * channels * bytes * rate,
          audio.begin() + end * channels * bytes * rate,
          std::ostreambuf_iterator<char>(*out_stream));
    } else if (c == '\n') {
      continue;
    }
    std::cout << "start: " << start << " end: " << end << std::endl;
  }
  while ((c = getchar()) != 'q');
  if (player_thread.joinable()) {
    player.stop();
    player_thread.join();
  }
}
