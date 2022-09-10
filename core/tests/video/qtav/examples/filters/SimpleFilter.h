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

#ifndef QTAV_EXAMPLE_SIMPLE_FILTER_H
#define QTAV_EXAMPLE_SIMPLE_FILTER_H

// Qt includes

#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QImage>
#include <QString>

// Local includes

#include "Filter.h"
#include "VideoFrame.h"
#include "QtAV_Statistics.h"

using namespace QtAV;

class SimpleFilter : public VideoFilter
{
    Q_OBJECT

public:

    explicit SimpleFilter(QObject* const parent = nullptr);
    virtual ~SimpleFilter();

    void enableRotate(bool r);
    void enableWaveEffect(bool w);

    bool isSupported(VideoFilterContext::Type ct) const override
    {
        return ((ct == VideoFilterContext::QtPainter) ||
                (ct == VideoFilterContext::X11));
    }

    void setText(const QString& text);
    QString text() const;

    // show image if text is null

    void setImage(const QImage& img);

    void prepare();

protected:

    virtual void process(Statistics* statistics, VideoFrame* frame) override;
    virtual void timerEvent(QTimerEvent*);

private:

    bool            mCanRot;
    bool            mWave;
    QElapsedTimer   mTime;
    qreal           mStartValue;
    QString         mText;
    QMatrix4x4      mMat;
    QImage          mImage;
};

#endif // QTAV_EXAMPLE_SIMPLE_FILTER_H
