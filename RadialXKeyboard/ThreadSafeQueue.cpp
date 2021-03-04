//
//  ThreadSafeQueue.cpp
//  RadialXKeyboard
//
//  Created by LuisValencia on 3/3/21.
//

#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <CoreGraphics/CGGeometry.h>
#include <condition_variable>
#include <mutex>
#include <queue>

// A threadsafe-queue.
template<class T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;
public:
    ThreadSafeQueue() : queue(), mutex(), condition() {
    }

    ~ThreadSafeQueue() {
    };

    void enqueue(T t){
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(t);
        condition.notify_one();
    }

    T dequeue(void){
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty()) {
            // release lock as long as the wait and reacquire it afterwards.
            condition.wait(lock);
        }
        T val = queue.front();
        queue.pop();
        return val;
    }

    bool isEmpty() {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }
};

#endif
