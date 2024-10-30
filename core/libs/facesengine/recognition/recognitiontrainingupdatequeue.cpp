/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2024-10-28
 * Description : Threadsafe queue for submitting face training removal requests
 *
 * SPDX-FileCopyrightText: 2024      by Michael Miller <micahel underscore miller at msn dot com>
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */


#include "recognitiontrainingupdatequeue.h"

namespace Digikam
{

SharedQueue<QString> RecognitionTrainingUpdateQueue::queue;
QList<const QThread*> RecognitionTrainingUpdateQueue::readers;
int RecognitionTrainingUpdateQueue::ref = 0;

RecognitionTrainingUpdateQueue::RecognitionTrainingUpdateQueue()
{
    ++ref;
}

RecognitionTrainingUpdateQueue::~RecognitionTrainingUpdateQueue()
{
    --ref;
    if (0 == ref)
    {
        queue.push_back(endSignal());
    }
}

void RecognitionTrainingUpdateQueue::registerReaderThread(const QThread* thread)
{
    readers.append(thread);
}

void RecognitionTrainingUpdateQueue::unregisterReaderThread(const QThread* thread)
{
    if (readers.contains(thread))
    {
        readers.removeAt(readers.contains(thread));
    }
}

} // namespace Digikam
