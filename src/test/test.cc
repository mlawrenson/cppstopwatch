#include <chrono>
#include <thread>


#include "stopwatch/stopwatch.hh"

int main(int argc, char** argv)
{
  // Set up stopwatch
  Stopwatch swatch;
  swatch.set_mode(REAL_TIME);

  swatch.start("3sec");
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  swatch.pause("3sec");

  swatch.start("1sec");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  swatch.stop("1sec");

  swatch.start("3sec");
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  swatch.stop("3sec");

  swatch.report_all();
  return 0;
}
