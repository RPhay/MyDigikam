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

#include "SimpleFilter.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QWidget>
#include <QTime>
#include <QRandomGenerator>

SimpleFilter::SimpleFilter(QObject* const parent)
    : VideoFilter(parent),
      mCanRot    (true),
      mWave      (true)
{
    srand(QTime::currentTime().msec());                      // krazy:exclude=syscalls
    mStartValue = (qreal)(rand() % 1000) / qreal(1000.0);    // krazy:exclude=syscalls
    mTime.start();
    startTimer(100);
}

SimpleFilter::~SimpleFilter()
{
}

void SimpleFilter::enableRotate(bool r)
{
    mCanRot = r;
}

void SimpleFilter::enableWaveEffect(bool w)
{
    mWave = w;
}

void SimpleFilter::setText(const QString& text)
{
    mText = text;
}

QString SimpleFilter::text() const
{
    return mText;
}

void SimpleFilter::setImage(const QImage& img)
{
    mImage = img;
}

void SimpleFilter::prepare()
{
    VideoFilterContext* const ctx = static_cast<VideoFilterContext*>(context());

    if      (!mText.isEmpty())
    {
        ctx->font.setPixelSize(32);
        ctx->font.setBold(true);

        if (!mCanRot)
            return;

        mMat.translate(ctx->rect.center().x(), 0, 0);
    }
    else if (!mImage.isNull())
    {
        if (!mCanRot)
            return;

        mMat.translate(ctx->rect.x() + mImage.width() / 2, 0, 0);
    }

    if (mCanRot)
    {
        mMat.rotate(mStartValue * 360, 0, 1, -0.1);
    }
}

void SimpleFilter::timerEvent(QTimerEvent*)
{
    if (qobject_cast<QWidget*>(parent()))
        ((QWidget*)parent())->update();
}

void SimpleFilter::process(Statistics* statistics, VideoFrame* frame)
{
    Q_UNUSED(statistics);
    Q_UNUSED(frame);

    if (!isEnabled())
        return;

    int t                         = mTime.elapsed() / 100;
    VideoFilterContext* const ctx = static_cast<VideoFilterContext*>(context());

    if (mCanRot)
    {
        mMat.rotate(2, 0, 1, -0.1);
        ctx->transform = mMat.toTransform();
    }

    if (mText.isEmpty())
    {
        if (mImage.isNull())
            return;

        if (mCanRot)
        {
            ctx->drawImage(QPointF(-mImage.width() / 2, ctx->rect.y()),
                           mImage,
                           QRectF(0, 0, mImage.width(), mImage.height()));
        }
        else
        {
            ctx->drawImage(ctx->rect.topLeft(),
                           mImage,
                           QRectF(0, 0, mImage.width(), mImage.height()));
        }

        return;
    }

    if (mWave)
    {
        static const int sin_tbl[16] =
        {
             0, 38, 71, 92, 100, 92, 71, 38, 0, -38, -71, -92, -100, -92, -71, -38
        };

        QFontMetrics fm(ctx->font);
        int h = fm.height();
        int x = ctx->rect.x();
        int y = ctx->rect.y() + h + fm.descent();

        for (int i = 0 ; i < mText.size() ; ++i)
        {
            int i16 = (t + i) & 15;
            ctx->pen.setColor(QColor::fromHsv((15-i16)*16, 255, 255));

            if (mCanRot)
                ctx->drawPlainText(QPointF(x - fm.boundingRect(mText).width() / 2 - ctx->rect.x(),
                                           y - sin_tbl[i16] * h / 400), mText.mid(i, 1));
            else
                ctx->drawPlainText(QPointF(x, y - sin_tbl[i16] * h / 400), mText.mid(i, 1));

            x += fm.boundingRect(mText[i]).width();
        }
    }
    else
    {
        qreal c = fabs(sin((float)t));
        c      += mStartValue;
        c      /= 2.0;
        QLinearGradient g(0, 0, 100, 32);
        g.setSpread(QGradient::ReflectSpread);
        g.setColorAt(0, QColor::fromHsvF(c, 1, 1, 1));
        g.setColorAt(1, QColor::fromHsvF((c > 0.5) ? c - 0.5 : c + 0.5, 1, 1, 1));
        ctx->pen.setBrush(QBrush(g));
        ctx->drawRichText(ctx->rect, mText);

        return;
/*
        if (mCanRot)
        {
            QFontMetrics fm(ctx->font);
            ctx->drawPlainText(QRectF(-fm.boundingRect(mText).width() / 2,
                                      ctx->rect.y(), ctx->rect.width(), ctx->rect.height()),
                               Qt::TextWordWrap, mText);
        }
        else
        {
            ctx->drawPlainText(ctx->rect, Qt::TextWordWrap, mText);
        }
*/
    }
}
