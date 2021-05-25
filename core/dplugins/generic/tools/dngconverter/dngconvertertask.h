/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2012-12-24
 * Description : DNG Converter thread
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#ifndef DIGIKAM_DNG_CONVERTER_TASK_H
#define DIGIKAM_DNG_CONVERTER_TASK_H

// Qt includes

#include <QUrl>

// Local includes

#include "actionthreadbase.h"
#include "dngconverteractions.h"

using namespace Digikam;

namespace DigikamGenericDNGConverterPlugin
{

class DNGConverterTask : public ActionJob
{
    Q_OBJECT

public:

    explicit DNGConverterTask(QObject* const parent, const QUrl& url, const DNGConverterAction& action);
    ~DNGConverterTask();

    void setBackupOriginalRawFile(bool b);
    void setCompressLossLess(bool b);
    void setUpdateFileDate(bool b);
    void setPreviewMode(int mode);

Q_SIGNALS:

    void signalStarting(const DigikamGenericDNGConverterPlugin::DNGConverterActionData& ad);
    void signalFinished(const DigikamGenericDNGConverterPlugin::DNGConverterActionData& ad);

public Q_SLOTS:

    void slotCancel();

protected:

    void run();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericDNGConverterPlugin

#endif // DIGIKAM_DNG_CONVERTER_TASK_H

