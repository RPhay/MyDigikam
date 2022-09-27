/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : interface to database information for shared tools.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2017      by Mario Frank <mario dot frank at uni minus potsdam dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_INFO_IFACE_H
#define DIGIKAM_DB_INFO_IFACE_H

// Local includes

#include "applicationsettings.h"
#include "dinfointerface.h"
#include "digikam_export.h"

namespace Digikam
{

class Album;

class DIGIKAM_GUI_EXPORT DBInfoIface : public DInfoInterface
{
    Q_OBJECT

public:

    explicit DBInfoIface(QObject* const parent,
                         const QList<QUrl>& lst = QList<QUrl>(),
                         const ApplicationSettings::OperationType type = ApplicationSettings::Unspecified);
    ~DBInfoIface()                                                                        override;

public:

    Q_SLOT void slotDateTimeForUrl(const QUrl& url, const QDateTime& dt, bool updModDate) override;
    Q_SLOT void slotMetadataChangedForUrl(const QUrl& url)                                override;

public:

    QList<QUrl> currentSelectedItems()                                              const override;
    QList<QUrl> currentAlbumItems()                                                 const override;
    void        parseAlbumItemsRecursive()                                                override;

    QList<QUrl> albumItems(Album* const album)                                      const;
    QList<QUrl> albumItems(int id)                                                  const override;
    QList<QUrl> albumsItems(const DAlbumIDs&)                                       const override;
    QList<QUrl> allAlbumItems()                                                     const override;

    DInfoMap    albumInfo(int)                                                      const override;

    DInfoMap    itemInfo(const QUrl&)                                               const override;
    void        setItemInfo(const QUrl&, const DInfoMap&)                           const override;

    QWidget*    albumChooser(QWidget* const parent)                                 const override;
    DAlbumIDs   albumChooserItems()                                                 const override;
    bool        supportAlbums()                                                     const override;

    QWidget*    uploadWidget(QWidget* const parent)                                 const override;
    QUrl        uploadUrl()                                                         const override;

    QUrl        defaultUploadUrl()                                                  const override;

    QAbstractItemModel* tagFilterModel()                                                  override;

    void        deleteImage(const QUrl& url)                                              override;

    void        openSetupPage(SetupPage page)                                             override;

#ifdef HAVE_MARBLE

    QList<GPSItemContainer*> currentGPSItems()                                      const override;

#endif

    QMap<QString, QString> passShortcutActionsToWidget(QWidget* const wdg)          const override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DB_INFO_IFACE_H
