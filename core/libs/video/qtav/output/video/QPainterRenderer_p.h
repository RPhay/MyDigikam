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

#ifndef QTAV_QPAINTER_RENDERER_P_H
#define QTAV_QPAINTER_RENDERER_P_H

#include "QPainterRenderer.h"

// Qt includes

#include <QImage>
#include <QPainter>

// Local includes

#include "VideoRenderer_p.h"

namespace QtAV
{

class DIGIKAM_EXPORT QPainterRendererPrivate : public VideoRendererPrivate
{
public:

    QPainterRendererPrivate()
        : painter(nullptr)
    {
    }

    virtual ~QPainterRendererPrivate()
    {
        if (painter)
        {
            delete painter;

            painter = nullptr;
        }
    }

    void setupQuality() override
    {
        switch (quality)
        {
            case QualityFastest:
            {
                painter->setRenderHint(QPainter::Antialiasing,          false);
                painter->setRenderHint(QPainter::TextAntialiasing,      false);
                painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
                painter->setRenderHint(QPainter::Antialiasing,          false);

                break;
            }

            case QualityBest:
            default:
            {
                painter->setRenderHint(QPainter::Antialiasing,          true);
                painter->setRenderHint(QPainter::TextAntialiasing,      true);
                painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
                painter->setRenderHint(QPainter::Antialiasing,          true);

                break;
            }
        }
    }

    // drawPixmap() is faster for on screen painting

public:

    QPixmap   pixmap;
    QPainter* painter = nullptr;
};

} // namespace QtAV

#endif // QTAV_QPAINTER_RENDERER_P_H
