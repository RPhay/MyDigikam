/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Foundation for all ML classifiers
 *
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <queue>
#include <mutex>
#include <condition_variable>

// local includes

#include "digikam_export.h"

namespace Digikam
{

template <typename T>
class DIGIKAM_EXPORT SharedQueue
{
public:

    SharedQueue()                   = default;
    ~SharedQueue()                  = default;

    T&   front()
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        while (queue_.empty())
        {
            front_.wait(mlock);
        }

        return queue_.front();
    }

    T&   pop_front()
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        while (queue_.empty())
        {
            front_.wait(mlock);
        }

        T& result = queue_.front();
        queue_.pop_front();
        back_.notify_one();
        return result;
    }

    void push_back(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        while (maxDepth_ <= queue_.size())
        {
            back_.wait(mlock);
        }

        queue_.push_back(item);
        mlock.unlock();          // Unlock before notificiation to minimize mutex con.
        front_.notify_one();     // Notify one waiting thread.
    }

    void push_back(T&& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        if (maxDepth_ <= queue_.size())
        {
            back_.wait(mlock);
        }

        queue_.push_back(std::move(item));
        mlock.unlock();          // Unlock before notificiation to minimize mutex con.
        front_.notify_one();     // Notify one waiting thread?
    }

    int  size()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        int size = queue_.size();
        mlock.unlock();

        return size;
    }

    bool empty()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        bool ret = queue_.empty();
        mlock.unlock();

        return ret;
    }

    void clear()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.clear();
        mlock.unlock();
    }

    int  maxDepth()
    {
        return maxDepth_;
    }

    void maxDepth(int depth)
    {
        maxDepth_ = depth;
    }

private:

    std::deque<T>           queue_;
    std::mutex              mutex_;
    std::condition_variable front_;
    std::condition_variable back_;
    std::size_t             maxDepth_ = std::numeric_limits<std::size_t>::max();
};

// template <typename T>
// SharedQueue<T>::SharedQueue()  {}

// template <typename T>
// SharedQueue<T>::~SharedQueue() {}

// template <typename T>
// T& SharedQueue<T>::front()
// {
//     std::unique_lock<std::mutex> mlock(mutex_);

//     while (queue_.empty())
//     {
//         front_.wait(mlock);
//     }

//     return queue_.front();
// }

// template <typename T>
// T& SharedQueue<T>::pop_front()
// {
//     std::unique_lock<std::mutex> mlock(mutex_);

//     while (queue_.empty())
//     {
//         front_.wait(mlock);
//     }
//     T& result = queue_.front();
//     queue_.pop_front();
//     back_.notify_one();
//     return result;
// }

// template <typename T>
// void SharedQueue<T>::push_back(T& item)
// {
//     std::unique_lock<std::mutex> mlock(mutex_);
//     while (maxDepth_ <= queue_.size())
//     {
//         back_.wait(mlock);
//     }
//     queue_.push_back(item);
//     mlock.unlock();         // Unlock before notificiation to minimize mutex con.
//     front_.notify_one();     // Notify one waiting thread.
// }

// template <typename T>
// void SharedQueue<T>::push_back(T&& item)
// {
//     std::unique_lock<std::mutex> mlock(mutex_);
//     if (maxDepth_ <= queue_.size())
//     {
//         back_.wait(mlock);
//     }
//     queue_.push_back(std::move(item));
//     mlock.unlock();         // Unlock before notificiation to minimize mutex con.
//     front_.notify_one();     // Notify one waiting thread?
// }

// template <typename T>
// int SharedQueue<T>::size()
// {
//     std::unique_lock<std::mutex> mlock(mutex_);
//     int size = queue_.size();
//     mlock.unlock();

//     return size;
// }

// template <typename T>
// bool SharedQueue<T>::empty()
// {
//     std::unique_lock<std::mutex> mlock(mutex_);
//     bool ret = queue_.empty();
//     mlock.unlock();

//     return ret;
// }

// template <typename T>
// void SharedQueue<T>::clear()
// {
//     std::unique_lock<std::mutex> mlock(mutex_);
//     queue_.clear();
//     mlock.unlock();
// }

// template <typename T>
// int SharedQueue<T>::maxDepth()
// {
//     return maxDepth_;
// }

// template <typename T>
// void SharedQueue<T>::maxDepth(int depth)
// {
//     maxDepth_ = depth;
// }

} // namespace Digikam
