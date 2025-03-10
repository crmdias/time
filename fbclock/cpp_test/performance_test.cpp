// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include <chrono>
#include <iostream>
#include <vector>
#include "time/fbclock/fbclock.h"

int main() {
  std::vector<uint32_t>
      time_histogram; // create a histogramm of call time in us

  time_histogram.resize(1001, 0);
  fbclock_lib fbclock = {};
  int res = fbclock_init(&fbclock, FBCLOCK_PATH);
  if (res != 0) {
    std::cout << "Failed to init fbclock library: errno " << res << std::endl;
    exit(0);
  }
  for (int i = 0; i < 1000000; i++) {
    fbclock_truetime true_time = {};
    auto start_time = std::chrono::steady_clock::now();
    fbclock_gettime_utc(&fbclock, &true_time);
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time);
    if (duration.count() >= 1000)
      time_histogram[1000]++;
    else
      time_histogram[duration.count()]++;
    if (true_time.earliest_ns + 10000 <= true_time.latest_ns)
      std::cout << "WoU is more than 10us [" << true_time.earliest_ns << ","
                << true_time.latest_ns << "] " << std::endl;
  }
  std::cout << "Histogram of query time:" << std::endl;
  for (int i = 0; i < 1001; i++) {
    if (time_histogram[i])
      std::cout << i << "us: " << time_histogram[i] << std::endl;
  }
  fbclock_destroy(&fbclock);
}
