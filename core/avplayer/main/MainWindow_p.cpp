/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "MainWindow_p.h"

namespace AVPlayer
{

MainWindow::Private::Private()
{
}

MainWindow::Private::~Private()
{
}

void MainWindow::Private::workaroundRendererSize(MainWindow* const parent)
{
    if (!pRenderer)
        return;

    QSize s = parent->rect().size();
/*
    resize(QSize(s.width() - 1, s.height() - 1));
    resize(s); // window resize to fullscreen size will create another fullScreenChange event
*/
     pRenderer->widget()->resize(QSize(s.width() + 1, s.height() + 1));
     pRenderer->widget()->resize(s);
}

void MainWindow::Private::QLabelSetElideText(QLabel* const label, const QString& text, int W)
{
    QFontMetrics metrix(label->font());
    int width = label->width() - label->indent() - label->margin();

    if (W || label->parent())
    {
        int w = W;

        if (!w)
        {
            QWidget* const p = dynamic_cast<QWidget*>(label->parent());

            if (p)
                w = p->width();
        }

        width = qMax(w - label->indent() - label->margin() - 13 * (30), 0); // TODO: why 30?
    }

    QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
    label->setText(clippedText);
}

} // namespace AVPlayer
