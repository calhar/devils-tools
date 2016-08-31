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

  int start = 0;
  int end;

  in_filename = std::string(args[1]);
  out_filename = std::string(args[2]);
  start = std::stoi(args[3]);
  end = std::stoi(args[4]);

  std::ifstream in_filestream(in_filename, std::ios::binary);

  std::vector<char> audio((std::istreambuf_iterator<char>(in_filestream)),
                          (std::istreambuf_iterator<char>()));

  int samples = audio.size() / (channels * bytes);

  int max_time = samples; //(samples / 44100) * 44100;

  dtools::audio_player player;
  player.bits(8 * bytes);
  player.channels(channels);
  player.rate(44100);
  player.byte_format(AO_FMT_LITTLE);

  player.init_device();

  std::thread player_thread;
  char c = '0';
  do {
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
