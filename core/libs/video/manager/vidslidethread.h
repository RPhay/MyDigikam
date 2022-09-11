/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_VIDSLIDE_THREAD_H
#define DIGIKAM_VIDSLIDE_THREAD_H

// Local includes

#include "vidslidesettings.h"
#include "vidslidetask.h"
#include "actionthreadbase.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT VidSlideThread : public ActionThreadBase
{
    Q_OBJECT

public:

    explicit VidSlideThread(QObject* const parent);
    ~VidSlideThread() override;

    void processStream(VidSlideSettings* const settings);

Q_SIGNALS:

    void signalProgress(int);
    void signalDone(bool);
    void signalMessage(const QString&, bool);
};

} // namespace Digikam

#endif // DIGIKAM_VIDSLIDE_THREAD_H
