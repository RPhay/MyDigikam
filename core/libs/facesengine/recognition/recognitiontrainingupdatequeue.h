/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2024-10-28
 * Description : Threadsafe queue for submitting face training removal requests.
 *
 * SPDX-FileCopyrightText: 2024      by Michael Miller <micahel underscore miller at msn dot com>
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <queue>
#include <mutex>
#include <condition_variable>

// Qt includes

#include <QString>
#include <QThread>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

template <typename T>
class SharedQueue
{
public:

    SharedQueue();
    ~SharedQueue();

    T&   front();
    void pop_front();

    void push_back(T& item);
    void push_back(T&& item);

    int  size();
    bool empty();

private:

    std::deque<T>           queue_;
    std::mutex              mutex_;
    std::condition_variable cond_;
}; 

template <typename T>
SharedQueue<T>::SharedQueue()  {}

template <typename T>
SharedQueue<T>::~SharedQueue() {}

template <typename T>
T& SharedQueue<T>::front()
{
    std::unique_lock<std::mutex> mlock(mutex_);

    while (queue_.empty())
    {
        cond_.wait(mlock);
    }

    return queue_.front();
}

template <typename T>
void SharedQueue<T>::pop_front()
{
    std::unique_lock<std::mutex> mlock(mutex_);

    while (queue_.empty())
    {
        cond_.wait(mlock);
    }

    queue_.pop_front();
}

template <typename T>
void SharedQueue<T>::push_back(T& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(item);
    mlock.unlock();         // Unlock before notificiation to minimize mutex con.
    cond_.notify_one();     // Notify one waiting thread.
}

template <typename T>
void SharedQueue<T>::push_back(T&& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(std::move(item));
    mlock.unlock();         // Unlock before notificiation to minimize mutex con.
    cond_.notify_one();     // Notify one waiting thread?
}

template <typename T>
int SharedQueue<T>::size()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    int size = queue_.size();
    mlock.unlock();

    return size;
}

// -----------------------------------------------------

class DIGIKAM_GUI_EXPORT RecognitionTrainingUpdateQueue
{
public:

    RecognitionTrainingUpdateQueue();
    ~RecognitionTrainingUpdateQueue();

public:

    void push(const QString& hash)          { QString val = hash; queue.push_back(val); }
    void pop()                              { queue.pop_front();                        }
    QString front()                         { return queue.front();                     }

    QString endSignal()                     { return QLatin1String("TERMINATE");        }

    void registerReaderThread(const QThread* thread);
    void unregisterReaderThread(const QThread* thread);

private:

    static SharedQueue<QString>             queue;
    static QList<const QThread*>            readers;
    static int                              ref;

    Q_DISABLE_COPY(RecognitionTrainingUpdateQueue)
};

} // namespace Digikam
