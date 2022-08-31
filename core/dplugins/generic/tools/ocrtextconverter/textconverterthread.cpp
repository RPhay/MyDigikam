/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : Text Converter threads manager
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
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

#include "textconverterthread.h"

// Local includes

#include "textconvertertask.h"
#include "ocroptions.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterActionThread::Private
{
public:

    Private()
      : language(int(OcrOptions::Languages::DEFAULT)),
        psm(int(OcrOptions::PageSegmentationModes::DEFAULT)),
        oem(int(OcrOptions::EngineModes::DEFAULT)),
        dpi(300),
        isSaveTextFile(true),
        isSaveXMP(true)
    {
    }

    int  language;
    int  psm;
    int  oem;
    int  dpi;
    bool isSaveTextFile;
    bool isSaveXMP;
};

TextConverterActionThread::TextConverterActionThread(QObject* const parent)
    : ActionThreadBase(parent),
      d               (new Private)
{
    qRegisterMetaType<TextConverterActionData>();
}

TextConverterActionThread::~TextConverterActionThread()
{
    // cancel the thread

    cancel();

    // wait for the thread to finish

    wait();

    delete d;
}


void TextConverterActionThread::setLanguagesMode(int mode)
{
    d->language = mode;
}


void TextConverterActionThread::setPSMMode(int mode)
{
    d->psm = mode;
}

void TextConverterActionThread::setOEMMode(int mode)
{
    d->oem = mode;
}

void TextConverterActionThread::setDpi(int value)
{
    d->dpi = value;
}

void TextConverterActionThread::setIsSaveTextFile(bool check)
{
    d->isSaveTextFile = check;
}

void TextConverterActionThread::setIsSaveXMP(bool check)
{
    d->isSaveXMP = check;
}

void TextConverterActionThread::ocrProcessFile(const QUrl& url)
{
    QList<QUrl> oneFile;
    oneFile.append(url);
    ocrProcessFiles(oneFile);
}


void TextConverterActionThread::ocrProcessFiles(const QList<QUrl>& urlList)
{
    ActionJobCollection collection;

    for (QList<QUrl>::const_iterator it = urlList.constBegin() ; it != urlList.constEnd() ; ++it)
    {
        TextConverterTask* const t = new TextConverterTask(this, *it, PROCESS);
        t->setLanguagesMode(d->language);
        t->setPSMMode(d->psm);
        t->setOEMMode(d->oem);
        t->setDpi(d->dpi);
        t->setIsSaveTextFile(d->isSaveTextFile);
        t->setIsSaveXMP(d->isSaveXMP);

        connect(t, SIGNAL(signalStarting(DigikamGenericTextConverterPlugin::TextConverterActionData)),
                this, SIGNAL(signalStarting(DigikamGenericTextConverterPlugin::TextConverterActionData)));

        connect(t, SIGNAL(signalFinished(DigikamGenericTextConverterPlugin::TextConverterActionData)),
                this, SIGNAL(signalFinished(DigikamGenericTextConverterPlugin::TextConverterActionData)));

        connect(this, SIGNAL(signalCancelTextConverterTask()),
                t, SLOT(slotCancel()), Qt::QueuedConnection);


        collection.insert(t, 0);
    }

    appendJobs(collection);
}

void TextConverterActionThread::cancel()
{
    if (isRunning())
    {
        Q_EMIT signalCancelTextConverterTask();
    }

    ActionThreadBase::cancel();
}

}  // namespace DigikamGenericTextConverterPlugin
