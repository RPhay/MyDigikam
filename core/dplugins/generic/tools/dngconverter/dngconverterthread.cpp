/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : DNG Converter threads manager
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dngconverterthread.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "drawinfo.h"
#include "drawdecoder.h"
#include "dngconvertertask.h"
#include "dmetadata.h"
#include "digikam_debug.h"

namespace DigikamGenericDNGConverterPlugin
{

class DNGConverterActionThread::Private
{
public:

    Private()
      : backupOriginalRawFile(false),
        compressLossLess     (true),
        updateFileDate       (false),
        previewMode          (DNGWriter::MEDIUM)
    {
    }

    bool       backupOriginalRawFile;
    bool       compressLossLess;
    bool       updateFileDate;
    int        previewMode;
};

DNGConverterActionThread::DNGConverterActionThread(QObject* const parent)
    : ActionThreadBase(parent),
      d               (new Private)
{
    qRegisterMetaType<DNGConverterActionData>();
}

DNGConverterActionThread::~DNGConverterActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void DNGConverterActionThread::setBackupOriginalRawFile(bool b)
{
    d->backupOriginalRawFile = b;
}

void DNGConverterActionThread::setCompressLossLess(bool b)
{
    d->compressLossLess = b;
}

void DNGConverterActionThread::setUpdateFileDate(bool b)
{
    d->updateFileDate = b;
}

void DNGConverterActionThread::setPreviewMode(int mode)
{
    d->previewMode = mode;
}

void DNGConverterActionThread::processRawFile(const QUrl& url)
{
    QList<QUrl> oneFile;
    oneFile.append(url);
    processRawFiles(oneFile);
}

void DNGConverterActionThread::identifyRawFile(const QUrl& url)
{
    QList<QUrl> oneFile;
    oneFile.append(url);
    identifyRawFiles(oneFile);
}

void DNGConverterActionThread::identifyRawFiles(const QList<QUrl>& urlList)
{
    ActionJobCollection collection;

    for (QList<QUrl>::const_iterator it = urlList.constBegin() ; it != urlList.constEnd() ; ++it)
    {
        DNGConverterTask* const t = new DNGConverterTask(this, *it, IDENTIFY);
        t->setBackupOriginalRawFile(d->backupOriginalRawFile);
        t->setCompressLossLess(d->compressLossLess);
        t->setUpdateFileDate(d->updateFileDate);
        t->setPreviewMode(d->previewMode);

        connect(t, SIGNAL(signalStarting(DigikamGenericDNGConverterPlugin::DNGConverterActionData)),
                this, SIGNAL(signalStarting(DigikamGenericDNGConverterPlugin::DNGConverterActionData)));

        connect(t, SIGNAL(signalFinished(DigikamGenericDNGConverterPlugin::DNGConverterActionData)),
                this, SIGNAL(signalFinished(DigikamGenericDNGConverterPlugin::DNGConverterActionData)));

        connect(this, SIGNAL(signalCancelDNGConverterTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        collection.insert(t, 0);
    }

    appendJobs(collection);
}

void DNGConverterActionThread::processRawFiles(const QList<QUrl>& urlList)
{
    ActionJobCollection collection;

    for (QList<QUrl>::const_iterator it = urlList.constBegin() ; it != urlList.constEnd() ; ++it)
    {
        DNGConverterTask* const t = new DNGConverterTask(this, *it, PROCESS);
        t->setBackupOriginalRawFile(d->backupOriginalRawFile);
        t->setCompressLossLess(d->compressLossLess);
        t->setUpdateFileDate(d->updateFileDate);
        t->setPreviewMode(d->previewMode);

        connect(t, SIGNAL(signalStarting(DigikamGenericDNGConverterPlugin::DNGConverterActionData)),
                this, SIGNAL(signalStarting(DigikamGenericDNGConverterPlugin::DNGConverterActionData)));

        connect(t, SIGNAL(signalFinished(DigikamGenericDNGConverterPlugin::DNGConverterActionData)),
                this, SIGNAL(signalFinished(DigikamGenericDNGConverterPlugin::DNGConverterActionData)));

        connect(this, SIGNAL(signalCancelDNGConverterTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);

        collection.insert(t, 0);
    }

    appendJobs(collection);
}

void DNGConverterActionThread::cancel()
{
    if (isRunning())
    {
        emit signalCancelDNGConverterTask();
    }

    ActionThreadBase::cancel();
}

}  // namespace DigikamGenericDNGConverterPlugin
