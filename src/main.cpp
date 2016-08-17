#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

int main(int argc, char* args[]) {
  int channels = 2;
  int sampleSize = 2;

  std::ifstream inputFile(args[1], std::ios::binary);

  std::vector<char> audioData (
      (std::istreambuf_iterator<char>(inputFile)),
      (std::istreambuf_iterator<char>()));

  int samples = audioData.size() / (channels * sampleSize);

  for (int i = 0; i < samples / 2; ++i) {
    int offsetStart = i * channels * sampleSize;
    int offsetEnd = (samples - (i + 1)) * channels * sampleSize;

    for (int j = 0; j < channels * sampleSize; ++j) {
      char temp = audioData[offsetStart + j];
      audioData[offsetStart + j] = audioData[offsetEnd + j];
      audioData[offsetEnd + j] = temp;
    }
  }

  std::ofstream outputFile(args[2], std::ios::out | std::ios::binary);

  std::copy(
      audioData.begin(),
      audioData.end(),
      std::ostreambuf_iterator<char>(outputFile));

  return 0;
}
