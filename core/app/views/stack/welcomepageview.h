/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-20
 * Description : a widget to display a welcome page
 *               on root album.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_WELCOME_PAGE_VIEW_H
#define DIGIKAM_WELCOME_PAGE_VIEW_H

#include "digikam_config.h"

// Qt includes

#include <QByteArray>
#include <QString>
#include <QUrl>
#include <QWidget>
#include <QLabel>

#ifdef HAVE_QWEBENGINE
#   include <QWebEngineView>
#   include <QWebEnginePage>
#   include <QWebEngineSettings>
#else
#   include <qwebview.h>
#endif

// Local includes

#include "digikam_export.h"

namespace Digikam
{

#ifdef HAVE_QWEBENGINE

class WelcomePageViewPage : public QWebEnginePage
{
    Q_OBJECT

public:

    explicit WelcomePageViewPage(QObject* const parent = nullptr);
    ~WelcomePageViewPage()                                                          override;

    bool acceptNavigationRequest(const QUrl&, QWebEnginePage::NavigationType, bool) override;

Q_SIGNALS:

    void linkClicked(const QUrl&);

};

// -------------------------------------------------------------------

class WelcomePageView : public QWebEngineView

#else

class WelcomePageView : public QWebView

#endif

{
    Q_OBJECT

public:

    explicit WelcomePageView(QWidget* const parent);
    ~WelcomePageView()                                       override = default;

private:

    QByteArray  fileToString(const QString& aFileName) const;
    QStringList featuresTabContent()                   const;
    QStringList aboutTabContent()                      const;

private Q_SLOTS:

    void slotUrlOpen(const QUrl&);
    void slotThemeChanged();
};


} // namespace Digikam

#endif // DIGIKAM_WELCOME_PAGE_VIEW_H
