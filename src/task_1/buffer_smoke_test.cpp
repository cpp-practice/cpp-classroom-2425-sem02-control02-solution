#include "buffer.hpp"
#include "buffer.hpp"

#include "../simple_test.h"

#include <set>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <numeric>

using namespace std::chrono_literals;

class BufferTestUtils {
 public:
  void wait_till_size(size_t size) {
    std::unique_lock<std::mutex> lk(m);
    cv.wait_for(lk, 1000ms, [this, size]() { return storeroom.size() == size; });
    EXPECT_EQ(storeroom.size(), size);
  }

  std::vector<std::string> storeroom;
  std::mutex m;
  std::condition_variable cv;
};

TEST(BufferTest, single_execution_queue_single_buffer) {
  BufferTestUtils utils;
  {
    std::shared_ptr<control_02::ExecutionQueue> w = std::make_shared<control_02::ExecutionQueue>();
    {
      auto buf1 = control_02::Buffer::make_buffer();

      buf1->post([&utils]() {
        {
          std::unique_lock<std::mutex> lk{utils.m};
          utils.storeroom.push_back("Hello there!");
        }
        utils.cv.notify_one();
      });

      buf1->post([&utils]() {
        {
          std::unique_lock<std::mutex> lk{utils.m};
          utils.storeroom.push_back("Hello here!");
        }
        utils.cv.notify_one();
      });

      ASSERT_EQ(buf1->queue_size(), 2);
      buf1->move_to_execution_queue(w);
      utils.wait_till_size(2);
    }
  }
  std::vector<std::string> expected = {"Hello there!", "Hello here!"};
  ASSERT_EQ(utils.storeroom, expected);
}


TEST(BufferTest, single_execution_queue_multiple_buffers) {
  BufferTestUtils utils;
  {
    std::shared_ptr<control_02::ExecutionQueue> w = std::make_shared<control_02::ExecutionQueue>();

    {
      auto buf1 = control_02::Buffer::make_buffer();
      auto buf2 = control_02::Buffer::make_buffer();

      buf1->post([&utils]() {
        {
          std::unique_lock<std::mutex> lk{utils.m};
          utils.storeroom.push_back("Hello there!");
        }
        utils.cv.notify_one();
      });

      buf2->post([&utils]() {
        {
          std::unique_lock<std::mutex> lk{utils.m};
          utils.storeroom.push_back("Hello here!");
        }
        utils.cv.notify_one();
      });

      ASSERT_EQ(buf1->queue_size(), 1);
      ASSERT_EQ(buf2->queue_size(), 1);
      buf1->move_to_execution_queue(w);
      buf2->move_to_execution_queue(w);
      utils.wait_till_size(2);
    }
  }
  std::vector<std::string> expected = {"Hello there!", "Hello here!"};
  ASSERT_EQ(utils.storeroom, expected);
}

TEST(BufferTest, multiple_execution_queues_single_buffer) {
  BufferTestUtils utils;
  {
    std::shared_ptr<control_02::ExecutionQueue> w1 = std::make_shared<control_02::ExecutionQueue>();
    std::shared_ptr<control_02::ExecutionQueue> w2 = std::make_shared<control_02::ExecutionQueue>();

    {
      auto buf1 = control_02::Buffer::make_buffer();

      buf1->post([&utils]() {
        {
          std::unique_lock<std::mutex> lk{utils.m};
          utils.storeroom.push_back("Hello there!");
        }
        utils.cv.notify_one();
      });

      ASSERT_EQ(buf1->queue_size(), 1);
      buf1->move_to_execution_queue(w1);
      buf1->post([&utils]() {
        {
          std::unique_lock<std::mutex> lk{utils.m};
          utils.storeroom.push_back("Hello here!");
        }
        utils.cv.notify_one();
      });
      buf1->move_to_execution_queue(w2);
      utils.wait_till_size(2);
    }
  }
  std::vector<std::string> expected = {"Hello there!", "Hello here!"};
  ASSERT_EQ(utils.storeroom, expected);
}

TEST(BufferTest, test_overflow) {
  BufferTestUtils utils;
  {
    std::shared_ptr<control_02::ExecutionQueue> w1 = std::make_shared<control_02::ExecutionQueue>();

    {
      auto buf1 = control_02::Buffer::make_buffer(2);

      for (size_t i = 0; i < 10; i++) {
        buf1->post([&utils, i]() {
          {
            std::unique_lock<std::mutex> lk{utils.m};
            utils.storeroom.push_back(std::to_string(i));
          }
          utils.cv.notify_one();
        });
      }

      ASSERT_EQ(buf1->queue_size(), 2);
      buf1->move_to_execution_queue(w1);
      utils.wait_till_size(2);
    }
  }
  std::vector<std::string> expected = {"8", "9"};
  ASSERT_EQ(utils.storeroom, expected);
}

TEST(BufferTest, multiple_execution_queues_multiple_buffers) {
  BufferTestUtils utils;

  std::set<size_t> expected;
  std::set<size_t> numbers;
  {
    std::vector<std::shared_ptr<control_02::ExecutionQueue>> execution_queues;
    constexpr size_t execution_queues_num = 9;
    for (size_t i = 0; i < execution_queues_num; i++) {
      execution_queues.push_back(std::make_shared<control_02::ExecutionQueue>());
    }

    {
      std::vector<std::shared_ptr<control_02::Buffer>> buffers;
      constexpr size_t buffers_num = 9;
      for (size_t i = 0; i < buffers_num; i++) {
        buffers.push_back(control_02::Buffer::make_buffer());
        buffers.back()->move_to_execution_queue(execution_queues[i % execution_queues_num]);
      }
      for (size_t i = 0; i < 50; i++) {
        auto& buf = buffers[i % buffers_num];
        buf->post([&utils, &numbers, i]() {
          std::unique_lock lk(utils.m);
          numbers.insert(i);
          lk.unlock();
          utils.cv.notify_one();
        });
        expected.insert(i);
      }

      std::unique_lock<std::mutex> lk(utils.m);
      utils.cv.wait_for(lk, 1000ms, [&numbers]() { return numbers.size() == 50; });
    }
  }
  ASSERT_EQ(numbers, expected);
}


TESTING_MAIN()
