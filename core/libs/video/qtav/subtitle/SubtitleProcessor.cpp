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

#include "SubtitleProcessor.h"

// Qt includes

#include <QFile>

// Local includes

#include "QtAV_factory.h"
#include "digikam_debug.h"

namespace QtAV
{

FACTORY_DEFINE(SubtitleProcessor)       // clazy:exclude=function-args-by-ref

// can not declare in class member

extern bool RegisterSubtitleProcessorFFmpeg_Man();
extern bool RegisterSubtitleProcessorLibASS_Man();

void SubtitleProcessor::registerAll()
{
    static bool done = false;

    if (done)
        return;

    done = true;
    RegisterSubtitleProcessorFFmpeg_Man();

#if QTAV_HAVE(LIBASS)

    RegisterSubtitleProcessorLibASS_Man();

#endif

}

SubtitleProcessor::SubtitleProcessor()
    : m_width (0),
      m_height(0)
{
}

bool SubtitleProcessor::process(const QString& path)
{
    QFile f(path);

    if (!f.open(QIODevice::ReadOnly))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << "open subtitle file error: "
                                         << f.errorString();

        return false;
    }

    bool ok = process(&f);
    f.close();

    return ok;
}

QImage SubtitleProcessor::getImage(qreal pts, QRect* boundingRect)
{
    Q_UNUSED(pts)
    Q_UNUSED(boundingRect)

    return QImage();
}

SubImageSet SubtitleProcessor::getSubImages(qreal pts, QRect* boundingRect)
{
    Q_UNUSED(pts);
    Q_UNUSED(boundingRect);

    return SubImageSet();
}

void SubtitleProcessor::setFrameSize(int width, int height)
{
    if ((width == m_width) && (height == m_height))
        return;

    m_width  = width;
    m_height = height;
    onFrameSizeChanged(m_width, m_height);
}

QSize SubtitleProcessor::frameSize() const
{
    return QSize(m_width, m_height);
}

int SubtitleProcessor::frameWidth() const
{
    return m_width;
}

int SubtitleProcessor::frameHeight() const
{
    return m_height;
}

void SubtitleProcessor::onFrameSizeChanged(int width, int height)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
}

} // namespace QtAV
