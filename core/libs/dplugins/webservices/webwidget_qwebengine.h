/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-7-30
 * Description : Widget for displaying HTML in the backends
 *
 * SPDX-FileCopyrightText: 2018      by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_WEB_WIDGET_QWEBENGINE_H
#define DIGIKAM_WEB_WIDGET_QWEBENGINE_H

// Qt includes

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineCookieStore>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT WebWidget : public QWebEngineView
{
    Q_OBJECT

public:

    explicit WebWidget(QWidget* const parent = nullptr);
    ~WebWidget();

protected:

    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:

    void closeView(bool val);

private:

  class Private;
  Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_WEB_WIDGET_QWEBENGINE_H
