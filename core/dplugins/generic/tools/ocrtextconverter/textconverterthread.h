/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : Text Converter threads manager
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_TEXT_CONVERTER_THREAD_H
#define DIGIKAM_TEXT_CONVERTER_THREAD_H

// Qt includes

#include <QUrl>

// Local includes

#include "actionthreadbase.h"
#include "textconverteraction.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterActionThread : public ActionThreadBase
{
    Q_OBJECT

public:

    explicit TextConverterActionThread(QObject* const parent);
    ~TextConverterActionThread();

    void setLanguagesMode(int mode);
    void setPSMMode(int mode);
    void setOEMMode(int mode);
    void setDpi(int value);
    void setIsSaveTextFile(bool check);
    void setIsSaveXMP(bool check);

    void ocrProcessFile(const QUrl& url);
    void ocrProcessFiles(const QList<QUrl>& urlList);

    void cancel();

Q_SIGNALS:

    void signalStarting(const DigikamGenericTextConverterPlugin::TextConverterActionData& ad);
    void signalFinished(const DigikamGenericTextConverterPlugin::TextConverterActionData& ad);

    /**
     * Signal to emit to sub-tasks to cancel processing.
     */
    void signalCancelTextConverterTask();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_TEXT_CONVERTER_THREAD_H
