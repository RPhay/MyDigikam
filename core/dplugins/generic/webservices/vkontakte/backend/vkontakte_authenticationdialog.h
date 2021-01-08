/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2010      by Thomas McGuire <mcguire at kde dot org>
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_VKONTAKTE_AUTHENTICATIONDIALOG_H
#define DIGIKAM_VKONTAKTE_AUTHENTICATIONDIALOG_H

// Qt includes

#include <QDialog>

// Local includes

#include "vkontakte_apppermissions.h"

namespace Vkontakte
{

class AuthenticationDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AuthenticationDialog(QWidget* const parent);
    ~AuthenticationDialog() override;

    void setAppId(const QString& appId);
    void Q_DECL_DEPRECATED setPermissions(const QStringList& permissions);
    void setPermissions(Vkontakte::AppPermissions::Value permissions);
    void setDisplayMode(const QString& displayMode);
    void start();

Q_SIGNALS:

    void authenticated(const QString& accessToken);
    void canceled();

private Q_SLOTS:

    void urlChanged(const QUrl& url);
    void loadFinished(bool ok);
    void showErrorDialog();

private:

    class Private;
    Private* const d;
};

} // namespace Vkontakte

#endif // DIGIKAM_VKONTAKTE_AUTHENTICATIONDIALOG_H
