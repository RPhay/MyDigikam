/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "vidslidetask.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QDir>
#include <QImage>
#include <QSize>
#include <QPainter>
#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "frameutils.h"
#include "dfileoperations.h"
#include "transitionmngr.h"
#include "effectmngr.h"

namespace Digikam
{

class Q_DECL_HIDDEN VidSlideTask::Private
{
public:

    Private() = default;

public:

    VidSlideSettings*           settings = nullptr;
    QList<QUrl>::const_iterator curAudioFile;
};

VidSlideTask::VidSlideTask(VidSlideSettings* const settings)
    : ActionJob(),
      d        (new Private)
{
    d->settings = settings;

    if (d->settings->inputAudio.isEmpty())
    {
        d->curAudioFile = d->settings->inputAudio.constEnd();
    }
}

VidSlideTask::~VidSlideTask()
{
    cancel();
    delete d;
}

void VidSlideTask::run()
{
    int frameId = 1;

    // ---------------------------------------------
    // Setup output video file

    QUrl dest       = QUrl::fromLocalFile(d->settings->outputDir);
    dest            = dest.adjusted(QUrl::StripTrailingSlash);
    dest.setPath(dest.path() + QLatin1String("/videoslideshow.") + d->settings->videoFormat());
    QString outFile = dest.toLocalFile();
    QFileInfo fi(outFile);

    if (fi.exists() && (d->settings->conflictRule != FileSaveConflictBox::OVERWRITE))
    {
        outFile = DFileOperations::getUniqueFileUrl(dest).toLocalFile();
    }

    QImage qiimg;
    QSize osize = d->settings->videoSize();

    // --------------------------------------------------------------
    // Loop to encode frames with images list as temporary JPEG files

    TransitionMngr transmngr;
    transmngr.setOutputSize(osize);

    EffectMngr effmngr;
    effmngr.setOutputSize(osize);
    effmngr.setFrames(d->settings->imgFrames);

    for (int i = 0 ; ((i < d->settings->inputImages.count() + 1) && !m_cancel) ; ++i)
    {
        if (i == 0)
        {
            qiimg = FrameUtils::makeFramedImage(QString(), osize);
        }

        QString ofile;

        if (i < d->settings->inputImages.count())
        {
            ofile = d->settings->inputImages[i].toLocalFile();
        }

        QImage qoimg = FrameUtils::makeFramedImage(ofile, osize);

        // -- Transition encoding ----------

        transmngr.setInImage(qiimg);
        transmngr.setOutImage(qoimg);
        transmngr.setTransition(d->settings->transition);

        int ttmout = 0;
        QImage frame;

        do
        {
            frame = QImage(transmngr.currentFrame(ttmout));

            if (!frame.save(dest.path() + QDir::separator() + QString::fromLatin1(".frame_%1").arg(frameId), "JPEG"))
            {
                qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot encode transition frame" << frameId;
            }

            ++frameId;
        }
        while ((ttmout != -1) && !m_cancel);

        // -- Images encoding ----------

        if (i < d->settings->inputImages.count())
        {
            int count  = 0;
            int itmout = 0;
            effmngr.setImage(qoimg);
            effmngr.setEffect(d->settings->vEffect);

            do
            {
                qiimg = effmngr.currentFrame(itmout);
                frame = qiimg;

                if (!frame.save(dest.path() + QDir::separator() + QString::fromLatin1(".frame_%1").arg(frameId), "JPEG"))
                {
                    qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot encode transition frame" << frameId;
                }

                ++frameId;
                ++count;
            }
            while ((count < d->settings->imgFrames) && !m_cancel);
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "Encoded image" << i << "done";

        Q_EMIT signalMessage(i18n("Encoding %1 Done", ofile), false);
        Q_EMIT signalProgress(i);
    }

    if (!m_cancel)
    {
        Q_EMIT signalMessage(i18n("Output video is %1", outFile), false);
        d->settings->outputVideo = outFile;
    }

    Q_EMIT signalDone(!m_cancel);
}

} // namespace Digikam
