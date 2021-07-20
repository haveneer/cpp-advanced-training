#pragma once

#include "gtest/gtest.h"
#include <future>
#include <thread>

// from https://github.com/google/googletest/issues/348
#define ASSERT_DURATION_LE(secs, stmt)                                 \
  {                                                                    \
    std::promise<bool> completed;                                      \
    auto stmt_future = completed.get_future();                         \
    std::thread(                                                       \
        [&](std::promise<bool> &completed) {                           \
          stmt;                                                        \
          completed.set_value(true);                                   \
        },                                                             \
        std::ref(completed))                                           \
        .detach();                                                     \
    if (stmt_future.wait_for(std::chrono::seconds(secs)) ==            \
        std::future_status::timeout)                                   \
      GTEST_FATAL_FAILURE_("       timed out (> " #secs " seconds)."); \
  }
