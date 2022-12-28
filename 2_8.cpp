#include <thread>
#include <numeric>
#include <vector>
#include <algorithm>
#include <iostream>

#include "debug.h"

template <typename Iterator, typename T>
struct accumulate_block
{
    void operator()(Iterator first, Iterator last, T& result) {
        result = std::accumulate(first, last, result);
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    size_t length = std::distance(first, last);
    if (length <= 0) {
        return init;
    }

    // 每个线程至少计算25个元素的和
    uint32_t min_per_thread = 25;
    uint32_t max_threads = (length + min_per_thread - 1) / min_per_thread;

    uint32_t hardware_threads = std::thread::hardware_concurrency();
    uint32_t num_threads = std::min(hardware_threads == 0 ? 2 : hardware_threads, max_threads);

    unsigned long const block_size = length / num_threads;
    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);
    Iterator block_start = first;
    for (uint32_t i = 0; i < num_threads - 1; ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(accumulate_block<Iterator, T>(), block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iterator, T>()(block_start, last, std::ref(results[num_threads - 1]));
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    return std::accumulate(results.begin(), results.end(), 0);
}

int main()
{
    std::vector<uint64_t> v(100000000);
    std::generate(v.begin(), v.end(), [n = 0]() mutable { return ++n; });

    uint64_t val1, val2;
    time_spend_begin
    val1 = std::accumulate(v.begin(), v.end(), 0);
    time_spend_end

    time_spend_begin
    val2 = parallel_accumulate(v.begin(), v.end(), 0);
    time_spend_end

    std::cout << "val1 = " << val1 << ", val2 = " << val2 << std::endl;
    return 0;
}