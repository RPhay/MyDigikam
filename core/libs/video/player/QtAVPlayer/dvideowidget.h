/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-14
 * Description : QtAVPlayer integration class.
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QMediaService>
#include <QMediaObject>
#include <QVideoRendererControl>
#include <QVideoWidget>

// QtAVPlayer includes

#include "qavplayer.h"
#include "qavvideoframe.h"
#include "qavaudiooutput.h"

namespace Digikam
{

class Q_DECL_HIDDEN DVideoWidget : public QVideoWidget
{
public:

    explicit DVideoWidget(QWidget* const parent);
    ~DVideoWidget();

    QAVPlayer* player() const;

    bool setMediaObject(QMediaObject* object) override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam
