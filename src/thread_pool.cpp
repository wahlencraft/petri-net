#include "thread_pool.h"

#include <iostream>
#include <string>

using namespace std;

string const BOLD = "\033[1m";
string const END = "\033[0m";

atomic_int job_counter = 0;

ThreadPool::ThreadPool(unsigned threads) {
    if (threads == 0 || threads > std::thread::hardware_concurrency() - 1)
        threads = std::thread::hardware_concurrency() - 1;

    for (unsigned i = 0; i < threads; ++i) {
        std::thread worker([this]() {
        while (true) {
            int job_id;
            std::function<void()> task;
            /* pop a task from queue, and execute it. */
            {
                std::unique_lock lock(mtx);
                cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                if (stop && tasks.empty())
                    return;
                /* even if stop = 1, once tasks is not empty, then
                 * excucte the task until tasks queue become empty
                 */
                job_id = job_counter++;
                //std::cout << BOLD << "Popping job #" << job_id << " from the queue\n";
                task = std::move(tasks.front());
                tasks.pop();
                ++running_jobs;
            }
            task();
            --running_jobs;
            cv.notify_all();
            }
        });
        workers.emplace_back(std::move(worker));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (stop) {
            throw std::runtime_error("Can't enqueue more jobs when the treadpool is stopping.");
        }
        tasks.emplace(job);
    }
    cv.notify_one();
}

void ThreadPool::wait_for_jobs_to_finish() {
    do {
        std::unique_lock lock(mtx);
        cv.wait(lock, [this]() { return tasks.empty(); });
    } while (running_jobs);
}

bool ThreadPool::full() const {
    return running_jobs >= workers.size();
}

