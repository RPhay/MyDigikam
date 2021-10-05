/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorite edit dialog
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SHOWFOTO_STACK_VIEW_FAVORITE_DLG_H
#define SHOWFOTO_STACK_VIEW_FAVORITE_DLG_H

// Qt includes

#include <QString>
#include <QDialog>

// Local includes

#include "digikam_config.h"

namespace ShowFoto
{

class ShowfotoStackViewFavorites;

class ShowfotoStackViewFavoriteDlg : public QDialog
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewFavoriteDlg(ShowfotoStackViewFavorites* const parent,
                                          bool create = false);
    ~ShowfotoStackViewFavoriteDlg() override;

    void setName(const QString& name);
    QString name()     const;

    void setIcon(const QString& icon);
    QString icon()     const;

    void setUrls(const QList<QUrl>& urls);
    QList<QUrl> urls() const;

public:

    static bool favoriteEdit(ShowfotoStackViewFavorites* const parent, QString& name, QString& icon, QList<QUrl>& urls);
    static bool favoriteCreate(ShowfotoStackViewFavorites* const parent, QString& name, QString& icon, QList<QUrl>& urls);

private Q_SLOTS:

    void slotIconChanged();
    void slotIconResetClicked();
    void slotAccept();
    void slotModified();

private:

    bool canAccept() const;

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_FAVORITE_DLG_H
