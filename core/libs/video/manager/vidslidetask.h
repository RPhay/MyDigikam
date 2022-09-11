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

#ifndef DIGIKAM_VIDSLIDE_TASK_H
#define DIGIKAM_VIDSLIDE_TASK_H

// Qt includes

#include <QString>

// Local includes

#include "vidslidesettings.h"
#include "actionthreadbase.h"
#include "digikam_export.h"

namespace Digikam
{

class VidSlideTask : public ActionJob
{
    Q_OBJECT

public:

    explicit VidSlideTask(VidSlideSettings* const settings);
    ~VidSlideTask();

Q_SIGNALS:

    void signalMessage(const QString&, bool);
    void signalDone(bool);

protected:

    void run() override;

private:

    // Disable
    VidSlideTask(QObject*);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_VIDSLIDE_TASK_H
