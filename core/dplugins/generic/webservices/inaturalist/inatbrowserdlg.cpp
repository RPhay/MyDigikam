/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-21
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

#include "inatbrowserdlg.h"
#include "digikam_config.h"

// Qt includes

#include <QGridLayout>
#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QToolBar>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QHash>

#ifdef HAVE_QWEBENGINE
#   include <QWebEngineView>
#   include <QWebEnginePage>
#   include <QWebEngineProfile>
#   include <QWebEngineCookieStore>
#else
#   include <qwebview.h>
#   include <QWebFrame>
#   include <QNetworkCookieJar>
#endif

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "statusprogressbar.h"
#include "searchtextbar.h"
#include "dxmlguiwindow.h"
#include "digikam_debug.h"

namespace DigikamGenericINatPlugin
{

#ifndef HAVE_QWEBENGINE

/**
 * Derived class that exposes protected function allCookies().
 */
class InatBrowserCookieJar : public QNetworkCookieJar
{
public:

    InatBrowserCookieJar(QObject* const parent = nullptr)
        : QNetworkCookieJar(parent)
    {
    }

    ~InatBrowserCookieJar()
    {
    }

    QList<QNetworkCookie> getAllCookies() const
    {
        return allCookies();
    }
};

#endif // !defined HAVE_QWEBENGINE

class Q_DECL_HIDDEN INatBrowserDlg::Private
{
public:

    explicit Private()
        : home       (QLatin1String("https://www.inaturalist.org/users/api_token")),
          browser    (nullptr),
          toolbar    (nullptr),
          apiKeyFound(false)
    {
    }

public:

    const QUrl                        home;

#ifdef HAVE_QWEBENGINE

    QWebEngineView*                   browser;

#else

    QWebView*                         browser;

#endif

    QString                           username;
    QToolBar*                         toolbar;
    bool                              apiKeyFound;

    QHash<QByteArray, QNetworkCookie> cookies;
};

INatBrowserDlg::INatBrowserDlg(const QString& username,
                               const QList<QNetworkCookie>& cookies,
                               QWidget* const parent)
    : QDialog(parent),
      d      (new Private)
{
    setModal(false);
    d->username = username;

#ifdef HAVE_QWEBENGINE

    d->browser                             = new QWebEngineView(this);
    QWebEngineCookieStore* const cookieJar = d->browser->page()->profile()->cookieStore();
    cookieJar->deleteAllCookies();

    connect(cookieJar, SIGNAL(cookieAdded(QNetworkCookie)),
            this, SLOT(slotCookieAdded(QNetworkCookie)));

    connect(cookieJar, SIGNAL(cookieRemoved(QNetworkCookie)),
            this, SLOT(slotCookieRemoved(QNetworkCookie)));

#else

    d->browser                         = new QWebView(this);
    d->browser->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QNetworkCookieJar* const cookieJar = new InatBrowserCookieJar();
    d->browser->page()->networkAccessManager()->setCookieJar(cookieJar);

#endif

    QDateTime now(QDateTime::currentDateTime());

    for (auto cookie : cookies)
    {
        if (filterCookie(cookie, false, now))
        {
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Setting cookie" << cookie;

#ifdef HAVE_QWEBENGINE

            cookieJar->setCookie(cookie);

#else

            cookieJar->insertCookie(cookie);

#endif

        }
    }

    // --------------------------

    d->toolbar = new QToolBar(this);
    d->toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

#ifdef HAVE_QWEBENGINE

    d->toolbar->addAction(d->browser->pageAction(QWebEnginePage::Back));
    d->toolbar->addAction(d->browser->pageAction(QWebEnginePage::Forward));
    d->toolbar->addAction(d->browser->pageAction(QWebEnginePage::Reload));
    d->toolbar->addAction(d->browser->pageAction(QWebEnginePage::Stop));

#else

    d->toolbar->addAction(d->browser->pageAction(QWebPage::Back));
    d->toolbar->addAction(d->browser->pageAction(QWebPage::Forward));
    d->toolbar->addAction(d->browser->pageAction(QWebPage::Reload));
    d->toolbar->addAction(d->browser->pageAction(QWebPage::Stop));

#endif

    QAction* const gohome  = new QAction(QIcon::fromTheme(QLatin1String("go-home")),
                                         i18n("Home"), this);
    gohome->setToolTip(i18n("Go back to Home page"));
    d->toolbar->addAction(gohome);

    // ----------------------

    QGridLayout* const grid = new QGridLayout(this);
    grid->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    grid->addWidget(d->toolbar, 0, 0, 1, 1);
    grid->addWidget(d->browser, 1, 0, 1, 3);
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(1, 10);
    setLayout(grid);

    // ----------------------

    connect(d->browser, SIGNAL(titleChanged(QString)),
            this, SLOT(slotTitleChanged(QString)));

    connect(d->browser, SIGNAL(loadFinished(bool)),
            this, SLOT(slotLoadingFinished(bool)));

    connect(gohome, SIGNAL(triggered()),
            this, SLOT(slotGoHome()));

    connect(this, SIGNAL(signalWebText(QString)),
            this, SLOT(slotWebText(QString)));

    // ----------------------

    resize(800, 800);

    slotGoHome();
}

INatBrowserDlg::~INatBrowserDlg()
{
    delete d;
}

void INatBrowserDlg::closeEvent(QCloseEvent* e)
{
    if (!d->apiKeyFound)
    {
        emit signalApiToken(QString(), QList<QNetworkCookie>());
    }

    e->accept();
}

void INatBrowserDlg::slotTitleChanged(const QString& title)
{
    setWindowTitle(title);
}

void INatBrowserDlg::slotLoadingFinished(bool status)
{
    QString url = d->browser->url().toString();

    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Url" << url << "loaded.";

    if      (status && (url == d->home.toString()))
    {

#ifdef HAVE_QWEBENGINE

        d->browser->page()->toPlainText(
            [this](const QString& text)
                {
                    emit signalWebText(text);
                }
        );

#else

        emit signalWebText(d->browser->page()->mainFrame()->toPlainText());

#endif

    }
    else if (!d->username.isEmpty() &&
             (url == QLatin1String("https://www.inaturalist.org/users/sign_in")))
    {
        QString script = QString(QLatin1String("document.getElementById(\"user_"
                                               "email\").value=\"%1\";")).
                         arg(d->username);

#ifdef HAVE_QWEBENGINE

        d->browser->page()->runJavaScript(script);

#else

        d->browser->page()->mainFrame()->evaluateJavaScript(script);

#endif

    }
}

void INatBrowserDlg::slotWebText(const QString& text)
{
    const QString key = QLatin1String("api_token");

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);

    if ((err.error == QJsonParseError::NoError) && doc.isObject() &&
        doc.object().contains(key))
    {

#ifdef HAVE_QWEBENGINE

        emit signalApiToken(doc.object()[key].toString(),
                            filterCookies(d->cookies.values(), false));

#else

        const InatBrowserCookieJar* jar = dynamic_cast<const InatBrowserCookieJar*>
            (d->browser->page()->networkAccessManager()->cookieJar());

        emit signalApiToken(doc.object()[key].toString(),
                            filterCookies(jar->getAllCookies(), false));

#endif

        d->apiKeyFound = true;
        close();
    }
}

void INatBrowserDlg::slotGoHome()
{
    d->browser->setUrl(d->home);
}

static QByteArray cookieKey(const QNetworkCookie& cookie)
{
    static const char sep('\n');

    return (
            cookie.name()            +
            sep                      +
            cookie.domain().toUtf8() +
            sep                      +
            cookie.path().toUtf8()
           );
}

bool INatBrowserDlg::filterCookie(const QNetworkCookie& cookie,
                                  bool keepSessionCookies,
                                  const QDateTime& now)
{
    return cookie.isSessionCookie() ? keepSessionCookies
                                    : (cookie.expirationDate() > now);
}

QList<QNetworkCookie> INatBrowserDlg::filterCookies(const QList<QNetworkCookie>& cookies,
                                                    bool keepSessionCookies)
{
    QList<QNetworkCookie> result;
    QDateTime now(QDateTime::currentDateTime());

    for (auto cookie : cookies)
    {
        if (filterCookie(cookie, keepSessionCookies, now))
        {
            result << cookie;
        }
    }

    return result;
}

void INatBrowserDlg::slotCookieAdded(const QNetworkCookie& cookie)
{
    d->cookies.insert(cookieKey(cookie), cookie);
}

void INatBrowserDlg::slotCookieRemoved(const QNetworkCookie& cookie)
{
    d->cookies.remove(cookieKey(cookie));
}

} // namespace DigikamGenericINatPlugin
