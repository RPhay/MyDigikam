/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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



#ifndef QTAV_EXAMPLE_SIMPLEFILTER_H
#define QTAV_EXAMPLE_SIMPLEFILTER_H

#include <QtAV/Filter.h>
#include <QElapsedTimer>
#include <QMatrix4x4>

namespace QtAV {

class SimpleFilter : public VideoFilter
{
    Q_OBJECT
public:
    SimpleFilter(QObject *parent = 0);
    virtual ~SimpleFilter();
    void enableRotate(bool r);
    void enableWaveEffect(bool w);
    bool isSupported(VideoFilterContext::Type ct) const {
        return ct == VideoFilterContext::QtPainter || ct == VideoFilterContext::X11;
    }
    void setText(const QString& text);
    QString text() const;
    //show image if text is null
    void setImage(const QImage& img);

    void prepare();

protected:
    virtual void process(Statistics* statistics, VideoFrame* frame);
    virtual void timerEvent(QTimerEvent *);
private:
    bool mCanRot;
    bool mWave;
    QElapsedTimer mTime;
    qreal mStartValue;
    QString mText;
    QMatrix4x4 mMat;
    QImage mImage;
};

} //namespace QtAV

#endif // QTAV_EXAMPLE_SIMPLEFILTER_H
