#include <gtest/gtest.h>

#include <array>
#include <optional>
#include <semaphore>
#include <stdexcept>
#include <thread>
#include <vector>
#include "util/AtomicCircularBufferQueue.hpp"

TEST(AtomicCircularBufferQueue, SingleThreadedTest) {
  util::AtomicCircularBufferQueue<int> queue;
  queue.pushBack(3);
  queue.pushBack(10);

  EXPECT_EQ(queue.tryPopFront(), 3);
  EXPECT_EQ(queue.tryPopFront(), 10);
  EXPECT_EQ(queue.tryPopFront(), std::nullopt);
}

TEST(AtomicCircularBufferQueue, LoopRoundTest) {
  util::AtomicCircularBufferQueue<int, 3> queue;
  queue.pushBack(3);
  queue.pushBack(10);

  EXPECT_THROW(queue.pushBack(5), std::runtime_error);

  EXPECT_EQ(queue.tryPopFront(), 3);

  queue.pushBack(5);

  EXPECT_EQ(queue.tryPopFront(), 10);
  EXPECT_EQ(queue.tryPopFront(), 5);
  EXPECT_EQ(queue.tryPopFront(), std::nullopt);
}

TEST(AtomicCircularBufferQueue, OneReaderManyWriters) {
  util::AtomicCircularBufferQueue<int> queue;
  std::array<bool, 1000> seen{false};

  std::vector<std::jthread> threads;
  std::counting_semaphore startSemaphore{0};
  threads.reserve(11);
  for (int i = 0; i < 10; i++) {
    threads.emplace_back([&startSemaphore, &queue, i]() {
      startSemaphore.acquire();
      for (int j = i * 100; j < (i + 1) * 100; j++) {
        queue.pushBackBlocking(j);
      }
    });
  }

  threads.emplace_back([&startSemaphore, &queue, &seen]() {
    startSemaphore.acquire();
    int seenCount = 0;
    while (seenCount < 1000) {
      std::optional<int> next = queue.tryPopFront();
      if (next.has_value()) {
        seen[*next] = true;
        seenCount++;
      }
    }
  });

  startSemaphore.release(11);

  for (auto& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }

  for (const auto& b : seen) {
    EXPECT_TRUE(b);
  }
}

TEST(AtomicCircularBufferQueue, ManyReadersOneWriter) {
  util::AtomicCircularBufferQueue<int> queue;
  std::array<bool, 1000> seen{false};

  std::vector<std::jthread> threads;
  std::counting_semaphore startSemaphore{0};
  threads.reserve(11);
  threads.emplace_back([&startSemaphore, &queue]() {
    startSemaphore.acquire();
    for (int j = 0; j < 1000; j++) {
      queue.pushBackBlocking(j);
    }
  });

  for (int i = 0; i < 10; i++) {
    threads.emplace_back([&startSemaphore, &queue, &seen]() {
      startSemaphore.acquire();
      int seenCount = 0;
      while (seenCount < 100) {
        std::optional<int> next = queue.tryPopFront();
        if (next.has_value()) {
          seen[*next] = true;
          seenCount++;
        }
      }
    });
  }

  startSemaphore.release(11);

  for (auto& t : threads) {
    t.join();
  }

  for (const auto& b : seen) {
    EXPECT_TRUE(b);
  }
}

TEST(AtomicCircularBufferQueue, ManyReadersManyWriters) {
  util::AtomicCircularBufferQueue<int> queue;
  std::array<bool, 1000> seen{false};

  std::vector<std::jthread> threads;
  std::counting_semaphore startSemaphore{0};
  threads.reserve(20);
  for (int i = 0; i < 10; i++) {
    threads.emplace_back([&startSemaphore, &queue, i]() {
      startSemaphore.acquire();
      for (int j = i * 100; j < (i + 1) * 100; j++) {
        queue.pushBackBlocking(j);
      }
    });
  }

  for (int i = 0; i < 10; i++) {
    threads.emplace_back([&startSemaphore, &queue, &seen]() {
      startSemaphore.acquire();
      int seenCount = 0;
      while (seenCount < 100) {
        std::optional<int> next = queue.tryPopFront();
        if (next.has_value()) {
          seen[*next] = true;
          seenCount++;
        }
      }
    });
  }

  startSemaphore.release(20);

  for (auto& t : threads) {
    t.join();
  }

  for (const auto& b : seen) {
    EXPECT_TRUE(b);
  }
}
