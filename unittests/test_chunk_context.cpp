#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest.h>
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>
#include <taskflow/algorithm/transform.hpp>
#include <taskflow/algorithm/find.hpp>
#include <iostream>
#include <mutex>

// --------------------------------------------------------
// Testcase: chunk context
// --------------------------------------------------------

// ChunkContext
TEST_CASE("Chunk.Context.for_each_index.static" * doctest::timeout(300))
{
  for (int tc = 4; tc < 64; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    taskflow.for_each_index(
        0, 100, 1, [&](int i)
        { count++; },
        tf::StaticPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(count == 100);
    REQUIRE(startCount == tc);
  }
}

static const int upper = 1000;

// ChunkContext
TEST_CASE("Chunk.Context.for_each_index.dynamic" * doctest::timeout(300))
{
  for (int tc = 4; tc < 64; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    taskflow.for_each_index(
        0, upper, 1, [&](int i)
        { count++; 
        },
        tf::DynamicPartitioner(upper/tc/2), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(count == upper);
    // Dynamic partitioner will spawn sometimes less tasks
    REQUIRE(startCount <= tc);
  }
}


TEST_CASE("Chunk.Context.for_each.static" * doctest::timeout(300))
{
  for (int tc = 4; tc < 16; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    std::vector<int> range(upper);
    // fill vector with 0, 1, 2, ..., 99
    std::iota(range.begin(), range.end(), 0);
    taskflow.for_each(
        begin(range), end(range), [&](int i)
        { count++; },
        tf::StaticPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(count == upper);
    REQUIRE(startCount == tc);
  }
}

TEST_CASE("Chunk.Context.for_each.dynamic" * doctest::timeout(300))
{
  for (int tc = 4; tc < 16; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    std::vector<int> range(upper);
    std::iota(range.begin(), range.end(), 0);
    taskflow.for_each(
        begin(range), end(range), [&](int i)
        { count++; },
        tf::DynamicPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(count == upper);
    REQUIRE(startCount == tc);
  }
}

TEST_CASE("Chunk.Context.transform.static" * doctest::timeout(300))
{
  // Write a test case for using the taskwrapper on tf::transform
  for (int tc = 4; tc < 16; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    std::vector<int> range(upper);
    std::iota(range.begin(), range.end(), 0);
    std::vector<int> result(upper);
    taskflow.transform(
        begin(range), end(range), begin(result), [&](int i)
        { return i; },
        tf::StaticPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(startCount == tc);
    REQUIRE(result == range);
  }
}

// Implement for dynamic case for transform
TEST_CASE("Chunk.Context.transform.dynamic" * doctest::timeout(300))
{
  for (int tc = 4; tc < 16; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    std::vector<int> range(upper);
    std::iota(range.begin(), range.end(), 0);
    std::vector<int> result(upper);
    taskflow.transform(
        begin(range), end(range), begin(result), [&](int i)
        { return i; },
        tf::DynamicPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(startCount <= tc);
    REQUIRE(result == range);
  }
}

// write a test for find using static partitioner
TEST_CASE("Chunk.Context.find.static" * doctest::timeout(300))
{
  for (int tc = 4; tc < 16; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    std::vector<int> range(upper);
    std::vector<int>::iterator result;
    std::iota(range.begin(), range.end(), 0);
    taskflow.find_if(
        begin(range), end(range), result, [&](int i)
        { return i == 500; },
        tf::StaticPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(startCount == tc);
    REQUIRE(*result == 500);
  }
}

// Same as above testcase but with dynamic partitioner
TEST_CASE("Chunk.Context.find.dynamic" * doctest::timeout(300))
{
  for (int tc = 4; tc < 16; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    std::vector<int> range(upper);
    std::vector<int>::iterator result;
    std::iota(range.begin(), range.end(), 0);
    taskflow.find_if(
        begin(range), end(range), result, [&](int i)
        { return i == 500; },
        tf::DynamicPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(startCount <= tc);
    REQUIRE(*result == 500);
  }
}

// dynamic and static partitioner for find_if_not
TEST_CASE("Chunk.Context.find_if_not.static" * doctest::timeout(300))
{
  for (int tc = 4; tc < 16; tc++)
  {
    tf::Executor executor(tc);
    std::atomic<int> startCount = 0;
    auto task_wrapper = [&](auto &&task)
    {
      startCount++;
      task();
    };
    std::atomic<int> count = 0;
    tf::Taskflow taskflow;
    std::vector<int> range(upper);
    std::vector<int>::iterator result;
    std::iota(range.begin(), range.end(), 0);
    taskflow.find_if_not(
        begin(range), end(range), result, [&](int i)
        { return i !=500; },
        tf::StaticPartitioner(1), task_wrapper);
    executor.run(taskflow).wait();

    REQUIRE(startCount == tc);
    REQUIRE(*result == 500);
  }
}
