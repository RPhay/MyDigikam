/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "MainWindow_p.h"

namespace AVPlayer
{

MainWindow::MainWindow(QWidget* const parent)
    : QWidget(parent),
      d      (new Private())
{

#if defined(Q_OS_MACX) && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    QApplication::setStyle(QStyleFactory::create(QLatin1String("Fusion")));

#endif

    d->pOSD              = new OSDFilter(this);
    d->pSubtitle         = new SubtitleFilter(this);

    setMouseTracking(true); // mouseMoveEvent without press.

    connect(this, SIGNAL(ready()),
            this, SLOT(processPendingActions()));
/*
    QTimer::singleShot(10, this, SLOT(setupUi()));
*/
    setupUi();
/*
    setToolTip(i18n("Click black area to use shortcut (see right click menu)"));
*/
    WindowEventFilter* const we = new WindowEventFilter(this);
    installEventFilter(we);

    connect(we, SIGNAL(fullscreenChanged()),
            this, SLOT(handleFullscreenChange()));
}

MainWindow::~MainWindow()
{
    if (d->preview)
    {
        d->preview->close();
        delete d->preview;
    }

    d->pHistory->save();
    d->pPlayList->save();

    if (d->pVolumeSlider && !d->pVolumeSlider->parentWidget())
    {
        d->pVolumeSlider->close();
        delete d->pVolumeSlider;
        d->pVolumeSlider = nullptr;
    }

    if (d->pStatisticsView)
    {
        delete d->pStatisticsView;
        d->pStatisticsView = nullptr;
    }

    delete d;
}

} // namespace AVPlayer
