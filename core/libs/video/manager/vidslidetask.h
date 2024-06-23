/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow frames from images.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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

    VidSlideSettings* m_settings = nullptr;
};

} // namespace Digikam
