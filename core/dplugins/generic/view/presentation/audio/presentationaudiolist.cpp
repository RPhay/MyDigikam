/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a presentation tool.
 *
 * SPDX-FileCopyrightText: 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "presentationaudiolist.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QWidget>
#include <QEvent>
#include <QFileInfo>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QIcon>
#include <QBrush>
#include <QMimeData>
#include <QUrl>
#include <QMessageBox>
#include <QApplication>

// QtAV includes

#include "AVPlayerCore.h"
#include "QtAV_Statistics.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

using namespace QtAV;

namespace DigikamGenericPresentationPlugin
{

class Q_DECL_HIDDEN PresentationAudioListItem::Private
{

public:

    explicit Private()
        : mediaObject(nullptr)
    {
    }

    QUrl          url;
    QString       artist;
    QString       title;
    QTime         totalTime;
    AVPlayerCore* mediaObject;
};

PresentationAudioListItem::PresentationAudioListItem(QListWidget* const parent, const QUrl& url)
    : QListWidgetItem(parent),
      d              (new Private)
{
    d->url = url;
    setIcon(QIcon::fromTheme(QLatin1String("audio-x-generic")).pixmap(48, QIcon::Disabled));

    d->totalTime   = QTime(0, 0, 0);
    d->mediaObject = new AVPlayerCore(this);

    connect(d->mediaObject, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),
            this, SLOT(slotMediaStateChanged(QtAV::MediaStatus)));

    connect(d->mediaObject, SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->mediaObject, SIGNAL(error(QtAV::AVError)),
            this, SLOT(slotPlayerError(QtAV::AVError)));

    d->mediaObject->setFile(url.toLocalFile());
    d->mediaObject->load();
}

PresentationAudioListItem::~PresentationAudioListItem()
{
    delete d;
}

QUrl PresentationAudioListItem::url() const
{
    return d->url;
}

void PresentationAudioListItem::setName(const QString& text)
{
    setText(text);
}

QString PresentationAudioListItem::artist() const
{
    return d->artist;
}

QString PresentationAudioListItem::title() const
{
    return d->title;
}

QTime PresentationAudioListItem::totalTime() const
{
    return d->totalTime;
}

void PresentationAudioListItem::slotPlayerError(const QtAV::AVError& err)
{
    if (err.error() != AVError::NoError)
    {
        qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "An error as occurred while playing (" << err.string() << ")";
        showErrorDialog(err.string());
    }
}

void PresentationAudioListItem::slotMediaStateChanged(QtAV::MediaStatus status)
{
    if (status == QtAV::InvalidMedia)
    {
        showErrorDialog(i18n("No detail available"));
    }
}

void PresentationAudioListItem::slotDurationChanged(qint64 duration)
{
    int hours    = (int)(duration  / (long int)(60 * 60 * 1000));
    int mins     = (int)((duration / (long int)(60 * 1000 )) - (long int)(hours * 60));
    int secs     = (int)((duration / (long int)1000) - (long int)(hours * 60 * 60) - (long int)(mins * 60));
    d->totalTime = QTime(hours, mins, secs);

    QHash<QString, QString> meta = d->mediaObject->statistics().metadata;
    d->artist    = meta.value(QLatin1String("artist"));
    d->title     = meta.value(QLatin1String("title"));

    if ( d->artist.isEmpty() && d->title.isEmpty() )
    {
        setText(d->url.fileName());
    }
    else
    {
        setText(i18nc("artist - title", "%1 - %2", artist(), title()));
    }

    Q_EMIT signalTotalTimeReady(d->url, d->totalTime);
}

void PresentationAudioListItem::showErrorDialog(const QString& err)
{
    QPointer<QMessageBox> msgBox = new QMessageBox(QApplication::activeWindow());
    msgBox->setWindowTitle(i18nc("@title:window", "Error"));
    msgBox->setText(i18n("%1 may not be playable.", d->url.fileName()));
    msgBox->setDetailedText(err);
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setDefaultButton(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();

    d->artist = d->url.fileName();
    d->title  = i18n("This file may not be playable.");
    setText(i18nc("artist - title", "%1 - %2", artist(), title()));
    setBackground(QBrush(Qt::red));
    setForeground(QBrush(Qt::white));
    QFont errorFont = font();
    errorFont.setBold(true);
    errorFont.setItalic(true);
    setFont(errorFont);
    delete msgBox;
}

// ------------------------------------------------------------------

PresentationAudioList::PresentationAudioList(QWidget* const parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setSortingEnabled(false);
    setIconSize(QSize(32, 32));
}

void PresentationAudioList::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void PresentationAudioList::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void PresentationAudioList::dropEvent(QDropEvent* e)
{
    QList<QUrl> list = e->mimeData()->urls();
    QList<QUrl> urls;

    Q_FOREACH (const QUrl& url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.isFile() && fi.exists())
        {
            urls.append(QUrl(url));
        }
    }

    e->acceptProposedAction();

    if (!urls.isEmpty())
    {
        Q_EMIT signalAddedDropItems(urls);
    }
}

QList<QUrl> PresentationAudioList::fileUrls()
{
    QList<QUrl> files;

    for (int i = 0 ; i < count() ; ++i)
    {
        PresentationAudioListItem* const sitem = dynamic_cast<PresentationAudioListItem*>(item(i));

        if (sitem)
        {
            files << QUrl(sitem->url());
        }
    }

    return files;
}

} // namespace DigikamGenericPresentationPlugin
