#ifndef _SIMPLE_THREADPOOL_H_
#define _SIMPLE_THREADPOOL_H_

#include <vector>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <iostream>
#include <stdexcept>
#include <memory> //unique_ptr

const int MAX_THREAD_NUM = 100;
typedef std::function<void(void)> Task;

class ThreadPool
{
public:
    ThreadPool(int threadNum = 1);
    ~ThreadPool();

    bool addTask(Task task);

private:
    void work();
    static void* worker(void *arg);

private:
    int threadNum;
    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    std::mutex tasks_mutex;
    std::condition_variable cv;
    bool stop;
};

ThreadPool::ThreadPool(int threadNum) : stop(false)
{
    if (threadNum <= 0 || threadNum > MAX_THREAD_NUM)
        throw std::exception();

    for (int i = 0; i< threadNum; ++i)
    {
        workers.emplace_back(ThreadPool::worker, this);
    }
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_ptr<std::mutex> lock;
        stop = true;//using atomic instead?
    }

    cv.notify_all();//wake up all threads
    for (auto &work : workers)
        work.join();
}

bool ThreadPool::addTask(Task task)
{
    tasks_mutex.lock();
    tasks.push(task);
    tasks_mutex.unlock();
    cv.notify_one();//wake up anyone thread. anyone really?
    return true;
}

void* ThreadPool::worker(void* arg)
{
    ThreadPool *pool = (ThreadPool*)arg;
    pool->work();
    return pool;
}

void ThreadPool::work()
{
    while(!stop)
    {
        std::unique_lock<std::mutex> lock(this->tasks_mutex);
        this->cv.wait(lock, [this]{return !this->tasks.empty();});

        if (this->tasks.empty())
        {
            continue;
        }
        else
        {
            Task task = tasks.front();
            tasks.pop();
            task();
        }
    }
}

#endif
