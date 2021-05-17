/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool process stream parser.
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "exiftoolparser_p.h"

namespace Digikam
{

ExifToolParser::ExifToolParser(QObject* const parent)
    : QObject(parent),
      d      (new Private)
{
    // Create ExifTool parser instance.

    d->proc = new ExifToolProcess(this);

    for (int i = ExifToolProcess::LOAD_METADATA ; i < ExifToolProcess::NO_ACTION ; ++i)
    {
        d->evLoops << new QEventLoop(this);
    }

    connect(MetaEngineSettings::instance(), SIGNAL(signalSettingsChanged()),
            this, SLOT(slotMetaEngineSettingsChanged()));

    d->hdls << connect(d->proc, &ExifToolProcess::signalCmdCompleted,
                       this, &ExifToolParser::slotCmdCompleted);

    d->hdls << connect(d->proc, &ExifToolProcess::signalErrorOccurred,
                       this, &ExifToolParser::slotErrorOccurred);

    d->hdls << connect(d->proc, &ExifToolProcess::signalFinished,
                       this, &ExifToolParser::slotFinished);

    slotMetaEngineSettingsChanged();
}

ExifToolParser::~ExifToolParser()
{
    for (int i = ExifToolProcess::LOAD_METADATA ; i < ExifToolProcess::NO_ACTION ; ++i)
    {
        if (d->evLoops[i])
        {
            d->evLoops[i]->exit();
            delete d->evLoops[i];
        }
    }

    foreach (QMetaObject::Connection hdl, d->hdls)
    {
        disconnect(hdl);
    }

    delete d;
}

void ExifToolParser::setExifToolProgram(const QString& path)
{
    d->proc->setProgram(path);
}

QString ExifToolParser::currentPath() const
{
    return d->currentPath;
}

ExifToolParser::ExifToolData ExifToolParser::currentData() const
{
    return d->exifToolData;
}

QString ExifToolParser::currentErrorString() const
{
    return d->proc->errorString();
}

void ExifToolParser::slotMetaEngineSettingsChanged()
{
    d->proc->setProgram(MetaEngineSettings::instance()->settings().exifToolPath);

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool path:" << d->proc->program();
}

bool ExifToolParser::exifToolAvailable() const
{
    bool ret = d->proc->checkExifToolProgram();

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Check ExifTool availability:" << ret;

    return ret;
}

} // namespace Digikam
