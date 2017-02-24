#include <chrono>
#include <thread>


#include "stopwatch/stopwatch.hh"

int main(int argc, char** argv)
{
  // Set up stopwatch
  Stopwatch swatch;
  swatch.set_mode(REAL_TIME);

  swatch.start("2sec");
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  swatch.pause("2sec");

  swatch.start("1sec");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  swatch.stop("1sec");

  for (int i = 0; i < 10; i++) {
    swatch.start("10x0.1s");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    swatch.stop("10x0.1s");
  }

  swatch.start("2sec");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  swatch.stop("2sec");

  swatch.report_all();
  return 0;
}
