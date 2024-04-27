/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-01
 * Description : Widget for displaying HTML in the backends - QtWebKit version
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_HTML_WIDGET_QWEBKIT_H
#define DIGIKAM_HTML_WIDGET_QWEBKIT_H

// Qt includes

#include <qwebview.h>

// Local includes

#include "geoifacecommon.h"
#include "geoifacetypes.h"
#include "geocoordinates.h"

namespace Digikam
{

class HTMLWidget : public QWebView
{
    Q_OBJECT

public:

    explicit HTMLWidget(QWidget* const parent = nullptr);
    ~HTMLWidget();

    void loadInitialHTML(const QString& initialHTML);
    QVariant runScript(const QString& scriptCode, bool async = true);
    bool runScript2Coordinates(const QString& scriptCode, GeoCoordinates* const coordinates);
    void mouseModeChanged(const GeoMouseModes mouseMode);
    void setSelectionRectangle(const GeoCoordinates::Pair& searchCoordinates);
    void removeSelectionRectangle();
    void centerOn(const qreal west, const qreal north, const qreal east, const qreal south,
                  const bool useSaneZoomLevel = true);
    void setSharedGeoIfaceObject(GeoIfaceSharedData* const sharedData);

protected:

    bool eventFilter(QObject* object, QEvent* event)    override;
    void mousePressEvent(QMouseEvent* e)                override;
    void mouseReleaseEvent(QMouseEvent* e)              override;
    void mouseMoveEvent(QMouseEvent* e)                 override;

protected Q_SLOTS:

    void slotHTMLCompleted(bool ok);
    void slotScanForJSMessages(const QString& message);
    void progress(int progress);

Q_SIGNALS:

    void signalHTMLEvents(const QStringList& events);
    void signalMessageEvent(const QString& message);
    void signalJavaScriptReady();
    void selectionHasBeenMade(const Digikam::GeoCoordinates::Pair& coordinatesRect);

private:

    class Private;
    Private* const      d = nullptr;

    GeoIfaceSharedData* s = nullptr;
};

} // namespace Digikam

#endif // DIGIKAM_HTML_WIDGET_QWEBKIT_H
