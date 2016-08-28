#include <fstream>
#include <cstring>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>

int main(int argc, char* args[]) {
  int channels = 2;
  int sampleSize = 2;

  std::string inFilename;
  std::string outFilename;

  if (argc > 1) {
    for (auto iter = args + 1; iter != args + argc; ++iter) {
      if (strcmp(*iter, "-i") == 0) {
        inFilename = std::string(*(++iter));
      } else if (strcmp(*iter, "-o") == 0) {
        outFilename = std::string(*(++iter));
      } else if (strcmp(*iter, "-c") == 0) {
        channels = std::stoi(*(++iter));
      } else if (strcmp(*iter, "-s") == 0) {
        sampleSize = std::stoi(*(++iter));
      }
    }
  }

  std::ifstream inFilestream;
  std::istream* inStream = &std::cin;
  if (!inFilename.empty()) {
    inFilestream = std::ifstream(inFilename, std::ios::binary);
    inStream = &inFilestream;
  }

  std::vector<char> audioData (
      (std::istreambuf_iterator<char>(*inStream)),
      (std::istreambuf_iterator<char>()));

  int samples = audioData.size() / (channels * sampleSize);

  for (int i = 0; i < samples / 2; ++i) {
    int offsetStart = i * channels * sampleSize;
    int offsetEnd = (samples - (i + 1)) * channels * sampleSize;

    std::swap_ranges(
        audioData.begin() + offsetStart,
        audioData.begin() + offsetStart + (channels * sampleSize),
        audioData.begin() + offsetEnd);
  }

  std::ofstream outFilestream;
  std::ostream* outStream = &std::cout;
  if (!outFilename.empty()) {
    outFilestream = std::ofstream(
        outFilename,
        std::ios::out | std::ios::binary);
    outStream = &outFilestream;
  }

  std::copy(
      audioData.begin(),
      audioData.end(),
      std::ostreambuf_iterator<char>(*outStream));

  return 0;
}
