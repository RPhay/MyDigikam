/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Foundation for all machine learning classifiers
 *
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

 #pragma once

 // Qt includes
 
 #include <QMutex>
 #include <QWaitCondition>
 #include <QQueue>
 
 // C++ includes
 
 #include <limits>
 
 // Local includes
 
 #include "digikam_export.h"
 
 namespace Digikam
 {
 
 template <typename T>
 class DIGIKAM_EXPORT SharedQueue
 {
 public:
 
    SharedQueue()  = default;
    ~SharedQueue() = default;

 public:
 
    T& front()
    {
        QMutexLocker frontLock(&frontMutex_);

        while (queue_.isEmpty())
        {
            front_.wait(&frontMutex_);
        }

        QMutexLocker lock(&mutex_);

        return queue_.head();
     }
 
    T pop_front()
    {
        QMutexLocker frontLock(&frontMutex_);

        while (queue_.isEmpty())
        {
            front_.wait(&frontMutex_);
        }

        QMutexLocker lock(&mutex_);

        T result = queue_.dequeue();
        back_.wakeOne();

        return result;
    }
 
    void push_back(T const& item)
    {
        QMutexLocker backLock(&backMutex_);

        while (maxDepth_ <= queue_.size())
        {
            back_.wait(&backMutex_);
        }

        QMutexLocker lock(&mutex_);

        queue_.enqueue(item);
        front_.wakeOne();     // Notify one waiting thread.
     }
 
     void push_back(T&& item)
     {
        QMutexLocker backLock(&backMutex_);

        while (maxDepth_ <= queue_.size())
        {
            back_.wait(&backMutex_);
        }

        QMutexLocker lock(&mutex_);

        queue_.enqueue(std::move(item));
        front_.wakeOne();     // Notify one waiting thread.
     }
 
     int size()
     {
        QMutexLocker lock(&mutex_);
        int size = queue_.size();

        return size;
     }
 
     bool isEmpty()
     {
        QMutexLocker lock(&mutex_);
        bool ret = queue_.isEmpty();

        return ret;
     }
 
     void clear()
     {
        QMutexLocker lock(&mutex_);
        queue_.clear();
     }
 
     int maxDepth() const
     {
         return maxDepth_;
     }
 
     void setMaxDepth(int depth)
     {
         maxDepth_ = depth;
     }
 
     int maxDepthLimit()
     {
         return std::numeric_limits<int>::max();
     }
     
 private:
 
     QQueue<T>               queue_;
     QMutex                  mutex_;
     QMutex                  frontMutex_;
     QMutex                  backMutex_;
     QWaitCondition          front_;
     QWaitCondition          back_;
     qsizetype               maxDepth_ = std::numeric_limits<int>::max();
 };
 
 } // namespace Digikam
 