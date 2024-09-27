/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-30-08
 * Description : batch thumbnails generator
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "thumbsgenerator.h"

// Qt includes

#include <QApplication>
#include <QString>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "coredb.h"
#include "coredbalbuminfo.h"
#include "albummanager.h"
#include "applicationsettings.h"
#include "coredbaccess.h"
#include "iteminfo.h"
#include "thumbsdbaccess.h"
#include "thumbsdb.h"
#include "maintenancethread.h"
#include "digikam_config.h"

namespace Digikam
{

class Q_DECL_HIDDEN ThumbsGenerator::Private
{
public:

    Private() = default;

    bool               rebuildAll = true;

    AlbumList          albumList;

    QStringList        allPicturesPath;

    MaintenanceThread* thread     = nullptr;
};

ThumbsGenerator::ThumbsGenerator(const bool rebuildAll,
                                 const AlbumList& list,
                                 ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("ThumbsGenerator"), parent),
      d              (new Private)
{
    d->albumList = list;

    init(rebuildAll);
}

ThumbsGenerator::ThumbsGenerator(const bool rebuildAll,
                                 int albumId,
                                 ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("ThumbsGenerator"), parent),
      d              (new Private)
{
    d->albumList.append(AlbumManager::instance()->findPAlbum(albumId));

    init(rebuildAll);
}

ThumbsGenerator::~ThumbsGenerator()
{
    delete d;
}

void ThumbsGenerator::init(const bool rebuildAll)
{
    d->rebuildAll = rebuildAll;
    d->thread     = new MaintenanceThread(this);

    connect(d->thread, SIGNAL(signalCompleted()),
            this, SLOT(slotDone()));

    connect(d->thread, SIGNAL(signalAdvance(ItemInfo,QImage)),
            this, SLOT(slotAdvance(ItemInfo,QImage)));
}

void ThumbsGenerator::setUseMultiCoreCPU(bool b)
{
    d->thread->setUseMultiCore(b);
}

void ThumbsGenerator::slotCancel()
{
    d->thread->cancel();
    MaintenanceTool::slotCancel();
}

void ThumbsGenerator::slotStart()
{
    MaintenanceTool::slotStart();

    setLabel(i18n("Thumbs"));

    ProgressManager::addProgressItem(this);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (d->albumList.isEmpty())
    {
        d->albumList = AlbumManager::instance()->allPAlbums();
    }

    for (AlbumList::const_iterator it = d->albumList.constBegin();
         !canceled() && (it != d->albumList.constEnd()); ++it)
    {
        if (!(*it))
        {
            continue;
        }

        if      ((*it)->type() == Album::PHYSICAL)
        {
            const auto pathes = CoreDbAccess().db()->getItemURLsInAlbum((*it)->id());

            for (const QString& path : pathes)
            {
                if (!d->allPicturesPath.contains(path))
                {
                    d->allPicturesPath << path;
                }
            }
        }
        else if ((*it)->type() == Album::TAG)
        {
            const auto pathes = CoreDbAccess().db()->getItemURLsInTag((*it)->id());

            for (const QString& path : pathes)
            {
                if (!d->allPicturesPath.contains(path))
                {
                    d->allPicturesPath << path;
                }
            }
        }
    }

    if (!d->rebuildAll)
    {
        QHash<QString, int> filePaths = ThumbsDbAccess().db()->getFilePathsWithThumbnail();
        QStringList::iterator it      = d->allPicturesPath.begin();

        while (it != d->allPicturesPath.end())
        {
            if (filePaths.contains(*it))
            {
                it = d->allPicturesPath.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // remove non-image or video files from the list

    QStringList::iterator it = d->allPicturesPath.begin();

    while (it != d->allPicturesPath.end())
    {
        ItemInfo info = ItemInfo::fromLocalFile(*it);

        if ((info.category() != DatabaseItem::Image) &&
            (info.category() != DatabaseItem::Video) &&
            (info.category() != DatabaseItem::Audio))
        {
            it = d->allPicturesPath.erase(it);
        }
        else
        {
            ++it;
        }
    }

    QApplication::restoreOverrideCursor();

    if (d->allPicturesPath.isEmpty())
    {
        slotDone();

        return;
    }

    setTotalItems(d->allPicturesPath.count());

    d->thread->generateThumbs(d->allPicturesPath);
    d->thread->start();
}

void ThumbsGenerator::slotAdvance(const ItemInfo& inf, const QImage& img)
{
    QString lbl = i18n("Thumbnail for: %1\n", inf.name());
    lbl.append(i18n("Path: %1", inf.relativePath()));
    setLabel(lbl);
    setThumbnail(QPixmap::fromImage(img));
    advance(1);
}

} // namespace Digikam

#include "moc_thumbsgenerator.cpp"
