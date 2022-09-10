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

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QStringList>
#include <QTime>

// Local includes

#include "Subtitle.h"
#include "digikam_debug.h"

using namespace QtAV;

class Q_DECL_HIDDEN SubtitleObserver : public QObject
{
    Q_OBJECT

public:

    SubtitleObserver(QObject* const parent = nullptr)
      : QObject(parent)
    {
    }

    void observe(Subtitle* const sub)
    {
        connect(sub, SIGNAL(contentChanged()),
                this, SLOT(onSubtitleChanged()));
    }

private Q_SLOTS:

    void onSubtitleChanged()
    {
        Subtitle* const sub = qobject_cast<Subtitle*>(sender());
        qCDebug(DIGIKAM_TESTS_LOG) << "subtitle changed at "
                                   << sub->timestamp() << "s\n"
                                   << sub->getText();
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    qCDebug(DIGIKAM_TESTS_LOG) << "help: ./subtitle [-engine engine] [-f file] [-fuzzy] [-t sec] [-t1 sec] [-count n]";
    qCDebug(DIGIKAM_TESTS_LOG) << "-fuzzy: fuzzy match subtitle name";
    qCDebug(DIGIKAM_TESTS_LOG) << "-t: set subtitle begin time";
    qCDebug(DIGIKAM_TESTS_LOG) << "-t1: set subtitle end time";
    qCDebug(DIGIKAM_TESTS_LOG) << "-count: set subtitle frame count from t to t1";
    qCDebug(DIGIKAM_TESTS_LOG) << "-engine: subtitle processing engine, can be 'ffmpeg' and 'libass'";
    qCDebug(DIGIKAM_TESTS_LOG) << "-dir: add subtitle search directories";

    QString file;
    bool fuzzy = false;
    int t      = -1;
    int t1     = -1;
    int count  = 1;
    int i      = a.arguments().indexOf(QLatin1String("-f"));

    if (i > 0)
    {
        file = a.arguments().at(i+1);
    }

    i = a.arguments().indexOf(QLatin1String("-fuzzy"));

    if (i > 0)
        fuzzy = true;

    i = a.arguments().indexOf(QLatin1String("-t"));

    if (i > 0)
        t = a.arguments().at(i+1).toInt();

    i = a.arguments().indexOf(QLatin1String("-t1"));

    if (i > 0)
        t1 = a.arguments().at(i+1).toInt();

    i = a.arguments().indexOf(QLatin1String("-count"));

    if (i > 0)
        count = a.arguments().at(i+1).toInt();

    QString engine;
    i = a.arguments().indexOf(QLatin1String("-engine"));

    if (i > 0)
        engine = a.arguments().at(i+1);

    QStringList dirs;
    i = a.arguments().indexOf(QLatin1String("-dir"));

    while (i > 0)
    {
        dirs += a.arguments().at(i+1).split(QLatin1Char(';'));
        i = a.arguments().indexOf(QLatin1String("-dir"), i+2);
    }

    Subtitle sub;

    if (!engine.isEmpty())
        sub.setEngines(QStringList() << engine);

    qCDebug(DIGIKAM_TESTS_LOG) << "supported extensions: "
                               << sub.supportedSuffixes();

    if (file.isEmpty())
        return 0;

    sub.setFileName(file);
    sub.setDirs(dirs);
    sub.setFuzzyMatch(fuzzy);
    SubtitleObserver sob;
    sob.observe(&sub);
    QElapsedTimer timer;
    timer.start();
    sub.load();

    if (!sub.isLoaded())
        return -1;

    qCDebug(DIGIKAM_TESTS_LOG) << "process subtitle file elapsed:"
                               << timer.elapsed() << "ms";

    timer.restart();

    if ((t < 0) && (t1 >= 0))
    {
        t     = 0;
        count = 1;
    }

    if (t >= 0)
    {
        if (t1 <= t)
        {
            sub.setTimestamp(qreal(t));

            qCDebug(DIGIKAM_TESTS_LOG) << sub.timestamp()
                                       << "s:" << sub.getText();

            QImage img(sub.getImage(720, 400));

            img.save(QString::fromLatin1("sub-%1.png")
                     .arg(sub.timestamp(), 0, 'f', 2));
        }
        else
        {
            if (count < 2)
                count = 2;

            const qreal kInterval = (t1 - t) / qreal(count - 1);

            for (int n = 0 ; n < count ; ++n)
            {
                sub.setTimestamp(qreal(t) + qreal(n) * kInterval);

                qCDebug(DIGIKAM_TESTS_LOG) << sub.timestamp()
                                           << "s:" << sub.getText();

                QImage img(sub.getImage(720, 400));

                img.save(QString::fromLatin1("sub-%1.png")
                         .arg(sub.timestamp(), 0, 'f', 2));
            }
        }
    }

    qCDebug(DIGIKAM_TESTS_LOG) << "find subtitle content elapsed:"
                               << timer.elapsed() << "ms";

    return 0;
}

#include "subtitle.moc"
