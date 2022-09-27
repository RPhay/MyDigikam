/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : interface to item information for shared tools
 *               based on DMetadata. This interface is used in all cases
 *               where no database is available (aka Showfoto).
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DMETA_INFO_IFACE_H
#define DIGIKAM_DMETA_INFO_IFACE_H

// Local includes

#include "digikam_export.h"
#include "dinfointerface.h"

namespace Digikam
{

class DIGIKAM_EXPORT DMetaInfoIface : public DInfoInterface
{
    Q_OBJECT

public:

    explicit DMetaInfoIface(QObject* const, const QList<QUrl>&);
    ~DMetaInfoIface()                                             override;

    Q_SLOT void slotDateTimeForUrl(const QUrl& url,
                                   const QDateTime& dt,
                                   bool updModDate)               override;

    Q_SLOT void slotMetadataChangedForUrl(const QUrl& url)        override;

    Q_SIGNAL void signalItemChanged(const QUrl& url);

public:

    QList<QUrl> currentSelectedItems()                      const override;
    QList<QUrl> currentAlbumItems()                         const override;
    QList<QUrl> allAlbumItems()                             const override;
    void        parseAlbumItemsRecursive()                        override;

    DInfoMap    itemInfo(const QUrl&)                       const override;
    void        setItemInfo(const QUrl&, const DInfoMap&)   const override;

    bool        supportAlbums()                             const override;

    QWidget* uploadWidget(QWidget* const parent)            const override;
    QUrl     uploadUrl()                                    const override;

    QUrl     defaultUploadUrl()                             const override;
    void     deleteImage(const QUrl& url)                         override;

    Q_SIGNAL void signalRemoveImageFromAlbum(const QUrl&);

#ifdef HAVE_MARBLE

    QList<GPSItemContainer*> currentGPSItems()              const override;

#endif

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DMETA_INFO_IFACE_H
