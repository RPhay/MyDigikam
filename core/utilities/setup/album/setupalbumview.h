/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-02-01
 * Description : album view configuration setup tab
 *
 * SPDX-FileCopyrightText: 2003-2004 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_ALBUM_VIEW_H
#define DIGIKAM_SETUP_ALBUM_VIEW_H

// Qt includes

#include <QScrollArea>

namespace Digikam
{

class SetupAlbumView : public QScrollArea
{
    Q_OBJECT

public:

    enum AlbumTab
    {
        IconView = 0,
        FolderView,
        Preview,
        FullScreen,
        MimeType,
        Category
    };

public:

    explicit SetupAlbumView(QWidget* const parent = nullptr);
    ~SetupAlbumView() override;

    void setActiveTab(AlbumTab tab);
    AlbumTab activeTab() const;

    void applySettings();

    bool useLargeThumbsHasChanged() const;

private:

    void readSettings();

private Q_SLOTS:

    void slotUseLargeThumbsToggled(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_ALBUM_VIEW_H
