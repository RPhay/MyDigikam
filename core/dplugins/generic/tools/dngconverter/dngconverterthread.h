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

#ifndef DIGIKAM_DNG_CONVERTER_THREAD_H
#define DIGIKAM_DNG_CONVERTER_THREAD_H

// Qt includes

#include <QUrl>

// Local includes

#include "actionthreadbase.h"
#include "dngconverteractions.h"

using namespace Digikam;

namespace DigikamGenericDNGConverterPlugin
{

class DNGConverterActionData;

class DNGConverterActionThread : public ActionThreadBase
{
    Q_OBJECT

public:

    explicit DNGConverterActionThread(QObject* const parent);
    ~DNGConverterActionThread();

    void setBackupOriginalRawFile(bool b);
    void setCompressLossLess(bool b);
    void setUpdateFileDate(bool b);
    void setPreviewMode(int mode);

    void identifyRawFile(const QUrl& url);
    void identifyRawFiles(const QList<QUrl>& urlList);

    void processRawFile(const QUrl& url);
    void processRawFiles(const QList<QUrl>& urlList);

    void cancel();

Q_SIGNALS:

    void signalStarting(const DigikamGenericDNGConverterPlugin::DNGConverterActionData& ad);
    void signalFinished(const DigikamGenericDNGConverterPlugin::DNGConverterActionData& ad);

    /**
     * Signal to emit to sub-tasks to cancel processing.
     */
    void signalCancelDNGConverterTask();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericDNGConverterPlugin

#endif // DIGIKAM_DNG_CONVERTER_THREAD_H
