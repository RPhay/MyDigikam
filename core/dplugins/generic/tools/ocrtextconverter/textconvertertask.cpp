/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : Text Converter thread
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "textconvertertask.h"

// Local includes

#include "digikam_debug.h"
#include "ocrtesseractengine.h"
#include "ocroptions.h"
#include "drawdecoder.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterTask::Private

{
public:

    Private()
      : language(int(OcrOptions::Languages::DEFAULT)),
        psm(int(OcrOptions::PageSegmentationModes::DEFAULT)),
        oem(int(OcrOptions::EngineModes::DEFAULT)),
        dpi(300),
        isSaveTextFile(true),
        isSaveXMP(true),
        cancel(false),
        action(TextConverterAction())
    {
    }

    int                  language;
    int                  psm;
    int                  oem;
    int                  dpi;
    bool                 isSaveTextFile;
    bool                 isSaveXMP;

    bool                 cancel;

    QUrl                 url;
    TextConverterAction  action;

    OcrTesseracrEngine   ocrEngine;
};

TextConverterTask::TextConverterTask(QObject* const parent,
                                     const QUrl& fileUrl,
                                     const TextConverterAction& action)
    : ActionJob(parent),
      d        (new Private)
{
    d->url    = fileUrl;
    d->action = action;
}

TextConverterTask::~TextConverterTask()
{
    slotCancel();
    delete d;
}


void TextConverterTask::setLanguagesMode(int mode)
{
    d->language = mode;
}

void TextConverterTask::setPSMMode(int mode)
{
    d->psm = mode;
}

void TextConverterTask::setOEMMode(int mode)
{
    d->oem = mode;
}

void TextConverterTask::setDpi(int value)
{
    d->dpi = value;
}

void TextConverterTask::setIsSaveTextFile(bool check)
{
    d->isSaveTextFile = check;
}

void TextConverterTask::setIsSaveXMP(bool check)
{
    d->isSaveXMP = check;
}

void TextConverterTask::run()
{

    if (d->cancel)
    {
        return;
    }

    switch (d->action)
    {
        case PROCESS:
        {
            TextConverterActionData ad1;
            ad1.action    = PROCESS;
            ad1.fileUrl   = d->url;
            ad1.starting  = true;
            Q_EMIT signalStarting(ad1);

            d->ocrEngine.setInputFile(d->url.toLocalFile());
            d->ocrEngine.setIsSaveTextFile(d->isSaveTextFile);
            d->ocrEngine.setIsSaveXMP(d->isSaveXMP);
            d->ocrEngine.setLanguagesMode(d->language);
            d->ocrEngine.setPSMMode(d->psm);
            d->ocrEngine.setOEMMode(d->oem);
            d->ocrEngine.setDpi(d->dpi);
            int ret = d->ocrEngine.runOcrProcess();

            TextConverterActionData ad2;
            ad2.action     = PROCESS;
            ad2.fileUrl    = d->url;
            ad2.destPath   = d->ocrEngine.outputFile();
            ad2.result     = ret;
            ad2.outputText = d->ocrEngine.outputText();

            Q_EMIT signalFinished(ad2);
            break;
        }

        default:
        {
            qCritical(DIGIKAM_GENERAL_LOG) << "Unknown action specified";
            break;
        }
    }

    Q_EMIT signalDone();

}

void TextConverterTask::slotCancel()
{
    d->cancel = true;
}


} // namespace DigikamGenericTextConverterPlugin
