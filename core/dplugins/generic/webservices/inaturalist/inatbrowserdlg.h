/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a simple web browser dialog based on Qt WebEngine.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
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

#ifndef DIGIKAM_INAT_BROWSER_DLG_H
#define DIGIKAM_INAT_BROWSER_DLG_H

// Qt include

#include <QDialog>
#include <QWidget>
#include <QUrl>
#include <QString>
#include <QCloseEvent>
#include <QNetworkCookie>
#include <QDateTime>

namespace DigikamGenericINatPlugin
{

class SearchTextSettings;

class INatBrowserDlg : public QDialog
{
    Q_OBJECT

public:

    INatBrowserDlg(const QString& username, const QList<QNetworkCookie>&,
                   QWidget* const parent);
    ~INatBrowserDlg() override;

    static bool filterCookie(const QNetworkCookie& cookie,
                             bool keepSessionCookies, const QDateTime&);

    /**
     * filter out expired cookies and, if requested, session cookies
     */
    static QList<QNetworkCookie> filterCookies(const QList<QNetworkCookie>&,
                                               bool keepSessionCookies = true);

Q_SIGNALS:

    void signalApiToken(const QString&, const QList<QNetworkCookie>&);
    void signalWebText(const QString&);

private Q_SLOTS:

    void slotLoadingFinished(bool);
    void slotTitleChanged(const QString&);
    void slotWebText(const QString&);
    void slotGoHome();
    void slotCookieAdded(const QNetworkCookie&);
    void slotCookieRemoved(const QNetworkCookie&);

protected:

    void closeEvent(QCloseEvent*) override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_BROWSER_DLG_H
