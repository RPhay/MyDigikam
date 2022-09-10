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

// Qt includes

#include <QApplication>
#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>

// Local includes

#include "VideoFrameExtractor.h"
#include "GraphicsItemRenderer.h"
#include "WidgetRenderer.h"
#include "GLWidgetRenderer2.h"
#include "VideoPreviewWidget.h"
#include "digikam_debug.h"

using namespace QtAV;

class Q_DECL_HIDDEN VideoFrameObserver : public QObject
{
    Q_OBJECT

public:

    VideoFrameObserver(QObject* const parent = nullptr)
      : QObject  (parent),
        pos      (0),
        nb       (1),
        extracted(0)
    {
        view = VideoRenderer::create(VideoRendererId_GLWidget2);
        view->widget()->resize(400, 300);
        view->widget()->show();

        connect(&extractor, SIGNAL(frameExtracted(QtAV::VideoFrame)),
                this, SLOT(onVideoFrameExtracted(QtAV::VideoFrame)));
    }

    void setParameters(qint64 msec, int count)
    {
        pos = msec;
        nb  = count;
    }

    void start(const QString& file)
    {
        extractor.setAsync(false);
        extractor.setSource(file);
        startTimer(20);
        timer.start();
        extractor.setPosition(pos);
    }

    void startAsync(const QString& file)
    {
        extractor.setAsync(true);
        extractor.setSource(file);
        startTimer(20);
        timer.start();
        extractor.setPosition(pos);
    }

public Q_SLOTS:

    void onVideoFrameExtracted(const QtAV::VideoFrame& frame)
    {
        view->receive(frame);
        qApp->processEvents();
        frame.toImage().save(QString::fromLatin1("%1.png").arg(frame.timestamp()));

        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("frame %dx%d @%f",
                frame.width(), frame.height(), frame.timestamp());

        if (++extracted >= nb)
        {
            qCDebug(DIGIKAM_TESTS_LOG).noquote()
                << QString::asprintf("elapsed: %lld.",
                    timer.elapsed());

            return;
        }

        extractor.setPosition(pos + extracted * 1000);
    }

protected:

    void timerEvent(QTimerEvent*)
    {
        qApp->processEvents(); // avoid ui blocking if async is not used
    }

private:

    VideoRenderer*      view;
    qint64              pos;
    int                 nb;
    int                 extracted;
    VideoFrameExtractor extractor;
    QElapsedTimer       timer;
};

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    int idx = a.arguments().indexOf(QLatin1String("-f"));

    if (idx < 0)
    {
        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("-f file -t sec -n count -asyc");

        return -1;
    }

    QString file = a.arguments().at(idx + 1);
    idx          = a.arguments().indexOf(QLatin1String("-t"));
    int t        = 0;

    if (idx > 0)
        t = a.arguments().at(idx + 1).toInt();

    int n = 1;
    idx   = a.arguments().indexOf(QLatin1String("-n"));

    if (idx > 0)
        n = a.arguments().at(idx + 1).toInt();

    bool async = a.arguments().contains(QString::fromLatin1("-async"));

    VideoFrameObserver obs;
    obs.setParameters(t * 1000, n);

    if (async)
        obs.startAsync(file);
    else
        obs.start(file);

    return a.exec();
}

#include "extract.moc"
